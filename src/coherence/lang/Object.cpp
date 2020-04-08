/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Object.hpp"

#include "coherence/lang/Class.hpp"
#include "coherence/lang/CloneNotSupportedException.hpp"
#include "coherence/lang/HeapAnalyzer.hpp"
#include "coherence/lang/InterruptedException.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/Thread.hpp"
#include "coherence/lang/ThreadLocalReference.hpp"
#include "coherence/lang/WeakReference.hpp"

#include "coherence/util/IdentityHashMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MappedSet.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/lang/Allocator.hpp"
#include "private/coherence/lang/ClassInfo.hpp"
#include "private/coherence/lang/LifeCycle.hpp"
#include "private/coherence/lang/WeakReferenceImpl.hpp"
#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/NativeThread.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <stdlib.h>

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_TYPED_CLASS(Object);

using coherence::native::NativeAtomic32;
using coherence::native::NativeCondition;
using coherence::native::NativeThread;

using coherence::util::IdentityHashMap;
using coherence::util::Map;
using coherence::util::MappedSet;
using coherence::util::Set;

// defined in System.cpp
extern HeapAnalyzer* coh_get_heap_analyzer();

// defined in Thread.cpp
extern void coh_checkThreadDumpRequest();

#define COH_CHECK_THREAD_DUMP_REQUEST() \
    static int cCheckIters = 5000; \
    if (cCheckIters-- < 0) \
        { \
        cCheckIters = ((int) System::getInterruptResolution()) * 100; \
        Thread::currentThread(); \
        }

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return the maximum number of spins that should be made when attempting
    * to acquire a member lock.
    */
    size32_t getMaxMemberLockSpin()
        {
        // COH-12944 requires checking system property prefix for either coherence or tangosol
        static const char* achSpinTmp1 = getenv("coherence.spin.max");
        static const char* achSpinTmp2 = achSpinTmp1 == NULL
                    ? getenv("CoherenceSpinMax")
                    : achSpinTmp1;
        static const char* achSpinTmp3 = achSpinTmp2 == NULL
                    ? getenv("tangosol.coherence.spin.max")
                    : achSpinTmp2;
        static const char* achSpin     = achSpinTmp3 == NULL
                    ? getenv("TangosolCoherenceSpinMax")
                    : achSpinTmp3;
        static const size32_t cSpin = achSpin == NULL ? 32 : atoi(achSpin) + 1;
        return cSpin;
        }
    COH_STATIC_INIT(getMaxMemberLockSpin());
    }

/**
 * Return the thread-local set used in computing a deep sizeOf for an
 * object.
 */
Set::Handle coh_getThreadLocalSizeOfSet()
    {
    static FinalHandle<ThreadLocalReference> hRef(System::common(),
            ThreadLocalReference::create());
    MappedSet::Handle hSet = cast<MappedSet::Handle>(hRef->get());
    if (hSet == NULL)
        {
        hSet = MappedSet::create((Map::Handle) IdentityHashMap::create());
        hRef->set(hSet);
        }
    return hSet;
    }
COH_STATIC_INIT(coh_getThreadLocalSizeOfSet());

// ----- constructors -------------------------------------------------------

Object::Object()
    : m_atomicLifeCycleState(0, false), m_atomicLifeCycleRefs(0, false),
      m_pCondition(NULL), m_pWeakThis(NULL), m_pSmartMemberStack(NULL)
    {
    COH_CHECK_THREAD_DUMP_REQUEST();
    }

Object::Object(const Object& /*that*/)
    : m_atomicLifeCycleState(0, false), m_atomicLifeCycleRefs(0, false),
      m_pCondition(NULL), m_pWeakThis(NULL), m_pSmartMemberStack(NULL)
    {
    COH_CHECK_THREAD_DUMP_REQUEST();
    }


// ----- Object interface ---------------------------------------------------

Object::Handle Object::clone() const
    {
    return Object::create();
    }

bool Object::equals(Object::View v) const
    {
    return this == get_pointer(v);
    }

size32_t Object::hashCode() const
    {
    size_t nHash = size_t(this);
    if (sizeof(Object*) > sizeof(size32_t))
        {
        // >32b address into 32b hash; assume 64b
        // XOR high bits with swizzled low bits
        return size32_t(size64_t(nHash) >> 32) ^               // high 32 bits
               size32_t((nHash << 4 | ((nHash >> 28) & 0xF))); // swizzled low
        }

    return size32_t(nHash);
    }

bool Object::isImmutable() const
    {
    LifeCycle::Refs refs(m_atomicLifeCycleRefs.get());
    return refs.value.cHandle == 0 && refs.value.cView != 0;
    }

TypedHandle<const String> Object::toString() const
    {
    return COH_TO_STRING(Class::getTypeName(typeid(*this)) << "@" << static_cast<void const* const>(this));
    }

