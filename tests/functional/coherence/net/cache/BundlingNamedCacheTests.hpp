/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Random.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Random;
using coherence::util::Set;

/**
* A collection of functional tests for a Coherence*Extend client that uses
* operation bundling to perform cache operations.
*
* @author lh  2012.06.12
*/

/**
* BundlingNamedCache Test Suite.
*/
class BundlingNamedCacheTests : public CxxTest::TestSuite
    {
    class TestConcurrentPutRunner
        : public class_spec<TestConcurrentPutRunner,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestConcurrentPutRunner>;

        protected:
            TestConcurrentPutRunner(NamedCache::Handle hCache, Object::View v)
                : m_hCache(self(), hCache), f_vMonitor(self(), v)
                {
                }

        public:
            virtual void run()
                {
                waitForSemaphore(f_vMonitor);
                int32_t ofStart = getThreadIndex() * COUNT;
                for (int32_t i = 0; i < COUNT; i++)
                    {
                    m_hCache->put(Integer32::create(ofStart + i), COH_TO_STRING(ofStart + i));
                    }
                }

        protected:
            FinalHandle<NamedCache> m_hCache;
            FinalView<Object>       f_vMonitor;
        };

    class TestPutAllRunner
        : public class_spec<TestPutAllRunner,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestPutAllRunner>;

        protected:
            TestPutAllRunner(NamedCache::Handle hCache, Object::View v)
                : m_hCache(self(), hCache), f_vMonitor(self(), v)
                {
                }

        public:
            void run()
            {
            waitForSemaphore(f_vMonitor);
            int32_t ofStart = getThreadIndex() * COUNT;
            Map::Handle hMapTemp = HashMap::create();
            for (int32_t i = 0; i < COUNT; i++)
                {
                hMapTemp->put(Integer32::create(ofStart + i), COH_TO_STRING(ofStart + i));
                Random::Handle hRand = Random::getInstance();

                if ((int32_t) hMapTemp->size() > hRand->nextInt32(5) ||
                        i == COUNT - 1)
                    {
                    m_hCache->putAll(hMapTemp);
                    hMapTemp->clear();
                    }
                }
            }

        protected:
            FinalHandle<NamedCache> m_hCache;
            FinalView<Object>       f_vMonitor;
        };

    class TestConcurrentGetRunner
        : public class_spec<TestConcurrentGetRunner,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestConcurrentGetRunner>;

        protected:
            TestConcurrentGetRunner(NamedCache::Handle hCache, Object::View v)
                : m_hCache(self(), hCache), f_vMonitor(self(), v)
                {
                }

        public:
            void run()
            {
            waitForSemaphore(f_vMonitor);
            for (int32_t i = 0; i < COUNT; i++)
                {
                Random::Handle hRand = Random::getInstance();
                int32_t        iKey = hRand->nextInt32(m_hCache->size());
                Object::View vVal = m_hCache->get(Integer32::create(iKey));
                TS_ASSERT(COH_TO_STRING(iKey)->equals(vVal));
                }
            }

        protected:
            FinalHandle<NamedCache> m_hCache;
            FinalView<Object>       f_vMonitor;
        };

    class TestConcurrentGetSameKeyRunner
        : public class_spec<TestConcurrentGetSameKeyRunner,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestConcurrentGetSameKeyRunner>;

        protected:
            TestConcurrentGetSameKeyRunner(NamedCache::Handle hCache, Object::View v)
                : m_hCache(self(), hCache), f_vMonitor(self(), v)
                {
                }

        public:
        virtual void run()
            {
            waitForSemaphore(f_vMonitor);
            for (int32_t i = 0; i < COUNT; i++)
                {
                Object::View vVal = m_hCache->get(Integer32::create(0));
                TS_ASSERT(vVal->equals(String::create("0")));
                }
            }

        protected:
            FinalHandle<NamedCache> m_hCache;
            FinalView<Object>       f_vMonitor;
        };

    class TestConcurrentGetAllRunner
        : public class_spec<TestConcurrentGetAllRunner,
            extends<Object>,
            implements<Runnable> >
        {
        friend class factory<TestConcurrentGetAllRunner>;

        protected:
            TestConcurrentGetAllRunner(NamedCache::Handle hCache, Object::View v)
                : m_hCache(self(), hCache), f_vMonitor(self(), v)
                {
                }

        public:
        virtual void run()
            {
            waitForSemaphore(f_vMonitor);
            Set::Handle hSetKeys = HashSet::create();
            for (int32_t i = 0; i < COUNT; i++)
                {
                Random::Handle  hRand = Random::getInstance();
                Integer32::View vKey  = Integer32::create(hRand->nextInt32(m_hCache->size()));

                hSetKeys->add(vKey);
                if ((int32_t) hSetKeys->size() > hRand->nextInt32(5))
                    {
                    Map::View vMap = m_hCache->getAll(hSetKeys);

                    TS_ASSERT(hSetKeys->size() == vMap->size());
                    hSetKeys->clear();

                    for (Iterator::Handle hIter = vMap->entrySet()->iterator(); hIter->hasNext(); )
                        {
                        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

                        TS_ASSERT(COH_TO_STRING(vEntry->getKey())->equals(vEntry->getValue()));
                        }
                    }
                }
            }

            FinalHandle<NamedCache> m_hCache;
            FinalView<Object>       f_vMonitor;
        };

    // ----- Operation bundling Extend tests --------------------------------
    public:
        /**
        * Test concurrent put operations.
        */
        void testConcurrentPut()
            {
            const NamedCache::Handle hCache = cast<NamedCache::Handle>(ensureCleanCache(getCACHE_NAME(), getCACHE_CONFIG()));

            Object::View     v     = getSEMAPHORE();
            Runnable::Handle hTask = TestConcurrentPutRunner::create(hCache, v);

            resetSemaphore();
            runParallel(hTask, v, THREADS);

            testCacheContent(hCache, THREADS * COUNT);
            hCache->clear();
            }

        /**
        * Test concurrent putAll operations.
        */
        void testConcurrentPutAll()
            {
            const NamedCache::Handle hCache = cast<NamedCache::Handle>(ensureCleanCache(getCACHE_NAME(), getCACHE_CONFIG()));

            Object::View     v     = getSEMAPHORE();
            Runnable::Handle hTask = TestPutAllRunner::create(hCache, v);
            resetSemaphore();
            runParallel(hTask, v, THREADS);

            testCacheContent(hCache, THREADS * COUNT);
            hCache->clear();
            }

        /**
        * Test concurrent get operations.
        */
        void testConcurrentGet()
            {
            const NamedCache::Handle hCache = cast<NamedCache::Handle>(ensureCleanCache(getCACHE_NAME(), getCACHE_CONFIG()));

            fill(hCache, THREADS * COUNT);
            Object::View     v     = getSEMAPHORE();
            Runnable::Handle hTask = TestConcurrentGetRunner::create(hCache, v);

            resetSemaphore();
            runParallel(hTask, v, THREADS);

            // test all threads hitting the same key
            hTask = TestConcurrentGetSameKeyRunner::create(hCache, v);

            resetSemaphore();
            runParallel(hTask, v, THREADS);

            hCache->clear();
            }

        /**
        * Test concurrent getAll operations.
        */
        void testConcurrentGetAll()
            {
            const NamedCache::Handle hCache = cast<NamedCache::Handle>(ensureCleanCache(getCACHE_NAME(), getCACHE_CONFIG()));

            fill(hCache, THREADS * COUNT);
            Object::View     v     = getSEMAPHORE();
            Runnable::Handle hTask = TestConcurrentGetAllRunner::create(hCache, v);

            resetSemaphore();
            runParallel(hTask, v, THREADS);
            hCache->clear();

            // last test, restore to the default cache configuration
            setDefaultCacheConfigure();
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }

        /**
        * Run the specified task on multiple cThreads and wait for completion.
        *
        * @param hTask     the task to run
        * @param cThreads  the number of threads
        */
    protected:
        static void runParallel(Runnable::Handle hTask, Object::View v, int32_t cThreads)
            {
            Thread::Handle ahThreads[THREADS];
            String::View   vsName;
            for (int32_t i = 0; i < cThreads; i++)
                {
                ahThreads[i] = Thread::create(hTask, COH_TO_STRING(getPREFIX() << i));
                ahThreads[i]->start();
                }

            COH_SYNCHRONIZED (v)
                {
                s_fStart = true;
                v->notifyAll();
                }

            try
                {
                for (int32_t i = 0; i < cThreads; i++)
                    {
                    ahThreads[i]->join();
                    }
                }
            catch (InterruptedException::View e) {/*do nothing*/}
            }

        /**
        * Retrive the thread index from its name.
        *
        * @return the thread index
        */
        static int32_t getThreadIndex()
            {
            String::View vName = Thread::currentThread()->getName();
            int32_t      ofIx  = vName->lastIndexOf(getPREFIX());
            TS_ASSERT(ofIx >= 0);
            return Integer32::parse(vName->substring(ofIx + getPREFIX()->length()));
            }

        /**
        * Thread synchronization support.
        */
        static void resetSemaphore()
            {
            s_fStart = false;
            }

        /**
        * Thread synchronization support.
        */
        static void waitForSemaphore(Object::View v)
            {
            COH_SYNCHRONIZED (v)
                {
                while (!s_fStart)
                    {
                    try
                        {
                        v->wait();
                        }
                    catch (InterruptedException::View e) {/*do nothing*/}
                    }
                }
            }

    private:
        /**
        * Test the cache content.
        *
        * @param hCache  the cache to test
        * @param cnt     the count
        */
        static void testCacheContent(Map::Handle hCache, int32_t cnt)
            {
            testCacheContent(hCache, cnt, NULL);
            }

        /**
        * Test the cache content.
        *
        * @param hCache     the cache to test
        * @param cnt        the count
        * @param vExpected  the expected value
        */
        static void testCacheContent(Map::Handle hCache, int32_t cnt, Object::View vExpected)
            {
            TS_ASSERT(cnt == (int32_t) hCache->size());
            for (int32_t i = 0; i < cnt; i++)
                {
                Object::View vVal = hCache->get(Integer32::create(i));
                TS_ASSERT(vVal->equals(vExpected == NULL ? cast<String::View>(COH_TO_STRING(i)) : cast<String::View>(vExpected)));
                }
            }

        /**
        * Fill the specified map with <Integer32, String> entries.
        *
        * @param hCache  the cache to fill
        * @param cnt     the count
        */
        static void fill(Map::Handle hCache, int32_t cnt)
            {
            for (int32_t i = 0; i <= cnt; ++i)
                {
                hCache->put(Integer32::create(i), COH_TO_STRING(i));
                }
            }

    // ----- class local constants -------------------------------------------

    public:
        static String::View getCACHE_NAME()
            {
            return String::create("dist-test");
            }

        static String::View getCACHE_CONFIG()
            {
            return String::create("defaultconfig/extend-bundle-cache-config.xml");
            }

        static String::View getPREFIX()
            {
            return String::create("Thread-");
            }

        static Object::View getSEMAPHORE()
            {
            static FinalView<Object> vo(System::common(), Object::create());
            return vo;
            }

    // ----- fields and constants -------------------------------------------

    public:
        static const int32_t COUNT   = 1000;
        static const int32_t THREADS = 25;

    private:
        static Volatile<bool> s_fStart;
    };

COH_STATIC_INIT(BundlingNamedCacheTests::getSEMAPHORE());

Volatile<bool> BundlingNamedCacheTests::s_fStart(System::common());
