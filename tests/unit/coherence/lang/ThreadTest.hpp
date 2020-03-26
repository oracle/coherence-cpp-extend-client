/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/native/NativeCondition.hpp"

#include <iostream>


using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

/**
* Test Suite for the Thread object.
*/
class ThreadTest : public CxxTest::TestSuite
    {
    public:

    class TestRunnable
        : public class_spec<TestRunnable,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestRunnable>;

        protected:
            TestRunnable(Object::Handle hMonitor, bool fEscaped)
                : m_hMonitor(self(), hMonitor), m_fEscaped(fEscaped)
                {
                }

        public:
            virtual void run()
                {
                COH_ENSURE(_isEscaped() == m_fEscaped);
                COH_ENSURE(Thread::currentThread()->_isEscaped());

                Object::Handle hMonitor = m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    hMonitor->wait(10); // test delay
                    hMonitor->notify();
                    }
                }

        private:
            FinalHandle<Object> m_hMonitor;
            bool m_fEscaped;
        };

        void testThreadLifecycle()
            {
            Object::Handle hMonitor = Object::create();

            COH_SYNCHRONIZED(hMonitor)
                {
                Thread::Handle hThread = Thread::create(
                    TestRunnable::create(hMonitor, false));
                TS_ASSERT(hThread->getState() == Thread::state_initial);
                TS_ASSERT(!hThread->isAlive());

                hThread->start();

                TS_ASSERT(hThread->getState() == Thread::state_runnable);
                TS_ASSERT(hThread->isAlive());

                // thread created, waiting for signal
                hMonitor->wait();
                // thread creation confirmed

                hThread->join();

                TS_ASSERT(hThread->getState() == Thread::state_terminated);
                TS_ASSERT(!hThread->isAlive());
                }
            }

        void testThreadLifecycleEscaped()
            {
            Object::Handle hMonitor = Object::create();

            COH_SYNCHRONIZED(hMonitor)
                {
                Runnable::Handle hRunnable = TestRunnable::create(hMonitor, true);
                Thread::Handle hThread = Thread::create(hRunnable);
                TS_ASSERT(hThread->getState() == Thread::state_initial);
                TS_ASSERT(!hThread->isAlive());

                hThread->start();

                TS_ASSERT(hThread->getState() == Thread::state_runnable);
                TS_ASSERT(hThread->isAlive());

                // thread created, waiting for signal
                hMonitor->wait();
                // thread creation confirmed

                hThread->join();

                TS_ASSERT(hThread->getState() == Thread::state_terminated);
                TS_ASSERT(!hThread->isAlive());
                }
            }

    class TestInterruptable
        : public Runnable
        {
        public:
            static Runnable::Handle create()
                {
                return new TestInterruptable();
                }

        protected:
            TestInterruptable()
                {
                }

        public:
            virtual void run()
                {
                COH_SYNCHRONIZED (this)
                    {
                    while (true)
                        {
                        wait(0);
                        }
                    }
                }
        };

        void testInterrupt()
            {
            Thread::Handle hThread = Thread::create(TestInterruptable::create(), "MyThread");

            TS_ASSERT(!hThread->isAlive());
            hThread->start();
            TS_ASSERT(hThread->isAlive());

            // interrupt the thread and give it 1s to exit
            hThread->interrupt();
            hThread->join(1000);
            TS_ASSERT(hThread->getState() == Thread::state_terminated);
            TS_ASSERT(!hThread->isAlive());
            }

        // COH-13972 - increasing the upper bound for all timing windows
        //             to account for arbitrary system slowdowns
        void testTimeout()
            {
            Object::Handle h = Object::create();
            FinalHandle<Object> fh(System::common(), h); // test on escaped object
            int64_t ldtStart;
            int64_t cMillis;

            COH_SYNCHRONIZED (h)
                {
                ldtStart = System::safeTimeMillis();
                try
                    {
                    COH_TIMEOUT_AFTER(10)
                        {
                        for (int i = 0; i < 100; ++i) h->wait();
                        TS_ASSERT(false); // wait appears to be spinning
                        }
                    }
                catch (InterruptedException::View vex) {}

                cMillis = System::safeTimeMillis() - ldtStart;
                TS_ASSERT_LESS_THAN(5,       cMillis);
                TS_ASSERT_LESS_THAN(cMillis, 1000);

                ldtStart = System::safeTimeMillis();
                try
                    {
                    COH_TIMEOUT_AFTER(100)
                        {
                        for (int i = 0; i < 100; ++i) h->wait();
                        TS_ASSERT(false); // wait appears to be spinning
                        }
                    }
                catch (InterruptedException::View vex) {}

                cMillis = System::safeTimeMillis() - ldtStart;
                TS_ASSERT_LESS_THAN(50,      cMillis);
                TS_ASSERT_LESS_THAN(cMillis, 1000);

                ldtStart = System::safeTimeMillis();
                try
                    {
                    COH_TIMEOUT_AFTER(1000)
                        {
                        for (int i = 0; i < 100; ++i) h->wait();
                        TS_ASSERT(false); // wait appears to be spinning
                        }
                    }
                catch (InterruptedException::View vex) {}

                cMillis = System::safeTimeMillis() - ldtStart;
                TS_ASSERT_LESS_THAN(900,     cMillis);
                TS_ASSERT_LESS_THAN(cMillis, 2000);

                int64_t ldtOuterStart = System::safeTimeMillis();
                try
                    {
                    COH_TIMEOUT_AFTER(3000)
                        {
                        ldtStart = System::safeTimeMillis();
                        try
                            {
                            COH_TIMEOUT_AFTER(10)
                                {
                                for (int i = 0; i < 100; ++i) h->wait(100);
                                TS_ASSERT(false); // wait appears to be spinning
                                }
                            }
                        catch (InterruptedException::View vex) {}

                        cMillis = System::safeTimeMillis() - ldtStart;
                        TS_ASSERT_LESS_THAN(5,       cMillis);
                        TS_ASSERT_LESS_THAN(cMillis, 1000);

                        ldtStart = System::safeTimeMillis();
                        try
                            {
                            COH_TIMEOUT_AFTER(100)
                                {
                                for (int i = 0; i < 100; ++i) h->wait(1000);
                                TS_ASSERT(false); // wait appears to be spinning
                                }
                            }
                        catch (InterruptedException::View vex) {}

                        cMillis = System::safeTimeMillis() - ldtStart;
                        TS_ASSERT_LESS_THAN(50,      cMillis);
                        TS_ASSERT_LESS_THAN(cMillis, 1000);

                        ldtStart = System::safeTimeMillis();
                        try
                            {
                            COH_TIMEOUT_AFTER(1000)
                                {
                                for (int i = 0; i < 100; ++i) h->wait(10000);
                                TS_ASSERT(false); // wait appears to be spinning
                                }
                            }
                        catch (InterruptedException::View vex) {}

                        cMillis = System::safeTimeMillis() - ldtStart;
                        TS_ASSERT_LESS_THAN(900,     cMillis);
                        TS_ASSERT_LESS_THAN(cMillis, 2000);

                        try
                            {
                            COH_TIMEOUT_AFTER(5000) // try to extend timeout
                                {
                                for (int i = 0; i < 100; ++i) h->wait(); // remainder of the 2s timeout
                                TS_ASSERT(false); // wait appears to be spinning
                                }
                            }
                        catch (InterruptedException::View vex) {}
                        } // end of outer 2s timeout
                    }
                catch (InterruptedException::View vex) {}

                cMillis = System::safeTimeMillis() - ldtOuterStart;
                TS_ASSERT_LESS_THAN(2900,    cMillis);
                TS_ASSERT_LESS_THAN(cMillis, 4000);
                }
            }
    };