size64_t Object::sizeOf(bool fDeep) const
    {
    if (fDeep)
        {
        Set::Handle hSet = coh_getThreadLocalSizeOfSet();
        size64_t    cb   = 0;
        if (hSet->add(this))
            {
            bool fRoot = hSet->size() == 1;
            try
                {
                cb = sizeOf(false); // allow overridden impl to answer shallow size

                // walk smart member graph collecting deep sizeOfs
                for (const SmartMember* pSmart = m_pSmartMemberStack; pSmart != NULL;
                     pSmart = pSmart->m_pNext)
                    {
                    cb += pSmart->retained();
                    }

                if (m_pWeakThis != NULL)
                    {
                    cb += sizeof(WeakReferenceImpl);
                    }

                if (m_pCondition != NULL && _isEscaped())
                    {
                    cb += m_pCondition->sizeOf();
                    }
                }
            catch (...)
                {
                if (fRoot)
                    {
                    hSet->clear();
                    }
                throw;
                }
            if (fRoot)
                {
                hSet->clear();
                }
            }

        return cb;
        }
    else
        {
        return sizeof(Object);
        }
    }


// ----- synchronization and notification methods ---------------------------

void Object::_enterMonitorInternal() const
    {
    _ensureMonitor().lock();
    }

void Object::_exitMonitorInternal() const
    {
    _ensureMonitor().unlock();
    }

void Object::wait() const
    {
    wait(0);
    }

void Object::wait(int64_t cMillis) const
    {
    int32_t cMillisTest    = (int32_t) System::getInterruptResolution();
    int64_t cMillisTimeout = Thread::remainingTimeoutMillis();

    if (cMillisTimeout != Integer64::max_value && (cMillisTimeout < cMillis || cMillis == 0))
        {
        cMillis = cMillisTimeout;
        }

    LifeCycle::State state(m_atomicLifeCycleState.peek());
    if (!state.value.fEscaped && state.value.nMonitor == LifeCycle::flat)
        {
        // monitor pointer acts as a nested synchronization counter
        if (m_pCondition == NULL)
            {
            coh_throw_illegal_state("Invalid monitor state");
            }
        else if (cMillis == 0)
            {
            // we'd be stuck here indefinitely
            // COH_LOG("wait(0) called on unescaped object", 2);
            while (true)
                {
                Thread::sleep(cMillisTest);
                }
            }

        // Note: on this path we don't respect the interrupt check, but this object hasn't escaped so the
        // only impact is that it won't show up in thread dumps if we are waiting on it, which would be
        // bizarre as it hasn't escaped even.
        Thread::sleep(cMillis);
        return;
        }

    NativeCondition& condition      = _ensureMonitor();
    int32_t          cNotifications = condition.getNotificationCount();
    int64_t          ldtNow         = 0;
    int64_t          ldtFinal       = 0;
    int64_t          ldtEnd         = 0;

    do
        {
        if (Thread::interrupted())
            {
            COH_THROW (InterruptedException::create());
            }

        if (cMillis == 0)
            {
            // looping wait until notify
            condition.wait(cMillisTest);
            }
        else if (cMillis > cMillisTest && ldtFinal >= ldtNow)
            {
            // intermediate looping wait on check interval
            if (ldtNow == 0)
                {
                ldtNow   = System::currentTimeMillis();
                ldtEnd   = ldtNow + cMillis;
                ldtFinal = ldtEnd - cMillisTest;
                }

            condition.wait(cMillisTest);

            ldtNow = System::currentTimeMillis();
            if (ldtNow < ldtEnd - cMillis)
                {
                // clock rolled back
                break;
                }
            }
        else // cMillisRemain <= cMillisTest
            {
            // final wait
            // casts from 64 to 32 is safe as value is guaranteed to be <=
            // then cMillisTest which is 32b
            if (ldtNow == 0)
                {
                // also the first wait
                condition.wait(static_cast<int32_t>(cMillis));
                }
            else if (ldtEnd > ldtNow)
                {
                condition.wait(static_cast<int32_t>(ldtEnd - ldtNow));
                }

            // don't try to account for spurious wakeups; return even if
            // the clock doesn't say we've timed out
            break;
            }
        }
    while (cNotifications == condition.getNotificationCount());
    }

void Object::notify() const
    {
    LifeCycle::State state = m_atomicLifeCycleState.peek();
    if (state.value.fEscaped || state.value.nMonitor != LifeCycle::flat)
        {
        _ensureMonitor().notify();
        return;
        }

    // monitor pointer acts as a nested synchronization counter
    if (m_pCondition == NULL)
        {
        coh_throw_illegal_state("Invalid monitor state");
        }
    }

void Object::notifyAll() const
    {
    LifeCycle::State state = m_atomicLifeCycleState.peek();
    if (state.value.fEscaped || state.value.nMonitor != LifeCycle::flat)
        {
        _ensureMonitor().notifyAll();
        return;
        }

    // monitor pointer acts as a nested synchronization counter
    if (m_pCondition == NULL)
        {
        coh_throw_illegal_state("Invalid monitor state");
        }
    }

void Object::_acquireMemberReadLockInternal() const
    {
    LifeCycle::State  stateOld, stateAssumed, stateNew;
    int32_t           cSpinLimit = getMaxMemberLockSpin();

    stateOld = m_atomicLifeCycleState.peek();
    while (true)
        {
        stateNew = stateAssumed = stateOld;

        if (stateOld.value.nMemberWriteLockState == LifeCycle::gate_open)
            {
            ++stateNew.value.cMemberReadLock;
            }
        else
            {
            // assume it will open soon
            stateAssumed.value.nMemberWriteLockState =
                    stateNew.value.nMemberWriteLockState = LifeCycle::gate_open;
            stateAssumed.value.cMemberReadLock = 0;
            stateNew    .value.cMemberReadLock = 1;
            --cSpinLimit;
            }

        if (cSpinLimit == 0 || stateOld.value.cMemberReadLock == LifeCycle::max_read_locks)
            {
            NativeThread::yield(); // avoid Thread::yield(), COHCPP-331
            stateOld   = m_atomicLifeCycleState.get();
            cSpinLimit = getMaxMemberLockSpin();
            }
        else
            {
            stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
            if (stateOld == stateAssumed)
                {
                return;
                }
            }
        }
    }

