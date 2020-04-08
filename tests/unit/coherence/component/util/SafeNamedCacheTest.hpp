/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/component/util/SafeCacheService.hpp"

using namespace coherence::run::xml;
using namespace mock;

using coherence::component::util::SafeNamedCache;
using coherence::component::util::SafeCacheService;
using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::Filter;
using mock::BaseMock;
using mock::Expectation;

using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::ValueExtractor;
using coherence::util::InvocableMap;
using coherence::util::ValueExtractor;

COH_OPEN_NAMESPACE_ANON(SafeNamedCacheTest)

// non member MapEvent mock argument matcher
bool mapEventMatcher(ArrayList::View vExpectedArgs, ArrayList::View vActualArgs)
    {
    if (NULL == vExpectedArgs)
        {
        return NULL == vActualArgs;
        }

    if (vExpectedArgs->size() != vActualArgs->size())
        {
        return false;
        }

    for (Iterator::Handle hExpectedIter = vExpectedArgs->iterator(),
            hActualIter = vActualArgs->iterator(); hExpectedIter->hasNext();)
        {
        Object::View vObj = hExpectedIter->next();
        if (instanceof<MapEvent::View>(vObj))
            {
            MapEvent::View vExpectedEvent = cast<MapEvent::View>(vObj);
            MapEvent::View vActualEvent = cast<MapEvent::View>(hActualIter->next());

            if (!Object::equals(vExpectedEvent->getMap(), vActualEvent->getMap()) ||
                    vExpectedEvent->getId() != vActualEvent->getId() ||
                    !Object::equals(vExpectedEvent->getKey(), vActualEvent->getKey()) ||
                    !Object::equals(vExpectedEvent->getOldValue(), vActualEvent->getOldValue()) ||
                    !Object::equals(vExpectedEvent->getNewValue(), vActualEvent->getNewValue()))
                {
                return false;
                }
            }
        else
            {
            if (!Object::equals(vObj, hActualIter->next()))
                {
                return false;
                }
            }
        }
    return true;
    }


// Extends SafeNamedCache to make unit testing easier
class TestSafeNamedCache
    : public class_spec<TestSafeNamedCache,
        extends<SafeNamedCache> >
    {
    friend class factory<TestSafeNamedCache>;

    TestSafeNamedCache(MapListenerSupport::Handle hSupport)
        : super(hSupport)
        {
        }
    };

COH_CLOSE_NAMESPACE_ANON

/**
 * Test Suite for SafeNamedCache
 */
