/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/CachingMap.hpp"

#include "coherence/internal/net/NamedCacheDeactivationListener.hpp"

#include "coherence/net/NamedCache.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/Muterator.hpp"

#include "coherence/util/filter/InKeySetFilter.hpp"
#include "coherence/util/filter/NotFilter.hpp"

#include "private/coherence/net/cache/LocalConcurrentCache.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::internal::net::NamedCacheDeactivationListener;
using coherence::net::NamedCache;
using coherence::util::Collections;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::LinkedList;
using coherence::util::Muterator;
using coherence::util::filter::InKeySetFilter;
using coherence::util::filter::NotFilter;
using coherence::util::logging::Logger;

COH_OPEN_NAMESPACE_ANON(CachingMap)

// ----- helper class IgnoreList --------------------------------------------

/**
* Empty list that ignores any add operations.
*/
class IgnoreList
    : public class_spec<IgnoreList,
        extends<LinkedList> >
    {
    friend class factory<IgnoreList>;

    // ----- constructor --------------------------------------------

    protected:
        /**
        * Create an IgnoreList.
        */
        IgnoreList()
            {
            }

    // ----- List interface ----------------------------------------
    public:
        using LinkedList::add;

        /**
        * {@inheritDoc}
        */
        virtual bool add(size32_t /*i*/, Object::Holder /*oh*/)
            {
            return true;
            }

        using LinkedList::addAll;

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(size32_t /*i*/, Collection::View /*vc*/)
            {
            return true;
            }
    };

/**
* Release the cache.
*/
void releaseCache(Map::Handle hCache)
    {
    if (instanceof<LocalCache::Handle>(hCache))
        {
        cast<LocalCache::Handle>(hCache)->release();
        }
    else if (instanceof<NamedCache::Handle>(hCache))
        {
        cast<NamedCache::Handle>(hCache)->release();
        }
    }

COH_CLOSE_NAMESPACE_ANON

/**
* Get an empty list that ignores any add operations.
*
* @return the list
*/
List::Handle getIgnoreList()
    {
    static FinalHandle<List> hlist(System::common(),
            IgnoreList::create());
    return hlist;
    }
COH_STATIC_INIT(getIgnoreList());

/**
* Specifies whether the back map listener strictly adheres to the
* MapListenerSupport.SynchronousListener contract.
*
* @return true iff the listener strictly adheres to the
*              MapListenerSupport.SynchronousListener contract.
*/
bool getStrictSynchroListener()
    {
    static bool fStrictSyncroListener =
            Boolean::parse(System::getProperty(
                    "coherence.near.strictlistener", "true"));
    return fStrictSyncroListener;
    }
COH_STATIC_INIT(getStrictSynchroListener());


// ----- constructors ---------------------------------------------------