void Object::_releaseMemberReadLockInternal() const
    {
    LifeCycle::State stateOld, stateAssumed, stateNew;

    stateOld = m_atomicLifeCycleState.peek();
    do
        {
        stateNew = stateAssumed = stateOld;

        if (stateOld.value.cMemberReadLock == 0)
            {
            COH_THROW_STREAM (IllegalStateException,
                    "Unexpected lock state: " << stateOld);
            }
        --stateNew.value.cMemberReadLock;

        stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
        }
    while (stateOld != stateAssumed);
    }

void Object::_acquireMemberWriteLockInternal() const
    {
    LifeCycle::State stateOld, stateAssumed, stateNew;
    int32_t          cSpinLimit  = getMaxMemberLockSpin();
    bool             fGateKeeper = false;

    stateOld = m_atomicLifeCycleState.peek();
    while (true)
        {
        stateNew = stateAssumed = stateOld;

        if (fGateKeeper)
            {
            COH_ENSURE_EQUALITY(stateOld.value.nMemberWriteLockState, LifeCycle::gate_closing);

            // try to finish closing the gate (assume all readers release)
            stateNew.value.nMemberWriteLockState = LifeCycle::gate_closed;
            stateNew.value.cMemberReadLock = stateAssumed.value.cMemberReadLock = 0;
            }
        else
            {
            // try to become the gate keeper
            stateAssumed.value.nMemberWriteLockState = LifeCycle::gate_open;
            stateNew.value.nMemberWriteLockState =
                    stateOld.value.cMemberReadLock == 0
                        ? LifeCycle::gate_closed : LifeCycle::gate_closing;
            }

        if (stateAssumed != stateOld && --cSpinLimit == 0)
            {
            // we've made too many assumptions; hold off a bit
            NativeThread::yield(); // avoid Thread::yield(), COHCPP-331
            stateOld  = m_atomicLifeCycleState.get();
            cSpinLimit = getMaxMemberLockSpin();
            }
        else
            {
            stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
            if (stateOld == stateAssumed)
                {
                fGateKeeper = true;
                if (stateNew.value.nMemberWriteLockState == LifeCycle::gate_closed)
                    {
                    return;
                    }
                stateOld = stateNew;
                }
            }
        }
    }

void Object::_releaseMemberWriteLockInternal() const
    {
    LifeCycle::State stateOld, stateAssumed, stateNew;

    stateOld = m_atomicLifeCycleState.peek();
    do
        {
        stateNew = stateAssumed = stateOld;
        if (stateOld.value.cMemberReadLock == 0 &&
            stateOld.value.nMemberWriteLockState == LifeCycle::gate_closed)
            {
            stateNew.value.nMemberWriteLockState = LifeCycle::gate_open;
            stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
            }
        else
            {
            // either the gate is not gate_closed or we have readers when we
            // shouldn't
            COH_THROW_STREAM (IllegalStateException,
                "Unexpected lock state: " << stateOld);
            }
        }
    while (stateOld != stateAssumed);
    }


// ----- reference counting methods -----------------------------------------

TypedHandle<WeakReference> Object::_attachWeak()
    {
    return _ensureWeakReference();
    }

TypedHandle<const WeakReference> Object::_attachWeak() const
    {
    return _ensureWeakReference();
    }

void Object::onInit()
    {
    LifeCycle::State stateOld, stateAssumed, stateNew;

    // ensure we aren't being doubly initialized
    stateOld = m_atomicLifeCycleState.peek();
    COH_ENSURE(stateOld.value.nLifeStage == LifeCycle::initializing);
    bool fEscapedThis = stateOld.value.fEscaped;

    HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
    if (pAnalyzer != NULL)
        {
        pAnalyzer->registerObject(self());
        }

    // propagate to all SmartMembers
    for (SmartMember* pNext = m_pSmartMemberStack; pNext != NULL; pNext = pNext->m_pNext)
        {
        pNext->onInit();
        }

    stateAssumed = stateOld;
    do
        {
        stateNew = stateOld;
        stateNew.value.nLifeStage = LifeCycle::initialized;
        stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew, fEscapedThis);
        }
    while (stateAssumed != stateOld);
    }

void Object::onConst()
    {
    // propagate to all SmartMembers
    for (SmartMember* pNext = m_pSmartMemberStack; pNext != NULL; pNext = pNext->m_pNext)
        {
        if (pNext->m_nMutability == SmartMember::inherited)
            {
            pNext->onConst();
            }
        }
    }

