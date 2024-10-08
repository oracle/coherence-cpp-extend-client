/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_CONDITION_HPP
#define COH_WINDOWS_CONDITION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/native/NativeAtomic32.hpp"

#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/windows/WindowsMutex.hpp"

#include <iostream>
#include <time.h>
#include <windows.h>

COH_OPEN_NAMESPACE3(coherence,native,windows)

using namespace coherence::native;


/**
* Windows implementation of Condition.
*
* This implementation utilizes Windows Events which are not subject to the
* spurious wakeup problem. This implementation however can produce spurious
* wakeups, and users of Condition must account for them.
*
* @author 2007.12.10 mf
*/
class COH_EXPORT WindowsCondition
    : public NativeCondition, public WindowsMutex
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create a WindowsCondition.
        */
        WindowsCondition()
            : m_atomicWaiters(0),
              m_cAllowed(0),
              m_cNotifications(0)
            {
            m_eventBegin = CreateEvent(NULL, true, false, NULL);
            m_eventEnd   = CreateEvent(NULL, true, false, NULL);

            COH_ENSURE(m_eventBegin != NULL);
            COH_ENSURE(m_eventEnd   != NULL);

            // TODO - REMOVE THIS STATEMENT
            // test if it is an error to reset an unsignaled event
            // if it is we need to adjust some code
            COH_ENSURE(ResetEvent(m_eventBegin) != FALSE);
            }

        /**
        * @internal
        */
        virtual ~WindowsCondition()
            {
            COH_ENSURE(CloseHandle(m_eventBegin) != FALSE);
            COH_ENSURE(CloseHandle(m_eventEnd)   != FALSE);
            }


    // ----- Condition interface --------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void wait(int32_t cMillis)
            {
            COH_ENSURE_PARAM_RELATION(cMillis, >, 0);

            NativeAtomic32& atomicWaiters = m_atomicWaiters;
            HANDLE&         eventBegin    = m_eventBegin;

            assertOwnership();

            // only increment while mutex is locked, see notify(f)
            atomicWaiters.adjust(1);
            int32_t cLock = fullyUnlock();

            DWORD nResult = WaitForSingleObject(eventBegin, cMillis);
            switch (nResult)
                {
                case WAIT_OBJECT_0: // signaled (not spurious)
                case WAIT_TIMEOUT:  // timeout
                    break;

                default:
                    COH_THROW_STREAM(IllegalStateException,
                        "unexpected result during Condition.wait() == "
                        << nResult);
                }

            // Note: cleanup order intentionally identical to acquisition order
            if (atomicWaiters.adjust(-1) == m_cAllowed)
                {
                // all remaining threads are allowed to remain sleeping
                // reset eventBegin, and wakeup the notifier
                COH_ENSURE(ResetEvent(eventBegin) != FALSE);
                COH_ENSURE(SetEvent(m_eventEnd)   != FALSE);
                }

            // thread will sit here until the notifier is signaled and returns
            fullyLock(cLock);
            }

        /**
        * @inheritDoc
        */
        virtual void notify()
            {
            notify(false);
            }

        /**
        * @inheritDoc
        */
        virtual void notifyAll()
            {
            notify(true);
            }

        /**
        * @inheritDoc
        */
        virtual size32_t sizeOf() const
            {
            return (size32_t) sizeof(WindowsCondition);
            }

    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Notify a specific number of threads.
        *
        * @param cThreads  the number of threads to notify
        */
        void notify(bool fNotifyAll)
            {
            assertOwnership();

            // record each call to notify
            ++m_cNotifications;

            NativeAtomic32& atomicWaiters = m_atomicWaiters;
            int32_t         cWaiters      = atomicWaiters.get();
            // Note: the number of waiters cannot increase while we are in
            //       this method since we hold the mutex lock (see wait()),
            //       it can however shrink.
            if (cWaiters > 0)
                {
                HANDLE& eventBegin = m_eventBegin;
                HANDLE& eventEnd   = m_eventEnd;

                // it's possible that some waiting threads timeout before we
                // set cAllowed, and thus would not know to notify this thread
                while (true)
                    {
                    // ResetEvent should not be needed but in case it got left
                    // open by a thread timing out
                    COH_ENSURE(ResetEvent(eventEnd) != FALSE);

                    // ensure that no thread timed out between these two lines
                    m_cAllowed = fNotifyAll ? 0 : cWaiters - 1;
                    int32_t cCurr = atomicWaiters.get();

                    // validate that no thread timed out
                    if (cCurr == cWaiters)
                        {
                        // no threads timed out
                        break;
                        }
                    else if (cCurr == 0)
                        {
                        // all waiting threads timed out
                        return;
                        }
                    else if (cCurr < cWaiters)
                        {
                        // some but not all waiting threads timed out
                        // try again
                        cWaiters = cCurr;
                        }
                    else // cCurr > cWaiters
                        {
                        // threads were added, this can't happen
                        COH_THROW(IllegalStateException::create());
                        }
                    }

                // signal all waiting threads
                COH_ENSURE(SetEvent(eventBegin) != FALSE);

                // wait for at least the appropriate number of threads to
                // wakeup, the last required thread to wake up will signal
                // back to wake us up, that thread will also reset eventBegin
                COH_ENSURE_EQUALITY(WaitForSingleObject(eventEnd, INFINITE),
                        WAIT_OBJECT_0);

                // though likely already reset by the last thread to wake up
                // there is a slight chance that all threads timed out just
                // before we did SetEvent, in which case they could have call
                // ResetEvent before we did our SetEvent, and we want to ensure
                // we leave the Event reset
                COH_ENSURE(ResetEvent(eventBegin) != FALSE);
                }
            }

       /**
        * @inheritDoc
        */
        virtual int32_t getNotificationCount() const
            {
            return m_cNotifications;
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The number of threads currently waiting on this condition.
        */
        NativeAtomic32 m_atomicWaiters;

        /**
        * The number of threads allowed to remain asleep after notification.
        *
        * Note: On Windows volatile has the same memory semantics as Java,
        *       this is not guaranteed on other platforms, but this is
        *       a Windows specific file so we can utilize this.
        */
        volatile int32_t m_cAllowed;

        /**
        * The notification count.
        */
        int32_t m_cNotifications;

        /**
        * The manual reset event used to notify waiting threads.
        */
        HANDLE m_eventBegin;

        /**
        * The manual reset event used to inform the notifying thread that
        * all required waiting threads have awoken.
        */
        HANDLE m_eventEnd;
    };

COH_CLOSE_NAMESPACE3


// ---- NativeCondition factory methods -------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeCondition* NativeCondition::create()
    {
    return new coherence::native::windows::WindowsCondition();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_CONDITION_HPP