class SafeNamedCacheTest : public CxxTest::TestSuite
    {
    public:
    void testSize()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        mockNamedCache->size();
        mockNamedCache->lastExpectation()->setIntegerReturn(99);

        mockNamedCache->size();
        mockNamedCache->lastExpectation()->setIntegerReturn(0);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->size()==99);
        TS_ASSERT(cache->size()==0);
        //verify
        mockNamedCache->verify();
        }

    void testIsEmpty()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        mockNamedCache->isEmpty();
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->isEmpty();
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->isEmpty());
        TS_ASSERT(!cache->isEmpty());
        //verify
        mockNamedCache->verify();
        }

    void testContainsKey()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        mockNamedCache->containsKey(key);
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->containsKey(key);
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        Object::View key2 = String::create("key");
        TS_ASSERT(cache->containsKey(key2));
        TS_ASSERT(!cache->containsKey(key2));
        //verify
        mockNamedCache->verify();
        }

    void testContainsValue()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        mockNamedCache->containsValue(key);
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->containsValue(key);
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        Object::View key2 = String::create("key");
        TS_ASSERT(cache->containsValue(key2));
        TS_ASSERT(!cache->containsValue(key2));
        //verify
        mockNamedCache->verify();
        }

    void testGet()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        Object::Holder obj = String::create("foo");
        mockNamedCache->get(key);
        mockNamedCache->lastExpectation()->setObjectReturn(obj);

        //replay
        mockNamedCache->replay();
        Object::View key2 = String::create("key");
        TS_ASSERT(cache->get(key2)==obj);
        //verify
        mockNamedCache->verify();
        }

    void testPut()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        Object::Holder value = String::create("value");
        Object::Holder oldValue = String::create("oldValue");
        mockNamedCache->put(key, value);
        mockNamedCache->lastExpectation()->setObjectReturn(oldValue);

        mockNamedCache->put(key, value);
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->put(key, value)==oldValue);
        //TODO : Should be UnsupportedOperationException...
        TS_ASSERT_THROWS(cache->put(key, value),
                             Exception::View);

        //verify
        mockNamedCache->verify();
        }

    void testRemove()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        Object::Holder value = String::create("value");
        mockNamedCache->remove(key);
        mockNamedCache->lastExpectation()->setObjectReturn(value);

        mockNamedCache->remove(key);
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->remove(key)==value);

        //TODO : Should be UnsupportedOperationException...
        TS_ASSERT_THROWS(cache->remove(key),
                Exception::View);
        //verify
        mockNamedCache->verify();
        }

    void testPutAll()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Map::View map = MockNamedCache::create();
        mockNamedCache->putAll(map);

        mockNamedCache->putAll(map);
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        cache->putAll(map);

        //TODO : Should be UnsupportedOperationException...
        TS_ASSERT_THROWS(cache->putAll(map),
                Exception::View);

        //verify
        mockNamedCache->verify();
        }

    void testClear()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        mockNamedCache->clear();

        mockNamedCache->clear();
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        cache->clear();

        //TODO : Should be UnsupportedOperationException...
        TS_ASSERT_THROWS(cache->clear(),
                Exception::View);

        //verify
        mockNamedCache->verify();
        }

    void testKeySet()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();
        mockNamedCache->keySet();
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->keySet()==set);

        //verify
        mockNamedCache->verify();
        }

    void testValues()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();

        mockNamedCache->values();
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->values()==set);

        //verify
        mockNamedCache->verify();
        }

    void testEntrySet()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();

        mockNamedCache->entrySet();
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->entrySet()==set);

        //verify
        mockNamedCache->verify();
        }

    void testGetAll()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::View keySet = DummySet::create();
        Map::Handle map = MockNamedCache::create();
        mockNamedCache->getAll(keySet);
        mockNamedCache->lastExpectation()->setObjectReturn(map);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->getAll(keySet)==map);
        //verify
        mockNamedCache->verify();
        }

    void testPutWithExpiry()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        Object::Holder value = String::create("value");
        Object::Holder oldValue = String::create("oldValue");
        mockNamedCache->put(key, value, 99);
        mockNamedCache->lastExpectation()->setObjectReturn(oldValue);

        mockNamedCache->put(key, value, 99);
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->put(key, value, 99)==oldValue);
        //TODO : Should be UnsupportedOperationException...
        TS_ASSERT_THROWS(cache->put(key, value, 99),
                Exception::View);

        //verify
        mockNamedCache->verify();
        }

    void testRelease()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockSafeCacheService::Handle mockSafeCacheService =
                getMockSafeCacheService(cache);

        //set expectations
        mockSafeCacheService->drainEvents();
        mockSafeCacheService->releaseCache(cache);

        //replay
        mockSafeCacheService->replay();
        //do something to force the cache to initialize ...
        cache->size();
        TS_ASSERT(!cache->isReleased());
        TS_ASSERT(!is_null(cache->getNamedCache()));
        cache->release();
        TS_ASSERT(cache->isReleased());
        TS_ASSERT(is_null(cache->getNamedCache()));
        //verify
        mockSafeCacheService->verify();
        }

    void testDestroy()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockSafeCacheService::Handle mockSafeCacheService =
                getMockSafeCacheService(cache);

        //set expectations
        mockSafeCacheService->drainEvents();
        mockSafeCacheService->destroyCache(cache);

        //replay
        mockSafeCacheService->replay();
        //do something to force the cache to initialize ...
        cache->size();
        TS_ASSERT(!cache->isReleased());
        TS_ASSERT(!is_null(cache->getNamedCache()));
        cache->destroy();
        TS_ASSERT(cache->isReleased());
        TS_ASSERT(is_null(cache->getNamedCache()));
        //verify
        mockSafeCacheService->verify();
        }

    void testGetSetCacheName()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();

        TS_ASSERT(NULL == cache->getCacheName());
        cache->setCacheName("foo");
        TS_ASSERT(cache->getCacheName()->equals("foo"));
        }

    void testIsActive()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        mockNamedCache->clear();
        mockNamedCache->isActive();
        mockNamedCache->lastExpectation()->setBoolReturn(false);
        mockNamedCache->isActive();
        mockNamedCache->lastExpectation()->setBoolReturn(true);
        mockNamedCache->isActive();
        mockNamedCache->lastExpectation()->setException(UnsupportedOperationException::create("expected"));

        //replay
        mockNamedCache->replay();
        //first call should return false... the cache is not initialized.
        TS_ASSERT(!cache->isActive());
        //do something to force the cache to initialize ...
        cache->clear();
        TS_ASSERT(!cache->isActive());
        TS_ASSERT(cache->isActive());
        //exception from the underlying cache should result in a false return.
        TS_ASSERT(!cache->isActive());

        //verify
        mockNamedCache->verify();
        }

    void testGetCacheService()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockSafeCacheService::Handle mockSafeCacheService =
                getMockSafeCacheService(cache);

        NamedCache::CacheServiceHandle hCacheService =
                cache->getCacheService();

        TS_ASSERT(hCacheService==mockSafeCacheService);
        }

    void testGetSetNamedCache()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();

        TS_ASSERT(is_null(cache->getNamedCache()));

        MockNamedCache::Handle mockNamedCache = MockNamedCache::create();
        cache->setNamedCache(mockNamedCache);
        TS_ASSERT(cache->getNamedCache()==mockNamedCache);
        }

    void testGetSetSafeCacheService()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();

        TS_ASSERT(is_null(cache->getSafeCacheService()));

        MockSafeCacheService::Handle mockSafeCacheService =
                MockSafeCacheService::create();
        cache->setSafeCacheService(mockSafeCacheService);
        TS_ASSERT(cache->getSafeCacheService()==mockSafeCacheService);
        }


    void testLock()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        mockNamedCache->lock(key);
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->lock(key);
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->lock(key));
        TS_ASSERT(!cache->lock(key));
        //verify
        mockNamedCache->verify();
        }


    void testLockWithWait()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        mockNamedCache->lock(key, 99);
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->lock(key, 98);
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->lock(key, 99));
        TS_ASSERT(!cache->lock(key, 98));
        //verify
        mockNamedCache->verify();
        }

    void testUnlock()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View key = String::create("key");
        mockNamedCache->unlock(key);
        mockNamedCache->lastExpectation()->setBoolReturn(true);

        mockNamedCache->unlock(key);
        mockNamedCache->lastExpectation()->setBoolReturn(false);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->unlock(key));
        TS_ASSERT(!cache->unlock(key));
        //verify
        mockNamedCache->verify();
        }

    void testInvoke()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View returned = String::create("return");
        Object::View key = String::create("key");
        InvocableMap::EntryProcessor::Handle entityProcessor = DummyEntryProcessor::create();

        mockNamedCache->invoke(key, entityProcessor);
        mockNamedCache->lastExpectation()->setObjectReturn(returned);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->invoke(key, entityProcessor)==returned);
        //verify
        mockNamedCache->verify();
        }

    void testInvokeAll()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Map::View returned = MockNamedCache::create();
        Collection::View collection = ArrayList::create();
        InvocableMap::EntryProcessor::Handle entityProcessor = DummyEntryProcessor::create();

        mockNamedCache->invokeAll(collection, entityProcessor);
        mockNamedCache->lastExpectation()->setObjectReturn(returned);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->invokeAll(collection, entityProcessor)==returned);
        //verify
        mockNamedCache->verify();
        }

    void testInvokeAllWithFilter()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Map::View returned = MockNamedCache::create();
        Filter::View filter = DummyFilter::create();
        InvocableMap::EntryProcessor::Handle entityProcessor = DummyEntryProcessor::create();

        mockNamedCache->invokeAll(filter, entityProcessor);
        mockNamedCache->lastExpectation()->setObjectReturn(returned);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->invokeAll(filter, entityProcessor)==returned);
        //verify
        mockNamedCache->verify();
        }

    void testAggregate()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View returned = String::create("return");
        Collection::View collection = ArrayList::create();
        InvocableMap::EntryAggregator::Handle entryAggregator = DummyEntryAggregator::create();

        mockNamedCache->aggregate(collection, entryAggregator);
        mockNamedCache->lastExpectation()->setObjectReturn(returned);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->aggregate(collection, entryAggregator)==returned);
        //verify
        mockNamedCache->verify();
        }

    void testAggregateWithFilter()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Object::View returned = String::create("return");
        Filter::View filter = DummyFilter::create();
        InvocableMap::EntryAggregator::Handle entryAggregator = DummyEntryAggregator::create();

        mockNamedCache->aggregate(filter, entryAggregator);
        mockNamedCache->lastExpectation()->setObjectReturn(returned);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->aggregate(filter, entryAggregator)==returned);
        //verify
        mockNamedCache->verify();
        }

    void testKeySetWithFilter()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();
        Filter::View filter = DummyFilter::create();
        mockNamedCache->keySet(filter);
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->keySet(filter)==set);

        //verify
        mockNamedCache->verify();
        }

    void testEntrySetWithFilter()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();
        Filter::View filter = DummyFilter::create();
        mockNamedCache->entrySet(filter);
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->entrySet(filter)==set);

        //verify
        mockNamedCache->verify();
        }

    void testEntrySetWithComparator()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        Set::Handle set = DummySet::create();
        Filter::View filter = DummyFilter::create();
        Comparator::View comparator = DummyComparator::create();
        mockNamedCache->entrySet(filter, comparator);
        mockNamedCache->lastExpectation()->setObjectReturn(set);

        //replay
        mockNamedCache->replay();
        TS_ASSERT(cache->entrySet(filter, comparator)==set);

        //verify
        mockNamedCache->verify();
        }

    void testAddIndex()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        ValueExtractor::View extractor = DummyValueExtractor::create();
        Comparator::View comparator = DummyComparator::create();
        mockNamedCache->addIndex(extractor, true, comparator);

        //replay
        mockNamedCache->replay();
        cache->addIndex(extractor, true, comparator);

        //verify
        mockNamedCache->verify();
        }

    void testRemoveIndex()
        {
        SafeNamedCache::Handle cache = SafeNamedCache::create();
        MockNamedCache::Handle mockNamedCache = getMockNamedCache(cache);

        //set expectations
        ValueExtractor::View extractor = DummyValueExtractor::create();
        mockNamedCache->removeIndex(extractor);

        //replay
        mockNamedCache->replay();
        cache->removeIndex(extractor);

        //verify
        mockNamedCache->verify();
        }

    // Event tests

    // ObservableMap

    void testAddKeyListener()
        {
        // using TestSafeNamedCache so that we can set MapListenerSupport
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        String::View vKey = String::create("key");
        MockMapListener::Handle hMockListener = MockMapListener::create();

        // mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty((Object::View) vKey);
        hMockListenerSupport->lastExpectation()->setBoolReturn(false);
        hMockListenerSupport->containsStandardListeners((Object::View) vKey);
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, (Object::View) vKey, false);
        hMockListenerSupport->replay();

        hCache->addKeyListener(hMockListener, vKey, false);

        hMockListenerSupport->verify();
        }

    void testAddKeyListener_ListenerSupportIsEmpty()
        {
        // using TestSafeNamedCache so that we can set MapListenerSupport
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        String::View vKey = String::create("key");
        // create mock MapListenerSupport and set on cache
        MockMapListener::Handle hMockListener = MockMapListener::create();
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        // listener support mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty((Object::View) vKey);
        // return true from isEmpty, causes addMapListener to be called
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, (Object::View) vKey, false);
        hMockListenerSupport->replay();

        // cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->addKeyListener(hCache, vKey, false);
        hMockCache->replay();

        hCache->addKeyListener(hMockListener, vKey, false);

        hMockListenerSupport->verify();
        hMockCache->verify();
        }

    void testAddKeyListener_ListenerSupportIsEmpty_ThrowsException()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        MockCacheService::Handle hMockService = MockCacheService::create();
        String::View vKey = String::create("key");
        MockMapListener::Handle hMockListener = MockMapListener::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        // listener support mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty((Object::View) vKey);
        // return true from isEmpty, causes addMapListener to be called
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, (Object::View) vKey, false);
        // removeListener() should be called due to addKeyListener() throwing an exception
        hMockListenerSupport->removeListener(hMockListener, (Object::View) vKey);
        hMockListenerSupport->replay();

        // cache mock expectations
        hMockCache->setStrict(false);
        hMockCache->addKeyListener(hCache, vKey, false);
        // throw exception from addKeyListener()
        Exception::Handle hEx = RuntimeException::create("test");
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        // mock service expectations
        hMockService->setStrict(false);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        TS_ASSERT_THROWS(hCache->addKeyListener(hMockListener, vKey, false), RuntimeException::View);

        hMockListenerSupport->verify();
        hMockCache->verify();
        }

    void testAddKeyListener_NullListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();

        // test passing in NULL listener; ensure no exception is thrown
        hCache->addKeyListener(NULL, NULL, false);
        }

    void testAddKeyListener_ListenerIsSelf()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        String::View vKey = String::create("key");
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addKeyListener(hCache, vKey, false);
        hMockCache->replay();

        // call method being tested
        hCache->addKeyListener(hCache, vKey, false);

        // verify addKeyListener was called on mock
        hMockCache->verify();
        }

    void testAddKeyListener_ListenerIsSelf_CacheThrowsException()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        String::View vKey = String::create("key");
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addKeyListener(hCache, vKey, false);
        // throw exception from addKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested and ensure exception is thrown
        TS_ASSERT_THROWS(hCache->addKeyListener(hCache, vKey, false),
                         RuntimeException::View);

        hMockCache->verify();
        hMockService->verify();
        }

    void testAddKeyListener_ListenerIsSelf_CacheThrowsException_ServiceNotRunning()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        String::View vKey = String::create("key");
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addKeyListener(hCache, vKey, false);
        // throw exception from addKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        // service is not running
        hMockService->lastExpectation()->setBoolReturn(false);
        hMockService->replay();

        // ensure exception is not thrown if service is not running
        hCache->addKeyListener(hCache, vKey, false);

        hMockCache->verify();
        hMockService->verify();
        }

    void testAddKeyListener_SynchronousListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        String::View vKey = String::create("key");
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        MockNamedCacheSynchronous::Handle hMockCacheSync = MockNamedCacheSynchronous::create();
        MockCacheService::Handle hMockService = MockCacheService::create();
        hCache->setNamedCache(hMockCache);

        // set cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->addKeyListener(hMockCacheSync, vKey, true);
        hMockCache->replay();

        // set service expectations
        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested
        hCache->addKeyListener(hMockCacheSync, vKey, true);

        // verify mock calls
        hMockCache->verify();
        hMockService->verify();
        }

    void testAddKeyListener_MapTriggerListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        String::View vKey = String::create("key");
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        MapListener::Handle hTriggerListener = MockMapTriggerListener::create();
        MockCacheService::Handle hMockService = MockCacheService::create();
        hCache->setNamedCache(hMockCache);

        // set cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->addKeyListener(hTriggerListener, vKey, true);
        hMockCache->replay();

        // set service expectations
        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested
        hCache->addKeyListener(hTriggerListener, vKey, true);

        // verify mock calls
        hMockCache->verify();
        hMockService->verify();
        }

    void testRemoveKeyListener_Empty()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        MockMapListener::Handle hMockListener = MockMapListener::create();
        String::View vKey = String::create("key");

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, (Object::View) vKey);
        hMockListenerSupport->isEmpty((Object::View) vKey);
        // empty after remove
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->replay();

        // mock cache expectations
        hMockCache->setStrict(true);
        // removeKeyListener() should be called because MapListenerSupport::isEmpty() returned true
        hMockCache->removeKeyListener(hCache, vKey);
        hMockCache->replay();


        hCache->removeKeyListener(hMockListener, vKey);
        // verify that mock expectations were met
        hMockListenerSupport->verify();
        hMockCache->verify();
        }

    void testRemoveKeyListener_NotEmpty()
        {
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock MapListenerSupport and set on cache

        MockMapListener::Handle hMockListener = MockMapListener::create();
        String::View vKey = String::create("key");

        // mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, (Object::View) vKey);
        hMockListenerSupport->isEmpty((Object::View) vKey);
        // not empty
        hMockListenerSupport->lastExpectation()->setBoolReturn(false);
        hMockListenerSupport->replay();

        hCache->removeKeyListener(hMockListener, vKey);
        // verify that mock expectations were met
        hMockListenerSupport->verify();
        }

    void testRemoveKeyListener_NullListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // ensure that exception is not thrown
        hCache->removeKeyListener(NULL, NULL);
        }

    void testRemoveKeyListener_ListenerIsSelf()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        String::View vKey = String::create("key");

        // set mock expectattions
        hMockCache->setStrict(true);
        hMockCache->removeKeyListener(hCache, vKey);
        hMockCache->replay();

        // pass self in as listener
        hCache->removeKeyListener(hCache, vKey);

        hMockCache->verify();
        }

    void testRemoveKeyListener_SynchronousListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        String::View vKey = String::create("key");

        // set mock expectattions
        hMockCache->setStrict(true);
        hMockCache->removeKeyListener(hCacheSync, vKey);
        hMockCache->replay();

        // pass synchronous listener in as listener
        hCache->removeKeyListener(hCacheSync, vKey);

        hMockCache->verify();
        }

    void testRemoveKeyListener_SynchronousListener_CacheThrowsException()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        String::View vKey = String::create("key");
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->removeKeyListener(hCacheSync, vKey);
        // throw exception from removeKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested and ensure exception is thrown
        TS_ASSERT_THROWS(hCache->removeKeyListener(hCacheSync, vKey),
                         RuntimeException::View);

        hMockCache->verify();
        hMockService->verify();
        }

    void testRemoveKeyListener_SynchronousListener_CacheThrowsException_CacheNotActive()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        String::View vKey = String::create("key");
        Exception::Handle hEx = RuntimeException::create("test");

        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->removeKeyListener(hCacheSync, vKey);
        // throw exception from removeKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        // cache is not active; causes exception to be eaten
        hMockCache->lastExpectation()->setBoolReturn(false);
        hMockCache->replay();

        // call method being tested and ensure no exception is thrown
        hCache->removeKeyListener(hCacheSync, vKey);

        hMockCache->verify();
        }

    void testAddFilterListener()
        {
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        Filter::View vFilter = MockFilter::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        MockMapListener::Handle hMockListener = MockMapListener::create();

        // mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty(vFilter);
        hMockListenerSupport->lastExpectation()->setBoolReturn(false);
        hMockListenerSupport->containsStandardListeners(vFilter);
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, vFilter, false);
        hMockListenerSupport->replay();

        hCache->addFilterListener(hMockListener, vFilter, false);

        hMockListenerSupport->verify();
        }

    void testAddFilterListener_ListenerSupportIsEmpty()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        MockMapListener::Handle hMockListener = MockMapListener::create();
        Filter::View vFilter = MockFilter::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        // listener support mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty(vFilter);
        // return true from isEmpty, causes addFilterListener to be called
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, vFilter, false);
        hMockListenerSupport->replay();

        // cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->addFilterListener(hCache, vFilter, false);
        hMockCache->replay();

        hCache->addFilterListener(hMockListener, vFilter, false);

        hMockListenerSupport->verify();
        hMockCache->verify();
        }

    void testAddFilterListener_ListenerSupportIsEmpty_ThrowsException()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        MockMapListener::Handle hMockListener = MockMapListener::create();
        MockCacheService::Handle hMockService = MockCacheService::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        // listener support mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->isEmpty((Filter::View) NULL);
        // return true from isEmpty, causes addFilterListener to be called
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->addListener(hMockListener, (Filter::View) NULL, false);
        // removeListener() should be called due to addFilterListener() throwing an exception
        hMockListenerSupport->removeListener(hMockListener, (Filter::View) NULL);
        hMockListenerSupport->replay();

        // cache mock expectations
        hMockCache->setStrict(false);
        hMockCache->addFilterListener(hCache);
        // throw exception from addFilterListener()
        Exception::Handle hEx = RuntimeException::create("test");
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        // mock service expectations
        hMockService->setStrict(false);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        TS_ASSERT_THROWS(hCache->addFilterListener(hMockListener), RuntimeException::View);

        hMockListenerSupport->verify();
        hMockCache->verify();
        }

     void testAddFilterListener_NullListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();

        // test passing in NULL listener; ensure no exception is thrown
        hCache->addFilterListener(NULL);
        }

     void testAddFilterListener_ListenerIsSelf()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        Filter::View vFilter = MockFilter::create();
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addFilterListener(hCache, vFilter, true);
        hMockCache->replay();

        // call method being tested
        hCache->addFilterListener(hCache, vFilter, true);

        // verify addKeyListener was called on mock
        hMockCache->verify();
        }

      void testAddFilterListener_ListenerIsSelf_CacheThrowsException()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        Filter::View vFilter = MockFilter::create();
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addFilterListener(hCache, vFilter, true);
        // throw exception from addKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested and ensure exception is thrown
        TS_ASSERT_THROWS(hCache->addFilterListener(hCache, vFilter, true),
                         RuntimeException::View);

        hMockCache->verify();
        hMockService->verify();
        }

      void testAddFilterListener_ListenerIsSelf_CacheThrowsException_ServiceNotRunning()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        Filter::View vFilter = MockFilter::create();
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->addFilterListener(hCache, vFilter, false);
        // throw exception from addKeyListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        // service is not running
        hMockService->lastExpectation()->setBoolReturn(false);
        hMockService->replay();

        // ensure exception is not thrown if service is not running
        hCache->addFilterListener(hCache, vFilter, false);

        hMockCache->verify();
        hMockService->verify();
        }

      void testAddFilterListener_SynchronousListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        Filter::View vFilter = MockFilter::create();
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        MockNamedCacheSynchronous::Handle hMockCacheSync = MockNamedCacheSynchronous::create();
        MockCacheService::Handle hMockService = MockCacheService::create();
        hCache->setNamedCache(hMockCache);

        // set cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->addFilterListener(hMockCacheSync, vFilter, true);
        hMockCache->replay();

        // set service expectations
        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested
        hCache->addFilterListener(hMockCacheSync, vFilter, true);

        // verify mock calls
        hMockCache->verify();
        hMockService->verify();
        }

      void testAddFilterListener_MapTriggerListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        Filter::View vFilter = MockFilter::create();
        // create mock named cache and set on cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        MapListener::Handle hTriggerListener = MockMapTriggerListener::create();
        MockCacheService::Handle hMockService = MockCacheService::create();
        hCache->setNamedCache(hMockCache);

        // set cache mock expectations
        hMockCache->setStrict(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->addFilterListener(hTriggerListener, vFilter, true);
        hMockCache->replay();

        // set service expectations
        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested
        hCache->addFilterListener(hTriggerListener, vFilter, true);

        // verify mock calls
        hMockCache->verify();
        hMockService->verify();
        }

    void testRemoveFilterListener_Empty()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);

        MockMapListener::Handle hMockListener = MockMapListener::create();
        Filter::View vFilter = MockFilter::create();

        // mock listener support expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, vFilter);
        hMockListenerSupport->isEmpty(vFilter);
        // empty after remove
        hMockListenerSupport->lastExpectation()->setBoolReturn(true);
        hMockListenerSupport->replay();

        // mock cache expectations
        hMockCache->setStrict(true);
        // removeFilterListener() should be called because MapListenerSupport::isEmpty() returned true
        hMockCache->removeFilterListener(hCache, vFilter);
        hMockCache->replay();

        hCache->removeFilterListener(hMockListener, vFilter);
        // verify that mock expectations were met
        hMockListenerSupport->verify();
        hMockCache->verify();
        }

    void testRemoveFilterListener_NotEmpty()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);

        MockMapListener::Handle hMockListener = MockMapListener::create();
        Filter::View vFilter = MockFilter::create();

        // mock expectations
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->removeListener(hMockListener, vFilter);
        hMockListenerSupport->isEmpty(vFilter);
        // not empty
        hMockListenerSupport->lastExpectation()->setBoolReturn(false);
        hMockListenerSupport->replay();

        hCache->removeFilterListener(hMockListener, vFilter);
        // verify that mock expectations were met
        hMockListenerSupport->verify();
        }

     void testRemoveFilterListener_NullListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // ensure that exception is not thrown
        hCache->removeFilterListener(NULL, NULL);
        }

     void testRemoveFilterListener_ListenerIsSelf()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        Filter::View vFilter = MockFilter::create();

        // set mock expectattions
        hMockCache->setStrict(true);
        hMockCache->removeFilterListener(hCache, vFilter);
        hMockCache->replay();

        // pass self in as listener
        hCache->removeFilterListener(hCache, vFilter);

        hMockCache->verify();
        }

     void testRemoveFilterListener_SynchronousListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        Filter::View vFilter = MockFilter::create();

        // set mock expectattions
        hMockCache->setStrict(true);
        hMockCache->removeFilterListener(hCacheSync, vFilter);
        hMockCache->replay();

        // pass synchronous listener in as listener
        hCache->removeFilterListener(hCacheSync, vFilter);

        hMockCache->verify();
        }

     void testRemoveFilterListener_MapTriggerListener()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create MapTriggerListener
        MapListener::Handle hTriggerListener = MockMapTriggerListener::create();
        Filter::View vFilter = MockFilter::create();

        // set mock expectattions
        hMockCache->setStrict(true);
        hMockCache->removeFilterListener(hTriggerListener, vFilter);
        hMockCache->replay();

        // pass MapTriggerListener in as listener
        hCache->removeFilterListener(hTriggerListener, vFilter);

        hMockCache->verify();
        }

     void testRemoveFilterListener_SynchronousListener_CacheThrowsException()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        Filter::View vFilter = MockFilter::create();
        Exception::Handle hEx = RuntimeException::create("test");
        MockCacheService::Handle hMockService = MockCacheService::create();

        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->removeFilterListener(hCacheSync, vFilter);
        // throw exception from removeFilterListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        hMockCache->lastExpectation()->setBoolReturn(true);
        hMockCache->getCacheService();
        hMockCache->lastExpectation()->setObjectReturn(hMockService);
        hMockCache->replay();

        hMockService->setStrict(true);
        hMockService->isRunning();
        hMockService->lastExpectation()->setBoolReturn(true);
        hMockService->replay();

        // call method being tested and ensure exception is thrown
        TS_ASSERT_THROWS(hCache->removeFilterListener(hCacheSync, vFilter),
                         RuntimeException::View);

        hMockCache->verify();
        hMockService->verify();
        }

      void testRemoveFilterListener_SynchronousListener_CacheThrowsException_CacheNotActive()
        {
        SafeNamedCache::Handle hCache = SafeNamedCache::create();
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create synchronous listener
        MapListener::Handle hCacheSync = MockNamedCacheSynchronous::create();
        Filter::View vFilter = MockFilter::create();
        Exception::Handle hEx = RuntimeException::create("test");

        // set mock expectations
        hMockCache->setStrict(true);
        hMockCache->removeFilterListener(hCacheSync, vFilter);
        // throw exception from removeFilterListener
        hMockCache->lastExpectation()->setException(hEx);
        hMockCache->isActive();
        // cache is not active; causes exception to be eaten
        hMockCache->lastExpectation()->setBoolReturn(false);
        hMockCache->replay();

        // call method being tested and ensure no exception is thrown
        hCache->removeFilterListener(hCacheSync, vFilter);

        hMockCache->verify();
        }

    // MapListener

    void testEntryInserted()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);

        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->lastExpectation()->setObjectReturn(hMockCache);
        hMockEvent->replay();

        hMockListenerSupport->fireEvent(
                MapListenerSupport::convertEvent(hMockEvent, hCache, NULL, NULL), true);
        hMockListenerSupport->lastExpectation()->setMatcher(&mapEventMatcher);
        hMockListenerSupport->replay();

        hCache->entryInserted(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }

    void testEntryInserted_EventSourceDoesntMatch()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);

        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->setObjectReturn(MockNamedCache::create());
        hMockEvent->replay();

        // no calls expected on listener support since event doesn't match
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->replay();

        hCache->entryInserted(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }

    void testEntryUpdated()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);

        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->lastExpectation()->setObjectReturn(hMockCache);
        hMockEvent->replay();

        hMockListenerSupport->fireEvent(
                MapListenerSupport::convertEvent(hMockEvent, hCache, NULL, NULL), true);
        hMockListenerSupport->lastExpectation()->setMatcher(&mapEventMatcher);
        hMockListenerSupport->replay();

        hCache->entryUpdated(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }

    void testEntryUpdated_EventSourceDoesntMatch()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->setObjectReturn(MockNamedCache::create());
        hMockEvent->replay();

        // no calls expected on listener support since event doesn't match
        hMockListenerSupport->setStrict(true);
        hMockListenerSupport->replay();

        hCache->entryUpdated(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }

    void testEntryDeleted()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->setObjectReturn(hMockCache);
        hMockEvent->replay();

        hMockListenerSupport->fireEvent(
                MapListenerSupport::convertEvent(hMockEvent, hCache, NULL, NULL), true);
        hMockListenerSupport->setMatcher(&mapEventMatcher);
        hMockListenerSupport->replay();

        hCache->entryDeleted(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }

    void testEntryDeleted_EventSourceDoesntMatch()
        {
        // create mock MapListenerSupport and set on cache
        MockMapListenerSupport::Handle hMockListenerSupport =
                MockMapListenerSupport::create();
        // using TestSafeNamedCache so that we can set MapListenerSupport
        TestSafeNamedCache::Handle hCache = TestSafeNamedCache::create(hMockListenerSupport);
        // create mock cache and set on safe cache
        MockNamedCache::Handle hMockCache = MockNamedCache::create();
        hCache->setNamedCache(hMockCache);
        // create mock MapEvent
        MockMapEvent::Handle hMockEvent = MockMapEvent::create();

        // set mock expectations
        hMockEvent->getSource();
        // return the same cache that was set on the safe cache
        hMockEvent->setObjectReturn(MockNamedCache::create());
        hMockEvent->replay();

        // no calls expected on listener support since event doesn't match
        hMockListenerSupport->replay();

        hCache->entryDeleted(hMockEvent);

        hMockEvent->verify();
        hMockListenerSupport->verify();
        }


    private:

    MockNamedCache::Handle getMockNamedCache(SafeNamedCache::Handle cache)
        {
        MockSafeCacheService::Handle mockSafeCacheService =
                MockSafeCacheService::create();
        cache->setSafeCacheService(mockSafeCacheService);

        MockNamedCache::Handle mockNamedCache =
                mockSafeCacheService->getMockCacheService()->getMockNamedCache();
        return mockNamedCache;
        }

    MockSafeCacheService::Handle getMockSafeCacheService(
            SafeNamedCache::Handle cache)
        {
        MockSafeCacheService::Handle mockSafeCacheService =
                MockSafeCacheService::create();
        cache->setSafeCacheService(mockSafeCacheService);

        return mockSafeCacheService;
        }

    };
