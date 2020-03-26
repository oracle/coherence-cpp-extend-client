/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheStore.hpp"
#include "coherence/net/cache/IterableCacheLoader.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashMap.hpp"


#include "private/coherence/net/cache/LocalConcurrentCache.hpp"

#include "common/TestListener.hpp"

class LocalConcurrentCacheSuite : public CxxTest::TestSuite
    {
    public:
        class TestLockRunner
            : public class_spec<TestLockRunner,
                extends<Object>,
                implements<Runnable> >
            {
            public:
                friend class factory<TestLockRunner>;

                TestLockRunner(LocalConcurrentCache::Handle hCache)
                    : m_hCache(self(), hCache), m_hMonitor(self(), Object::create())
                    {
                    }

                virtual void run()
                    {
                    Integer32::Handle h = Integer32::create(1);

                    COH_SYNCHRONIZED(m_hMonitor)
                        {
                        m_hCache->lock(h);
                        m_hMonitor->notify();

                        m_hMonitor->wait();

                        m_hCache->unlock(h);
                        m_hMonitor->notify();
                        }
                    }

                MemberHandle<LocalConcurrentCache> m_hCache;
                MemberHandle<Object>               m_hMonitor;
            };

        void testLockUnlock()
            {
            LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
            hCache->setLockingEnforced(true);
            TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);

            Object::Handle hMonitor = hRunner->m_hMonitor;
            COH_SYNCHRONIZED(hMonitor)
                {
                Thread::Handle hThread = Thread::create(hRunner);
                hThread->start();
                hMonitor->wait();

                TS_ASSERT(hCache->lock(Integer32::create(1), 0) == false);
                hMonitor->notify();
                hMonitor->wait();
                TS_ASSERT(hCache->lock(Integer32::create(1), 0) == true);
                }
            }

            void testClear()
                {
                LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
                hCache->setLockingEnforced(true);
                for (int x = 0; x < 25; ++x)
                    {
                    Integer32::Handle hI = Integer32::create(x);
                    hCache->put(hI, hI);
                    }

                TS_ASSERT(hCache->size() == 25);
                hCache->clear();
                TS_ASSERT(hCache->size() == 0);

                for (int x = 0; x < 25; ++x)
                    {
                    Integer32::Handle hI = Integer32::create(x);
                    hCache->put(hI, hI);
                    }

                TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);

                // Make sure that we fail to lock here
                hCache->setWaitMillis(0);
                Object::Handle hMonitor = hRunner->m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    Thread::Handle hThread = Thread::create(hRunner);
                    hThread->start();
                    hMonitor->wait();

                    TS_ASSERT_THROWS(hCache->clear(), ConcurrentModificationException::View);
                    hMonitor->notify();
                    }
                }

            void testPut()
                {
                LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
                hCache->setLockingEnforced(true);
                Integer32::Handle hI = Integer32::create(1);
                hCache->put(hI, hI);

                TS_ASSERT(hCache->get(hI) == hI);

                // Make sure that we fail to lock here
                TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);
                hCache->setWaitMillis(0);
                Object::Handle hMonitor = hRunner->m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    Thread::Handle hThread = Thread::create(hRunner);
                    hThread->start();
                    hMonitor->wait();
                    TS_ASSERT_THROWS(hCache->put(hI, Integer32::create(3)), ConcurrentModificationException::View);
                    TS_ASSERT(hCache->get(hI) == hI);
                    hMonitor->notify();
                    }
                }

            void testPutAll()
                {
                LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
                hCache->setLockingEnforced(true);
                HashMap::Handle              hMap   = HashMap::create();

                for(int x = 0; x < 25; ++x)
                    {
                    Integer32::Handle hI = Integer32::create(x);
                    hMap->put(hI, hI);
                    }

                hCache->putAll(hMap);
                TS_ASSERT(hCache->size() == 25);
                TS_ASSERT(hMap->equals(hCache));

                for(Iterator::Handle hIter = hMap->keySet()->iterator(); hIter->hasNext(); )
                     {
                     Integer32::View hI = cast<Integer32::View>(hIter->next());
                     hMap->put(hI, Integer32::create(2*hI->getInt32Value()));
                     }

                 // Make sure that we fail to lock here
                 TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);
                 hCache->setWaitMillis(0);
                 Object::Handle hMonitor = hRunner->m_hMonitor;
                 COH_SYNCHRONIZED(hMonitor)
                     {
                     Thread::Handle hThread = Thread::create(hRunner);
                     hThread->start();
                     hMonitor->wait();
                     TS_ASSERT_THROWS(hCache->putAll(hMap), ConcurrentModificationException::View);
                     for(int x = 0; x < 25; ++x)
                         {
                         TS_ASSERT(!hMap->equals(hCache));
                         }
                     hMonitor->notify();
                     }
                }

            void testRemove()
                {
                LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
                hCache->setLockingEnforced(true);
                Integer32::Handle hI = Integer32::create(1);
                hCache->put(hI, hI);

                TS_ASSERT(hCache->get(hI) == hI);
                TS_ASSERT(hCache->remove(hI) == hI);
                TS_ASSERT(hCache->get(hI) == NULL);
                hCache->put(hI, hI);

                // Make sure that we fail to lock here
                TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);
                hCache->setWaitMillis(0);
                Object::Handle hMonitor = hRunner->m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    Thread::Handle hThread = Thread::create(hRunner);
                    hThread->start();
                    hMonitor->wait();
                    TS_ASSERT_THROWS(hCache->remove(hI), ConcurrentModificationException::View);
                    TS_ASSERT(hCache->get(hI) == hI);
                    hMonitor->notify();
                    }
                }

            void testNoLockingEnforced()
                {
                LocalConcurrentCache::Handle hCache = LocalConcurrentCache::create();
                TS_ASSERT(hCache->isLockingEnforced() == false);

                HashMap::Handle              hMap   = HashMap::create();
                Integer32::Handle            hI     = Integer32::create(1);

                hCache->put(hI, hI);

                for(int x = 0; x < 25; ++x)
                    {
                    Integer32::Handle hX = Integer32::create(x);
                    hMap->put(hX, hX);
                    }

                // Make sure that we fail to lock here
                TestLockRunner::Handle hRunner = TestLockRunner::create(hCache);
                hCache->setWaitMillis(0);
                Object::Handle hMonitor = hRunner->m_hMonitor;
                COH_SYNCHRONIZED(hMonitor)
                    {
                    Thread::Handle hThread = Thread::create(hRunner);
                    hThread->start();
                    hMonitor->wait();

                    hCache->clear();
                    hCache->put(hI, hI);
                    hCache->putAll(hMap);
                    hCache->remove(hI);

                    hMonitor->notify();
                    }
                }
        };