void Object::onEscape(bool fEscaped) const
    {
    LifeCycle::State state  = m_atomicLifeCycleState.peek();

    // ensure the transition
    COH_ENSURE_RELATION(bool, state.value.fEscaped, !=, fEscaped);

    // flip the escaped bit
    state.value.fEscaped = fEscaped;
    m_atomicLifeCycleState.poke(state);

    if (fEscaped) // escape this object
        {
        // potentially inflate our monitor
        size_t cEnter = reinterpret_cast<size_t>(m_pCondition);
        if (state.value.nMonitor == LifeCycle::flat && cEnter != 0)
            {
            // monitor pointer is acting as a nested sync count
            m_pCondition = NULL;

            while (cEnter--)
                {
                _enterMonitor();
                }
            state = m_atomicLifeCycleState.peek();
            }

        // escape our weak reference
        WeakReference* pWeakThis = m_pWeakThis;
        if (pWeakThis != NULL)
            {
            // there is an edge condition we must protect against.  This
            // object is now marked as escaped, but it hasn't completed
            // the process. Of specific concern is the attachment count
            // contributed by the WeakReference. While an object is
            // escaped it is assumed that the attachment made by its WR
            // is ATTACHMENT_INCREMENT. But while it is doing that work
            // this isn't true. There is a check to block attachments
            // when the attach count equals the WR attaches.  We need to
            // ensure that that check doesn't get triggered while we
            // allow the WR to re-attach
            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            bool fAdjust = false;
            if (refs.value.cHandle == LifeCycle::escaped_increment)
                {
                // in the danger zone, increment by one to avoid
                fAdjust = true;
                ++refs.value.cHandle;
                m_atomicLifeCycleRefs.poke(refs);
                }
            pWeakThis->_attach(/*fEscaped*/ true); // attach escaped new
            pWeakThis->_detach(/*fEscaped*/ false); // detach un-escaped old

            // undo our temporary increment
            if (fAdjust)
                {
                refs = m_atomicLifeCycleRefs.peek();
                --refs.value.cHandle;
                m_atomicLifeCycleRefs.poke(refs);
                }
            }
        }
    else // capture this object
        {
        // don't touch the monitor, if it was inflated we'll assume it will
        // be likely this object will escape again, avoid the cost of creating
        // and destroying monitors

        // capture our weak reference
        WeakReference* pWeakThis = m_pWeakThis;
        if (pWeakThis != NULL)
            {
            pWeakThis->_attach(/*fEscaped*/ false); // attach un-escaped new
            pWeakThis->_detach(/*fEscaped*/ true); // detach escaped old
            }
        }

    // walk smart member graph updating escape states
    for (const SmartMember* pSmart = m_pSmartMemberStack; pSmart != NULL;
         pSmart = pSmart->m_pNext)
        {
        pSmart->onEscape(fEscaped);
        }
    }

Object* Object::_attachCaptured(bool fEscaped)
    {
    // NOTE: the key transitions in the method MUST be accounted for in _attach
    LifeCycle::State state(m_atomicLifeCycleState.peek());
    LifeCycle::Refs  refs(m_atomicLifeCycleRefs.peek());

    switch (state.value.nLifeStage)
        {
        case LifeCycle::destructing:
            // the object is being destroyed, no new handles allowed
            // the only "legal" way we could get here would be for a
            // destructor to inadvertently create a new handle/view to
            // itself from within the destructor
            return NULL;

        case LifeCycle::constructing:
            // first attach
            state.value.nLifeStage = LifeCycle::initializing;
            refs.value.cHandle    = 1;
            m_atomicLifeCycleState.poke(state);
            m_atomicLifeCycleRefs.poke(refs);

            // first attachment, call onInit method
            this->onInit();
            // NOTE: we may have now escaped

            // ensure that all derivative classes called their parent's onInit()
            state = m_atomicLifeCycleState.peek();
            if (state.value.nLifeStage == LifeCycle::initializing)
                {
                COH_THROW (IllegalStateException::create(
                        "derived class failed to call super's onInit method"));
                }

            if (fEscaped)
                {
                // very rare case of escaping on first attach
                // we always do onInit() pre-escape, so we defer the escaping
                // until afterwards
                _attach(/*fEscaped*/ true);  // account for fEscaped == true
                _detach(/*fEscaped*/ false); // undo = 1 from above
                }

            return this;

        default: // common case
            {
            unsigned int cHandle = refs.value.cHandle;
            switch (cHandle)
                {
                case 0:
                    // apparently we are kept alive by only views
                    return NULL;

                case 1:
                    if (refs.value.nWeakReference == LifeCycle::inflated)
                        {
                        // the only remaining handle is weak, no new ones
                        // allowed this protects against a race condition where
                        // a thread legitimately tries to attach through a
                        // WeakReference while the last non-weak handle
                        // detaches. We must ensure that the detach wins, and
                        // that the attach from weak acts as if the object had
                        // no handles, which is true.
                        return NULL;
                        }
                    break; // fall out and attach

                case LifeCycle::max_handles:
                    // immortal object
                    if (fEscaped)
                        {
                        // object is transitioning to escaped state
                        _escape();
                        }
                    return this; // already immortal; avoid increment

                default:
                    break; // fall out and attach
                }

            // common case; attach handle
            if (fEscaped)
                {
                if (cHandle + LifeCycle::escaped_increment > LifeCycle::max_handles)
                    {
                    refs.value.cHandle = LifeCycle::max_handles;
                    }
                else
                    {
                    refs.value.cHandle = cHandle + LifeCycle::escaped_increment;
                    }
                m_atomicLifeCycleRefs.poke(refs);
                _escape();
                }
            else
                {
                refs.value.cHandle = cHandle + 1;
                m_atomicLifeCycleRefs.poke(refs);
                }

            if (refs.value.cHandle == LifeCycle::max_handles)
                {
                HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
                if (pAnalyzer != NULL)
                    {
                    pAnalyzer->registerImmortal(self());
                    }
                }

            return this;
            }
        }
    }

