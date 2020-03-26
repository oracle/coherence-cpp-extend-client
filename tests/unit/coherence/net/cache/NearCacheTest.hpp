/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/net/cache/CachingMap.hpp"
#include "coherence/net/cache/LocalCache.hpp"
#include "coherence/net/cache/NearCache.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"

#include "common/TestUtils.hpp"

using coherence::net::cache::CacheMap;
using coherence::net::cache::LocalCache;
using coherence::util::Collections;
using coherence::util::HashMap;

COH_PRAGMA_PUSH // Windows warnings

class TestNearCache
    : public class_spec<TestNearCache,
        extends<NearCache> >
    {
    friend class factory<TestNearCache>;

    protected:
        TestNearCache(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
                CachingMap::InvalidationStrategy nStrategy = CachingMap::listen_auto,
                ConcurrentMap::Handle hMapControl = NULL)
            : super(hMapFront, hMapBack, nStrategy, hMapControl)
            {
            }

    public:
        virtual MemberListener::View getBackMapListener() const
            {
            return f_hListenerBackService;
            }

    };

class NearCacheTest : public CxxTest::TestSuite
    {
    public:

    void testGetBackCache()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto);
        TS_ASSERT(hNearCache->getBackCache() == hBackMockNamedCache);

        }

    void testGetCacheName()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        //set expectations
        hBackMockNamedCache->getCacheName();
        hBackMockNamedCache->setObjectReturn(String::create("foo"));

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto, hMockConcurrentMap);
        TS_ASSERT(hNearCache->getCacheName()->equals(String::create("foo")));

        //verify
        hBackMockNamedCache->verify();
        }

    void testGetCacheService()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockCacheService::Handle hMockCacheService = MockCacheService::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        //set expectations
        hBackMockNamedCache->getCacheService();
        hBackMockNamedCache->setObjectReturn(hMockCacheService);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto, hMockConcurrentMap);
        TS_ASSERT(hNearCache->getCacheService() == hMockCacheService);

        //verify
        hBackMockNamedCache->verify();
        }

    void testIsActive()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        //set expectations
        hBackMockNamedCache->isActive();
        hBackMockNamedCache->setBoolReturn(true);

        hBackMockNamedCache->isActive();
        hBackMockNamedCache->setBoolReturn(false);

        hBackMockNamedCache->isActive();
        hBackMockNamedCache->setException(IllegalStateException::create());

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto, hMockConcurrentMap);
        TS_ASSERT(hNearCache->isActive());
        TS_ASSERT(!hNearCache->isActive());
        TS_ASSERT(!hNearCache->isActive());

        //verify
        hBackMockNamedCache->verify();
        }

    void testRelease()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockCacheService::Handle hMockCacheService = MockCacheService::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        //set expectations
        hBackMockNamedCache->getCacheService();
        hBackMockNamedCache->setObjectReturn(hMockCacheService);

        hMockConcurrentMap->lock(ConcurrentMap::getLockAll(), 0);
        hMockConcurrentMap->setBoolReturn(true);

        //replay
        hBackMockNamedCache->replay();
        hMockConcurrentMap->replay();
        hMockCacheService->replay();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto, hMockConcurrentMap);
        hNearCache->release();

        //verify
        hBackMockNamedCache->verify();
        hMockConcurrentMap->verify();
        hMockCacheService->verify();
        }

    void testDestroy()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockCacheService::Handle hMockCacheService = MockCacheService::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        //set expectations
        hBackMockNamedCache->getCacheService();
        hBackMockNamedCache->setObjectReturn(hMockCacheService);

        hMockConcurrentMap->lock(ConcurrentMap::getLockAll(), 0);
        hMockConcurrentMap->setBoolReturn(true);

        hBackMockNamedCache->destroy();

        //replay
        hBackMockNamedCache->replay();
        hMockConcurrentMap->replay();
        hMockCacheService->replay();

        NearCache::Handle hNearCache =
                NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_auto, hMockConcurrentMap);
        hNearCache->destroy();

        //verify
        hBackMockNamedCache->verify();
        hMockConcurrentMap->verify();
        hMockCacheService->verify();
        }


    void testPut()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vKey = String::create("key");
        Object::Handle hValue = String::create("value");

        //set expectations
        hBackMockNamedCache->put(vKey, hValue, 0);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_present, hMockConcurrentMap);

        hNearCache->put(vKey, hValue, 0);

        //verify
        hBackMockNamedCache->verify();
        }

    void testLock()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vKey = String::create("key");

        //set expectations
        hBackMockNamedCache->lock(vKey, 1);
        hBackMockNamedCache->setBoolReturn(true);
        hFrontMockNamedCache->remove(vKey);

        hBackMockNamedCache->lock(vKey, 0);
        hBackMockNamedCache->setBoolReturn(true);
        hFrontMockNamedCache->remove(vKey);

        //replay
        hBackMockNamedCache->replay();
        hFrontMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->lock(vKey, 1);
        hNearCache->lock(vKey);

        //verify
        hBackMockNamedCache->verify();
        hFrontMockNamedCache->verify();
        }

    void testUnlock()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vKey = String::create("key");

        //set expectations
        hBackMockNamedCache->unlock(vKey);
        hBackMockNamedCache->setBoolReturn(true);

        hBackMockNamedCache->unlock(vKey);
        hBackMockNamedCache->setBoolReturn(false);

        //replay
        hBackMockNamedCache->replay();
        hFrontMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->unlock(vKey));
        TS_ASSERT(!hNearCache->unlock(vKey));

        //verify
        hBackMockNamedCache->verify();
        hFrontMockNamedCache->verify();
        }


    void testKeySet()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Filter::Handle filter = DummyFilter::create();
        Set::Handle  returnSet = DummySet::create();

        //set expectations
        hBackMockNamedCache->keySet(filter);
        hBackMockNamedCache->setObjectReturn(returnSet);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->keySet(filter)->equals(returnSet));

        //verify
        hBackMockNamedCache->verify();
        }

    void testEntrySet()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Filter::Handle filter = DummyFilter::create();
        Set::Handle  returnSet = DummySet::create();
        Comparator::Handle comparator = DummyComparator::create();

        //set expectations
        hBackMockNamedCache->entrySet(filter);
        hBackMockNamedCache->setObjectReturn(returnSet);

        hBackMockNamedCache->entrySet(filter, comparator);
        hBackMockNamedCache->setObjectReturn(returnSet);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->entrySet(filter)->equals(returnSet));
        TS_ASSERT(hNearCache->entrySet(filter, comparator)->equals(returnSet));

        //verify
        hBackMockNamedCache->verify();
        }

    void testAddIndex()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        ValueExtractor::Handle extractor = DummyValueExtractor::create();
        Comparator::Handle comparator = DummyComparator::create();

        //set expectations
        hBackMockNamedCache->addIndex(extractor, true, comparator);
        hBackMockNamedCache->addIndex(extractor, false, comparator);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->addIndex(extractor, true, comparator);
        hNearCache->addIndex(extractor, false, comparator);

        //verify
        hBackMockNamedCache->verify();
        }

    void testRemoveIndex()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        ValueExtractor::Handle extractor = DummyValueExtractor::create();

        //set expectations
        hBackMockNamedCache->removeIndex(extractor);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->removeIndex(extractor);

        //verify
        hBackMockNamedCache->verify();
        }

    void testInvoke()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vKey = String::create("key");
        Object::View vReturn = String::create("return");
        InvocableMap::EntryProcessor::Handle processor = DummyEntryProcessor::create();

        //set expectations
        hBackMockNamedCache->invoke(vKey, processor);
        hBackMockNamedCache->setObjectReturn(vReturn);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->invoke(vKey, processor)->equals(vReturn));

        //verify
        hBackMockNamedCache->verify();
        }

    void testInvokeAllWithFilter()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Map::Handle hMap = HashMap::create();
        Filter::Handle filter = DummyFilter::create();
        InvocableMap::EntryProcessor::Handle processor = DummyEntryProcessor::create();

        //set expectations
        hBackMockNamedCache->invokeAll((Filter::View)filter, processor);
        hBackMockNamedCache->setObjectReturn(hMap);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->invokeAll((Filter::View)filter, processor)->equals(hMap));

        //verify
        hBackMockNamedCache->verify();
        }

    void testInvokeAllWithCollection()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Map::Handle hMap = HashMap::create();
        Collection::Handle hCollection = HashSet::create();
        InvocableMap::EntryProcessor::Handle processor = DummyEntryProcessor::create();

        //set expectations
        hBackMockNamedCache->invokeAll((Collection::View)hCollection, processor);
        hBackMockNamedCache->setObjectReturn(hMap);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->invokeAll((Collection::View)hCollection, processor)->equals(hMap));

        //verify
        hBackMockNamedCache->verify();
        }

    void testAggregateWithFilter()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vReturn = String::create("return");
        Filter::Handle filter = DummyFilter::create();
        InvocableMap::EntryAggregator::Handle aggregator = DummyEntryAggregator::create();

        //set expectations
        hBackMockNamedCache->aggregate((Filter::View)filter, aggregator);
        hBackMockNamedCache->setObjectReturn(vReturn);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->aggregate((Filter::View)filter, aggregator)->equals(vReturn));

        //verify
        hBackMockNamedCache->verify();
        }

    void testAggregateWithCollection()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        Object::View vReturn = String::create("return");
        Collection::Handle hCollection = HashSet::create();
        InvocableMap::EntryAggregator::Handle aggregator = DummyEntryAggregator::create();

        //set expectations
        hBackMockNamedCache->aggregate((Collection::View)hCollection, aggregator);
        hBackMockNamedCache->setObjectReturn(vReturn);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        TS_ASSERT(hNearCache->aggregate((Collection::View)hCollection, aggregator)->equals(vReturn));

        //verify
        hBackMockNamedCache->verify();
        }

    void testAddKeyListener()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        MockMapListener::Handle hListener = MockMapListener::create();
        Object::View vKey = String::create("key");

        //set expectations
        hBackMockNamedCache->addKeyListener(hListener, vKey, true);
        hBackMockNamedCache->addKeyListener(hListener, vKey, false);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->addKeyListener(hListener, vKey, true);
        hNearCache->addKeyListener(hListener, vKey, false);

        //verify
        hBackMockNamedCache->verify();
        }

    void testRemoveKeyListener()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        MockMapListener::Handle hListener = MockMapListener::create();
        Object::View vKey = String::create("key");

        //set expectations
        hBackMockNamedCache->removeKeyListener(hListener, vKey);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->removeKeyListener(hListener, vKey);

        //verify
        hBackMockNamedCache->verify();
        }

    void testAddFilterListener()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        MockMapListener::Handle hListener = MockMapListener::create();
        Filter::Handle filter = DummyFilter::create();

        //set expectations
        hBackMockNamedCache->addFilterListener(hListener, filter, true);
        hBackMockNamedCache->addFilterListener(hListener, filter, false);
        hBackMockNamedCache->addFilterListener(hListener, filter, false);
        hBackMockNamedCache->addFilterListener(hListener, NULL, false);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->addFilterListener(hListener, filter, true);
        hNearCache->addFilterListener(hListener, filter, false);
        hNearCache->addFilterListener(hListener, filter);
        hNearCache->addFilterListener(hListener);

        //verify
        hBackMockNamedCache->verify();
        }

    void testRemoveFilterListener()
        {
        MockNamedCache::Handle hFrontMockNamedCache = MockNamedCache::create();
        MockNamedCache::Handle hBackMockNamedCache = MockNamedCache::create();

        MockConcurrentMap::Handle hMockConcurrentMap = MockConcurrentMap::create();

        MockMapListener::Handle hListener = MockMapListener::create();
        Filter::Handle filter = DummyFilter::create();

        //set expectations
        hBackMockNamedCache->removeFilterListener(hListener, filter);
        hBackMockNamedCache->removeFilterListener(hListener, NULL);

        //replay
        hBackMockNamedCache->replay();

        NearCache::Handle hNearCache =
            NearCache::create(hFrontMockNamedCache, hBackMockNamedCache,
                        CachingMap::listen_none, hMockConcurrentMap);

        hNearCache->removeFilterListener(hListener, filter);
        hNearCache->removeFilterListener(hListener);

        //verify
        hBackMockNamedCache->verify();
        }
    };