CachingMap::CachingMap(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
        InvalidationStrategy strategy, ConcurrentMap::Handle hMapControl)
        : f_hMapBack(self(), hMapBack, /*fMutable*/ true),
          f_hMapFront(self(), hMapFront, /*fMutable*/ true),
          m_strategyTarget(strategy),
          m_strategyCurrent(CachingMap::listen_none),
          m_hListener(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_hSimpleListener(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_hListenerDeactivation(self(), (DeactivationListener::Handle) NULL, /*fMutable*/ true),
          f_hListenerFront(self(), (FrontMapListener::Handle) NULL, /*fMutable*/ true),
          m_hFilterListener(self(), (Filter::Handle) NULL, /*fMutable*/ true),
          f_hMapControl(self(), NULL == hMapControl
                  ? (ConcurrentMap::Handle) LocalConcurrentCache::create(
                         /*fEnfocLocking*/ false, /*cWaitMillis*/-1L,
                         LocalConcurrentCache::max_units,
                         LocalConcurrentCache::never_expire)
                  : hMapControl, /*fMutable*/ true),
          f_hStats(self(), SimpleCacheStatistics::create(), /*fMutable*/ true),
          m_cInvalidationHits(0),
          m_cInvalidationMisses(0),
          m_cRegisterListener(0),
          m_fReleased(false),
          f_htloKeys(self(), ThreadLocalReference::create(), /*fMutable*/ true),
          f_vKeyGlobal(self(), Object::create())
    {
    COH_ENSURE_RELATION(Map::Handle, hMapFront, !=, NULL);
    COH_ENSURE_RELATION(Map::Handle, hMapBack,  !=, NULL);

    if (hMapControl == NULL)
        {
        cast<LocalConcurrentCache::Handle>(f_hMapControl)->setAllowMutableValues(true);
        }
    }


// ----- life-cycle ---------------------------------------------------------

void CachingMap::release()
    {
    ConcurrentMap::Handle hMapControl = getControlMap();
    if (!hMapControl->lock(ConcurrentMap::getLockAll(), 0))
        {
        // Note: we cannot do a blocking LOCK_ALL as any event which came
        // in while the ThreadGate is in the closing state would cause the
        // service thread to spin.  Unlike clear() there is no benefit in
        // sleeping/retrying here as we know that there are other active
        // threads, thus if we succeed they would get the
        // IllegalStateException
        COH_THROW (IllegalStateException::create(
                "Cache is in active use by other threads."));
        }

    // finally construct
    struct Finally
        {
        Finally(CachingMap::Handle _hThis)
            : hThis(_hThis)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle hMapControl = hThis->getControlMap();
            hMapControl->remove(hThis->f_vKeyGlobal);
            hMapControl->unlock(ConcurrentMap::getLockAll());
            }

        CachingMap::Handle hThis;
        } finally(this);

    try
        {
        hMapControl->put(f_vKeyGlobal, getIgnoreList());
        switch (m_strategyCurrent)
            {
            case listen_present:
                unregisterFrontListener();
                unregisterListeners(getFrontMap()->keySet());
                break;

            case listen_logical:
            case listen_all:
                unregisterListener();
                break;

            case listen_none:
            case listen_auto:
                break;
            }

        unregisterDeactivationListener();

        // release locally stored resources if possible
        releaseCache((CacheMap::Handle) f_hMapFront);
        releaseCache(hMapControl);

        m_hListener             = NULL;
        m_fReleased             = true;
        m_hFilterListener       = NULL;
        m_hListenerDeactivation = NULL;
        }
    catch (RuntimeException::View)
        {
        // ignore.
        }
    }


// ----- accessors ----------------------------------------------------------

CacheMap::Handle CachingMap::getFrontMap() const
    {
    if (m_fReleased)
        {
        COH_THROW (IllegalStateException::create("Cache is not active"));
        }

    return f_hMapFront;
    }

CacheMap::Handle CachingMap::getBackMap() const
    {
    if (m_fReleased)
        {
        COH_THROW (IllegalStateException::create("Cache is not active"));
        }

    return f_hMapBack;
    }

CachingMap::InvalidationStrategy
        CachingMap::getInvalidationStrategy() const
    {
    return m_strategyTarget;
    }

ConcurrentMap::Handle CachingMap::getControlMap() const
    {
    return f_hMapControl;
    }

CacheStatistics::Handle CachingMap::getCacheStatistics() const
    {
    return (SimpleCacheStatistics::Handle) f_hStats;
    }

int64_t CachingMap::getInvalidationHits() const
    {
    return m_cInvalidationHits;
    }

int64_t CachingMap::getInvalidationMisses() const
    {
    return m_cInvalidationMisses;
    }

int64_t CachingMap::getTotalRegisterListener() const
    {
    return m_cRegisterListener;
    }

bool CachingMap::isCoherent() const
    {
    return m_hListener != NULL;
    }


// ----- CachingMap interface -----------------------------------------------

void CachingMap::invalidateFront(Object::View vKey) const
    {
    if (NULL == getFrontMap()->remove(vKey))
        {
        ++m_cInvalidationMisses;
        }
    else
        {
        unregisterListener(vKey);
        ++m_cInvalidationHits;
        }
    }

void CachingMap::finalizePut(Object::View vKey, Object::Holder ohValue,
        List::Handle hlistEvents, int64_t cMillis)
    {
    CacheMap::Handle      hMapFront       = getFrontMap();
    ConcurrentMap::Handle hMapControl     = getControlMap();
    InvalidationStrategy  strategyTarget  = m_strategyTarget;
    InvalidationStrategy  strategyCurrent = m_strategyCurrent;

    if (listen_none == strategyTarget)
        {
        // we're not validating; simply update the front
        if (ohValue != NULL)
            {
            hMapFront->put(vKey, ohValue, cMillis);
            }
        }
    else if (getIgnoreList() == hlistEvents)
        {
        // IGNORE_LIST indicates that the entry is not already in the
        // front; we're not going to add it
        hMapControl->remove(vKey);
        }
    else if (NULL == hlistEvents)
        {
        COH_THROW_STREAM (IllegalStateException,
                "Encountered unexpected key "
                << vKey << "; this may be caused by concurrent "
                << "modification of the supplied key(s), or by an "
                << "inconsistent hashCode() or equals() implementation.");
        }
    else
        {
        // validate events and update the front if possible
        COH_SYNCHRONIZED (hlistEvents)
            {
            // put operation itself should generate one "natural"
            // INSERT or UPDATE; anything else should be considered
            // as an invalidating event
            bool fValid;
            if (NULL == ohValue)
                {
                fValid = false;
                }
            else
                {
                switch (hlistEvents->size())
                    {
                    case 0:
                        if (getStrictSynchroListener() &&
                               (listen_all == strategyCurrent ||
                                listen_logical == strategyCurrent ||
                                hMapFront->containsKey(vKey)))
                            {
                            COH_LOG ("Expected an insert/update for " << vKey
                                   << ", but none have been received",
                                   Logger::level_warning);
                            fValid = false;
                            }
                        else
                            {
                            fValid = true;
                            }
                        break;
                    case 1:
                        {
                        MapEvent::View vMapEvent = cast<MapEvent::View>
                                (hlistEvents->get(0));
                        int32_t        nId       = vMapEvent->getId();
                        fValid  = MapEvent::entry_inserted == nId ||
                                  MapEvent::entry_updated == nId;
                        break;
                        }
                    default:
                        fValid = false;
                        break;
                    }
                }

            if (fValid)
                {
                if (NULL == hMapFront->put(vKey, ohValue, cMillis) &&
                        listen_present == strategyTarget)
                    {
                    // this entry was evicted from behind us, and thus
                    // we haven't been listening to its events for
                    // some time, so we may not have the current value
                    hMapFront->remove(vKey);
                    }
                }
            else
                {
                invalidateFront(vKey);
                }

            // remove event list from the control map; in this case
            // it must be done while still under sycnhronization
            hMapControl->remove(vKey);
            }
        }
    }

Object::Holder CachingMap::put(Object::View vKey, Object::Holder ohValue,
        bool fReturn, int64_t cMillis)
    {
    int64_t                   ldtStart        = System::safeTimeMillis();
    CacheMap::Handle          hMapFront       = getFrontMap();
    CacheMap::Handle          hMapBack        = getBackMap();
    InvalidationStrategy      strategyTarget  = m_strategyTarget;
    InvalidationStrategy      strategyCurrent = m_strategyCurrent;
    ConcurrentMap::Handle     hMapControl     = getControlMap();

    hMapControl->lock(vKey, -1);

    // finally construct
    struct Finally
        {
        Finally(CachingMap::View _vThis, Object::View _vKey)
            : vThis(_vThis), vKey(_vKey)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle hMapControl = vThis->getControlMap();
            hMapControl->unlock(vKey);
            }

        CachingMap::View vThis;
        Object::View     vKey;
        } finally(this, vKey);

    List::Handle hlistEvents = NULL;

    // obtain current front value; if the new value is NULL then
    // remove from the front map since we will ignore any changes
    Object::Holder hFront = NULL == ohValue ? hMapFront->remove(vKey) :
                   hMapFront->get(vKey);

    if (strategyTarget != listen_none)
        {
        // NOTE: put() will not register any new key-based listeners;
        // per-key registering for new entries would double the number
        // of synchronous network operations; instead we defer the
        // registration until the first get; we are assuming that
        // "get(a), put(a)", or "put(a), put(b)" are more likely
        // sequences then "put(a), get(a)"

        if (NULL == ohValue)
            {
            // we won't cache NULL values, so no need to listen
            hMapControl->put(vKey, hlistEvents = getIgnoreList());
            if (hFront != NULL)
                {
                // the value was previously in the front, cleanup
                unregisterListener(vKey);
                }
            }
        else if (hFront != NULL ||
                 listen_all     == strategyCurrent ||
                 listen_logical == strategyCurrent)
            {
            // we are already registered for events covering this key
            // when back map operations returns we may choose to cache
            // the new [non-NULL] value into the front map.  This is
            // cheap since we already have a listener (global or key)
            // registered for this entry
            hMapControl->put(vKey, hlistEvents = LinkedList::create());
            }
        else
            {
            // we are not registered for events covering this key
            // we will ignore any changes; this allows us to avoid the
            // cost of registering a listener and/or generating a
            // questionably usefull LinkedList allocation which could
            // become tenured
            hMapControl->put(vKey, hlistEvents = getIgnoreList());
            }
        }

    Object::Holder hOrig;
    try
        {
        // the back map calls could be network calls
        // generating events on a service thread
        if (cMillis > 0 || fReturn)
            {
            // normal put with return value
            hOrig = hMapBack->put(vKey, ohValue, cMillis);
            }
        else
            {
            // optimize out the return value
            hMapBack->putAll(Collections::singletonMap(vKey, ohValue));
            hOrig = NULL;
            }
        }
    catch (RuntimeException::View vEx)
        {
        // we don't know the state of the back; cleanup and invalidate
        // this key on the front
        hMapControl->remove(vKey);
        try
            {
            invalidateFront(vKey);
            }
        catch (RuntimeException::View)
            {
            // ignore.
            }
        COH_THROW (vEx);
        }

    // cleanup, and update the front if possible
    finalizePut(vKey, ohValue, hlistEvents, cMillis);

    f_hStats->registerPut(ldtStart);
    return hOrig;
    }

