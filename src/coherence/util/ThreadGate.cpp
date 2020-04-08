/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ThreadGate.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)


// ----- local class: AtomicState -------------------------------------------

/**
* AtomicState is a data structure that tracks the state of the ThreadGate
* including the number of unmatched enter calls, the state of the gate
*/
union AtomicState
    {
    // ----- constructors -----------------------------------------------

    /**
    * Construct an AtomicState from a raw int32_t
    *
    * @param nraw
    */
    AtomicState(const int32_t nraw = 0)
        : raw(nraw)
        {
        }

    /**
    * Atomatic type conversion to int32_t
    */
    operator int32_t() const
        {
        return raw;
        }

    /**
    * Enum representing the different fields held by the atomic state.
    */
    typedef enum
        {
        ENTERS,
        STATUS
        } Field;

    // ----- data members -----------------------------------------------

    /**
    * Raw uninterpreted bit field, used for interation with AtomicCounter.
    */
    int32_t raw;

    /**
    * Structured representation of state.
    */
    struct
        {
        /**
        * The current state of the ThreadGate.
        */
        unsigned int nStatus : 2;

        /**
        * The number of threads that have entered the gate.
        */
        unsigned int cEnters : 30; // see max_enters
        } value;

    // ----- helper methods ---------------------------------------------

    /**
    * Atomically update the state of specified field.
    *
    * @param lState   the state to update
    * @param nField   the field of the state to update
    * @param cAdjust  the new value of the state or the amount to adjust the
    *                 state by
    *
    * @return the new value for the state
    */
    static AtomicState adjustState(NativeAtomic32& nState,
            AtomicState::Field nField, int32_t cAdjust)
        {
        AtomicState nStatusOld, nStatusAssumed, nStatusNew;

        bool fDirty = true;
        nStatusOld = nState.peek();
        do
            {
            REDO:
            nStatusNew = nStatusAssumed = nStatusOld;
            switch(nField)
                {
                case ENTERS:
                    if (nStatusNew.value.cEnters += cAdjust
                            == ThreadGate::max_enters)
                        {
                        if (fDirty)
                            {
                            nStatusOld = nState.get();
                            fDirty = false;
                            goto REDO; // avoid continue
                            }
                        COH_ENSURE(int32_t(nStatusNew.value.cEnters += cAdjust) != ThreadGate::max_enters);
                        }

                    break;

                case STATUS:
                    nStatusNew.value.nStatus = (unsigned int) cAdjust;

                    break;

                default:
                    COH_THROW_STREAM(IllegalArgumentException,
                                     "unknown field: " << nField);
                }
            nStatusOld = nState.update(nStatusAssumed, nStatusNew);
            }
        while (nStatusOld != nStatusAssumed);

        return nStatusNew;
        }
    };

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t ThreadGate::max_enters)
COH_EXPORT_SPEC_MEMBER(const int64_t ThreadGate::infinite)
COH_EXPORT_SPEC_MEMBER(const int64_t ThreadGate::immediate)


// ----- constructor --------------------------------------------------------

ThreadGate::ThreadGate()
    : m_nAtomicState(0), m_cClose(0),
      m_vClosingThread(self()),
      f_htlcEnters(self(), ThreadLocalReference::create(), /*fMutable*/ true)
    {
    }


// ----- ThreadGate interface -----------------------------------------------

bool ThreadGate::barEntry(int64_t cMillis)
    {
    Thread::View vThread = Thread::currentThread();
    if (getClosingThread() == vThread)
        {
        // we've already closed or are closing the gate
        setCloseCount(getCloseCount() + 1);
        return true;
        }

    COH_SYNCHRONIZED(this)
        {
        while (true)
            {
            if (NULL == getClosingThread())
                {
                if (updateStatus(gate_closing) == gate_destroyed)
                    {
                    // oops gate was destroyed while we were waiting
                    updateStatus(gate_destroyed);
                    COH_THROW (IllegalStateException::create("Threadgate.close:"
                                  " ThreadGate has been destroyed."));
                    }

                setClosingThread(vThread);
                setCloseCount(1);
                return true;
                }
            // gate is already closed or closing, wait for notification
            cMillis = doWait(cMillis);
            if (cMillis == 0)
                {
                return false;
                }
            }
        }
    }

