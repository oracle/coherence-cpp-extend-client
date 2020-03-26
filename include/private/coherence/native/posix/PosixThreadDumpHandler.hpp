/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_THREADDUMP_HANDLER_HPP
#define COH_POSIX_THREADDUMP_HANDLER_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThreadDumpHandler.hpp"

#include <signal.h>

using namespace std;

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * flag indicating if the handler thread should stop.
    */
    static bool COH_HANDLER_STOP = false;

    extern "C"
        {
        static void coh_signal_loop()
            {
            bool     fNamed = false;
            sigset_t setSig;
            int      signal;

            sigemptyset(&setSig);
            sigaddset(&setSig, SIGQUIT);

            while (true)
                {
                if (sigwait(&setSig, &signal) == 0)
                    {
                    if (COH_HANDLER_STOP)
                        {
                        return;
                        }
                    else if (!fNamed)
                        {
                        // delay naming the thread until first signal
                        fNamed = true;
                        coherence::lang::Thread::currentThread()
                            ->setName("Signal Dispatcher");
                        }


                    switch (signal)
                        {
                        case SIGQUIT:
                            coherence::native::NativeThreadDumpHandler::doThreadDump();
                            break;

                        default:
                            break;
                        }
                    }
                }
            }

        static void* coh_signal_handler(void* /*pvArg*/)
            {
            coh_signal_loop();
            pthread_exit(NULL);
            return NULL;
            }
        }
    }

COH_OPEN_NAMESPACE3(coherence,native,posix)

/**
* PosixThreadDumpInitializer initializes the PosixThreadDump api.
*/
class PosixThreadDumpInitializer
    {
    private:
        PosixThreadDumpInitializer()
                : m_fCleanup(false)
            {
            sigset_t       setSig;
            pthread_attr_t attr;

            sigemptyset(&setSig);
            sigaddset(&setSig, SIGQUIT);
            pthread_sigmask(SIG_BLOCK, &setSig, NULL); // block all -3 signals on main thread

            COH_ENSURE_EQUALITY(pthread_attr_init(&attr), 0);
            // COH-7977 run as a daemon thread
            COH_ENSURE_EQUALITY(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED), 0);
            COH_ENSURE_EQUALITY(pthread_create(&m_thread, &attr, &coh_signal_handler, NULL), 0);
            COH_ENSURE_EQUALITY(pthread_attr_destroy(&attr), 0);

            // record that cleanup is necessary when coherence library is unloaded
            m_fCleanup = true;
            }

        ~PosixThreadDumpInitializer()
            {
            if (m_fCleanup)
                {
                m_fCleanup       = false;
                COH_HANDLER_STOP = true;

                // signal thread but don't wait for it to exit as a detached thread cannot be joined
                pthread_kill(m_thread, SIGQUIT);
                }
            }

    public:
        /**
        * Ensure the handler is enabled
        */
        static void ensure()
            {
            static PosixThreadDumpInitializer singleton;
            }

    private:
        bool      m_fCleanup;
        pthread_t m_thread;
    };
COH_STATIC_INIT(PosixThreadDumpInitializer::ensure());

COH_CLOSE_NAMESPACE3

#endif // COH_POSIX_THREADDUMP_HANDLER_HPP
