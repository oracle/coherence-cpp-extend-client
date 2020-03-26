/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/cache/LocalConcurrentCache.hpp"

#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashSet.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::ConcurrentModificationException;
using coherence::util::HashSet;


// ----- constructors -------------------------------------------------------

LocalConcurrentCache::LocalConcurrentCache(bool fEnforceLocking,
        int64_t                   cWaitMillis,
        int32_t                   cUnits,
        int32_t                   cExpiryMillis,
        float64_t                 dflPruneLevel,
        size32_t                  cInitialBuckets,
        float32_t                 flLoadFactory,
        float32_t                 flGrowthRate)
    : super(cUnits, cExpiryMillis, dflPruneLevel, cInitialBuckets,
      flLoadFactory, flGrowthRate), m_fEnforceLocking(fEnforceLocking),
      m_cWaitMillis(cWaitMillis),
      f_hMapLock(self(), SafeHashMap::create(), /*fMutable*/ true),
      f_hGateMap(self(), ThreadGate::create(), /*fMutable*/ true)
    {
    }


// ----- ConcurrentMap interface --------------------------------------------

bool LocalConcurrentCache::lock(Object::View vKey, int64_t cWait) const
    {
    SafeHashMap::Handle hMapLock = f_hMapLock;
    ThreadGate::Handle  hGateMap = f_hGateMap;

    if (vKey == ConcurrentMap::getLockAll())
        {
        return hGateMap->close(cWait);
        }

    if (!hGateMap->enter(cWait))
        {
        return false;
        }

    Lock::Handle hLock    = NULL;

    struct lockFinally
        {
        lockFinally(SafeHashMap::Handle _hMapLock,
                ThreadGate::Handle _hGateMap, Lock::Handle _hLock,
                Object::View _vKey)
            : hMapLock(_hMapLock), hGateMap(_hGateMap), hLock(_hLock),
              vKey(_vKey), fSuccess(false)
            {
            }

        ~lockFinally()
            {
            if (!fSuccess)
                {
                if (hLock != NULL && hLock->isDiscardable())
                    {
                    hMapLock->remove(vKey);
                    }
                hGateMap->exit();
                }
            }

        SafeHashMap::Handle hMapLock;
        ThreadGate::Handle  hGateMap;
        Lock::Handle        hLock;
        Object::View        vKey;
        bool fSuccess;
        } finally(hMapLock, hGateMap, hLock, vKey);

    while (true)
        {
        COH_SYNCHRONIZED (hMapLock)
            {
            hLock = cast<Lock::Handle>(hMapLock->get(vKey));
            if (hLock == NULL)
                {
                hLock = instantiateLock(vKey);
                hLock->assign(0); // this will succeed without blocking
                hMapLock->put(vKey, hLock);
                return finally.fSuccess = true;
                }
            else
                {
                // perform a quick, non-blocking check to see if the
                // current thread already owns the lock
                if (hLock->isOwnedByCaller())
                    {
                    hLock->assign(0); // this will succeed without blocking
                    return finally.fSuccess = true;
                    }
                }
            }

        COH_SYNCHRONIZED (hLock)
            {
            // make sure the lock didn't just get removed
            if (hLock == hMapLock->get(vKey))
                {
                return finally.fSuccess = hLock->assign(cWait);
                }
            }
        }
    }

bool LocalConcurrentCache::lock(Object::View vKey) const
    {
    return lock(vKey, 0);
    }

bool LocalConcurrentCache::unlock(Object::View vKey) const
    {
    if (vKey == ConcurrentMap::getLockAll())
        {
        try
            {
            f_hGateMap->open();
            return true;
            }
        catch (IllegalStateException::View)
            {
            return false;
            }
        }

    bool fReleased = true;
    while (true)
        {
        Lock::Handle hLock = cast<Lock::Handle>(f_hMapLock->get(vKey));
        if (NULL == hLock)
            {
            break;
            }

        COH_SYNCHRONIZED (hLock)
            {
            if (f_hMapLock->get(vKey) == hLock)
                {
                fReleased = hLock->release();
                if (hLock->isDiscardable())
                    {
                    f_hMapLock->remove(vKey);
                    }
                break;
                }
            }
        }

    try
        {
        f_hGateMap->exit();
        }
    catch (IllegalStateException::View) {}

    return fReleased;
    }


