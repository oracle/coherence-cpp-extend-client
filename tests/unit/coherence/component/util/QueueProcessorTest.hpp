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
using coherence::util::Queue;
using coherence::component::util::QueueProcessor;

const int32_t CONSUMABLE_EVENTS = 256;

COH_OPEN_NAMESPACE (test)

class TestQP
    : public class_spec<TestQP,
        extends<QueueProcessor> >
    {
    friend class factory<TestQP>;

    protected:

        TestQP()
            : m_cEvents(0), m_cNotifies(0), m_hMonitor(self(), Object::create())
            {
            }

    public:
        void onNotify()
            {
            Thread::currentThread()->sleep(10);
            ++m_cNotifies;
            while (!isExiting())
                {
                Object::Holder ohValue = getQueue()->removeNoWait();
                if (NULL != ohValue)
                    {
                    ++m_cEvents;
                    }
                else
                    {
                    break;
                    }
                }

            COH_SYNCHRONIZED(m_hMonitor)
                {
                m_hMonitor->notifyAll();
                }
            }

    public:
        int32_t m_cEvents;
        int32_t m_cNotifies;
        FinalHandle<Object> m_hMonitor;
    };

COH_CLOSE_NAMESPACE // test

using namespace test;

/**
* Test Suite for the QueueProcessor object.
*
* 01.02.08 nsa
*/
class QueueProcessorTest : public CxxTest::TestSuite
    {
    public:
        void testQueueProcessor()
            {
            TestQP::Handle hQP = TestQP::create();
            hQP->setWaitMillis(30);

            Queue::Handle hQueue = hQP->getQueue();

            for (int32_t x = 0; x < CONSUMABLE_EVENTS/2; ++x)
                {
                hQueue->add(String::create("test_value"));
                }

            // We need to make sure that all the work is done to gaurantee
            // that there are two notifies!

            COH_SYNCHRONIZED(hQP->m_hMonitor)
                {
                hQP->start();
                hQP->m_hMonitor->wait(0);
                }

            Thread::currentThread()->sleep(5);

            for (int32_t x = 0, ce = CONSUMABLE_EVENTS/2; x < ce; ++x)
                {
                hQueue->add(String::create("test_value"));
                }

            COH_SYNCHRONIZED(hQP->m_hMonitor)
                {
                hQP->m_hMonitor->wait(0);
                }

            hQP->stop();

            TS_ASSERT(hQP->m_cEvents == CONSUMABLE_EVENTS);
            TS_ASSERT(hQP->m_cNotifies >= 2);
            }
    };
