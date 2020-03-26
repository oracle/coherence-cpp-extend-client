/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/ThreadGate.hpp"

#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;
using namespace std;
using coherence::util::ThreadGate;

/**
* Test Suite for the AtomicCounter object.
*
* 01.09.08 nsa
*/
class ThreadGateTest : public CxxTest::TestSuite
    {
    class WriterThread
        : public class_spec<WriterThread,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<WriterThread>;

        protected:
            WriterThread(ThreadGate::Handle hGate, bool& test, bool fAutoFinish = false)
                : m_hGate(self(), hGate), m_Test(test), m_fAutoFinish(fAutoFinish)
                {
                }

        public:
            void setAutoFinish(bool fAutoFinish)
                {
                m_fAutoFinish = fAutoFinish;
                }

            bool getAutoFinish(bool fAutoFinish)
                {
                return m_fAutoFinish;
                }

            bool getTest()
                {
                return m_Test;
                }

            /**
            * Invoke the Runnable.
            */
            virtual void run()
                {
                if (m_fAutoFinish == true)
                    {
                    COH_SYNCHRONIZED(m_hGate)
                        {
                        m_hGate->wait();
                        }
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    COH_GATE_ENTER(m_hGate, ThreadGate::infinite)
                        {
                        COH_GATE_CLOSE(m_hGate, ThreadGate::infinite)
                            {
                            m_Test = true;

                            if (!m_fAutoFinish)
                                {
                                COH_SYNCHRONIZED(this)
                                    {
                                    notify();
                                    wait();
                                    }
                                }

                            m_Test = false;
                            }
                        }
                    }
                }

        protected:
            FinalHandle<ThreadGate> m_hGate;
            bool m_fAutoFinish;
            bool& m_Test;

        };

        class WriterThreadNoEnter
            : public class_spec<WriterThreadNoEnter,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<WriterThreadNoEnter>;

        protected:
            WriterThreadNoEnter(ThreadGate::Handle hGate, bool& test, bool fAutoFinish = false)
                : m_hGate(self(), hGate), m_Test(test), m_fAutoFinish(fAutoFinish)
                {
                }

        public:
            void setAutoFinish(bool fAutoFinish)
                {
                m_fAutoFinish = fAutoFinish;
                }

            bool getAutoFinish(bool fAutoFinish)
                {
                return m_fAutoFinish;
                }

            bool getTest()
                {
                return m_Test;
                }

            /**
            * Invoke the Runnable.
            */
            virtual void run()
                {
                if (m_fAutoFinish == true)
                    {
                    COH_SYNCHRONIZED(m_hGate)
                        {
                        m_hGate->wait();
                        }
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    COH_GATE_CLOSE(m_hGate, ThreadGate::infinite)
                        {
                        m_Test = true;

                        if (!m_fAutoFinish)
                            {
                            COH_SYNCHRONIZED(this)
                                {
                                notify();
                                wait();
                                }
                            }

                        m_Test = false;
                        }
                    }
                }

        protected:
            FinalHandle<ThreadGate> m_hGate;
            bool m_fAutoFinish;
            bool& m_Test;
        };

        class WriterThreadBarEnter
            : public class_spec<WriterThreadBarEnter,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<WriterThreadBarEnter>;

        protected:
            WriterThreadBarEnter(ThreadGate::Handle hGate, bool& test, bool fAutoFinish = true)
                : m_hGate(self(), hGate), m_Test(test), m_fAutoFinish(fAutoFinish)
                {
                }

        public:
            void setAutoFinish(bool fAutoFinish)
                {
                m_fAutoFinish = fAutoFinish;
                }

            bool getAutoFinish(bool fAutoFinish)
                {
                return m_fAutoFinish;
                }

            bool getTest()
                {
                return m_Test;
                }

            /**
            * Invoke the Runnable.
            */
            virtual void run()
                {
                if (m_fAutoFinish == true)
                    {
                    COH_SYNCHRONIZED(m_hGate)
                        {
                        m_hGate->wait();
                        }
                    }

                bool fGateLocked = true;
                for (int32_t x = 0; x < 5; ++x)
                    {
                    COH_GATE_BAR(m_hGate, ThreadGate::infinite)
                        {
                        while (int32_t cEnters = m_hGate->getActiveCount() > 0)
                            {
                            fGateLocked = false;
                            COH_SYNCHRONIZED(m_hGate)
                                {
                                m_hGate->wait();
                                }
                            }

                        COH_GATE_CLOSE(m_hGate, ThreadGate::infinite)
                            {
                            m_Test = true;

                            fGateLocked = true;

                            m_Test = false;
                            }
                        }
                    }

                COH_ENSURE(fGateLocked);
                }

        protected:
            FinalHandle<ThreadGate> m_hGate;
            bool m_fAutoFinish;
            bool& m_Test;
        };

        class ReaderThread
            : public class_spec<ReaderThread,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<ReaderThread>;

        protected:
            ReaderThread(ThreadGate::Handle hGate, bool& test, bool fAutoFinish = true)
                : m_hGate(self(), hGate), m_Test(test), m_fAutoFinish(fAutoFinish)
                {
                }

        public:
            void setAutoFinish(bool fAuto)
                {
                m_fAutoFinish = fAuto;
                }

            bool getAutoFinish(bool fAutoFinish)
                {
                return m_fAutoFinish;
                }

            /**
            * Invoke the Runnable.
            */
            virtual void run()
                {
                if (m_fAutoFinish == true)
                    {
                    COH_SYNCHRONIZED(m_hGate)
                        {
                        m_hGate->wait();
                        }
                    }

                for (int32_t x = 0; x < 500; ++x)
                    {
                    COH_GATE_ENTER(m_hGate, ThreadGate::infinite)
                        {
                        Thread::sleep(10);
                        COH_ENSURE(m_Test == false);
                        }
                    }

                }

        protected:
            FinalHandle<ThreadGate> m_hGate;
            bool m_fAutoFinish;
            bool& m_Test;
        };

    public:
        void testGateState()
            {
            bool               fTest     = false;
            ThreadGate::Handle hGate     = ThreadGate::create();
            ReaderThread::Handle hReader = ReaderThread::create(hGate, fTest, false);
            WriterThread::Handle hWriter = WriterThread::create(hGate, fTest, false);
            Thread::Handle thReader = Thread::create(hReader);

            thReader->start();
            Thread::Handle thWriter = Thread::create(hWriter);

            COH_SYNCHRONIZED(hWriter)
                {
                thWriter->start();

                hWriter->wait();

                TS_ASSERT(fTest == true);
                TS_ASSERT(hGate->getActiveCount() == 1);
                TS_ASSERT(hGate->getCloseCount() == 1);

                hWriter->setAutoFinish(true);
                hWriter->notify();
                }

            thReader->join();
            thWriter->join();

            TS_ASSERT(hGate->getActiveCount() == 0);
            TS_ASSERT(fTest == false);
            }

        void testGateWriterLoad()
            {
            bool               fTest = false;
            ThreadGate::Handle hGate = ThreadGate::create();

            Thread::Handle ahThreads[20];

            for (int32_t x = 0; x < 20; ++x)
                {
                if (x < 10)
                    {
                    ahThreads[x] = Thread::create(ReaderThread::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                else
                    {
                    ahThreads[x] = Thread::create(WriterThreadNoEnter::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                }

            //Give the threads a chance to spin up
            COH_SYNCHRONIZED(hGate)
                {
                hGate->wait(200);
                hGate->notifyAll();
                }

            for (int32_t x = 0; x < 20; ++x)
                {
                ahThreads[x]->join();
                }

            TS_ASSERT(fTest == false);
            TS_ASSERT(hGate->getActiveCount() == 0);
            }

        void testGateWriterEnterLoad()
            {
            bool               fTest = false;
            ThreadGate::Handle hGate = ThreadGate::create();

            Thread::Handle ahThreads[20];

            for (int32_t x = 0; x < 20; ++x)
                {
                if (x < 10)
                    {
                    ahThreads[x] = Thread::create(ReaderThread::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                else
                    {
                    ahThreads[x] = Thread::create(WriterThread::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                }

            //Give the threads a chance to spin up
            COH_SYNCHRONIZED(hGate)
                {
                hGate->wait(200);
                hGate->notifyAll();
                }

            for (int32_t x = 0; x < 20; ++x)
                {
                ahThreads[x]->join();
                }

            TS_ASSERT(fTest == false);
            TS_ASSERT(hGate->getActiveCount() == 0);
            }

        void testGateWriterBarEnterLoad()
            {
            bool               fTest = false;
            ThreadGate::Handle hGate = ThreadGate::create();

            Thread::Handle ahThreads[20];
            for (int32_t x = 0; x < 20; ++x)
                {
                if (x < 19)
                    {
                    ahThreads[x] = Thread::create(ReaderThread::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                else
                    {
                    ahThreads[x] = Thread::create(WriterThreadBarEnter::create(hGate, fTest));
                    ahThreads[x]->start();
                    }
                }

            //Give the threads a chance to spin up
            COH_SYNCHRONIZED(hGate)
                {
                hGate->wait(200);
                hGate->notifyAll();
                }


             for (int32_t x = 0; x < 20; ++x)
                {
                ahThreads[x]->join();
                }

            TS_ASSERT(fTest == false);
            TS_ASSERT(hGate->getActiveCount() == 0);
            }
    };
