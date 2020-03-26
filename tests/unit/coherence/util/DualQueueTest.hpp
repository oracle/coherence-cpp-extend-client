/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/DualQueue.hpp"

using namespace coherence::lang;
using namespace std;
using coherence::util::DualQueue;



/**
* Test Suite for the DualQueue object.
*
* 01.02.08 nsa
*/
class DualQueueTest : public CxxTest::TestSuite
    {
    class ProducerThread
        : public class_spec<ProducerThread,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<ProducerThread>;

        protected:
            ProducerThread(DualQueue::Handle hQueue)
                : m_hQueue(self(), hQueue)
                {
                }

        public:
            virtual void run()
                {
                for (int32_t x = 0; x < 256; ++x)
                    {
                    m_hQueue->add(COH_TO_STRING("Value: " << x));
                    }
                }

        protected:
            FinalHandle<DualQueue> m_hQueue;
        };

    class ConsumerThread
        : public class_spec<ConsumerThread,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<ConsumerThread>;

        protected:
            ConsumerThread(DualQueue::Handle hQueue)
                    : m_hQueue(self(), hQueue), m_iTestCount(0)
                    {
                    }

        public:
            virtual void run()
                {
                while (m_iTestCount != 256)
                    {
                    Object::Holder oh = m_hQueue->remove();
                    ++m_iTestCount;
                    }
                }

            virtual int32_t getCount() const
                {
                return m_iTestCount;
                }

        protected:
            FinalHandle<DualQueue> m_hQueue;
            int32_t m_iTestCount;
        };

    public:

        void testAdd()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");

            hQueue->add(hS);

            TS_ASSERT(hQueue->size() == 1);
            TS_ASSERT(hS == hQueue->remove());
            }

        void testAddHead()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");
            String::Handle    hS2    = String::create("TEST2");

            hQueue->setBatchSize(2);
            hQueue->add(hS);
            hQueue->addHead(hS2);

            TS_ASSERT(hQueue->size() == 2);
            TS_ASSERT(hS2 == hQueue->remove());
            }

        void testFlush()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");
            String::Handle    hS2    = String::create("TEST2");

            hQueue->setBatchSize(10);
            hQueue->add(hS);
            hQueue->add(hS2);

            TS_ASSERT(hQueue->getStatsFlushed() == 0);
            TS_ASSERT(hQueue->getFlushState() == DualQueue::flush_pending);

            hQueue->flush();
            TS_ASSERT(hQueue->getStatsFlushed() == 1);
            TS_ASSERT(hQueue->getFlushState() == DualQueue::flush_explicit);

            Object::Holder oh = hQueue->remove();

            TS_ASSERT(hS == oh);
            TS_ASSERT(hQueue->remove() == hS2);
            TS_ASSERT(hQueue->size() == 0);

            hQueue->add(hS);
            TS_ASSERT(hQueue->getFlushState() == DualQueue::flush_pending);
            }

        void testIsEmpty()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");

            TS_ASSERT(hQueue->isEmpty() == true);
            hQueue->add(hS);
            TS_ASSERT(hQueue->isEmpty() == false);
            }

        void testPeekNoWait()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");
            String::Handle    hS2    = String::create("TEST2");

            hQueue->add(hS);
            hQueue->add(hS2);

            TS_ASSERT(hS == hQueue->peekNoWait());
            }

        void testRemove()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");

            hQueue->add(hS);

            TS_ASSERT(hQueue->remove() == hS);
            }

        void testRemoveNoWait()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");

            hQueue->add(hS);

            TS_ASSERT(hQueue->removeNoWait() == hS);
            }

        void testSize()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            String::Handle    hS     = String::create("TEST");

            hQueue->add(hS);

            TS_ASSERT(hQueue->size() == 1);
            }

        void testProducerConsumer()
            {
            DualQueue::Handle hQueue = DualQueue::create();
            ProducerThread::Handle hProducer = ProducerThread::create(hQueue);
            Thread::Handle hP = Thread::create(hProducer);

            hP->start();

            ConsumerThread::Handle hConsumer = ConsumerThread::create(hQueue);
            Thread::Handle hC = Thread::create(hConsumer);

            hC->start();

            hP->join();
            hC->join();

            TS_ASSERT(hConsumer->getCount() == 256);
            }
    };