Object* Object::_attachEscaped(bool fEscaped)
    {
    LifeCycle::Refs refsOld, refsAssumed, refsNew;
    unsigned int    cAttach = fEscaped ? LifeCycle::escaped_increment : 1;

    refsOld = m_atomicLifeCycleRefs.peek();

    if (refsOld.value.cHandle == 0)
        {
        // we cannot get here if we are constructing because that would mean
        // we were born with state.value.fEscaped == true, even if the first
        // attach is an escaped attach we are still born unescaped

        // the object is being destroyed, no new handles allowed
        // the only "legal" way we could get here would be for a
        // destructor to inadvertently create a new handle/view to
        // itself from within the destructor

        // or all prior handles have detached, no new ones allowed
        return NULL;
        }

    unsigned int cHandle;
    unsigned int cHandleNew;
    do
        {
        cHandle    = refsOld.value.cHandle;
        cHandleNew = cHandle + cAttach;
        if (refsOld.value.nWeakReference == LifeCycle::inflated &&
            cHandle                      == LifeCycle::escaped_increment)
            {
            // the only remaining handle is weak, no new ones allowed
            // this protects against a race condition where a thread
            // legitimately tries to attach through a WeakReference while
            // the last non-weak handle detaches. We must ensure that the
            // detach wins, and that the attach from weak acts as if the
            // object had no handles, which is true.
            return NULL;
            }
        else if (cHandleNew > LifeCycle::max_handles)
            {
            if (cHandle == LifeCycle::max_handles)
                {
                // already immortal; avoid increment
                return this;
                }
            cHandleNew = LifeCycle::max_handles; // attempt to make it immortal
            }

        // handle attachment is allowable
        refsNew = refsAssumed = refsOld;
        refsNew.value.cHandle = cHandleNew;

        refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew);
        }
    while (refsOld != refsAssumed);

    if (cHandleNew == 2 && refsOld.value.cView == 0)
        {
        // Though the object is marked as escaped, we've just proved it is
        // now visible to only this thread.  We have two references to consider
        // the one we are assigning, and the one we are assigning from, if
        // both have a value of 1 then neither is escaped.

        // Note: as we are doing a handle attach, it means we're attaching
        // from a Handle (or Holder acting as one).  If there are any Views
        // then we can't capture, as this would mean we have three references
        // and even if they are all unescaped, the View could be from a
        // different thread.
        _capture();
        }
    else if (cHandleNew == LifeCycle::max_handles)
        {
        HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
        if (pAnalyzer != NULL)
            {
            pAnalyzer->registerImmortal(self());
            }
        }

    return this;
    }

const Object* Object::_attachCaptured(bool fEscaped) const
    {
    // NOTE: the key transitions in the method MUST be accounted for in _attach
    switch (LifeCycle::State(m_atomicLifeCycleState.peek()).value.nLifeStage)
        {
        case LifeCycle::constructing:
            {
            // we need to run onInit() as a Handle, keep Handle alive until
            // View attach completes
            Object::Handle h = const_cast<Object*>(this);
            // as we've run onInit we could have now escaped, come back through
            // front door
            return _attach(fEscaped);
            }

        case LifeCycle::destructing:
            // the object is being destroyed, no new handles allowed
            // the only "legal" way we could get here would be for a
            // destructor to inadvertently create a new handle/view to
            // itself from within the destructor
            return NULL;

        default: // common case
            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            unsigned int    cView = refs.value.cView;
            switch (cView)
                {
                case LifeCycle::max_views:
                    // immortal object
                    if (fEscaped)
                        {
                        // object is transitioning to escaped state
                        _escape();
                        }
                    return this; // already immortal; avoid increment

                case 1:
                    if (refs.value.cHandle == 0 &&
                        refs.value.nWeakReference == LifeCycle::inflated)
                        {
                        // or the only remaining handle is weak, no new
                        // attachments allowed
                        return NULL;
                        }
                    // fall through

                default:
                    // common case; attach view
                    if (fEscaped)
                        {
                        // object is transitioning to escaped state
                        if (cView + LifeCycle::escaped_increment > LifeCycle::max_views)
                            {
                            refs.value.cView = LifeCycle::max_views;
                            }
                        else
                            {
                            refs.value.cView = cView + LifeCycle::escaped_increment;
                            }
                        m_atomicLifeCycleRefs.poke(refs);
                        _escape();
                        }
                    else
                        {
                        refs.value.cView = cView + 1;
                        m_atomicLifeCycleRefs.poke(refs);
                        }

                    if (refs.value.cView == LifeCycle::max_views)
                        {
                        HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
                        if (pAnalyzer != NULL)
                            {
                            pAnalyzer->registerImmortal(self());
                            }
                        }

                    return this;
                }
        }
    }

