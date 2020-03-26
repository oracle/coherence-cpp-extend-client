/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "private/coherence/util/logging/Logger.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace std;

/**
* Test suite for thread's notify, notifyAll and wait methods.
*/
class ThreadOperationsTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Tests thread's notify method.
        * This test creates producer and two consumers; starts consumers,
        * tests that they are in wait state; starts producer, and asserts
        * that consumers are properly notified and executed.
        */
        void testNotify()
            {
            Object::Handle    hMonitor  = Object::create();
            QueueTest::Handle hQueue    = QueueTest::create(Object::create(), hMonitor);
            Producer::Handle  hProducer = Producer::create(hQueue, false);
            Consumer::Handle  hConsumer = Consumer::create(hQueue, 0);

            Thread::Handle hProducerThread  = Thread::create(hProducer, "Producer");
            Thread::Handle hConsumerThread1 = Thread::create(hConsumer, "Consumer1");
            Thread::Handle hConsumerThread2 = Thread::create(hConsumer, "Consumer2");

            TS_ASSERT(0 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());
            COH_SYNCHRONIZED(hMonitor)
                {
                hConsumerThread1->start();
                hMonitor->wait();

                hConsumerThread2->start();
                hMonitor->wait();
                }

            // consumerThreads should wait get
            TS_ASSERT(2 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());

            COH_SYNCHRONIZED(hMonitor)
                {
                // start the producer which should wake consumers
                hProducerThread->start();
                hMonitor->wait();
                }

            // ensure at least one consumer should be waken and executed
            TS_ASSERT(2 == hConsumer->getCountWaitGet());
            TS_ASSERT(hConsumer->getCountWakeGet() > 0);
            }

        /**
        * Tests thread's notifyAll method.
        * This test creates producer and three consumers; starts consumers,
        * tests that they are in wait state; starts producer, and asserts
        * that ALL consumers are properly notified and executed.
        */
        void testNotifyAll()
            {
            Object::Handle    hMonitor  = Object::create();
            QueueTest::Handle hQueue    = QueueTest::create(Object::create(), hMonitor);
            Producer::Handle  hProducer = Producer::create (hQueue, true);
            Consumer::Handle  hConsumer = Consumer::create (hQueue, 0);

            Thread::Handle hProducerThread  = Thread::create(hProducer, "Producer");
            Thread::Handle hConsumerThread1 = Thread::create(hConsumer, "Consumer1");
            Thread::Handle hConsumerThread2 = Thread::create(hConsumer, "Consumer2");
            Thread::Handle hConsumerThread3 = Thread::create(hConsumer, "Consumer3");

            TS_ASSERT(0 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());
            COH_SYNCHRONIZED(hMonitor)
                {
                hConsumerThread1->start();
                hMonitor->wait();

                hConsumerThread2->start();
                hMonitor->wait();

                hConsumerThread3->start();
                hMonitor->wait();
                }

            // consumerThreads should wait get
            TS_ASSERT(3 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());

            COH_SYNCHRONIZED(hMonitor)
                {
                // start the producer which should wake consumers
                hProducerThread->start();
                hMonitor->wait();
                }

            // consumers are done by now
            TS_ASSERT(3 == hConsumer->getCountWaitGet());
            TS_ASSERT(3 == hConsumer->getCountWakeGet());
            }

        /**
        * Tests thread's wait method.
        * This test creates producer and consumer; starts consumer with
        * 1000 millisec wait, tests that consumer is in wait state after
        * 200 millisec, two times in a row; finally, it waits 1000millisec
        * more and asserts that consumer is waken and executed.
        */
        void testWait()
            {
            Object::Handle    hMonitor  = Object::create();
            QueueTest::Handle hQueue    = QueueTest::create(Object::create(), hMonitor);
            Producer::Handle  hProducer = Producer::create(hQueue, false);
            Consumer::Handle  hConsumer = Consumer::create(hQueue, 1000);

            Thread::Handle hProducerThread = Thread::create(hProducer, "Producer");
            Thread::Handle hConsumerThread = Thread::create(hConsumer, "Consumer");

            TS_ASSERT(0 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());
            COH_SYNCHRONIZED(hMonitor)
                {
                hConsumerThread->start();
                hMonitor->wait();
                }

            // consumerThread should wait get
            TS_ASSERT(1 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());

            COH_SYNCHRONIZED(hMonitor)
                {
                hMonitor->wait(200);
                }

            // consumerThread should still wait get
            TS_ASSERT(1 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());

            COH_SYNCHRONIZED(hMonitor)
                {
                hMonitor->wait(1000);
                }

            // consumer should be done by now
            TS_ASSERT(1 == hConsumer->getCountWaitGet());
            TS_ASSERT(1 == hConsumer->getCountWakeGet());
            }

        /**
        * Tests that newly created thread doesn't steal others thread notify.
        * This test creates producer and two consumers; starts consumers,
        * tests that they are in wait state; starts producer, and asserts
        * that ALL consumers are properly notified and executed; finally it
        * creates new consumer and asserts that it goes to wait queue.
        */
        void testNoStealNotify()
            {
            Object::Handle    hMonitor  = Object::create();
            QueueTest::Handle hQueue    = QueueTest::create(Object::create(), hMonitor);
            Producer::Handle  hProducer = Producer::create(hQueue, true);
            Consumer::Handle  hConsumer = Consumer::create(hQueue, 0);

            Thread::Handle hProducerThread  = Thread::create(hProducer, "Producer");
            Thread::Handle hConsumerThread1 = Thread::create(hConsumer, "Consumer1");
            Thread::Handle hConsumerThread2 = Thread::create(hConsumer, "Consumer2");

            TS_ASSERT(0 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());
            COH_SYNCHRONIZED(hMonitor)
                {
                hConsumerThread1->start();
                hMonitor->wait();

                hConsumerThread2->start();
                hMonitor->wait();
                }

            // consumerThreads should wait get
            TS_ASSERT(2 == hConsumer->getCountWaitGet());
            TS_ASSERT(0 == hConsumer->getCountWakeGet());

            COH_SYNCHRONIZED(hMonitor)
                {
                hProducerThread->start();
                hMonitor->wait();
                }

            // consumers are done by now
            TS_ASSERT(2 == hConsumer->getCountWaitGet());
            TS_ASSERT(2 == hConsumer->getCountWakeGet());

            Thread::Handle hConsumerThreadNew = Thread::create(hConsumer, "ConsumerNew");

            COH_SYNCHRONIZED(hMonitor)
                {
                hConsumerThreadNew->start();
                hMonitor->wait();
                }

            // newly created consumer should be in waiting queue
            TS_ASSERT(3 == hConsumer->getCountWaitGet());
            TS_ASSERT(2 == hConsumer->getCountWakeGet());

            // stop ConsumerNew
            TS_ASSERT(hConsumerThreadNew->isAlive());
            hConsumerThreadNew->interrupt();
            hConsumerThreadNew->join();
            }


    // ----- utility test classes -------------------------------------------

    public:
        class QueueTest
            : public Object
            {
            public:
                typedef TypedHandle<QueueTest> Handle;
                typedef TypedHandle<const QueueTest> View;

                static Handle create(Object::Handle hMonitor, Object::Handle hTestMonitor)
                    {
                    return new QueueTest(hMonitor, hTestMonitor);
                    }

            protected:
                QueueTest(Object::Handle hMonitor, Object::Handle hTestMonitor)
                    : m_hMonitor(self(), hMonitor),
                      m_hTestMonitor(self(), hTestMonitor)
                    {
                    }

            public:
                int32_t getCountWaitGet()
                    {
                    return m_cWaitGet;
                    }

                int32_t getCountWakeGet()
                    {
                    return m_cWakeGet;
                    }

                int32_t get(int32_t millisec)
                    {
                    Object::Handle hObject = m_hMonitor;
                    COH_SYNCHRONIZED (hObject)
                        {
                        if (!m_fValueSet)
                            {
                            m_cWaitGet++;
                            COH_SYNCHRONIZED (m_hTestMonitor)
                                {
                                m_hTestMonitor->notify();
                                }

                            hObject->wait(millisec);

                            m_cWakeGet++;
                            if (0 == m_cWaitGet - m_cWakeGet)
                                {
                                COH_SYNCHRONIZED (m_hTestMonitor)
                                    {
                                    m_hTestMonitor->notify();
                                    }
                                }
                            }

                        m_fValueSet = false;

                        return m_cNumber;
                        }
                    }

                void put(int32_t number, bool isNotifyAll)
                    {
                    Object::Handle hObject = m_hMonitor;
                    COH_SYNCHRONIZED (hObject)
                        {
                        m_cNumber = number;
                        m_fValueSet = true;

                        if (!isNotifyAll)
                            {
                            hObject->notify();
                            }
                        else 
                            {
                            hObject->notifyAll();
                            }
                        }
                    }

            private:
                bool    m_fValueSet;
                int32_t m_cNumber;
                int32_t m_cWaitGet;
                int32_t m_cWakeGet;
                MemberHandle<Object> m_hMonitor;
                MemberHandle<Object> m_hTestMonitor;
            };

        class Producer
            : public Runnable
            {
            public:
                typedef TypedHandle<Producer> Handle;
                typedef TypedHandle<const Producer> View;

                static Handle create(QueueTest::Handle hQueue, bool isNotifyAll)
                    {
                    return new Producer(hQueue, isNotifyAll);
                    }

            protected:
                Producer(QueueTest::Handle hQueue, bool isNotifyAll)
                    : m_isNotifyAll(isNotifyAll),
                      m_hQueue(self(), hQueue)
                    {
                    }

            protected:
                void run()
                    {
                    m_hQueue->put(10, m_isNotifyAll);
                    }

            private:
                bool m_isNotifyAll;
                MemberHandle<QueueTest> m_hQueue;
            };


        class Consumer
            : public Runnable
            {
            public:
                typedef TypedHandle<Consumer> Handle;
                typedef TypedHandle<const Consumer> View;

                static Handle create(QueueTest::Handle hQueue, int32_t millis)
                    {
                    return new Consumer(hQueue, millis);
                    }

            protected:
                Consumer(QueueTest::Handle hQueue, int32_t millis)
                    : m_millis(millis),
                      m_hQueue(self(), hQueue)
                    {
                    }

            public:
                int getCountWaitGet()
                    {
                    return m_hQueue->getCountWaitGet();
                    }

                int getCountWakeGet()
                    {
                    return m_hQueue->getCountWakeGet();
                    }
            
            protected:
                void run()
                    {
                    m_hQueue->get(m_millis);
                    }

            private:
                int32_t m_millis;
                MemberHandle<QueueTest> m_hQueue;
            };
    };
