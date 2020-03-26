/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_THREAD_HPP
#define COH_POSIX_THREAD_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThread.hpp"

#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <sstream>

extern "C"
    {
    static void* coh_startPosix(void* vpArg);
    }

COH_OPEN_NAMESPACE3(coherence,native,posix)


class COH_EXPORT PosixThread
    : public NativeThread
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a new PosixThread
        */
        PosixThread(Runnable::Handle hRunnable, size32_t cBytesStack)
            : f_hRunnable(System::common(), hRunnable),
              m_cBytesStack(cBytesStack)
            {
            }


    // ----- NativeThread interface -----------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void start()
                {
                size32_t       cBytesStack = m_cBytesStack;
                pthread_attr_t attr;
                pthread_t      thread;

                COH_ENSURE_EQUALITY(pthread_attr_init(&attr), 0);
                COH_ENSURE_EQUALITY(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED), 0);
                if (cBytesStack != 0)
                    {
                    COH_ENSURE_EQUALITY(pthread_attr_setstacksize(&attr, cBytesStack), 0);
                    }
                COH_ENSURE_EQUALITY(pthread_create(&thread, &attr, &coh_startPosix, this), 0);
                COH_ENSURE_EQUALITY(pthread_attr_destroy(&attr), 0);
                }

        /**
        * @inheritDoc
        */
        virtual void run()
            {
            f_hRunnable->run();
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Thread's runnable.
        */
        FinalHandle<Runnable> f_hRunnable;

        /**
        * The configured stack size.
        */
        size32_t m_cBytesStack;
    };

COH_CLOSE_NAMESPACE3


// ----- helper function ----------------------------------------------------

extern "C"
    {
    /**
    * pthread compatibile start function
    *
    * @param pvArg start argument (pointer to NativeThread)
    *
    * @return NULL
    */
    static void* coh_startPosix(void* pvArg)
        {
        COH_AUTO_PTR<coherence::native::posix::PosixThread>
            pThread((coherence::native::posix::PosixThread*) pvArg);
        pThread->run();
        return NULL;
        }
    }


// ----- NativeThread static methods ----------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

void NativeThread::create(Runnable::Handle hRunnable, size32_t cBytesStack)
    {
    (new coherence::native::posix::PosixThread(hRunnable, cBytesStack))->start();
    }

void NativeThread::yield()
    {
    sched_yield();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_THREAD_HPP
