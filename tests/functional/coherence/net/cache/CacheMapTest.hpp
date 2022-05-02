/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/cache/CacheEvent.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

#include "common/TestListener.hpp"
#include "common/TestUtils.hpp"

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::net::cache::CacheEvent;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Map;


/**
* CacheMap Test Suite.
*/
class CacheMapTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getAll().
        */
        void testGetAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-cache");

            String::View vsKey1 = "key1";
            String::View vsKey2 = "key2";
            String::View vsKey3 = "key3";
            String::View vsKey4 = "key4";
            String::View vsVal1 = "val1";
            String::View vsVal2 = "val2";
            String::View vsVal3 = "val3";
            String::View vsVal4 = "val4";

            Collection::Handle hColKeys = ArrayList::create();
            hColKeys->add(vsKey1);
            hColKeys->add(vsKey2);
            hColKeys->add(vsKey3);

            Map::View vMap = hCache->getAll(hColKeys);
            TS_ASSERT(vMap->isEmpty());

            hCache->put(vsKey1, vsVal1);
            vMap = hCache->getAll(hColKeys);
            TS_ASSERT(vMap->size() == 1);
            TS_ASSERT(vMap->get(vsKey1)->equals(vsVal1));

            hCache->put(vsKey2, vsVal2);
            vMap = hCache->getAll(hColKeys);
            TS_ASSERT(vMap->size() == 2);
            TS_ASSERT(vMap->get(vsKey1)->equals(vsVal1));
            TS_ASSERT(vMap->get(vsKey2)->equals(vsVal2));

            hCache->put(vsKey3, vsVal3);
            vMap = hCache->getAll(hColKeys);
            TS_ASSERT(vMap->size() == 3);
            TS_ASSERT(vMap->get(vsKey1)->equals(vsVal1));
            TS_ASSERT(vMap->get(vsKey2)->equals(vsVal2));
            TS_ASSERT(vMap->get(vsKey3)->equals(vsVal3));

            hCache->put(vsKey4, vsVal4);
            vMap = hCache->getAll(hColKeys);
            TS_ASSERT(vMap->size() == 3);
            TS_ASSERT(vMap->get(vsKey1)->equals(vsVal1));
            TS_ASSERT(vMap->get(vsKey2)->equals(vsVal2));
            TS_ASSERT(vMap->get(vsKey3)->equals(vsVal3));
            }

        /**
        * Test put() with expiry.
        */
        void testPutExpiry()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-cache");

            String::View vsKey = "key";
            String::View vsVal = "val";

            hCache->put(vsKey, vsVal, 1000);
            Object::Holder oh = hCache->get(vsKey);
            TS_ASSERT(vsVal->equals(oh));

            TestListener::Handle hListener = TestListener::create();
            hCache->addMapListener(hListener);
            std::cout << "### DEBUG SLEEPING" << "\n";
            Thread::currentThread()->sleep(1500);
            oh = hCache->get(vsKey);
            TS_ASSERT(NULL == oh);

            CacheEvent::View hCacheEvent = cast<CacheEvent::View>(hListener->getEvent());
            TS_ASSERT(hCacheEvent->isExpired())
            }

        /**
        * Test put() with expiry (local).
        */
        void testPutExpiryLocal()
            {
            NamedCache::Handle hCache = ensureCleanCache("local-cache");

            String::View vsKey = "key";
            String::View vsVal = "val";

            hCache->put(vsKey, vsVal, 1000);
            Object::Holder oh = hCache->get(vsKey);
            TS_ASSERT(vsVal->equals(oh));

            TestListener::Handle hListener = TestListener::create();
            hCache->addMapListener(hListener);

            Thread::currentThread()->sleep(1500);
            oh = hCache->get(vsKey);
            TS_ASSERT(NULL == oh);

            CacheEvent::View hCacheEvent = cast<CacheEvent::View>(hListener->getEvent());
            TS_ASSERT(hCacheEvent->isExpired())
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }
    };