Map::View CachingMap::getAll(Collection::View vColKeys) const
    {
    int64_t  ldtStart = System::safeTimeMillis();

    // Step 1: retrieve all we can from the front map first
    Map::View vMapResultFront = f_hMapFront->getAll(vColKeys);
    if (!vMapResultFront->isEmpty())
        {
        f_hStats->registerHits(vMapResultFront->size(), ldtStart);
        }

    if (vMapResultFront->size() == vColKeys->size())
        {
        // all keys found in front
        return vMapResultFront;
        }

    Set::Handle hSetFrontMiss = HashSet::create(vColKeys);
    hSetFrontMiss->removeAll(vMapResultFront->keySet());

    Map::Handle hMapResult = HashMap::create(vColKeys->size());
    hMapResult->putAll(vMapResultFront);

    // Step 2: Lock the missing keys without blocking
    CacheMap::Handle      hMapFront   = getFrontMap();
    ConcurrentMap::Handle hMapControl = getControlMap();
    InvalidationStrategy  strategy    = ensureInvalidationStrategy();
    size32_t              cMisses     = hSetFrontMiss->size();
    Set::Handle           hSetLocked  = HashSet::create(cMisses);

    List::Handle          hlistEvents = listen_none == strategy ?
            (List::Handle) NULL : (List::Handle)LinkedList::create();

        {
        // finally construct
        struct Finally
            {
            Finally(CachingMap::View _vThis, Set::Handle _hSetLocked)
                : vThis(_vThis), hSetLocked(_hSetLocked)
                {
                }

            ~Finally()
                {
                ConcurrentMap::Handle hMapControl = vThis->getControlMap();
                for (Iterator::Handle hIter = hSetLocked->iterator();
                        hIter->hasNext();)
                    {
                    hMapControl->unlock(hIter->next());
                    }
                }

            CachingMap::View vThis;
            Set::Handle      hSetLocked;
            } finally(this, hSetLocked);

        for (Muterator::Handle hIter = hSetFrontMiss->iterator();
                hIter->hasNext();)
            {
            Object::View vKey = hIter->next();
            if (hMapControl->lock(vKey, 0L))
                {
                hSetLocked->add(vKey);
                if (strategy != listen_none)
                    {
                    hMapControl->put(vKey, hlistEvents);
                    }
                }
            }

        if (strategy == listen_present)
            {
            // Step 3: Register listeners and try to get the values
            // through priming events
            registerListeners(hSetLocked);

            COH_SYNCHRONIZED(hlistEvents)
                {
                for (int i = hlistEvents->size() - 1; i >= 0; --i)
                    {
                    MapEvent::View vEvt = cast<MapEvent::View>(hlistEvents->get(i));

                    if (isPriming(vEvt))
                        {
                        Object::View vKey = vEvt->getKey();

                        hMapResult->put(vKey, vEvt->getNewValue());
                        hSetFrontMiss->remove(vKey);
                        hlistEvents->remove(i);
                        }
                    }
                }
            }

        // Step 4: do a bulk getAll() for all the front misses
        //         that were not "primed"
        if (!hSetFrontMiss->isEmpty())
            {
            try
                {
                // COH-4447: materialize the converted results to avoid
                //           unnecessary repeated deserialization
                hMapResult->putAll(HashMap::create(getBackMap()->getAll(hSetFrontMiss)));
                }
            catch (RuntimeException::View vEx)
                {
                if (listen_none != strategy)
                    {
                    for (Iterator::Handle hIter = hSetLocked->iterator(); hIter->hasNext(); )
                        {
                        Object::View vKey = hIter->next();
                        if (strategy == listen_present)
                            {
                             unregisterListener(vKey);
                            }
                        hMapControl->remove(vKey);
                        }
                    }
                throw;
                }
            }

        // Step 5: for the locked keys move the retrieved values to the front
        if (listen_none == strategy)
            {
            for (Iterator::Handle hIter = hSetLocked->iterator(); hIter->hasNext(); )
                {
                Object::View   vKey    = hIter->next();
                Object::Holder ohValue = hMapResult->get(vKey);

                if (ohValue != NULL)
                    {
                    hMapFront->put(vKey, ohValue);
                    }
                }
            }
        else
            {
            Set::Handle hSetInvalid = HashSet::create();
            Set::Handle hSetAdd     = HashSet::create(hSetLocked);

            // remove entries invalidated during the getAll() call
            COH_SYNCHRONIZED (hlistEvents)
                {
                // getAll() operation itself can generate not more
                // than one synthetic INSERT per key; anything else
                // should be considered as an invalidating event
                // (see additional comment at "get" processing)
                for (Iterator::Handle hIter = hlistEvents->iterator();
                        hIter->hasNext();)
                    {
                    MapEvent::View vEvt = cast<MapEvent::View>(hIter->next());
                    Object::View   vKey = vEvt->getKey();

                    // always start with removing the key from the
                    // result set, so a second event is always
                    // treated as an invalidatation
                    bool fValid = hSetAdd->remove(vKey)
                            && MapEvent::entry_inserted == vEvt->getId()
                            && instanceof<CacheEvent::View>(vEvt)
                            && (cast<CacheEvent::View>(vEvt))->isSynthetic();

                    if (!fValid)
                        {
                        hSetInvalid->add(vKey);
                        m_cInvalidationHits = m_cInvalidationHits + 1;
                        }
                    }

                // Adding to the front cache could cause a large number
                // of evictions. Instead of unregistering the listeners
                // individually, try to collect them for a bulk unregistration.
                Set::Handle hSetUnregister = setKeyHolder();

                    {
                    // finally construct
                    struct UnregisterFinally
                        {
                        UnregisterFinally(CachingMap::View _vThis, Set::Handle _hSetUnregister)
                            : vThis(_vThis), hSetUnregister(_hSetUnregister)
                            {
                            }

                        ~UnregisterFinally()
                            {
                            if (NULL != hSetUnregister)
                                {
                                vThis->unregisterListeners(hSetUnregister);
                                vThis->removeKeyHolder();
                                }
                            }

                        CachingMap::View vThis;
                        Set::Handle      hSetUnregister;
                        } unregisterFinally(this, hSetUnregister);

                    for (Iterator::Handle hIter = hSetLocked->iterator();
                        hIter->hasNext();)
                        {
                        Object::View   vKey    = hIter->next();
                        Object::Holder ohValue = hMapResult->get(vKey);
                        if (ohValue != NULL && !hSetInvalid->contains(vKey))
                            {
                            hMapFront->put(vKey, ohValue);
                            }
                        else // null or invalid
                            {
                            if (ohValue == NULL)
                                {
                                hMapResult->remove(vKey);
                                }
                            hMapFront->remove(vKey);
                            unregisterListener(vKey);
                            }
                        // remove must occur under sync (if we're caching) otherwise we risk losing events
                        hMapControl->remove(vKey);
                        }
                    }
                }
            }

        f_hStats->registerMisses(cMisses, ldtStart);
        return hMapResult;
        }
    }