bool ThreadGate::close(int64_t cMillis)
    {
    Thread::View vThread = Thread::currentThread();
    if (getClosingThread() == vThread && getStatus() == gate_closed)
        {
        // we've already closed the gate
        setCloseCount(getCloseCount() + 1);
        return true;
        }

    NativeAtomic32&       lAtomicState = m_nAtomicState;
    int32_t               cEnterThis   = getCounter()->getCount();
    bool                  fReenter     = false;
    bool                  fReopen      = false;

    // This is the state we need in order to close the gate
    // In order to cleanly support lock promotion, the number of enters must
    // be 0 to close the gate
    AtomicState statusReq;
    statusReq.value.nStatus = gate_open;
    statusReq.value.cEnters = 0;

    // This will be the state of the gate once the gate has been closed
    AtomicState statusEnd;
    statusEnd.value.nStatus = gate_closed;
    statusEnd.value.cEnters = 0;

    COH_SYNCHRONIZED (this)
        {
        try
            {
            if (getClosingThread() == vThread)
                {
                statusReq.value.nStatus = gate_closing;
                statusReq.value.cEnters = 0;

                // if we've also "entered" we need to temporarily decrement
                // the counter so that the last thread to exit the gate will
                // know to notify us.
                if (cEnterThis > 0)
                    {
                    fReenter = true;
                    AtomicState::adjustState(lAtomicState, AtomicState::ENTERS, -1);
                    }
                }

            while (true)
                {
                Thread::View vClosingThread = getClosingThread();

                if (statusReq == lAtomicState.update(statusReq, statusEnd))
                    {
                    // we've closed the gate
                    setCloseCount(getCloseCount() + 1);
                    setClosingThread(vThread);
                    fReopen = false; // we've got the lock don't open it now
                    closeFinally(fReenter, fReopen); // make sure to cleanup
                    return true;
                    }
                else if (NULL == vClosingThread)
                    {
                    // transition to CLOSING state
                    if (updateStatus(gate_closing) == gate_destroyed)
                        {
                        // oops gate was destroyed while we were waiting
                        updateStatus(gate_destroyed);
                        COH_THROW (IllegalStateException::create(
                            "ThreadGate.close: ThreadGate has been destroyed."));

                        }
                    setClosingThread(vThread);

                    statusReq.value.nStatus = gate_closing;
                    statusReq.value.cEnters = 0;

                    fReopen                 = true;

                    // if we've also "entered we need to temporarily decrement
                    // the counter so that the last thread to exit the gate
                    // will notify us
                    if (cEnterThis > 0 && !fReenter)
                        {
                        fReenter = true;
                        AtomicState::adjustState(lAtomicState, AtomicState::ENTERS, -1);
                        }

                    // as we've just transitioned to CLOSING we must retest
                    // the active count since existing threads only notify
                    // when the state is CLOSING, thus we can't go to doWait
                    // without retesting
                    continue;
                    }
                else if (vClosingThread != vThread)
                    {
                    if (cEnterThis > 0 && !fReenter)
                        {
                        // if we've also "entered" we need to temporarily
                        // decrement the counter so that the last thread to
                        // exit the gate will notify us
                        fReenter = true;

                        AtomicState status =
                            AtomicState::adjustState(lAtomicState,
                                AtomicState::ENTERS, -1);

                        if (status.value.cEnters == 0)
                            {
                            notifyAll();
                            }
                        }
                    }

                // gate is closed or closing, wait for notification
                cMillis = doWait(cMillis);
                if (cMillis == 0)
                    {
                    closeFinally(fReenter, fReopen);
                    return false;
                    }
                }
            }
            catch (...)
                {
                closeFinally(fReenter, fReopen);
                throw;
                }
        }
    }

void ThreadGate::destroy()
    {
    COH_SYNCHRONIZED(this)
        {
        switch(getStatus())
            {
            case gate_closed:
                {
                Thread::View vThread = Thread::currentThread();
                if (vThread != getClosingThread())
                    {
                    COH_THROW_STREAM(IllegalStateException,
                        "ThreadGate.destryo: Gate was not closed by this thread! "
                        << self());
                    }
                updateStatus(gate_destroyed);
                setClosingThread(NULL);
                notifyAll();
                }
                break;

            case gate_destroyed:
                // the gate has already been destoyed
                break;

            default:
                COH_THROW_STREAM(IllegalStateException,
                                 "ThreadGate.destroy: Gate is not closed! "
                                 << self());
            }
        }
    }

