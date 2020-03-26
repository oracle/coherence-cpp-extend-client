/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/NativeThreadLocal.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;


/**
* Test Suite for the NativeThreadLocal object.
*/
class NativeThreadLocalTest : public CxxTest::TestSuite
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
                    NativeThreadLocal* pnatl = s_pnatlValue;
                    COH_SYNCHRONIZED (m_hMonitor)
                        {
                        TS_ASSERT(pnatl->get() == NULL);

                        pnatl->set(this);

                        TS_ASSERT(pnatl->get() == this);
                        }
                    }

            private:
                FinalHandle<Object> m_hMonitor;

                static NativeThreadLocal* s_pnatlValue;
            };

        void testStart()
            {
            Object::Handle hMonitor = Object::create();

            Thread::Handle hThread1 = Thread::create(
                TestRunnable::create(hMonitor));
            Thread::Handle hThread2 = Thread::create(
                TestRunnable::create(hMonitor));

            hThread1->start();
            hThread2->start();

            hThread1->join();
            hThread2->join();
            }
    };

NativeThreadLocal* NativeThreadLocalTest::TestRunnable::s_pnatlValue = NativeThreadLocal::create();

