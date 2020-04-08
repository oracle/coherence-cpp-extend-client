/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/util/logging/Logger.hpp"


using namespace coherence::lang;
using namespace coherence::native;
using namespace std;

/**
* Test Suite for the ThreadGroup object.
*/
class ThreadTestGroup : public CxxTest::TestSuite
    {
    public:

    class TestRunnable
        : public class_spec<TestRunnable,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestRunnable>;

        protected:
            TestRunnable(Object::Handle hMonitor)
                : m_hMonitor(self(), hMonitor)
                {
                }

        public:
            virtual void run()
                {
                Object::Handle hMonitor = m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    hMonitor->notify();
                    hMonitor->wait();
                    }
                }

        private:
            FinalHandle<Object> m_hMonitor;
        };


        void testSystemGroup()
            {
            ThreadGroup::Handle hGroup = Thread::currentThread()->getThreadGroup();
            TS_ASSERT(hGroup->getName()->equals("system"));
            TS_ASSERT(hGroup->getParent() == NULL);
            }

        void testEmptyGroupDestroy()
            {
            ThreadGroup::Handle hGroup = ThreadGroup::create("foo");
            hGroup->destroy();
            }

        void testNonEmptyGroupDestroy()
            {
            try
                {
                Thread::currentThread()->getThreadGroup()->destroy();
                TS_ASSERT(false); // unexpected
                }
            catch (IllegalStateException::View vew)
                {
                // expected
                }
            }

        void testAutoGroupParent()
            {
            ThreadGroup::Handle hGroup = ThreadGroup::create("foo");
            ThreadGroup::Handle hGroupSystem = Thread::currentThread()->getThreadGroup();
            TS_ASSERT(hGroup->getParent() == hGroupSystem);
            hGroup->destroy();
            }

        void testGroupSizeInterruptJoin()
            {
            Object::Handle      hMon    = Object::create();
            ThreadGroup::Handle hGroup  = ThreadGroup::create("foo");
            Thread::Handle      hThread = Thread::create(TestRunnable::create(hMon), "bar", hGroup);
            ThreadGroup::Handle hGroupSystem = Thread::currentThread()->getThreadGroup();
            size32_t            cSystem = hGroupSystem->activeCount();
            TS_ASSERT(hGroup->activeCount() == 0);

            COH_SYNCHRONIZED (hMon)
                {
                hThread->start();

                hMon->wait(); // wait for it to start
                TS_ASSERT(hGroup->activeCount() == 1);
                TS_ASSERT(hGroupSystem->activeCount() == cSystem + 1);
                }
            hGroup->interrupt(); // interrupt from second wait
            hGroup->join();

            TS_ASSERT(hGroup->activeCount() == 0);
            TS_ASSERT(hGroupSystem->activeCount() == cSystem);
            hGroup->destroy();
            }
    };