bool ThreadGate::enter(int64_t cMillis)
    {
    NativeAtomic32&             lAtomicState = m_nAtomicState;
    ThreadGate::Counter::Handle tlcEnters    = getCounter();
    int32_t                     cEntersLocal = 0;

    if ((cEntersLocal = tlcEnters->increment()) > 1)
        {
        // we were already in the gate or are the one which has closed it thus
        // we must get in regardless of the state
        if (cEntersLocal == ThreadGate::max_enters)
            {
            // we've hit the max number of Reenters allowed
            tlcEnters->decrement();
            COH_THROW (IllegalStateException::create("Max number of enters reached for"
                                                    " this thread."));
            }
        return true;
        }

    AtomicState currState = lAtomicState.peek();
    if (currState.value.nStatus != gate_open)
        {
        if (getClosingThread() == Thread::currentThread())
            {
            // we have closed the gate, but have not yet entered it, make sure the
            // atomic is up to date.
            AtomicState::adjustState(lAtomicState, AtomicState::ENTERS, 1);
            return true;
            }
        }

    bool fSuccess = false;
    try
        {
        bool fLoop = true;
        while (fLoop)
            {
            AtomicState assumedStatus = lAtomicState.peek();
            AtomicState updatedStatus = assumedStatus;
            ++updatedStatus.value.cEnters;

            switch(assumedStatus.value.nStatus)
                {
                case gate_open:
                    if (assumedStatus ==
                        lAtomicState.update(assumedStatus, updatedStatus))
                        {
                        // atomic update succeeded confirming that the gate
                        // remained open and we made it in
                        fSuccess = true;
                        fLoop    = false; // stop looping
                        }
                    break; // retry

                case gate_closing:
                case gate_closed:
                    // we know that we're not already in the gate and are not the
                    // one closing the gate; wait for it to open
                    COH_SYNCHRONIZED(this)
                    {
                    assumedStatus = lAtomicState.get();
                    if (assumedStatus.value.nStatus == gate_closing ||
                        assumedStatus.value.nStatus == gate_closed)
                        {
                        cMillis = doWait(cMillis);
                        if (cMillis == 0)
                            {
                            fSuccess = false;
                            fLoop    = false; // stop looping
                            }
                        }
                    }
                break; // retry

                case gate_destroyed:
                    tlcEnters->decrement();
                    COH_THROW (IllegalArgumentException::create(
                        "ThreadGate::enter: ThreadGate has been destroyed."));

                default:
                    tlcEnters->decrement();
                    COH_THROW_STREAM(IllegalArgumentException,
                        "ThreadGate::enter: ThreadGate has an invalid status."
                        << self());

                }
            }
        }
    catch (...)
        {
        enterFinally(tlcEnters, fSuccess);
        throw;
        }

    enterFinally(tlcEnters, fSuccess);
    return fSuccess;
    }

void ThreadGate::exit()
    {
    ThreadGate::Counter::Handle tlcEnters    = getCounter();
    int32_t                     cEntersLocal = tlcEnters->decrement();

    if (cEntersLocal == 0)
        {
        AtomicState status =
            AtomicState::adjustState(m_nAtomicState, AtomicState::ENTERS, -1);

        if (status.value.nStatus == gate_closing
            && status.value.cEnters == 0)
            {
            // we were the last to exit and the gate is in the CLOSING state
            // notify everyone to ensure that we notify the closing threads
            COH_SYNCHRONIZED(this)
                {
                notifyAll();
                }
            }
        }

    if (cEntersLocal < 0)
        {
        tlcEnters->increment();
        COH_THROW_STREAM(IllegalStateException,
                         "ThreadGate::exit: Thread has already exited: "
                         << self());
        }
    }

void ThreadGate::open()
    {
    if (Thread::currentThread() == getClosingThread())
        {
        int32_t cClosed = getCloseCount() - 1;
        if (cClosed >= 0)
            {
            setCloseCount(cClosed);
            if (cClosed == 0)
                {
                COH_SYNCHRONIZED(this)
                    {
                    updateStatus(gate_open);
                    setClosingThread(NULL);
                    notifyAll();
                    }
                }
            }
        return;
        }
    COH_THROW_STREAM(IllegalStateException,
                     "ThreadGate::open: Gate was not closed by this thread: "
                     << self());
    }

int32_t ThreadGate::getActiveCount() const
    {
    AtomicState status = m_nAtomicState.get();
    return status.value.cEnters;
    }

bool ThreadGate::isActiveThread() const
    {
    ThreadGate::Counter::View tlcEnters = getCounter();
    return tlcEnters->getCount() > 0;
    }

int32_t ThreadGate::getCloseCount() const
    {
    return m_cClose;
    }

ThreadGate::Status ThreadGate::getStatus() const
    {
    AtomicState status = m_nAtomicState.get();
    return (ThreadGate::Status)status.value.nStatus;
    }

void ThreadGate::setCloseCount(int32_t cClose)
    {
    m_cClose = cClose;
    }

Thread::View ThreadGate::getClosingThread() const
    {
    return m_vClosingThread;
    }

void ThreadGate::setClosingThread(Thread::View vThread)
    {
    m_vClosingThread = vThread;
    }