const Object* Object::_attachEscaped(bool fEscaped) const
    {
    LifeCycle::Refs refsOld, refsAssumed, refsNew;
    unsigned int    cAttach = fEscaped ? LifeCycle::escaped_increment : 1;
    unsigned int    nLifeStage = LifeCycle::State(m_atomicLifeCycleState.peek()).value.nLifeStage;

    if (nLifeStage == LifeCycle::destructing ||
        nLifeStage == LifeCycle::constructing)
        {
        // the object is being destroyed, no new handles allowed
        // the only "legal" way we could get here would be for a
        // destructor to inadvertently create a new handle/view to
        // itself from within the destructor

        // or the object is being constructed (escape during construction is
        // not allowed)
        return NULL;
        }

    refsOld = m_atomicLifeCycleRefs.peek();
    unsigned int cView;
    unsigned int cViewNew;
    do
        {
        cView    = refsOld.value.cView;
        cViewNew = cView + cAttach;
        if (refsOld.value.nWeakReference  == LifeCycle::inflated &&
            refsOld.value.cHandle + cView == LifeCycle::escaped_increment)
            {
            // the only remaining handle is weak, no new attachments
            // allowed; this must be within the CAS loop as we could be
            // racing during an attach from a WeakReference just the last
            // non weak handle/view detaches, the WeakReference cannot win
            return NULL;
            }
        else if (cViewNew > LifeCycle::max_views)
            {
            if (cView == LifeCycle::max_views)
                {
                // already immortal; avoid increment
                return this;
                }
            cViewNew = LifeCycle::max_views; // attempt to make it immortal
            }

        // view attachment is allowable
        refsNew = refsAssumed = refsOld;
        refsNew.value.cView   = cViewNew;

        refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew);
        }
    while (refsOld != refsAssumed);

    if (cViewNew <= 2 && // allows short circuit avoiding pulling cHandle
        cViewNew + refsOld.value.cHandle == 2)
        {
        // Though the object is marked as escaped, we've just proved it is
        // now visible to only this thread.  We have two references to consider
        // the one we are assigning, and the one we are assigning from, if
        // both have a value of 1 then neither is escaped.
        _capture();
        }
    else if (refsNew.value.cView == LifeCycle::max_views)
        {
        HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
        if (pAnalyzer != NULL)
            {
            pAnalyzer->registerImmortal(self());
            }
        }


    return this;
    }

void Object::_detachCaptured()
    {
    // NOTE: the key transitions in the method MUST be accounted for in _detach
    LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());

    unsigned int cHandle = refs.value.cHandle;
    switch (cHandle)
        {
        case 0:
            COH_THROW_STREAM (IllegalStateException, "Detach handle " << refs);

        case LifeCycle::max_handles:
            return; // immortal; avoid decrement

        default:
            // common case
            break;
        }

    // handle detach allowable
    refs.value.cHandle = --cHandle;
    m_atomicLifeCycleRefs.poke(refs);

    // Perform transition specific cleanup.
    switch (cHandle) // post decrement comparison
        {
        case 0:
            // handle potential onConst or delete
            if (refs.value.cView == 0)
                {
                // fully detached; block the object's destructor from trying to
                // revive itself by creating new handles/views
                _destroy();
                }
            else
                {
                // only views remain
                onConst();
                }
            break;

        case 1:
            // handle potential WeakReference cleanup
            if (refs.value.nWeakReference == LifeCycle::inflated)
                {
                // we now have just one Handle, and the only thing preventing
                // us from being immutable is our WeakReference.
                // Note attachInternal will not give out new Handles now, so
                // we can safely flip the WeakReference to be a View
                ((WeakReferenceImpl*) (m_pWeakThis->_cast(COH_CLASS_ID(WeakReferenceImpl))))->flip();
                // we may have been deleted now by the recursive call
                }
            break;

        default:
            // no-op
            break;
        }
    }

void Object::_detachEscaped(bool fEscaped)
    {
    LifeCycle::Refs refsOld, refsAssumed, refsNew;
    unsigned int    cDetach    = fEscaped ? LifeCycle::escaped_increment : 1;
    const Object*   cpDetach   = NULL;

    refsOld = m_atomicLifeCycleRefs.peek();
    do
        {
        switch (refsOld.value.cHandle)
            {
            case 0:
                COH_THROW_STREAM (IllegalStateException, "Detach escaped handle "
                        << refsOld);

            case 1:
            case LifeCycle::escaped_increment:
                // final handle detach
                if (NULL == cpDetach && refsOld.value.cView > 0)
                    {
                    // we are about to transition this Object to being only
                    // referenced via Views; create a temporary View to ensure
                    // that the Object remains alive, so that we can safely
                    // call onConst
                    cpDetach = this;
                    cpDetach->_attach(/*fEscaped*/ false); // view attach
                    }
                break;

            case LifeCycle::max_handles:
                return; // immortal; avoid decrement

            default:
                break; // common case
            }

        // handle detach allowable
        refsNew = refsAssumed = refsOld;
        refsNew.value.cHandle  -= cDetach;

        refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew);
        }
    while (refsOld != refsAssumed);

    // Perform transition specific cleanup. Note that each of these cleanup
    // operations will occur at most once, as the transitions are one-way.
    // This means that we are automatically protected from two threads ever
    // attempting the same cleanup operation.
    switch (refsNew.value.cHandle)
        {
        case 0:
            // last handle has detached
            if (refsNew.value.cView == 0)
                {
                // no more attachments
                _destroy();
                }
            else
                {
                // only views remain
                onConst();
                }
            break;

        case LifeCycle::escaped_increment:
            if (refsNew.value.nWeakReference == LifeCycle::inflated)
                {
                // we now have just one Handle, and the only thing preventing
                // us from being immutable is our WeakReference.
                // Note attachInternal will not give out new Handles now, so
                // we can safely flip the WeakReference to be a View
                ((WeakReferenceImpl*) (m_pWeakThis->_cast(COH_CLASS_ID(WeakReferenceImpl))))->flip();
                // we may have been deleted now by the recursive call
                }
            break;

        default:
            // no-op
            break;
        }

    if (cpDetach)
        {
        cpDetach->_detach(/*fEscaped*/ false);
        }
    }

