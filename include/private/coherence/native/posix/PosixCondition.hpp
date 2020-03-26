/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_CONDITION_HPP
#define COH_POSIX_CONDITION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"

#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/posix/PosixMutex.hpp"

#include <iostream>
#include <pthread.h>
#include <sstream>
#include <sys/errno.h>
#include <sys/time.h>

COH_OPEN_NAMESPACE3(coherence,native,posix)


/**
* Posix implementation of Condition.
*
* Note: this Condition implementation is only compatible with Mutex's deriving
*       from PosixMutex.
*/
class COH_EXPORT PosixCondition
    : public NativeCondition, public PosixMutex
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create a PosixCondition associated with a given PosixMutex.
        */
        PosixCondition()
            : PosixMutex(true),
              m_cNotifications(0),
              m_cNotifyOnRelease(0)
            {
            COH_ENSURE_EQUALITY(pthread_cond_init(&m_cond, NULL), 0);
            }

        /**
        * @internal
        */
        virtual ~PosixCondition()
            {
            COH_ENSURE_EQUALITY(pthread_cond_destroy(&m_cond), 0);
            }


    // ----- Condition interface --------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void wait(int32_t cMillis)
            {
            COH_ENSURE_PARAM_RELATION(cMillis, >, 0);

            assertOwnership();

            int32_t cLock = preWaitUnlock();
            try
                {
                struct timeval  tvStart;
                struct timespec tsExp;

                COH_ENSURE_EQUALITY(gettimeofday(&tvStart, NULL), 0);

                tsExp.tv_sec  = tvStart.tv_sec;
                tsExp.tv_nsec = tvStart.tv_usec * 1000;
                if (cMillis >= 1000)
                    {
                    tsExp.tv_sec += cMillis / 1000;
                    cMillis      %= 1000;
                    }
                tsExp.tv_nsec += ((long) cMillis) * 1000000L;
                if (tsExp.tv_nsec >= 1000000000L)
                    {
                    ++tsExp.tv_sec;
                    tsExp.tv_nsec -= 1000000000L;
                    }

                int nResult = pthread_cond_timedwait(&m_cond,
                        getPThreadMutex(), &tsExp);
                switch (nResult)
                    {
                    case 0: // wakeup, may be intentional or spurious
                    case ETIMEDOUT: // timeout
                        break;

                    default:
                        COH_THROW_STREAM(IllegalStateException,
                            "unexpected result during Condition.wait() == "
                            << nResult);
                    }

                postWaitLock(cLock);
                }
            catch (...)
                {
                postWaitLock(cLock);
                throw;
                }
            }

        /**
        * @inheritDoc
        */
        virtual void notify()
            {
            assertOwnership();
            ++m_cNotifications;
            ++m_cNotifyOnRelease; // increment because multiple notifies should be multiple signals
            // see unlock below; defer COH_ENSURE_EQUALITY(pthread_cond_signal(&m_cond), 0);
            }

        /**
        * @inheritDoc
        */
        virtual void notifyAll()
            {
            assertOwnership();
            ++m_cNotifications;
            m_cNotifyOnRelease += 2; // technically MAX_INT but see unlock below, we treat > 1 as broadcast
            // see unlock below; defer COH_ENSURE_EQUALITY(pthread_cond_broadcast(&m_cond), 0);
            }

        /**
        * @inheritDoc
        */
        virtual int32_t getNotificationCount() const
            {
            return m_cNotifications;
            }

        /**
        * @inheritDoc
        */
        virtual size32_t sizeOf() const
            {
            return (size32_t) sizeof(PosixCondition);
            }

    // ----- Mutex interface ------------------------------------------------

    virtual void unlock()
        {
        int32_t cLock   = m_cLock;            // clean read while holding the lock
        int32_t cNotify = m_cNotifyOnRelease; // clean read while holding the lock

        if (cLock == 1)
            {
            m_cNotifyOnRelease = 0; // reset if this is a full release, while still holding the lock
            }

        PosixMutex::unlock();

        // now that we're unlocked we can signal the blocked thread without
        // risking that they wake up only to contend with us as they could
        // if we signaled before releasing the lock.  Note this is allowable
        // as signal/broadcast doesn't require that the caller hold the lock
        if (cLock == 1) // we just did the final unlock
            {
            // see duplicate of this code in preWaitUnlock
            if (cNotify == 1)
                {
                COH_ENSURE_EQUALITY(pthread_cond_signal(&m_cond), 0);
                }
            else if (cNotify > 1)
                {
                COH_ENSURE_EQUALITY(pthread_cond_broadcast(&m_cond), 0);
                }
            }
        }

    // ----- helpers --------------------------------------------------------

    /**
    * Unlock the mutex to the point where it is only locked once, this can
    * only be called by the mutex holder
    *
    * @return the number of times the Mutex needs to be relocked
    */
    int32_t preWaitUnlock()
        {
        // Note: we intentionaly over decrement m_cLock by one, that is
        // if we perform N unlock() calls, we perform N+1 decrements (each
        // unlock does a decrement.  The reason is that pthread_cond_wait
        // will internally perform the final pthread_mutex_unlock, and we
        // need to account for that lock, leaving m_cLock == 0 while we
        // are in wait
        int32_t cLock = --m_cLock;

        for (int32_t i = 0; i < cLock; ++i)
            {
            unlock();
            }

        // we're going to leave this fully unlocked once we invoke wait, if
        // we have a pending signal we must issue it now, note this won't
        // wake this thread up from wait, but if there was another thread
        // also in wait it would effect it.  Note we can't actually do this
        // after fully unlocking as we do above, so it isn't an optimization
        // here, but is needed for correctness
        int32_t cNotify = m_cNotifyOnRelease; // clean read while holding the lock
        m_cNotifyOnRelease = 0; // reset it (note we still hold the lock)
        if (cNotify == 1)
            {
            COH_ENSURE_EQUALITY(pthread_cond_signal(&m_cond), 0);
            }
        else if (cNotify > 1)
            {
            COH_ENSURE_EQUALITY(pthread_cond_broadcast(&m_cond), 0);
            }

        return cLock;
        }

    /**
    * re-lock the mutex multiple times
    *
    * @param cLock  the number of times to lock the mutex, note this
    *               value is updatd based on the number of times it is
    *               actually gets locked
    */
    void postWaitLock(int32_t& cLock)
        {
        ++m_cLock; // To be here it we hold the lock; see note in preWaitUnlock
        while (cLock--)
            {
            lock();
            }
        }

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The posix condition.
        */
        pthread_cond_t m_cond;

        /**
        * The Notification count.
        */
        int32_t m_cNotifications;

        /**
         * Action to take upon fully unlocking the mutex
         * 0  - none
         * 1  - signal
         * >1 - broadcast
         */
        int32_t m_cNotifyOnRelease;
    };

COH_CLOSE_NAMESPACE3


// ---- NativeCondition factory methods -------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeCondition* NativeCondition::create()
    {
    return new coherence::native::posix::PosixCondition();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_CONDITION_HPP