ThreadGate::Status ThreadGate::updateStatus(ThreadGate::Status nStatus)
    {
    AtomicState state =
            AtomicState::adjustState(m_nAtomicState, AtomicState::STATUS,
                                     nStatus);

    return (ThreadGate::Status)state.value.nStatus;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ThreadGate::toString() const
    {
    String::View vsState;
    switch(getStatus())
        {
        case gate_open:
            vsState = "gate_open";
            break;
        case gate_closing:
            vsState = "gate_closing";
            break;
        case gate_closed:
            vsState = "gate_closed";
            break;
        case gate_destroyed:
            vsState = "gate_destroyed";
            break;
        default:
            vsState = "invalid";
            break;
        }

    return COH_TO_STRING("ThreadGate{State="  << vsState <<
            ", ActiveCount="    << getActiveCount() <<
            ", CloseCount="     << getCloseCount() <<
            ", ClosingThread= " << getClosingThread() <<
            "}");
    }


// ----- inner class: Counter -----------------------------------------------

// ----- Counter: constructor -------------------------------------------

ThreadGate::Counter::Counter(int32_t c)
    : m_c(c)
    {
    }

// ----- Counter: Counter interface -------------------------------------

int32_t ThreadGate::Counter::getCount() const
    {
    return m_c;
    }

void ThreadGate::Counter::setCount(int32_t c)
    {
    m_c = c;
    }

int32_t ThreadGate::Counter::increment()
    {
    return ++m_c;
    }

int32_t ThreadGate::Counter::decrement()
    {
    return --m_c;
    }

// ----- Counter: Object interface --------------------------------------

TypedHandle<const String> ThreadGate::Counter::toString() const
    {
    return COH_TO_STRING("ThreadLocalCounter{ThreadId=" << Thread::currentThread()->getId()
        << " count=" << m_c << '}');
    }


// ----- GateBlock: constructor -----------------------------------------

ThreadGate::GateBlock::GateBlock(BlockType nType, ThreadGate::Handle hGate,
        int64_t cMillis)
        : m_hGate(NULL), m_nType(nType)
    {
    bool fResult = false;
    switch (nType)
        {
        case bar:
            fResult = hGate->barEntry(cMillis);
            break;

        case enter:
            fResult = hGate->enter(cMillis);
            break;

        case close:
            fResult = hGate->close(cMillis);
            break;
        }

    if (fResult)
        {
        m_hGate = hGate;
        }
    }

ThreadGate::GateBlock::GateBlock(const GateBlock& that)
    {
    m_hGate = that.m_hGate;
    m_nType = that.m_nType;

    that.m_hGate = NULL;
    }

ThreadGate::GateBlock::~GateBlock()
    {
    ThreadGate::Handle hGate = m_hGate;
    if (NULL != hGate)
        {
        m_hGate = NULL;
        try
            {
            switch(m_nType)
                {
                case enter:
                    hGate->exit();
                    break;

                case close:
                case bar:
                    hGate->open();
                    break;
                }
            }
        catch (Exception::View e)
            {
            COH_LOG("Error resetting gate to previous state: " << e, 1);
            // eat it
            }
        }
    }

ThreadGate::GateBlock::operator bool() const
    {
    return NULL != m_hGate;
    }


// ----- internal helpers ---------------------------------------------------

int64_t ThreadGate::doWait(int64_t cMillis)
    {
    if (cMillis == 0)
        {
        return 0;
        }

    int64_t lTime = System::currentTimeMillis();

    try
        {
        wait(std::max((int64_t) 0, cMillis));
        }
    catch (InterruptedException::View e)
        {
        Thread::currentThread()->interrupt();
        COH_THROW (e);
        }

    int64_t lEnd = System::currentTimeMillis();

    if (lEnd < lTime)
        {
        return 0;
        }

    return cMillis < 0 ? cMillis : std::max((int64_t) 0, (cMillis - lEnd - lTime));
    }

ThreadGate::Counter::Handle ThreadGate::getCounter() const
    {
    Counter::Handle hLC =
        cast<Counter::Handle>(f_htlcEnters->get());

    if (NULL == hLC)
        {
        hLC = Counter::create();
        f_htlcEnters->set(hLC);
        }

    return hLC;
    }

void ThreadGate::closeFinally(bool fReenter, bool fReopen)
    {
    if (fReenter)
        {
        //decrement the local counter
        getCounter()->decrement();

        // Reenter the gate using the enter method to ensure we wait if
        // we were not the thread to close the gate and we timed out
        enter(-1);
        }

    if (fReopen)
        {
        setClosingThread(NULL);
        updateStatus(gate_open);
        notifyAll();
        }
    }

void ThreadGate::enterFinally(Counter::Handle hCounter, bool fSuccess)
    {
    if (!fSuccess)
        {
        hCounter->decrement();
        }
    }

COH_CLOSE_NAMESPACE2