void Object::_detachCaptured() const
    {
    // NOTE: the key transitions in the method MUST be accounted for in _detach
    LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());

    unsigned int cView = refs.value.cView;
    switch (cView)
        {
        case 0:
            COH_THROW_STREAM (IllegalStateException, "Detach view " << refs);

        case LifeCycle::max_views:
            return; // immortal; avoid decrement

        default:
            // common case
            break;
        }

    // view detach allowable
    refs.value.cView = --cView;
    m_atomicLifeCycleRefs.poke(refs);

    // Perform transition specific cleanup.
    if (refs.value.cHandle == 0)
        {
        switch (cView)
            {
            case 0:
                {
                // fully detached; block the object's destructor from trying to
                // revive itself by creating new handles/views
                _destroy();
                break;
                }

            case 1:
                if (refs.value.nWeakReference == LifeCycle::inflated)
                    {
                    // break link between weak reference and this Object
                    ((WeakReferenceImpl*) (m_pWeakThis->_cast(COH_CLASS_ID(WeakReferenceImpl))))->clear();
                    // at this point we will have been dereferenced by our
                    // WeakReference and we will have been deleted by the
                    // recursive call
                    }
                break;

            default:
                // no-op
                break;
            }
        }
    }

void Object::_detachEscaped(bool fEscaped) const
    {
    LifeCycle::Refs refsOld, refsAssumed, refsNew;
    unsigned int    cDetach = fEscaped ? LifeCycle::escaped_increment : 1;

    refsOld = m_atomicLifeCycleRefs.peek();
    do
        {
        switch (refsOld.value.cView)
            {
            case 0:
                COH_THROW_STREAM (IllegalStateException, "Detach escaped view "
                        << refsOld);

            case LifeCycle::max_views:
                return; // immortal; avoid decrement

            default:
                break; // common case
            }

        // view detach allowable
        refsNew = refsAssumed = refsOld;
        refsNew.value.cView  -= cDetach;

        refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew);
        }
    while (refsOld != refsAssumed);

    // Perform transition specific cleanup. Note that each of these cleanup
    // operations will occur at most once, as the transitions are one-way.
    // This means that we are automatically protected from two threads ever
    // attempting the same cleanup operation.
    if (refsNew.value.cHandle == 0)
        {
        // handle potential delete or WeakReference cleanup
        switch (refsNew.value.cView)
            {
            case 0:
                // fully detached; block the object's destructor from trying to
                // revive itself by creating new handles/views
                _destroy();
                break;

            case LifeCycle::escaped_increment:
                if (refsNew.value.nWeakReference == LifeCycle::inflated)
                    {
                    // break link between weak reference and this Object
                    ((WeakReferenceImpl*) (m_pWeakThis->_cast(COH_CLASS_ID(WeakReferenceImpl))))->clear();
                    // at this point we will have been dereferenced by our
                    // WeakReference and we will have been deleted by the
                    // recursive call
                    }
                break;

            default:
                // no-op
                break;
            }
        }
    }

void Object::_destroy() const
    {
    // "escaped" or not, we are only visible to a single thread
    LifeCycle::State state(m_atomicLifeCycleState.peek());
    state.value.nLifeStage = LifeCycle::destructing;
    m_atomicLifeCycleState.poke(state);

    HeapAnalyzer* pAnalyzer = coh_get_heap_analyzer();
    if (pAnalyzer != NULL)
        {
        pAnalyzer->unregisterObject(self());
        }

    // at this point the object can't get new attachments, we do what would
    // typically be done in a destructor here.  Removing the destructors
    // demonstrated a ~20% performance gain in an object creation/destruction
    // test
    delete m_pCondition;
    m_pCondition = NULL;

    WeakReference* pWeakThis = m_pWeakThis;
    if (pWeakThis)
        {
        m_pWeakThis = NULL;
        pWeakThis->_detach(state.value.fEscaped);
        }

    // trigger full destruction
    delete this;

    COH_CHECK_THREAD_DUMP_REQUEST();
    }

void Object::_escape() const
    {
    // start escaping this object
    try
        {
        onEscape(/*fEscaped*/ true);
        }
    catch (Exception::View ve)
        {
        COH_THROW (IllegalStateException::create(
                "Error escaping Object", ve));
        }
    catch (...)
        {
        COH_THROW (IllegalStateException::create(
                "Error escaping Object"));
        }

    // ensure we reached Object::onEscape()
    COH_ENSURE(LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped);
    }