// ----- Map interface ------------------------------------------------------

size32_t CachingMap::size() const
    {
    return getBackMap()->size();
    }

bool CachingMap::isEmpty() const
    {
    return getBackMap()->isEmpty();
    }

bool CachingMap::containsKey(Object::View vKey) const
    {
    if (f_hMapFront->containsKey(vKey))
        {
        f_hStats->registerHit();
        return true;
        }

    Map::Handle           hMapFront   = getFrontMap();
    ConcurrentMap::Handle hMapControl = getControlMap();
    hMapControl->lock(vKey, -1);

    // finally construct
    struct Finally
        {
        Finally(CachingMap::View _vThis, Object::View _vKey)
            : vThis(_vThis), vKey(_vKey)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle mapControl = vThis->getControlMap();
            mapControl->remove(vKey);
            mapControl->unlock(vKey);
            }

        CachingMap::View vThis;
        Object::View     vKey;
        } finally(this, vKey);

    if (hMapFront->containsKey(vKey))
        {
        f_hStats->registerHit();
        return true;
        }

    hMapControl->put(vKey, getIgnoreList());
    f_hStats->registerMiss();
    return getBackMap()->containsKey(vKey);
    }

bool CachingMap::containsValue(Object::View vValue) const
    {
    return getFrontMap()->containsValue(vValue)
        || getBackMap()->containsValue(vValue);
    }

Object::Holder CachingMap::get(Object::View vKey) const
    {
    // avoid attaching to FrontMap on cache hit
    Object::Holder ohValue = f_hMapFront->get(vKey);
    if (NULL != ohValue)
        {
        f_hStats->registerHit(); // avoid calculating time for hit
        return ohValue;
        }

    int64_t               ldtStart    = System::safeTimeMillis();
    Map::Handle           hMapFront   = getFrontMap();
    ConcurrentMap::Handle hMapControl = getControlMap();
    // TODO: use CACHE_LOCK to simulate try/finally
    hMapControl->lock(vKey, -1);

    // finally construct
    struct Finally
        {
        Finally(CachingMap::View _vThis, Object::View _vKey)
            : vThis(_vThis), vKey(_vKey)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle mapControl = vThis->getControlMap();
            mapControl->unlock(vKey);
            }

        CachingMap::View vThis;
        Object::View     vKey;
        } finally(this, vKey);

    ohValue = hMapFront->get(vKey);
    if (ohValue != NULL)
        {
        f_hStats->registerHit(ldtStart);
        return ohValue;
        }

    Map::Handle mapBack = getBackMap();

    if (listen_none == m_strategyTarget)
        {
        ohValue = mapBack->get(vKey);
        if (ohValue != NULL)
            {
            hMapFront->put(vKey, ohValue);
            }
        }
    else
        {
        List::Handle listEvents = LinkedList::create();
        hMapControl->put(vKey, listEvents);

        registerListener(vKey);

        bool fPrimed;
        COH_SYNCHRONIZED(listEvents)
            {
            int c;
            switch (c = listEvents->size())
                {
                case 0:
                    fPrimed = false;
                    break;

                default:
                    // check if the last event is a "priming" one
                    MapEvent::View vEvt = cast<MapEvent::View>
                        (listEvents->get(c-1));
                    if ((fPrimed = isPriming(vEvt)))
                        {
                        ohValue = vEvt->getNewValue();
                        listEvents->remove(c-1);
                        }
                    break;
                }
            }

        if (!fPrimed)
            {
            try
                {
                // this call could be a network call
                // generating events on a service thread
                ohValue = mapBack->get(vKey);
                }
            catch (RuntimeException::View vEx)
                {
                unregisterListener(vKey);
                hMapControl->remove(vKey);
                throw;
                }
            }

        COH_SYNCHRONIZED(listEvents)
            {
            if (NULL == ohValue)
                {
                // we don't cache NULL values
                unregisterListener(vKey);
                }
            else
                {
                // get operation itself can generate only
                // a synthetic INSERT; anything else should be
                // considered as an invalidating event
                bool fValid = true;
                switch (listEvents->size())
                    {
                    case 0:
                        break;

                    case 1:
                        {
                        // it's theoretically possible (though very
                        // unlikely) that another thread caused the
                        // entry expiration, reload and the synthetic
                        // insert all while this request had already
                        // been supplied with a value;
                        // we'll take our chance here to provide greater
                        // effectiveness for the more probable situation
                        MapEvent::View vEvt = cast<MapEvent::View>
                                (listEvents->get(0));

                        CacheEvent::View vCacheEvent = cast<CacheEvent::View>
                                (vEvt, false);

                        fValid = vCacheEvent != NULL &&
                                MapEvent::entry_inserted == vEvt->getId() &&
                                vCacheEvent->isSynthetic();
                        break;
                        }
                    default:
                        fValid = false;
                        break;
                    }

                if (fValid)
                    {
                    // Adding to the front cache could cause a large number
                    // of evictions. Instead of unregistering the listeners
                    // individually, try to collect them for a bulk unregistration.
                    Set::Handle hSetUnregister = setKeyHolder();

                    // finally construct
                    struct UnregisterFinally
                        {
                        UnregisterFinally(CachingMap::View _vThis, Set::Handle _hSetUnregister)
                            : vThis(_vThis), hSetUnregister(_hSetUnregister)
                            {
                            }

                        ~UnregisterFinally()
                            {
                            if (NULL != hSetUnregister)
                                {
                                vThis->unregisterListeners(hSetUnregister);
                                vThis->removeKeyHolder();
                                }
                            }

                        CachingMap::View vThis;
                        Set::Handle      hSetUnregister;
                        } unregisterFinally(this, hSetUnregister);

                    hMapFront->put(vKey, ohValue);
                    }
                else
                    {
                    unregisterListener(vKey);
                    m_cInvalidationHits = m_cInvalidationHits + 1;
                    }
                }
            // remove must occur under sync (if we're caching) otherwise we risk losing events
            hMapControl->remove(vKey);
            }
        }

    // update miss statistics
    f_hStats->registerMiss(ldtStart);
    return ohValue;
    }

