/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/CachingMap.hpp"
#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"

using coherence::net::cache::CacheMap;
using coherence::net::cache::LocalCache;
using coherence::util::ArrayList;
using coherence::util::HashMap;

COH_PRAGMA_PUSH // Windows warnings

class TestCachingMap
    : public class_spec<TestCachingMap,
        extends<CachingMap> >
    {
    friend class factory<TestCachingMap>;

    protected:
        TestCachingMap(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
                enum InvalidationStrategy nStrategy = CachingMap::listen_auto,
                ConcurrentMap::Handle hMapControl = NULL)
                : super(hMapFront, hMapBack, nStrategy, hMapControl)
            {
            }

    public:
        virtual MapListener::Handle getBackMapListener() const
            {
            return m_hListener;
            }

        virtual MapListener::Handle getFrontMapListener() const
            {
            return f_hListenerFront;
            }

        virtual void setCurrentInvalidationStrategy(enum InvalidationStrategy strategy)
            {
            m_strategyCurrent = strategy;
            }

        virtual Object::View getGlobalKey()
            {
            return f_vKeyGlobal;
            }

        virtual void invokeInvalidateFront(Object::View vKey) const
            {
            invalidateFront(vKey);
            }

    };


class CachingMapTest : public CxxTest::TestSuite
    {
    public:

    void testRelease()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
            TestCachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_auto, hMockConcurrentMap);

        //set expectations
        hMockConcurrentMap->lock(ConcurrentMap::getLockAll(), 0);
        hMockConcurrentMap->setBoolReturn(true);
        hMockConcurrentMap->put(hCachingMap->getGlobalKey(), ArrayList::create());
        hMockConcurrentMap->remove(hCachingMap->getGlobalKey());
        hMockConcurrentMap->unlock(ConcurrentMap::getLockAll());

        //replay
        hMockConcurrentMap->replay();
        hCachingMap->release();

        //verify
        hMockConcurrentMap->verify();
        }

    void testGetFrontMap()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        CachingMap::Handle hCachingMap = CachingMap::create(hFrontMap, hBackMap);

        TS_ASSERT(hCachingMap->getFrontMap() == hFrontMap);
        }

    void testGetBackMap()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        CachingMap::Handle hCachingMap = CachingMap::create(hFrontMap, hBackMap);

        TS_ASSERT(hCachingMap->getBackMap() == hBackMap);
        }

    void testGetInvalidationStrategy()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        CachingMap::Handle hCachingMap = CachingMap::create(hFrontMap, hBackMap);

        TS_ASSERT(hCachingMap->getInvalidationStrategy()== CachingMap::listen_auto );
        }

    void testGetControlMap()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_auto, hMockConcurrentMap);
        TS_ASSERT(hCachingMap->getControlMap()== hMockConcurrentMap );
        }


    void testGetCacheStatistics()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_auto, hMockConcurrentMap);


        CacheStatistics::Handle hStats = hCachingMap->getCacheStatistics();


        TS_ASSERT(hStats->getCacheMisses()== 0 );

        hCachingMap->get(String::create("foo"));

        TS_ASSERT(hStats->getCacheMisses()== 1 );
        }

    void testGetInvalidationHits()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_auto, hMockConcurrentMap);

        hCachingMap->put(String::create("foo"), String::create("bar"));
        hCachingMap->get(String::create("foo"));
        TS_ASSERT(hCachingMap->getInvalidationHits()== 0 );

        hCachingMap->invokeInvalidateFront(String::create("foo"));

        TS_ASSERT(hCachingMap->getInvalidationHits()== 1 );
        }

    void testGetInvalidationMisses()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_auto, hMockConcurrentMap);

        hCachingMap->put(String::create("foo"), String::create("bar"));

        TS_ASSERT(hCachingMap->getInvalidationMisses()== 0 );

        hCachingMap->invokeInvalidateFront(String::create("foobad"));

        TS_ASSERT(hCachingMap->getInvalidationMisses()== 1 );
        }

    void testGetTotalRegisterListener()
        {
        CacheMap::Handle hMockObservableFrontMap = LocalCache::create();
        CacheMap::Handle hMockObservableBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        String::View vKey = String::create("foo");

        TS_ASSERT(hCachingMap->getTotalRegisterListener()== 0 );

        hCachingMap->registerListener(vKey);

        TS_ASSERT(hCachingMap->getTotalRegisterListener()== 1 );
        }

    void testPut()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        hCachingMap->put(String::create("foo"), String::create("bar"));

        Object::Holder ohOldValue = hCachingMap->put(String::create("foo"), String::create("newbar"), true, 0);

        TS_ASSERT(ohOldValue->equals(String::create("bar")));
        }

    void testGetAll()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        hCachingMap->put(String::create("foo2"), String::create("bar2"));
        hCachingMap->put(String::create("foo3"), String::create("bar3"));

        ArrayList::Handle hKeys = ArrayList::create();

        hKeys->add(String::create("foo1"));
        hKeys->add(String::create("foo3"));
        hKeys->add(String::create("foo4"));

        Map::View hGet = hCachingMap->getAll(hKeys);

        TS_ASSERT(hGet->size()==2);
        TS_ASSERT(hGet->containsKey(String::create("foo1")));
        TS_ASSERT(hGet->containsKey(String::create("foo3")));
        }


    void testSize()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        TS_ASSERT(hCachingMap->size() == 0);
        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        TS_ASSERT(hCachingMap->size() == 1);
        hCachingMap->put(String::create("foo2"), String::create("bar2"));
        TS_ASSERT(hCachingMap->size() == 2);
        hCachingMap->put(String::create("foo3"), String::create("bar3"));
        TS_ASSERT(hCachingMap->size() == 3);
        hCachingMap->remove(String::create("foo3"));
        TS_ASSERT(hCachingMap->size() == 2);

        }

    void testIsEmpty()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        TS_ASSERT(hCachingMap->isEmpty());
        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        TS_ASSERT(!hCachingMap->isEmpty());
        hCachingMap->remove(String::create("foo1"));
        TS_ASSERT(hCachingMap->isEmpty());

        }

    void testContainsKey()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        TS_ASSERT(!hCachingMap->containsKey(String::create("foo1")));
        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        TS_ASSERT(hCachingMap->containsKey(String::create("foo1")));
        hCachingMap->remove(String::create("foo1"));
        TS_ASSERT(!hCachingMap->containsKey(String::create("foo1")));

        }

    void testContainsValue()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        TS_ASSERT(!hCachingMap->containsValue(String::create("bar1")));
        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        TS_ASSERT(hCachingMap->containsValue(String::create("bar1")));
        hCachingMap->remove(String::create("foo1"));
        TS_ASSERT(!hCachingMap->containsValue(String::create("bar1")));

        }

    void testGet()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        Object::Holder ohValue = hCachingMap->get(String::create("foo1"));
        TS_ASSERT(ohValue->equals(String::create("bar1")));

        ohValue = hCachingMap->get(String::create("foo2"));
        TS_ASSERT(NULL == ohValue);

        }

    void testRemove()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        TS_ASSERT(!hCachingMap->containsKey(String::create("foo1")));
        hCachingMap->put(String::create("foo1"), String::create("bar1"));
        TS_ASSERT(hCachingMap->containsKey(String::create("foo1")));
        hCachingMap->remove(String::create("foo1"));
        TS_ASSERT(!hCachingMap->containsKey(String::create("foo1")));

        }


    void testPutAll()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        Map::Handle hMap = HashMap::create();

        hMap->put(String::create("foo1"), String::create("bar1"));
        hMap->put(String::create("foo2"), String::create("bar2"));
        hMap->put(String::create("foo3"), String::create("bar3"));

        TS_ASSERT(!hCachingMap->containsKey(String::create("foo1")));
        TS_ASSERT(!hCachingMap->containsKey(String::create("foo2")));
        TS_ASSERT(!hCachingMap->containsKey(String::create("foo3")));

        hCachingMap->putAll(hMap);
        TS_ASSERT(hCachingMap->containsKey(String::create("foo1")));
        TS_ASSERT(hCachingMap->containsKey(String::create("foo2")));
        TS_ASSERT(hCachingMap->containsKey(String::create("foo3")));

        }

    void testClear()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        Map::Handle hMap = HashMap::create();

        hMap->put(String::create("foo1"), String::create("bar1"));
        hMap->put(String::create("foo2"), String::create("bar2"));
        hMap->put(String::create("foo3"), String::create("bar3"));

        hCachingMap->putAll(hMap);

        TS_ASSERT(hCachingMap->size() == 3);

        hCachingMap->clear();

        TS_ASSERT(hCachingMap->size() == 0);
        }


    void testKeySet()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        Map::Handle hMap = HashMap::create();

        hMap->put(String::create("foo1"), String::create("bar1"));
        hMap->put(String::create("foo2"), String::create("bar2"));
        hMap->put(String::create("foo3"), String::create("bar3"));

        hCachingMap->putAll(hMap);

        Set::View vKeys = hCachingMap->keySet();

        TS_ASSERT(vKeys->contains(String::create("foo1")));
        TS_ASSERT(vKeys->contains(String::create("foo2")));
        TS_ASSERT(vKeys->contains(String::create("foo3")));
        TS_ASSERT(vKeys->size() == 3);

        Set::Handle hKeys = hCachingMap->keySet();

        TS_ASSERT(hKeys->contains(String::create("foo1")));
        TS_ASSERT(hKeys->contains(String::create("foo2")));
        TS_ASSERT(hKeys->contains(String::create("foo3")));
        TS_ASSERT(hKeys->size() == 3);
        }


    void testValues()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        Map::Handle hMap = HashMap::create();

        hMap->put(String::create("foo1"), String::create("bar1"));
        hMap->put(String::create("foo2"), String::create("bar2"));
        hMap->put(String::create("foo3"), String::create("bar3"));

        hCachingMap->putAll(hMap);

        Collection::View vVals = hCachingMap->values();

        TS_ASSERT(vVals->contains(String::create("bar1")));
        TS_ASSERT(vVals->contains(String::create("bar2")));
        TS_ASSERT(vVals->contains(String::create("bar3")));
        TS_ASSERT(vVals->size() == 3);

        Collection::Handle hVals = hCachingMap->values();

        TS_ASSERT(hVals->contains(String::create("bar1")));
        TS_ASSERT(hVals->contains(String::create("bar2")));
        TS_ASSERT(hVals->contains(String::create("bar3")));
        TS_ASSERT(hVals->size() == 3);
        }


    void testEntrySet()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        Map::Handle hMap = HashMap::create();

        hMap->put(String::create("foo1"), String::create("bar1"));
        hMap->put(String::create("foo2"), String::create("bar2"));
        hMap->put(String::create("foo3"), String::create("bar3"));

        hCachingMap->putAll(hMap);

        Set::View vKeys = hCachingMap->entrySet();

        TS_ASSERT(vKeys->size() == 3);

        Set::Handle hKeys = hCachingMap->entrySet();

        TS_ASSERT(hKeys->size() == 3);
        }

    void testGetOrDefault()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_none, hMockConcurrentMap);

        Integer32::View     vKey   = Integer32::create(123);
        Integer32::Handle   hValue = Integer32::create(456);

        TS_ASSERT(hCachingMap->get(vKey) == NULL);
        Object::Holder ohObject = hCachingMap->getOrDefault(vKey, hValue);
        TS_ASSERT(ohObject->equals(hValue));
        }

    void testPutIfAbsent()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_none, hMockConcurrentMap);

        Integer32::View     vKey    = Integer32::create(123);
        Integer32::Handle   hValue1 = Integer32::create(456);
        Integer32::Handle   hValue2 = Integer32::create(789);

        TS_ASSERT(hCachingMap->get(vKey) == NULL);

        Object::Holder ohObject = hCachingMap->putIfAbsent(vKey, hValue1);
        TS_ASSERT(ohObject == NULL);
        TS_ASSERT(hCachingMap->get(vKey)->equals(hValue1));

        ohObject = hCachingMap->putIfAbsent(vKey, hValue2);
        TS_ASSERT(ohObject->equals(hValue1));
        }

    void testRemoveIfPresent()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_none, hMockConcurrentMap);

        Integer32::View   vKey    = Integer32::create(123);
        Integer32::View   vValue1 = Integer32::create(456);
        Integer32::View   vValue2 = Integer32::create(789);

        hCachingMap->put(vKey, vValue2);

        TS_ASSERT(!hCachingMap->remove(vKey, vValue1));
        TS_ASSERT(hCachingMap->remove(vKey, vValue2));
        }

    void testReplaceOldWithNew()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_none, hMockConcurrentMap);

        Integer32::View     vKey    = Integer32::create(123);
        Integer32::Handle   hValue1 = Integer32::create(456);
        Integer32::Handle   hValue2 = Integer32::create(789);

        hCachingMap->put(vKey, hValue1);     
        TS_ASSERT(hCachingMap->replace(vKey, hValue1, hValue2));
        TS_ASSERT(hCachingMap->get(vKey)->equals(hValue2));

        hCachingMap->remove(vKey);
        hCachingMap->put(vKey, hValue2);
        TS_ASSERT(!hCachingMap->replace(vKey, hValue1, hValue2));
        TS_ASSERT(hCachingMap->get(vKey)->equals(hValue2));
        }

    void testReplaceWithValue()
        {
        CacheMap::Handle hFrontMap = LocalCache::create();
        CacheMap::Handle hBackMap  = LocalCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        CachingMap::Handle hCachingMap =
                CachingMap::create(hFrontMap, hBackMap,
                        CachingMap::listen_none, hMockConcurrentMap);

        Integer32::View     vKey    = Integer32::create(123);
        Integer32::Handle   hValue1 = Integer32::create(456);
        Integer32::Handle   hValue2 = Integer32::create(789);

        TS_ASSERT(hCachingMap->replace(vKey, hValue1) == NULL);

        hCachingMap->put(vKey, hValue1);
        TS_ASSERT(hCachingMap->replace(vKey, hValue2)->equals(hValue1));
        TS_ASSERT(hCachingMap->get(vKey)->equals(hValue2));
        }

    void testRegisterListener()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        //set expectations
        hMockObservableBackMap->addFilterListener(hCachingMap->getBackMapListener(), NULL, true);

        //replay
        hMockObservableBackMap->replay();
        hCachingMap->registerListener();

        //verify
        hMockObservableBackMap->verify();
        }

    void testUnregisterListener()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        //set expectations
        hMockObservableBackMap->removeFilterListener(hCachingMap->getBackMapListener(), NULL);

        //replay
        hMockObservableBackMap->replay();
        hCachingMap->unregisterListener();

        //verify
        hMockObservableBackMap->verify();
        }


    void testRegisterListenerWithKey()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);


        String::View vKey = String::create("foo");

        //set expectations
        hMockObservableBackMap->addKeyListener(hCachingMap->getBackMapListener(),
                vKey, true);
        TS_ASSERT(hCachingMap->getTotalRegisterListener()== 0 );

        //replay
        hMockObservableBackMap->replay();
        hCachingMap->registerListener(vKey);

        TS_ASSERT(hCachingMap->getTotalRegisterListener()== 1 );

        //verify
        hMockObservableBackMap->verify();
        }


    void testUnregisterListenerWithKey()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        hCachingMap->setCurrentInvalidationStrategy(CachingMap::listen_present);

        String::View vKey = String::create("foo");

        //set expectations
        hMockConcurrentMap->lock(vKey, 0);
        hMockConcurrentMap->setBoolReturn(true);
        hMockObservableBackMap->removeKeyListener(hCachingMap->getBackMapListener(), vKey);
        hMockConcurrentMap->unlock(vKey);

        //replay
        hMockConcurrentMap->replay();
        hMockObservableBackMap->replay();
        hCachingMap->unregisterListener(vKey);

        //verify
        hMockConcurrentMap->verify();
        hMockObservableBackMap->verify();
        }

    void testRegisterFrontListener()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        hCachingMap->setCurrentInvalidationStrategy(CachingMap::listen_present);

        MapListener::Handle hFrontListener = hCachingMap->getFrontMapListener();

        //set expectations
        hMockObservableFrontMap->addFilterListener(hFrontListener,
                MapEventFilter::create(MapEventFilter::e_deleted), true);

        //replay
        hMockObservableFrontMap->replay();
        hCachingMap->registerFrontListener();

        //verify
        hMockObservableFrontMap->verify();
        }

    void testUnregisterFrontListener()
        {
        MockObservableMap::Handle hMockObservableFrontMap = MockObservableMap::create();
        MockObservableMap::Handle hMockObservableBackMap = MockObservableMap::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        TestCachingMap::Handle hCachingMap =
                TestCachingMap::create(hMockObservableFrontMap, hMockObservableBackMap,
                        CachingMap::listen_present, hMockConcurrentMap);

        MapListener::Handle hFrontListener = hCachingMap->getFrontMapListener();

        //set expectations
        hMockObservableFrontMap->removeFilterListener(hFrontListener,
                MapEventFilter::create(MapEventFilter::e_deleted));

        //replay
        hMockObservableFrontMap->replay();
        hCachingMap->unregisterFrontListener();

        //verify
        hMockObservableFrontMap->verify();
        }
    };


