/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/Queue.hpp"
#include "private/coherence/component/util/QueueProcessor.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::component::util::Daemon;
using coherence::lang::Exception;

COH_OPEN_NAMESPACE (test)

class TestDaemon
    : public class_spec<TestDaemon,
        extends<Daemon> >
    {
    friend class factory<TestDaemon>;

    public:
        typedef Daemon base;

    protected:
        TestDaemon(bool fExceptionOnStart = false)
            :  m_fEntered(false), m_fExited(false), m_fException(false),
               m_fInterrupted(false), m_fNotified(false), m_fWaited(false),
               m_fExceptionOnStart(fExceptionOnStart)
            {

            }

    // Daemon interface
    public:
        virtual void onEnter()
            {
            m_fEntered = true;

            if (m_fExceptionOnStart)
                {
                COH_THROW(IllegalStateException::create("Exception On Start"));
                }
            }

        virtual void onException(Exception::Holder)
            {
            m_fException = true;
            }

        virtual void onExit()
            {
            COH_SYNCHRONIZED(this)
                {
                m_fExited = true;
                notifyAll();
                }
            }

        virtual void onInterrupt(InterruptedException::Holder)
            {
            COH_SYNCHRONIZED(this)
                {
                m_fInterrupted = true;
                notifyAll();
                }
            }

        virtual void onNotify()
            {
            COH_SYNCHRONIZED(this)
                {
                m_fNotified = true;
                notifyAll();
                }
            }

        virtual void onWait()
            {
            COH_SYNCHRONIZED(this)
                {
                m_fWaited = true;
                notifyAll();

                // Ensure that notify fired back from main thread won't be
                // missed
                base::onWait();
                }
            }

    public:
        bool m_fEntered;
        bool m_fExited;
        bool m_fException;
        bool m_fInterrupted;
        bool m_fNotified;
        bool m_fWaited;

        //Input variable used to throw an exception on startup
        bool m_fExceptionOnStart;
    };

COH_CLOSE_NAMESPACE // test

using namespace test;

class DaemonTest : public CxxTest::TestSuite
    {
    public:
        void testDaemonState()
            {
            TestDaemon::Handle hTD = TestDaemon::create();
            hTD->setThreadName("testDaemonState");
            TS_ASSERT(hTD->getDaemonState() == Daemon::daemon_initial);

            hTD->start();
            TS_ASSERT(hTD->getDaemonState() == Daemon::daemon_running);

            hTD->stop();

            if (hTD->getDaemonState() >= Daemon::daemon_exiting)
                {
                while (hTD->getDaemonState() != Daemon::daemon_exited)
                    {
                    Thread::currentThread()->yield();
                    }
                }
            else
                {
                TS_FAIL("Stop called but Daemon is not exiting");
                }

            TS_ASSERT(hTD->getDaemonState() == Daemon::daemon_exited);
            }

        void testDaemonEvents()
            {
            TestDaemon::Handle hTD = TestDaemon::create(false);

            hTD->setThreadName("testDaemonEvents");
            hTD->start();

            //Ensure we ran onEnter
            TS_ASSERT(hTD->m_fEntered == true);

            //Ensure that we hit onWait()
            COH_SYNCHRONIZED(hTD)
                {
                while (!hTD->m_fWaited)
                    {
                    hTD->wait();
                    }
                }

            //Ensure that we hit onNotify
            COH_SYNCHRONIZED(hTD->getLock())
                {
                hTD->getLock()->notify();
                }

            COH_SYNCHRONIZED(hTD)
                {
                while (!hTD->m_fNotified)
                    {
                    hTD->wait(100);
                    hTD->notify();
                    }
                }

            //Ensure that we hit onExit
            hTD->stop();
            COH_SYNCHRONIZED(hTD)
                {
                while(!hTD->m_fExited)
                    {
                    hTD->wait();
                    }
                }
            }

        void testExceptionEvent()
            {
            TestDaemon::Handle hTD = TestDaemon::create(true);
            hTD->setThreadName("testExceptionEvent");

            try
                {
                hTD->start();
                }
            catch (Exception::View)
                {
                TS_ASSERT(hTD->m_fException == true);
                }

            hTD->stop();
            }

        void testInterrupt()
            {
            TestDaemon::Handle hTD = TestDaemon::create(false);
            hTD->setThreadName("testInterrupt");

            hTD->start();

            while (hTD->getDaemonState() != Daemon::daemon_running)
                {
                Thread::currentThread()->sleep(10);
                }

            Thread::Handle hThread = hTD->getThread();
            hThread->interrupt();

            COH_SYNCHRONIZED(hTD)
                {
                while (hTD->m_fInterrupted == false)
                    {
                    hTD->wait();
                    }
                }

            TS_ASSERT(hTD->m_fInterrupted == true);

            hTD->stop();
            while (hTD->isStarted())
                {
                Thread::currentThread()->yield();
                }
            }
    };