Object::Holder CachingMap::get(Object::View vKey)
    {
    return ((const CachingMap*) this)->get(vKey);
    }

Object::Holder CachingMap::put(Object::View vKey, Object::Holder ohValue)
    {
    return put(vKey, ohValue, true, 0L);
    }

Object::Holder CachingMap::remove(Object::View vKey)
    {
    Map::Handle           hMapFront   = getFrontMap();
    Map::Handle           hMapBack    = getBackMap();
    InvalidationStrategy  strategy    = m_strategyTarget;
    ConcurrentMap::Handle hMapControl = getControlMap();

    hMapControl->lock(vKey, -1);

    // finally construct
    struct Finally
        {
        Finally(CachingMap::View _vThis, Object::View _vKey)
            : vThis(_vThis), vKey(_vKey)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle hMapControl = vThis->getControlMap();
            InvalidationStrategy  strategy    = vThis->getInvalidationStrategy();
            if (strategy != listen_none)
                {
                hMapControl->remove(vKey);
                }
            hMapControl->unlock(vKey);
            }

        CachingMap::View vThis;
        Object::View     vKey;
        } finally(this, vKey);

    if (strategy != listen_none)
        {
        hMapControl->put(vKey, getIgnoreList());
        }

    if (hMapFront->remove(vKey) != NULL)
        {
        unregisterListener(vKey);
        }

    return hMapBack->remove(vKey);
    }

void CachingMap::putAll(Map::View vMap)
    {
    // optimize for caller doing a single blind put
    if (1 == vMap->size())
        {
        Iterator::Handle hIter = vMap->entrySet()->iterator();
        if (hIter->hasNext())
            {
            Object::View     vNext  = hIter->next();
            Map::Entry::View vEntry = cast<Map::Entry::View>(vNext);
            put(vEntry->getKey(), vEntry->getValue(), false, 0L);
            }
        return;
        }

    int32_t               strategyTarget  = m_strategyTarget;
    int32_t               strategyCurrent = m_strategyCurrent;
    bool                  fAllRegistered  = strategyCurrent == listen_all ||
                                            strategyCurrent == listen_logical;
    int64_t               ldtStart        = System::safeTimeMillis();
    ConcurrentMap::Handle hMapControl     = getControlMap();
    Map::Handle           hMapFront       = getFrontMap();
    Map::Handle           hMapBack        = getBackMap();
    Map::Handle           hMapLocked      = HashMap::create();
    List::Handle          hlistUnlockable = LinkedList::create();

    // finally construct
    struct Finally
        {
        Finally(CachingMap::View _vThis, Map::Handle _hMapLocked,
                Map::Handle _hMapFront, List::Handle _hlistUnlockable)
                : vThis(_vThis),
                  hMapLocked(_hMapLocked),
                  hMapFront(_hMapFront),
                  hlistUnlockable(_hlistUnlockable)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle hMapControl    = vThis->getControlMap();
            InvalidationStrategy  strategyTarget = vThis->getInvalidationStrategy();

            // invalidate and unlock anything which remains locked
            for (Iterator::Handle hIter = hMapLocked->keySet()->iterator();
                 hIter->hasNext();)
                {
                Object::View vKey = hIter->next();
                try
                    {
                    vThis->invalidateFront(vKey);
                    }
                catch (RuntimeException::View)
                    {
                    // ignore.
                    }

                hMapControl->remove(vKey);
                hMapControl->unlock(vKey);
                }

            // invalidate unlockable keys as needed
            if (hlistUnlockable != NULL && listen_none == strategyTarget)
                {
                // not using events, do it manually
                hMapFront->keySet()->removeAll(hlistUnlockable);
                }
            }

        CachingMap::View vThis;
        Map::Handle      hMapLocked;
        Map::Handle      hMapFront;
        List::Handle     hlistUnlockable;
        } finally(this, hMapLocked, hMapFront, hlistUnlockable);

    // lock keys where possible
    for (Iterator::Handle hIter = vMap->entrySet()->iterator();
            hIter->hasNext();)
        {
        Map::Entry::View vEntry  = cast<Map::Entry::View>(hIter->next());
        Object::View     vKey    = vEntry->getKey();
        Object::Holder   ohValue = vEntry->getValue();

        if (ohValue != NULL && hMapControl->lock(vKey, 0))
            {
            hMapLocked->put(vKey, ohValue);

            if (strategyTarget != listen_none)
                {
                // we only track keys which have registered listeners
                // thus avoiding the synchronous network call for event
                // registration
                hMapControl->put(vKey,
                        fAllRegistered || hMapFront->containsKey(vKey) ?
                        (List::Handle)LinkedList::create() :
                        getIgnoreList());
                }
            }
        else
            {
            // for NULL values or unlockable keys we will just push
            // the entry to the back, any required cleanup will occur
            // automatically during event validation or manually for
            // listen_none
            hlistUnlockable->add(vKey);
            }
        }

    // update the back with all entries
    hMapBack->putAll(vMap);

    // update front with locked keys where possible
    if (listen_none == strategyTarget)
        {
        // no event based cleanup to do, simply update the front
        hMapFront->putAll(hMapLocked);
        for (Muterator::Handle hIter = hMapLocked->keySet()->iterator();
             hIter->hasNext();)
            {
            hMapControl->unlock(hIter->next());
            hIter->remove();
            }
        // unlockable key cleanup in finally
        }
    else
        {
        // conditionally update locked keys based on event results
        for (Muterator::Handle hIter = hMapLocked->entrySet()->iterator();
             hIter->hasNext();)
            {
            Map::Entry::View vEntry  = cast<Map::Entry::View>(hIter->next());
            Object::View     vKey    = vEntry->getKey();
            List::Handle     hlist   = cast<List::Handle>
                    (hMapControl->get(vKey));
            finalizePut(vKey, vEntry->getValue(), hlist, 0);
            hMapControl->unlock(vKey);
            hIter->remove();
            }
        }

    f_hStats->registerPuts(vMap->size(), ldtStart);
    }

