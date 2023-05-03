/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/AtomicCounter.hpp"

using namespace coherence::lang;
using namespace std;
using coherence::util::AtomicCounter;

/**
* Test Suite for the AtomicCounter object.
*
* 01.02.08 nsa
*/
class AtomicCounterTest : public CxxTest::TestSuite
    {
    /**
    * Incrementor class that will increment by 1 an AtomicCount 1000 times
    * using the postIncrement method.
    */
    class AtomicIncrementor_1
        : public class_spec<AtomicIncrementor_1,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<AtomicIncrementor_1>;

        protected:
            AtomicIncrementor_1(AtomicCounter::Handle hc)
                : COH_NO_WARN(m_hcAtomic(self(), hc)),
                  f_vMonitor(self(), Object::create())
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    // notify the test thread that this incrementor thread has started
                    COH_SYNCHRONIZED(f_vMonitor)
                        {
                        f_vMonitor->notify();
                        }

                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postIncrement();
                    }
                }
            
            virtual Object::View getMonitor() const
                {
                return f_vMonitor;
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
            FinalView<Object>           f_vMonitor;
        };

        /**
        * Incrementor class that will increment by 5 an AtomicCount 1000 times
        * using the postIncrement method.
        */
        class AtomicIncrementor_5
            : public class_spec<AtomicIncrementor_5,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<AtomicIncrementor_5>;

        protected:
            AtomicIncrementor_5(AtomicCounter::Handle hc)
            : COH_NO_WARN(m_hcAtomic(self(), hc)),
              f_vMonitor(self(), Object::create())
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    // notify the test thread that this incrementor thread has started
                    COH_SYNCHRONIZED(f_vMonitor)
                        {
                        f_vMonitor->notify();
                        }

                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postIncrement(5);
                    }
                }
            
            virtual Object::View getMonitor() const
                {
                return f_vMonitor;
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
            FinalView<Object>           f_vMonitor;
        };

        /**
        * Decrementor class that will decrement by 1 an AtomicCount 1000 times
        * using the postDecrement method.
        */
        class AtomicDecrementor_1
            : public class_spec<AtomicDecrementor_1,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<AtomicDecrementor_1>;

        protected:
            AtomicDecrementor_1(AtomicCounter::Handle hc)
                : COH_NO_WARN(m_hcAtomic(self(), hc)),
                  f_vMonitor(self(), Object::create())
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    // notify the test thread that this decrementor thread has started
                    COH_SYNCHRONIZED(f_vMonitor)
                        {
                        f_vMonitor->notify();
                        }

                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postDecrement();
                    }
                }

            virtual Object::View getMonitor() const
                {
                return f_vMonitor;
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
            FinalView<Object>           f_vMonitor;
        };

        /**
        * Decrementor class that will decrement by 5 an AtomicCount 1000 times
        * using the postDecrement method.
        */
        class AtomicDecrementor_5
            : public class_spec<AtomicDecrementor_5,
                extends<Object>,
                implements<Runnable> >
        {
        friend class factory<AtomicDecrementor_5>;

        protected:
            AtomicDecrementor_5(AtomicCounter::Handle hc)
            : COH_NO_WARN(m_hcAtomic(self(), hc)),
              f_vMonitor(self(), Object::create())
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    // notify the test thread that this decrementor thread has started
                    COH_SYNCHRONIZED(f_vMonitor)
                        {
                        f_vMonitor->notify();
                        }

                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postDecrement(5);
                    }
                }

            virtual Object::View getMonitor() const
                {
                return f_vMonitor;
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
            FinalView<Object>           f_vMonitor;
        };

    public:
        /**
         * Test AtomicCount increment using 10 concurrent threads
         */
        void testIncrementThreads()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);

            Thread::Handle ahThreads[10];

            for (int32_t x = 0; x < 10; ++x)
                {
                Object::View vMonitor;

                if (x < 5)
                    {
                    AtomicIncrementor_1::Handle hIncrementor = AtomicIncrementor_1::create(hcAtomic);

                    vMonitor     = hIncrementor->getMonitor();
                    ahThreads[x] = Thread::create(hIncrementor);
                    }
                else
                    {
                    AtomicIncrementor_5::Handle hIncrementor = AtomicIncrementor_5::create(hcAtomic);

                    vMonitor     = hIncrementor->getMonitor();
                    ahThreads[x] = Thread::create(hIncrementor);
                    }

                COH_SYNCHRONIZED(vMonitor)
                    {
                    ahThreads[x]->start();
                    vMonitor->wait();
                    // once we get here, the incrementor thread has started and is in the hcAtomic synchronized block
                    }
                }

            COH_SYNCHRONIZED(hcAtomic)
                {
                // all incrementor threads must be in hcAtomic->wait() by this point
                hcAtomic->notifyAll();
                }

            for (int32_t x = 0; x < 10; ++x)
                {
                ahThreads[x]->join();
                }

            TS_ASSERT(hcAtomic->getCount() == int64_t(30000));
            }

        /**
         * Test AtomicCount decrement using 10 concurrent threads
         */
        void testDecrementThreads()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(30000);

            Thread::Handle ahThreads[10];

            for (int32_t x = 0; x < 10; ++x)
                {
                Object::View vMonitor;

                if (x < 5)
                    {
                    AtomicDecrementor_1::Handle hDecrementor = AtomicDecrementor_1::create(hcAtomic);

                    vMonitor     = hDecrementor->getMonitor();
                    ahThreads[x] = Thread::create(hDecrementor);
                    }
                else
                    {
                    AtomicDecrementor_5::Handle hDecrementor = AtomicDecrementor_5::create(hcAtomic);

                    vMonitor     = hDecrementor->getMonitor();
                    ahThreads[x] = Thread::create(hDecrementor);
                    }

                COH_SYNCHRONIZED(vMonitor)
                    {
                    ahThreads[x]->start();
                    vMonitor->wait();
                    // once we get here, the decrementor thread has started and is in the hcAtomic synchronized block
                    }
                }

            COH_SYNCHRONIZED(hcAtomic)
                {
                // all decrementor threads must be in hcAtomic->wait() by this point
                hcAtomic->notifyAll();
                }

            for (int32_t x = 0; x < 10; ++x)
                {
                ahThreads[x]->join();
                }

            TS_ASSERT(hcAtomic->getCount() == int64_t(0));
            }

        /**
         * Test AtomicCount increment() method
         */
        void testIncrement()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            int64_t result = hcAtomic->increment();

            TS_ASSERT(result == hcAtomic->getCount());
            TS_ASSERT(result == 1);
            }

        /**
         * Test AtomicCount increment(int64_t c) method
         */
        void testIncrement5()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            int64_t result = hcAtomic->increment(5);

            TS_ASSERT(result == hcAtomic->getCount());
            TS_ASSERT(result == 5);
            }

        /**
         * Test AtomicCount postIncrement() method
         */
        void testPostIncrement()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            int64_t result = hcAtomic->postIncrement();

            TS_ASSERT(result == 0);
            TS_ASSERT(hcAtomic->getCount() == 1);
            }

        /**
        * Test AtomicCount postIncrement(int64_t) method
        */
        void testPostIncrement5()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            int64_t result = hcAtomic->postIncrement(5);

            TS_ASSERT(result == 0);
            TS_ASSERT(hcAtomic->getCount() == 5);
            }

        /**
        * Test AtomicCount decrement() method
        */
        void testDecrement()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(1);
            int64_t result = hcAtomic->decrement();

            TS_ASSERT(result == hcAtomic->getCount());
            TS_ASSERT(result == 0);
            }

        /**
        * Test AtomicCount decrement(int64_t c) method
        */
        void testDecrement5()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(5);
            int64_t result = hcAtomic->decrement(5);

            TS_ASSERT(result == hcAtomic->getCount());
            TS_ASSERT(result == 0);
            }

        /**
        * Test AtomicCount postDecrement() method
        */
        void testPostDecrement()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(1);
            int64_t result = hcAtomic->postDecrement();

            TS_ASSERT(result == 1);
            TS_ASSERT(hcAtomic->getCount() == 0);
            }

        /**
        * Test AtomicCount postDecrement(int64_t c) method
        */
        void testPostDecrement5()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(5);
            int64_t result = hcAtomic->postDecrement(5);

            TS_ASSERT(result == 5);
            TS_ASSERT(hcAtomic->getCount() == 0);
            }

        /**
        * Test AtomicCount bool setCount method expecting a postive result
        */
        void testUpdatePositive()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            TS_ASSERT(hcAtomic->update(0, 5) == 0);
            TS_ASSERT(hcAtomic->getCount() == 5);
            }

        /**
        * Test AtomicCount bool setCount method expecting a negative result
        */
        void testSetCountBoolNegative()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            TS_ASSERT(hcAtomic->update(1, 5) == 0);
            TS_ASSERT(hcAtomic->getCount() == 0);
            }

        /**
        * Test AtomicCount void setCount method
        */
        void testSetCount()
            {
            AtomicCounter::Handle hcAtomic = AtomicCounter::create(0);
            int64_t result = hcAtomic->setCount(25);
            TS_ASSERT(result == 0);
            TS_ASSERT(hcAtomic->getCount() == 25);
            }
    };
