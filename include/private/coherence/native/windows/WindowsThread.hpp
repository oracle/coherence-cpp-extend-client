/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_THREAD_HPP
#define COH_WINDOWS_THREAD_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThread.hpp"

#include <iostream>
#include <sstream>
#include <windows.h>

COH_OPEN_NAMESPACE3(coherence,native,windows)


class COH_EXPORT WindowsThread
    : public NativeThread
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a new WindowsThread
        */
        WindowsThread(Runnable::Handle hRunnable, size32_t cBytesStack)
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
            HANDLE thread;
            COH_ENSURE((thread = CreateThread(NULL, m_cBytesStack,
                &startWindows, this, 0, NULL)) != NULL);
            // release thread so that it can self destruct upon completion
            COH_ENSURE(CloseHandle(thread) != FALSE);
            }

        /**
        * @inheritDoc
        */
        virtual void run()
            {
            f_hRunnable->run();
            }


    // ----- helper methods -------------------------------------------------

    /**
    * windows compatibile start function
    *
    * @param pvArg start argument (pointer to NativeThread)
    *
    * @return 0
    */
    static DWORD WINAPI startWindows(LPVOID pvArg)
        {
        COH_AUTO_PTR<WindowsThread> apThread((WindowsThread*) pvArg);
        apThread->run();
        return 0;
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


// ----- NativeThread factory methods ---------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

void NativeThread::create(Runnable::Handle hRunnable, size32_t cBytesStack)
    {
    (new coherence::native::windows::WindowsThread(hRunnable, cBytesStack))->start();
    }

void NativeThread::yield()
    {
    SwitchToThread();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_THREAD_HPP