void CachingMap::clear()
    {
    ConcurrentMap::Handle hMapControl = getControlMap();

    // Note: we cannot do a blocking LOCK_ALL as any event which came
    // in while the ThreadGate is in the closing state would cause the
    // service thread to spin.  Try for up ~1s before giving up and
    // issue the operation against the back, allowing events to perform
    // the cleanup.  We don't even risk a timed LOCK_ALL as whatever
    // time value we choose would risk a useless spin for that duration
    for (size32_t i = 0; !hMapControl->lock(ConcurrentMap::getLockAll(), 0);
            ++i)
        {
        if (i == 100)
            {
            getBackMap()->clear();
            if (listen_none == m_strategyTarget)
                {
                getFrontMap()->clear();
                }
            return;
            }
        try
            {
            Thread::sleep(10);
            }
        catch (InterruptedException::View vEx)
            {
            Thread::currentThread()->interrupt();
            COH_THROW (vEx);
            }
        }

    // finally construct
    struct Finally
        {
        Finally(CachingMap::Handle _hThis)
            : hThis(_hThis)
            {
            }

        ~Finally()
            {
            ConcurrentMap::Handle mapControl = hThis->getControlMap();
            mapControl->remove(hThis->f_vKeyGlobal);
            mapControl->unlock(ConcurrentMap::getLockAll());
            }

        CachingMap::Handle hThis;
        } finally(this);

    hMapControl->put(f_vKeyGlobal, getIgnoreList());

    Map::Handle hMapFront = getFrontMap();
    Map::Handle hMapBack  = getBackMap();

    switch (m_strategyCurrent)
        {
        case listen_present:
            unregisterFrontListener();
            try
                {
                for (Muterator::Handle hIter =
                        hMapFront->keySet()->iterator(); hIter->hasNext();)
                    {
                    unregisterListener(hIter->next());
                    hIter->remove();
                    }
                }
            catch (RuntimeException::View vEx)
                {
                // we're not going to reset the invalidation strategy
                // so we must keep the front listener around
                registerFrontListener();
                COH_THROW (vEx);
                }
            break;

        case listen_logical:
        case listen_all:
            unregisterListener();
            try
                {
                hMapFront->clear();
                }
            catch (RuntimeException::View vEx)
                {
                // we're not going to reset the invalidation strategy
                // so we must keep the front listener around
                registerFrontListener();
                COH_THROW (vEx);
                }
            break;

        default:
            hMapFront->clear();
            break;
        }
    resetInvalidationStrategy();
    hMapBack->clear();
    }

Set::View CachingMap::keySet() const
    {
    return getBackMap()->keySet();
    }

Set::Handle CachingMap::keySet()
    {
    Set::Handle hSet = getBackMap()->keySet();
    if (!isCoherent())
        {
        hSet = Collections::unmodifiableSet(hSet);
        }
    return hSet;
    }

Collection::View CachingMap::values() const
    {
    return getBackMap()->values();
    }

Collection::Handle CachingMap::values()
    {
    Collection::Handle hValues = getBackMap()->values();
    if (!isCoherent())
        {
        hValues = Collections::unmodifiableCollection(hValues);
        }
    return hValues;
    }

Set::View CachingMap::entrySet() const
    {
    return getBackMap()->entrySet();
    }

Set::Handle CachingMap::entrySet()
    {
    Set::Handle hSet = getBackMap()->entrySet();
    if (!isCoherent())
        {
        hSet = Collections::unmodifiableSet(hSet);
        }
    return hSet;
    }


// ----- Object interface ---------------------------------------------------

static const char*  COH_STRATEGY_TEXT[] =
    {
    "listen_none", "listen_present", "listen_all", "listen_auto", "listen_logical"
    };

TypedHandle<const String> CachingMap::toString() const
    {
    try
        {
        Map::Handle hMapFront = getFrontMap();
        Map::Handle hMapBack  = getBackMap();

        return COH_TO_STRING("CachingMap{FrontMap{class="
                << Class::getClassName(hMapFront)
                << ", size="
                << hMapFront->size()
                << "}, BackMap{class="
                << Class::getClassName(hMapBack)
                << ", size="
                << hMapBack->size()
                << "}, strategy="
                << COH_STRATEGY_TEXT[getInvalidationStrategy()]
                << ", CacheStatistics="
                << getCacheStatistics()
                << ", invalidation hits="
                << getInvalidationHits()
                << ", invalidation misses="
                << getInvalidationMisses()
                << ", listener registrations="
                << getTotalRegisterListener()
                << '}');
        }
    catch (IllegalStateException::View)
        {
        return COH_TO_STRING("CachingMap not active");
        }
    }

void CachingMap::onInit()
    {
    super::onInit();
    if (m_strategyTarget != listen_none)
        {
        if (instanceof<ObservableMap::Handle>(f_hMapBack))
            {
            m_hListener = instantiateBackMapListener(m_strategyTarget);
            // workaround to allow changing the map listener when connecting to a older proxy.
            // We cannot instantiate the listener in the registerListener const method.
            m_hSimpleListener = instantiateBackMapListener(listen_all);

            m_hListenerDeactivation = DeactivationListener::create(this);

            if (instanceof<ObservableMap::Handle>(f_hMapFront))
                {
                initialize(f_hListenerFront, instantiateFrontMapListener());
                }
            }
        else
            {
            m_strategyTarget = listen_none;
            }
        }
    }


// ----- back map listener support ------------------------------------------

void CachingMap::registerListener() const
    {
    cast<ObservableMap::Handle>(getBackMap())->
        addFilterListener(m_hListener, m_hFilterListener, true);
    }

void CachingMap::unregisterListener() const
    {
    cast<ObservableMap::Handle>(getBackMap())->
        removeFilterListener(m_hListener, m_hFilterListener);
    }

void CachingMap::registerListener(Object::View voKey) const
    {
    if (listen_present == ensureInvalidationStrategy())
        {
        try
            {
            cast<ObservableMap::Handle>(getBackMap())->addKeyListener(m_hListener, voKey, true);
            }
        catch (UnsupportedOperationException::View e)
            {
            // the back is of an older version; need to reset the
            // "old" non-priming listener
            m_hListener = m_hSimpleListener;
            cast<ObservableMap::Handle>(getBackMap())->addKeyListener(m_hListener, voKey, true);
            }

        m_cRegisterListener = m_cRegisterListener + 1;
        }
    }

void CachingMap::registerListeners(Set::Handle hSetKeys) const
    {
    if (listen_present == ensureInvalidationStrategy())
        {
        if (instanceof<PrimingListener::Handle>(m_hListener))
            {
            try
                {
                cast<ObservableMap::Handle>(getBackMap())->addFilterListener(m_hListener,
                        InKeySetFilter::create((Filter::Handle) NULL, hSetKeys), true);
                m_cRegisterListener = m_cRegisterListener + hSetKeys->size();
                return;
                }
            catch (UnsupportedOperationException::View e)
                {
                // the back is of an older version; need to reset the
                // "old" non-priming listener
                m_hListener = m_hSimpleListener;
                }
            }

        // use non-optimized legacy algorithm
        for (Muterator::Handle hIter = hSetKeys->iterator();
                 hIter->hasNext();)
            {
            registerListener(hIter->next());
            }
        }
    }

