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
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueManipulator.hpp"
#include "coherence/util/aggregator/Integer64Sum.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"
#include "coherence/util/processor/NumberMultiplier.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Filter;
using coherence::util::HashMap;
using coherence::util::InvocableMap;
using coherence::util::ValueExtractor;
using coherence::util::ValueManipulator;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;
using coherence::util::processor::NumberMultiplier;
using coherence::util::extractor::IdentityExtractor;

/**
* Test Suite for the InvocableMap interface of RemoteNamedCache.
*/
class InvocableMapSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the invoke method.
        */
        void testInvoke()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Integer32::View vKey = Integer32::create(3);

            hCache->put(Integer32::create(3), Integer32::create(8));

            NumberMultiplier::Handle vNumberMultiplier =
                    NumberMultiplier::create((ValueManipulator::View) NULL,
                            Integer32::create(7), false);

            TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(8)));
            TS_ASSERT(hCache->invoke(vKey,
                    vNumberMultiplier)->equals(Integer32::create(56)));
            TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(56)));
            }

        /**
        * Test the invokeAll method.
        */
        void testInvokeAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Object::View vKey1 = String::create("foo");
            Object::View vKey2 = String::create("bar");

            hCache->put(vKey1, Integer32::create(8));
            hCache->put(vKey2, Integer32::create(2));

            NumberMultiplier::Handle vNumberMultiplier =
                    NumberMultiplier::create((ValueManipulator::View) NULL,
                            Integer32::create(7), false);

            Collection::Handle hCollKeys = ArrayList::create();
            hCollKeys->add(vKey1);
            hCollKeys->add(vKey2);

            TS_ASSERT(hCache->get(vKey1)->equals(Integer32::create(8)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer32::create(2)));

            Map::View vResults = hCache->invokeAll((Collection::View)hCollKeys,
                    vNumberMultiplier);

            TS_ASSERT(vResults->get(vKey1)->equals(Integer32::create(56)));
            TS_ASSERT(vResults->get(vKey2)->equals(Integer32::create(14)));

            TS_ASSERT(hCache->get(vKey1)->equals(Integer32::create(56)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer32::create(14)));
            }


        /**
        * Test the invokeAll method.
        */
        void testInvokeAllWithFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Object::View vKey1 = String::create("foo");
            Object::View vKey2 = String::create("bar");

            hCache->put(vKey1, Integer32::create(8));
            hCache->put(vKey2, Integer32::create(2));

            NumberMultiplier::Handle vNumberMultiplier =
                    NumberMultiplier::create((ValueManipulator::View) NULL,
                            Integer32::create(7), false);

            TS_ASSERT(hCache->get(vKey1)->equals(Integer32::create(8)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer32::create(2)));

            NeverFilter::View vNeverFilter = NeverFilter::create();
            AlwaysFilter::View vAlwaysFilter = AlwaysFilter::create();

            Map::View vResults = hCache->invokeAll((Filter::View)vNeverFilter,
                    vNumberMultiplier);

            TS_ASSERT(vResults->size() == 0);

            TS_ASSERT(hCache->get(vKey1)->equals(Integer32::create(8)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer32::create(2)));

            vResults = hCache->invokeAll((Filter::View)vAlwaysFilter,
                    vNumberMultiplier);

            TS_ASSERT(vResults->get(vKey1)->equals(Integer32::create(56)));
            TS_ASSERT(vResults->get(vKey2)->equals(Integer32::create(14)));

            TS_ASSERT(hCache->get(vKey1)->equals(Integer32::create(56)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer32::create(14)));
            }


        /**
        * Test the aggregate method.
        */
        void testAggregate()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Object::View vKey1 = String::create("foo");
            Object::View vKey2 = String::create("bar");

            hCache->put(vKey1, Integer64::create(8));
            hCache->put(vKey2, Integer64::create(2));

            InvocableMap::EntryAggregator::Handle hAggregator =
                Integer64Sum::create((ValueExtractor::View) IdentityExtractor::getInstance());

            TS_ASSERT(hCache->get(vKey1)->equals(Integer64::create(8)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer64::create(2)));

            Collection::Handle hCollKeys = ArrayList::create();
            hCollKeys->add(vKey1);
            hCollKeys->add(vKey2);

            Object::Holder ohResults =
                    hCache->aggregate((Collection::View)hCollKeys, hAggregator);

            TS_ASSERT(Integer64::create(10)->equals(ohResults));
            }

        /**
        * Test the aggregate method.
        */
        void testAggregateWithFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Object::View vKey1 = String::create("foo");
            Object::View vKey2 = String::create("bar");

            hCache->put(vKey1, Integer64::create(8));
            hCache->put(vKey2, Integer64::create(2));

            InvocableMap::EntryAggregator::Handle hAggregator =
                Integer64Sum::create((ValueExtractor::View) IdentityExtractor::getInstance());

            TS_ASSERT(hCache->get(vKey1)->equals(Integer64::create(8)));
            TS_ASSERT(hCache->get(vKey2)->equals(Integer64::create(2)));

            NeverFilter::View vNeverFilter = NeverFilter::create();
            AlwaysFilter::View vAlwaysFilter = AlwaysFilter::create();

            Object::Holder ohResults =
                    hCache->aggregate((Filter::View)vNeverFilter, hAggregator);
            TS_ASSERT(NULL == ohResults);

            ohResults = hCache->aggregate((Filter::View)vAlwaysFilter,
                    hAggregator);
            TS_ASSERT(Integer64::create(10)->equals(ohResults));
            }

        /**
        * Test getOrDefault method
        */
        void testGetOrDefault()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hCache->putAll(hMap);

            TS_ASSERT(hCache->getOrDefault(vKey, vDefault)->equals(Integer32::create(398)));
            TS_ASSERT(hCache->getOrDefault(String::create("key5"), vDefault)->equals(vDefault));
            }

        /**
        * Test putIfAbsent method
        */
        void testPutIfAbsent()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hCache->putAll(hMap);

            TS_ASSERT(hCache->putIfAbsent(vKey, vDefault)->equals(Integer32::create(398)));
            TS_ASSERT(NULL == hCache->putIfAbsent(String::create("key5"), vDefault));
            TS_ASSERT(5 == hCache->size());
            TS_ASSERT(hCache->get(String::create("key5"))->equals(vDefault));
            }

        /**
        * Test removeValue method
        */
        void testRemove()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hCache->putAll(hMap);

            bool result = hCache->removeValue(vKey, Integer32::create(398));
            TS_ASSERT(result == true);
            TS_ASSERT(3 == hCache->size());
            }

        /**
        * Test removeValue method
        */
        void testReplace()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-invocable-map");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hCache->putAll(hMap);

            Object::View vResult = hCache->replace(vKey, vDefault);

            TS_ASSERT(Integer32::create(398)->equals(vResult));
            TS_ASSERT(vDefault->equals(hCache->get(vKey)));

            bool result = hCache->replace(vKey, Integer32::create(300), Integer32::create(450));
            TS_ASSERT(result == false);

            result = hCache->replace(vKey, vDefault, Integer32::create(450));
            TS_ASSERT(result == true);
            TS_ASSERT(Integer32::create(450)->equals(hCache->get(vKey)));
            }
    };