// ----- Map interface ------------------------------------------------------

void LocalConcurrentCache::clear()
    {
    if (isLockingEnforced())
        {
        COH_CACHE_TRYLOCK(this, ConcurrentMap::getLockAll(), getWaitMillis())
            {
            super::clear();
            }
        else
            {
            COH_THROW(ConcurrentModificationException::create("clear"));
            }
        }
    else
        {
        super::clear();
        }
    }

Object::Holder LocalConcurrentCache::put(Object::View vKey, Object::Holder ohValue)
    {
    bool fForceLock = isLockingEnforced();

    if (fForceLock)
        {
        COH_CACHE_TRYLOCK(this, ConcurrentMap::getLockAll(), getWaitMillis())
            {
            return super::put(vKey, ohValue);
            }
        else
            {
            COH_THROW_STREAM(ConcurrentModificationException,
                    "(thread=" << Thread::currentThread() << ") "
                    << getLockDescription(vKey));
            }
        }
    else
        {
        return super::put(vKey, ohValue);
        }
    }

void LocalConcurrentCache::putAll(Map::View vMap)
    {
    bool fForceLock = isLockingEnforced();

    if (fForceLock)
        {
        Set::Handle hSetLocked = HashSet::create();

        struct putAllFinally
            {
            putAllFinally(Set::Handle _hSet, LocalConcurrentCache::Handle _hCache)
                : hSet(_hSet), hCache(_hCache)
                {
                }

            ~putAllFinally()
                {
                for (Iterator::Handle hIter = hSet->iterator(); hIter->hasNext(); )
                    {
                    hCache->unlock(hIter->next());
                    }
                }

            Set::Handle                  hSet;
            LocalConcurrentCache::Handle hCache;
            } finally(hSetLocked, this);

        for (Iterator::Handle hIter = vMap->keySet()->iterator(); hIter->hasNext(); )
            {
            Object::View vKey = hIter->next();
            if (lock(vKey, getWaitMillis()))
                {
                hSetLocked->add(vKey);
                }
            else
                {
                COH_THROW_STREAM(ConcurrentModificationException,
                        "(thread=" << Thread::currentThread() << ") "
                        << getLockDescription(vKey));
                }
            }

        super::putAll(vMap);
        }
    else
        {
        super::putAll(vMap);
        }
    }

Object::Holder LocalConcurrentCache::remove(Object::View vKey)
    {
    bool fForceLock = isLockingEnforced();

    if (fForceLock)
        {
        COH_CACHE_TRYLOCK(this, vKey, getWaitMillis())
            {
            return super::remove(vKey);
            }
        else
            {
            COH_THROW_STREAM(ConcurrentModificationException,
                    "(thread=" << Thread::currentThread() << ") "
                    << getLockDescription(vKey));
            }
        }
    else
        {
        return super::remove(vKey);
        }
    }


// ----- accessors ----------------------------------------------------------

bool LocalConcurrentCache::isLockingEnforced() const
    {
    return m_fEnforceLocking;
    }

void LocalConcurrentCache::setLockingEnforced(bool fEnforce)
    {
    m_fEnforceLocking = fEnforce;
    }

int64_t LocalConcurrentCache::getWaitMillis() const
    {
    return m_cWaitMillis;
    }

void LocalConcurrentCache::setWaitMillis(int64_t cWaitMillis)
    {
    m_cWaitMillis = cWaitMillis;
    }


// ----- Describable interface ----------------------------------------------

String::View LocalConcurrentCache::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "LockingEnforced=" << isLockingEnforced()
        << ", WaitMillis=" << getWaitMillis()
        << ", ThreadGate=" << f_hGateMap
        << ", Locks={" << f_hMapLock << "}");
    }


// ----- helpers ------------------------------------------------------------

String::Handle LocalConcurrentCache::getLockDescription(Object::View vKey)
   {
   return COH_TO_STRING("key=\"" << vKey << "\", lock=" << f_hMapLock->get(vKey));
   }


// ----- inner class: Lock --------------------------------------------------

LocalConcurrentCache::Lock::Handle LocalConcurrentCache::instantiateLock(Object::View /*vKey*/) const
    {
    return Lock::create();
    }

