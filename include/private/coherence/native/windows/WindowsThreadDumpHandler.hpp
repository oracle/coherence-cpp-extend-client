/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_THREADDUMP_HANDLER_HPP
#define COH_WINDOWS_THREADDUMP_HANDLER_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThreadDumpHandler.hpp"

#include <windows.h>

using namespace std;

// ----- file local helpers -------------------------------------------------

namespace
    {
    BOOL WINAPI CtrlHandler(DWORD dwEvent)
        {
        BOOL bHandled = FALSE;

        switch (dwEvent)
            {
            case CTRL_BREAK_EVENT:
              coherence::lang::Thread::currentThread()
                  ->setName("Signal Dispatcher");
              coherence::native::NativeThreadDumpHandler::doThreadDump();
              bHandled = TRUE;
              break;
            }
        return bHandled;
        }
    }

COH_OPEN_NAMESPACE3(coherence,native,windows)

/**
* WindowsThreadDumpInitializer initializes the WindowsThreadDump api.
*/
class WindowsThreadDumpInitializer
    {
    private:
        WindowsThreadDumpInitializer()
                : m_fCleanup(false)
            {
            SetConsoleCtrlHandler(CtrlHandler, TRUE);

            // record that cleanup in necessary when coherence library is
            // unloaded
            m_fCleanup = true;
            }

        ~WindowsThreadDumpInitializer()
            {
            if (m_fCleanup)
                {
                m_fCleanup = false;
                SetConsoleCtrlHandler(CtrlHandler, FALSE);
                }
            }

    public:
        /**
        * Ensure the handler is enabled
        */
        static void ensure()
            {
            static WindowsThreadDumpInitializer singleton;
            }

    private:
        bool m_fCleanup;
    };
COH_STATIC_INIT(WindowsThreadDumpInitializer::ensure());

COH_CLOSE_NAMESPACE3

#endif // COH_WINDOWS_THREADDUMP_HANDLER_HPP
