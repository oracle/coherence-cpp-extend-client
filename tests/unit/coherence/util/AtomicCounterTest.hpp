/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
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
                : COH_NO_WARN(m_hcAtomic(self(), hc))
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postIncrement();
                    }
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
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
            : COH_NO_WARN(m_hcAtomic(self(), hc))
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postIncrement(5);
                    }
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
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
                : COH_NO_WARN(m_hcAtomic(self(), hc))
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postDecrement();
                    }
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
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
            : COH_NO_WARN(m_hcAtomic(self(), hc))
                {
                }

        public:
            virtual void run()
                {
                AtomicCounter::Handle hWord = m_hcAtomic;

                COH_SYNCHRONIZED(hWord)
                    {
                    hWord->wait();
                    }

                for (int32_t x = 0; x < 1000; ++x)
                    {
                    m_hcAtomic->postDecrement(5);
                    }
                }

        protected:
            MemberHandle<AtomicCounter> m_hcAtomic;
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
                if (x < 5)
                    {
                    ahThreads[x] = Thread::create(AtomicIncrementor_1::create(hcAtomic));
                    }
                else
                    {
                    ahThreads[x] = Thread::create(AtomicIncrementor_5::create(hcAtomic));
                    }
                ahThreads[x]->start();
                Thread::yield();
                }

            COH_SYNCHRONIZED(hcAtomic)
                {
                // give the incrementor threads time to start and enter the synchronized block;
                // COH-26096 - 100ms may not be long enough on slow machines
                hcAtomic->wait(1000);
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
                if (x < 5)
                    {
                    ahThreads[x] = Thread::create(AtomicDecrementor_1::create(hcAtomic));
                    }
                else
                    {
                    ahThreads[x] = Thread::create(AtomicDecrementor_5::create(hcAtomic));
                    }
                ahThreads[x]->start();
                Thread::yield();
                }

            COH_SYNCHRONIZED(hcAtomic)
                {
                // give the incrementor threads time to start and enter the synchronized block;
                // COH-26096 - 100ms may not be long enough on slow machines
                hcAtomic->wait(1000);
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