void Object::_capture() const
    {
    // Note: it is the caller's job to ensure that it is safe to capture the object
    // there are callers which do this even when the ref count is > 1, i.e. as part
    // of transitioning from 1->2
    try
        {
        onEscape(/*fEscaped*/ false);
        }
    catch (Exception::View ve)
        {
        COH_THROW (IllegalStateException::create(
                "Error unescaping Object", ve));
        }
    catch (...)
        {
        COH_THROW (IllegalStateException::create(
                "Error unescaping Object"));
        }

    // ensure we reached Object::onEscape()
    COH_ENSURE(!LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped);
    }

NativeCondition& Object::_ensureMonitor() const
    {
    // post-inflated optimization
    if (LifeCycle::State(m_atomicLifeCycleState.peek()).value.nMonitor ==
        LifeCycle::inflated)
        {
        return *m_pCondition;
        }

    // inflate or wait for inflation to finish
    LifeCycle::State stateOld, stateAssumed, stateNew;
    bool             fInflator = false;
    stateOld = m_atomicLifeCycleState.peek();
    while (true)
        {
        stateAssumed = stateNew = stateOld;

        switch (stateOld.value.nMonitor)
            {
            case LifeCycle::flat:
                // try to acquire allocation right
                stateNew.value.nMonitor = LifeCycle::inflating;
                stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
                if (stateOld != stateAssumed)
                    {
                    break;
                    }

                // this is now the inflating thread
                fInflator     = true;
                m_pCondition  = NativeCondition::create();
                stateAssumed = stateNew;
                // fall through

            case LifeCycle::inflating:
                if (fInflator)
                    {
                    // this is the inflating thread, move to the inflated state
                    stateNew.value.nMonitor = LifeCycle::inflated;
                    stateOld = m_atomicLifeCycleState.update(stateAssumed, stateNew);
                    }
                else
                    {
                    // wait for the inflating thread to complete
                    Thread::yield();
                    stateOld = m_atomicLifeCycleState.get();
                    }
                break;

            case LifeCycle::inflated:
                return *m_pCondition;

            default:
                COH_THROW_STREAM (IllegalStateException,
                    "Invalid Monitor inflation state: " << stateOld);
            }
        }
    }

TypedHandle<WeakReference> Object::_ensureWeakReference() const
    {
    // post-inflated optimization
    if (LifeCycle::Refs(m_atomicLifeCycleRefs.peek()).value.nWeakReference ==
        LifeCycle::inflated)
        {
        return m_pWeakThis;
        }

    LifeCycle::Refs refsOld, refsAssumed, refsNew;
    bool            fInflator = false;

    refsOld = m_atomicLifeCycleRefs.peek();
    bool fEscapedThis = LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped;
    while (true)
        {
        refsAssumed = refsNew = refsOld;

        switch (refsOld.value.nWeakReference)
            {
            case LifeCycle::flat:
                {
                // try to acquire allocation right
                refsNew.value.nWeakReference = LifeCycle::inflating;
                refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew, fEscapedThis);
                if (refsOld != refsAssumed)
                    {
                    break;
                    }

                // this is now the inflating thread
                fInflator = true;

                // cast away constness and attempt to attach via a Handle
                // this will only succeed if there are still other Handles
                // to this Object
                Handle                hThis = (Object*) this;
                WeakReference::Handle hWeak = NULL == hThis
                        ? WeakReferenceImpl::create((Object::View) this)
                        : WeakReferenceImpl::create(hThis);

                hWeak->_attach(fEscapedThis);
                m_pWeakThis   = get_pointer(hWeak);
                refsAssumed = refsNew;
                }
                // fall through

            case LifeCycle::inflating:
                if (fInflator)
                    {
                    // this is the inflating thread, move to the inflated state
                    refsNew.value.nWeakReference = LifeCycle::inflated;
                    refsOld = m_atomicLifeCycleRefs.update(refsAssumed, refsNew, fEscapedThis);
                    }
                else
                    {
                    // wait for the inflating thread to complete
                    Thread::yield();
                    refsOld = m_atomicLifeCycleRefs.get();
                    }
                break;

            case LifeCycle::inflated:
                return m_pWeakThis;

            default:
                COH_THROW_STREAM (IllegalStateException,
                    "Invalid WeakReferece inflation state: " << refsOld);
            }
        }
    }


// ----- static member functions --------------------------------------------

bool Object::equals(Object::View v1, Object::View v2)
    {
    if (v1 == v2)
        {
        return true;
        }

    if (NULL == v1 || NULL == v2)
        {
        return false;
        }

    try
        {
        return v1->equals(v2);
        }
    catch (const std::bad_cast&)
        {
        return false;
        }
    }

Object::Handle Object::clone(Object::View v)
    {
    return NULL == v ? (Object::Handle) NULL : v->clone();
    }

TypedHandle<const String> Object::toString(Object::View v)
    {
    return String::valueOf(v);
    }

// ----- memory management --------------------------------------------------

void* Object::operator new(size_t cb)
    {
    return Allocator::allocate(cb);
    }

void Object::operator delete(void* ab)
    {
    Allocator::release(ab);
    }

COH_CLOSE_NAMESPACE2