LocalConcurrentCache::Lock::Lock()
    : m_hThread(self()), m_cLock(0), m_cBlock(0)
    {
    }

// ----- Lock interface -------------------------------------------------

bool LocalConcurrentCache::Lock::assign(int64_t cWait)
    {
    while (isDirty())
        {
        if (cWait == 0)
            {
            return false;
            }
        cWait = waitForNotify(cWait);
        }

    int32_t cLock = m_cLock + 1;
    if (cLock == 1)
        {
        m_hThread = Thread::currentThread();
        }
    else if (cLock == Integer16::max_value)
        {
        COH_THROW_STREAM(RuntimeException, "Lock count overflow: " << *this);
        }
    m_cLock = (int16_t) cLock;

    return true;
    }

int64_t LocalConcurrentCache::Lock::waitForNotify(int64_t cWait)
    {
    int64_t lTime = System::safeTimeMillis();
    try
        {
        m_cBlock++;

        struct waitForNotifyFinally
            {
            waitForNotifyFinally(int16_t& cBlock)
                : m_cBlock(cBlock)
                {
                }

            ~waitForNotifyFinally()
                {
                m_cBlock--;
                }

            int16_t& m_cBlock;
            } finally(m_cBlock);

        // in case of thread death of the lock holder, do not wait forever,
        // because thread death becomes an implicit unlock, and this thread
        // needs to then wake up and take the lock
        const int64_t cMaxWait = 1000L;
        int64_t       cMillis  = (cWait <= 0L || cWait > cMaxWait) ? cMaxWait : cWait;

        wait(cMillis);
        }
    catch (InterruptedException::View e)
        {
        Thread::currentThread()->interrupt();
        COH_THROW(RuntimeException::create("Lock request interrupted"));
        }

    if (cWait > 0)
        {
        // reduce the waiting time by the elapsed amount
        cWait -= System::safeTimeMillis() - lTime;
        cWait = std::max(int64_t(0), cWait);
        }

    return cWait;
    }

bool LocalConcurrentCache::Lock::release()
    {
    if (isDirty())
        {
        return false;
        }

    int cLock = m_cLock - 1;

    if (cLock == 0)
        {
        m_hThread = NULL;
        }
    else if (cLock < 0)
        {
        cLock = 0;
        }
    m_cLock = (int16_t) cLock;

    if (cLock == 0)
        {
        if (m_cBlock > 0)
            {
            notify();
            }
        return true;
        }
    else
        {
        return false;
        }
    }

bool LocalConcurrentCache::Lock::isDirty()
    {
    Thread::Handle hThreadHolder  = m_hThread;
    Thread::Handle hThreadCurrent = Thread::currentThread();

    if (hThreadHolder != NULL && hThreadHolder != hThreadCurrent)
        {
        // make sure that the holder thread is still alive
        Thread::State nState = hThreadHolder->getState();
        if (Thread::state_initial < nState && nState < Thread::state_terminated)
            {
            return true;
            }

        // the holder is dead - release the lock
        m_hThread = NULL;
        m_cLock  = 0;

        if (m_cBlock > 0)
            {
            notify();
            }
        }
    return false;
    }

bool LocalConcurrentCache::Lock::isOwnedByCaller()
    {
    return m_hThread == Thread::currentThread();
    }

bool LocalConcurrentCache::Lock::isDiscardable()
    {
    return m_cLock == 0 && m_cBlock == 0;
    }

Thread::Handle LocalConcurrentCache::Lock::getLockThread()
    {
    return m_hThread;
    }

int32_t LocalConcurrentCache::Lock::getLockCount()
    {
    return m_cLock;
    }

int32_t LocalConcurrentCache::Lock::getBlockCount()
    {
    return m_cBlock;
    }

String::View LocalConcurrentCache::Lock::getLockTypeDescription() const
    {
    return "Lock";
    }

// ----- Object interface -----------------------------------------------

TypedHandle<const String> LocalConcurrentCache::Lock::toString() const
    {
    return COH_TO_STRING(getLockTypeDescription() <<
           "[" << m_hThread << ", cnt=" << m_cLock
           << ", block=" << m_cBlock << ']');
    }


COH_CLOSE_NAMESPACE3
