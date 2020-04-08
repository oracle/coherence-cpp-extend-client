/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/lang/ThreadLocalReference.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace coherence::native;
using namespace std;


/**
* Test Suite for the ThreadLocalReference object.
*/
class ThreadLocalReferenceTest : public CxxTest::TestSuite
    {
    public:
        class TestRunnable
            : public class_spec<TestRunnable,
                extends<Object>,
                implements<Runnable> >
            {
            friend class factory<TestRunnable>;

            protected:
                TestRunnable(ThreadLocalReference::Handle hTLS)
                    : m_hTLS(self(), hTLS)
                    {
                    }

            public:
                virtual void run()
                    {
                    Object::Handle h = Object::create();

                    ThreadLocalReference::Handle hTLS = m_hTLS;
                    COH_SYNCHRONIZED (hTLS)
                        {
                        Object::Handle hSelf = this;

                        TS_ASSERT(hTLS->get() == NULL);

                        hTLS->set(hSelf);

                        TS_ASSERT(hTLS->get() == hSelf);

                        Object::View vSelf = this;
                        ThreadLocalReference::View vTLS = hTLS;
                        TS_ASSERT(vTLS->get() == vSelf);
                        }
                    }
            private:
                FinalHandle<ThreadLocalReference> m_hTLS;
            };

        void testStart()
            {
            // As this test introduces new classes, we must force caching
            // of those class names ahead of time so our memory leak detection
            // doesn't get triggered because of it.
            {
            Class::getTypeName(typeid(TestRunnable));
            Thread::Handle hThreadPrime = Thread::create(TestRunnable::create(ThreadLocalReference::create()));
            hThreadPrime->start();
            hThreadPrime->join();
            System::getHeapAnalyzer()->getObjectCount();
            }

            int64_t cStart = System::getHeapAnalyzer()->getObjectCount();
                {
                ThreadLocalReference::Handle hTLS =
                        ThreadLocalReference::create();
                Thread::Handle hThread1 = Thread::create(
                        TestRunnable::create(hTLS));
                Thread::Handle hThread2 = Thread::create(
                        TestRunnable::create(hTLS));

                hThread1->start();
                hThread2->start();

                hThread1->join();
                hThread2->join();
                }
            for (int32_t i = 0; i < 10 && System::getHeapAnalyzer()->getObjectCount() != cStart; ++i)
                {
                Thread::sleep(100);
                }
            int64_t cEnd = System::getHeapAnalyzer()->getObjectCount();
            COH_ENSURE_RELATION(int64_t, cEnd, <=, cStart); // threads and TLS all cleaned up
            }
    };