void CachingMap::unregisterListener(Object::View vKey) const
    {
    if (listen_present == m_strategyCurrent)
        {
        ConcurrentMap::Handle hMapControl = getControlMap();
        if (hMapControl->lock(vKey, 0))
            {
            if (instanceof<PrimingListener::Handle>(m_hListener))
                {
                Set::Handle hSetKeys = cast<Set::Handle>(f_htloKeys->get());
                if (NULL != hSetKeys)
                    {
                    hSetKeys->add(vKey);

                    // the key is still locked; it will be unlocked
                    // along with other keys after bulk un-registration
                    // in the unregisterListeners(setKeys) method
                    return;
                    }
                }

            // finally construct
            struct Finally
                {
                Finally(CachingMap::View _vThis, Object::View _vKey)
                    : vThis(_vThis), vKey(_vKey)
                    {
                    }

                ~Finally()
                    {
                    ConcurrentMap::Handle mapControl = vThis->getControlMap();
                    mapControl->unlock(vKey);
                    }

                CachingMap::View vThis;
                Object::View     vKey;
                } finally(this, vKey);

            cast<ObservableMap::Handle>(getBackMap())->removeKeyListener(m_hListener, vKey);
            }
        }
    }

void CachingMap::unregisterListeners(Set::Handle hSetKeys) const
    {
    if (m_strategyCurrent == listen_present
        && instanceof<PrimingListener::Handle>(m_hListener))
        {
        if (!hSetKeys->isEmpty())
            {
            // finally construct
            struct Finally
                {
                Finally(CachingMap::View _vThis, Set::Handle _hSetKeys)
                : vThis(_vThis), hSetKeys(_hSetKeys)
                    {
                    }

                ~Finally()
                    {
                    ConcurrentMap::Handle mapControl = vThis->getControlMap();
                    for (Iterator::Handle hIter = hSetKeys->iterator();
                         hIter->hasNext();)
                        {
                        mapControl->unlock(hIter->next());
                        }
                    }

                CachingMap::View vThis;
                Set::Handle      hSetKeys;
                } finally(this, hSetKeys);

            cast<ObservableMap::Handle>(getBackMap())->removeFilterListener(m_hListener,
                InKeySetFilter::create((Filter::Handle) NULL, hSetKeys));
            }
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create("unregisterListeners can only be called with PRESENT strategy"));
        }
    }

Set::Handle CachingMap::setKeyHolder() const
    {
    if (m_strategyCurrent == listen_present
        && instanceof<PrimingListener::Handle>(m_hListener))
        {
        Set::Handle hSetKeys = HashSet::create();
        f_htloKeys->set(hSetKeys);
        return hSetKeys;
        }

    return NULL;
    }

void CachingMap::removeKeyHolder() const
    {
    f_htloKeys->set(NULL);
    }

void CachingMap::registerFrontListener() const
    {
    FrontMapListener::Handle hListener = f_hListenerFront;
    if (hListener != NULL)
        {
        hListener->registerWithMap();
        }
    }

void CachingMap::unregisterFrontListener() const
    {
    FrontMapListener::Handle hListener = f_hListenerFront;
    if (hListener != NULL)
        {
        hListener->unregisterFromMap();
        }
    }

CachingMap::InvalidationStrategy
        CachingMap::ensureInvalidationStrategy() const
    {
    // the situation in which (m_strategyCurrent != m_strategyTarget)
    // can happen either at the first map access following the
    // instantiation or after resetInvalidationStrategy() is called

    InvalidationStrategy strategyTarget = m_strategyTarget;
    switch (strategyTarget)
        {
        case listen_auto:
            // as of Coherence 12.1.2, default LISTEN_AUTO to LISTEN_PRESENT
        case listen_present:
            if (m_strategyCurrent != listen_present)
                {
                COH_SYNCHRONIZED(f_vKeyGlobal)
                    {
                    if (m_strategyCurrent != listen_present)
                        {
                        registerFrontListener();
                        registerDeactivationListener();

                        m_strategyCurrent = listen_present;
                        }
                    }
                }
            return listen_present;

        case listen_logical:
        case listen_all:
            if (m_strategyCurrent != strategyTarget)
                {
                COH_SYNCHRONIZED(f_vKeyGlobal)
                    {
                    if (m_strategyCurrent != strategyTarget)
                        {
                        if (strategyTarget == listen_logical)
                            {
                            // LOGICAL behaves like ALL, but with synthetic deletes filtered out
                            m_hFilterListener = NotFilter::create(
                                    CacheEventFilter::create(CacheEventFilter::e_deleted,
                                                             CacheEventFilter::e_synthetic));
                            }
                        registerListener();
                        registerDeactivationListener();

                        m_strategyCurrent = strategyTarget;
                        }
                    }
                }
            return strategyTarget;

        case listen_none:
            break;
        }
    return listen_none;
    }

void CachingMap::resetInvalidationStrategy()
    {
    m_strategyCurrent = listen_none;
    m_hFilterListener = NULL;
    }

MapListener::Handle CachingMap::instantiateBackMapListener(InvalidationStrategy strategy)
    {
    return strategy == listen_auto || strategy == listen_present
        ? (MapListener::Handle) PrimingListener::create(this)
        : (MapListener::Handle) SimpleListener::create(this);
    }

CachingMap::FrontMapListener::Handle CachingMap::instantiateFrontMapListener()
    {
    return FrontMapListener::create(this);
    }

void CachingMap::validate(MapEvent::View vEvent)
    {
    ConcurrentMap::Handle hMapControl = getControlMap();
    Object::View          vKey        = vEvent->getKey();
    int64_t               ldtStart    = 0L;

    for (int32_t i = 0; true; ++i)
        {
        if (hMapControl->lock(vKey, 0))
            {
            // finally construct
            struct Finally
                {
                Finally(CachingMap::View _vThis, Object::View _vKey)
                    : vThis(_vThis), vKey(_vKey)
                    {
                    }

                ~Finally()
                    {
                    ConcurrentMap::Handle mapControl = vThis->getControlMap();
                    mapControl->unlock(vKey);
                    }

                CachingMap::View vThis;
                Object::View     vKey;
                } finally(this, vKey);

            List::Handle hlistEvents =
                    cast<List::Handle>(hMapControl->get(vKey));
            if (NULL == hlistEvents)
                {
                if (!isPriming(vEvent))
                    {
                    // not in use; invalidate front entry
                    invalidateFront(vKey);
                    }
                }
            else
                {
                // this can only happen if the back map fires event on
                // the caller's thread (e.g. LocalCache)
                hlistEvents->add(vEvent);
                }
            return;
            }
        else
            {
            // check for a key based action
            List::Handle hlistEvents =
                    cast<List::Handle>(hMapControl->get(vKey));

            if (NULL == hlistEvents)
                {
                // check for a global action
                hlistEvents = cast<List::Handle>
                        (hMapControl->get(f_vKeyGlobal));
                if (NULL == hlistEvents)
                    {
                    // has not been assigned yet, or has been just
                    // removed or switched; try again
                    Thread::yield();
                    int64_t ldtNow = System::safeTimeMillis();
                    if (ldtStart == 0)
                        {
                        ldtStart = ldtNow;
                        }
                    else if (i > 5000 && ldtNow - ldtStart > 5000)
                        {
                        // we've been spinning and have given the other
                        // thread ample time to register the event list;
                        // the control map is corrupt
                        COH_LOG ("Detected a state corruption on the key \""
                            << vKey << "\", of class "
                            << Class::getClassName(vKey)
                            << " which is missing from the active key set "
                            << hMapControl->keySet()
                            << ". This could be caused by a mutating or "
                            << "inconsistent key implementation, or a "
                            << "concurrent modification to the map passed to "
                            << Class::getClassName(this) << ".putAll()",
                            Logger::level_error);

                        invalidateFront(vKey);
                        return;
                        }
                    continue;
                    }
                }

            COH_SYNCHRONIZED(hlistEvents)
                {
                List::Handle hlistKey = cast<List::Handle>
                        (hMapControl->get(vKey));
                if (hlistEvents == hlistKey || (NULL == hlistKey &&
                    hlistEvents == hMapControl->get(f_vKeyGlobal)))
                    {
                    hlistEvents->add(vEvent);
                    return;
                    }
                }
            }
        }
    }

bool CachingMap::isPriming(MapEvent::View vEvent)
    {
    return MapEvent::entry_updated == vEvent->getId()
            && instanceof<CacheEvent::View>(vEvent)
            && ((cast<CacheEvent::View>(vEvent))->isPriming() || (cast<CacheEvent::View>(vEvent))->isSynthetic());
    }

// ----- inner class: FrontMapListener --------------------------------------

// ----- constructors ---------------------------------------------------

CachingMap::FrontMapListener::FrontMapListener(CachingMap::Handle hMap)
    : f_vFilter(self(), MapEventFilter::create(MapEventFilter::e_deleted)),
      m_whMap(self(), hMap)
    {
    }

// ----- MapListener interface ------------------------------------------

void CachingMap::FrontMapListener::entryDeleted(
        MapEvent::View vEvent)
    {
    // explicitly removed or evicted from map,
    // unregister listener on back map

    CacheEvent::View vCacheEvent = cast<CacheEvent::View>(vEvent, false);
    if (NULL == vCacheEvent)
        {
        if (vCacheEvent->isSynthetic())
            {
            m_whMap->unregisterListener(vEvent->getKey());
            }
        }
    else
        {
        // plain MapEvent, can't tell if it was synthetic
        // or not but it is always appropriate to unregister
        // if we've removed something from the front. This
        // may be a double unregistration as the caller may
        // have already explicity unregistered
        m_whMap->unregisterListener(vEvent->getKey());
        }
    }

// ---- helper registration methods -------------------------------------

void CachingMap::FrontMapListener::registerWithMap()
    {
    ObservableMap::Handle hMap = cast<ObservableMap::Handle>
            (m_whMap->getFrontMap());

    hMap->addFilterListener(this, f_vFilter, true);
    }

void CachingMap::FrontMapListener::unregisterFromMap()
    {
    ObservableMap::Handle hMap = cast<ObservableMap::Handle>
            (m_whMap->getFrontMap());

    hMap->removeFilterListener(this, f_vFilter);
    }

void CachingMap::registerDeactivationListener() const
    {
    try
        {
        NamedCacheDeactivationListener::Handle hListener = m_hListenerDeactivation;
        if (hListener != NULL)
            {
            CacheMap::Handle hCacheMap = getBackMap();
            if(instanceof<NamedCache::Handle>(hCacheMap))
                {
                NamedCache::Handle hCache = cast<NamedCache::Handle>(hCacheMap);
                hCache->addMapListener(hListener);
                }
            }
        }
    catch (RuntimeException e) {};
    }

void CachingMap::unregisterDeactivationListener() const
    {
    try
        {
        NamedCacheDeactivationListener::Handle hListener = m_hListenerDeactivation;
        if (hListener != NULL)
            {
            CacheMap::Handle hCacheMap = getBackMap();
            if(instanceof<NamedCache::Handle>(hCacheMap))
                {
                NamedCache::Handle hCache = cast<NamedCache::Handle>(hCacheMap);
                hCache->removeMapListener(hListener);
                }
            }
        }
    catch (RuntimeException e) {}
    }

void CachingMap::resetFrontMap()
    {
    try
        {
        unregisterFrontListener();
        getFrontMap()->clear();
        }
    catch (RuntimeException e) {}

    resetInvalidationStrategy();
    }


// ----- inner class: SimpleListener ---------------------------------

// ----- constructors ---------------------------------------------------

CachingMap::SimpleListener::SimpleListener(CachingMap::Handle hMap)
    : m_whMap(self(), hMap)
    {
    }

// ----- MultiplexingMapListener interface ------------------------------

void CachingMap::SimpleListener::onMapEvent(
        MapEvent::View vEvent)
    {
    m_whMap->validate(vEvent);
    }


// ----- inner class: DeactivationListener ------------------------------

// ----- constructors ---------------------------------------------------

CachingMap::DeactivationListener::DeactivationListener(CachingMap::Handle hMap)
    : m_whMap(self(), hMap)
    {
    }

// ----- NamedCacheDeactivationListener interface -----------------------

void CachingMap::DeactivationListener::entryDeleted(
        MapEvent::View /*vEvent*/)
    {
    // destroy/disconnect event
    m_whMap->resetFrontMap();
    }

void CachingMap::DeactivationListener::entryUpdated(
        MapEvent::View /*vEvent*/)
    {
    // "truncate" event
    m_whMap->resetFrontMap();
    }

// ----- inner class: PrimingListener -----------------------------------

// ----- constructors ---------------------------------------------------

CachingMap::PrimingListener::PrimingListener(CachingMap::Handle hMap)
    : m_whMap(self(), hMap)
    {
    }

// ----- MultiplexingMapListener interface ------------------------------

void CachingMap::PrimingListener::onMapEvent(
        MapEvent::View vEvent)
    {
    m_whMap->validate(vEvent);
    }

COH_CLOSE_NAMESPACE3
