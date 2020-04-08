/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"

#include "coherence/net/cache/NearCache.hpp"

#include "coherence/util/aggregator/Integer64Sum.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"
#include "coherence/util/processor/NumberMultiplier.hpp"
#include "coherence/util/ValueManipulator.hpp"

#include "common/TestUtils.hpp"

using coherence::net::CacheFactory;
using coherence::net::cache::NearCache;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;
using coherence::util::Map;
using coherence::util::processor::NumberMultiplier;
using coherence::util::ValueManipulator;

// used to match event args in mock calls
bool nearCacheMatchListenerEvent(ArrayList::View vExpected, ArrayList::View vActual)
    {
    MapEvent::View vExpectedEvent = cast<MapEvent::View>(vExpected->get(0));
    MapEvent::View vActualEvent   = cast<MapEvent::View>(vActual->get(0));
    return Object::equals(vExpectedEvent->getKey(), vActualEvent->getKey())            &&
            vExpectedEvent->getId() == vActualEvent->getId()                           &&
            Object::equals(vExpectedEvent->getOldValue(), vActualEvent->getOldValue()) &&
            Object::equals(vExpectedEvent->getNewValue(), vActualEvent->getNewValue());
    }

/**
* CacheMap Test Suite.
*/
class NearCacheTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test initialization.
        */
        void testInitialize()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));
            TS_ASSERT(cache != NULL);
            TS_ASSERT(cache->getCacheName()->equals("near-extend"));
            TS_ASSERT(cache->isActive());
            TS_ASSERT(cache->getCacheService()->isRunning());

            cache->release();
            TS_ASSERT(cache->isActive() == false);
            }

        /**
        * Test named cache properties.
        */
        void testNearCacheProperties()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));
            String::Handle key   = "testNearCachePropertiesKey";
            String::Handle value = "testNearCachePropertiesValue";

            // NamedCache
            TS_ASSERT(cache->isActive());
            cache->clear();
            TS_ASSERT(cache->size() == 0);

            cache->put(key, value);
            TS_ASSERT(cache->size() == 1);
            TS_ASSERT(cache->get(key)->equals(value));
            }

        /**
        * Test named cache methods.
        */
        void testNearCacheMethods()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            String::Handle key = "testNearCachePropertiesKey";
            String::Handle value = "testNearCachePropertiesValue";

            ObjectArray::Handle keys = ObjectArray::create(3);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key3");

            ObjectArray::Handle values = ObjectArray::create(3);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");

            cache->clear();
            TS_ASSERT(cache->size() == 0);
            cache->put(key, value);
            TS_ASSERT(cache->size() == 1);
            TS_ASSERT(cache->get(key)->equals(value));
            TS_ASSERT(cache->containsKey(key));

            Object::Holder old = cache->put(keys[0], values[0]);
            TS_ASSERT(old == NULL);
            TS_ASSERT(cache->get(keys[0])->equals(values[0]));

            HashMap::Handle map = HashMap::create();
            map->put(keys[0], values[0]);
            map->put(keys[1], values[1]);
            map->put(keys[2], values[2]);
            cache->putAll(map);

            ArrayList::Handle list = ArrayList::create();
            for(size32_t i = 0, c = keys->length; i < c; ++i)
                {
                list->add(keys[i]);
                }
            Map::View results = cache->getAll(list);
            TS_ASSERT(map->size() == list->size());
            Object::Holder ooo = cache->get(keys[1]);
            Object::Holder ooo2 = map->get(keys[1]);
            TS_ASSERT(ooo->equals(ooo2));

            cache->remove(key);
            TS_ASSERT(cache->get(key) == NULL);

            Array<octet_t>::Handle hab  = Array<octet_t>::create(3);
            hab[0] = 1;
            hab[1] = 2;
            hab[2] = 3;
            Binary::View vBin = Binary::create(hab, 0, 3);
            String::Handle key4 = String::create("key4");
            cache->put(key4, vBin);
            Object::Holder o = cache->get(key4);
            TS_ASSERT(o != NULL);

            TS_ASSERT(instanceof<Binary::View>(o));
            TS_ASSERT(vBin->length() == (cast<Binary::View>(o))->length());

            TS_ASSERT(cache->size() > 0);
            cache->truncate();
            TS_ASSERT(cache->size() == 0);
            }

        /**
        * Test named cache's keys and collections.
        */
        void testNearCacheKeysCollections()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key3");
            keys[3] = String::create("key4");

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

            cache->clear();

            HashMap::Handle hMap = HashMap::create();
            hMap->put(keys[0], values[0]);
            hMap->put(keys[1], values[1]);
            hMap->put(keys[2], values[2]);
            hMap->put(keys[3], values[3]);
            cache->putAll(hMap);

            Set::Handle hKeys = cache->keySet();
            TS_ASSERT(hKeys->size() == 4);

            Iterator::Handle hIter = hKeys->iterator();
            while (hIter->hasNext())
                {
                Object::Holder vEntry = cast<Object::Holder>(hIter->next());

                TS_ASSERT(hMap->containsKey(vEntry));
                }
            }

        /**
        * Test named cache's value and collections.
        */
        void testNearCacheValuesCollections()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key3");
            keys[3] = String::create("key4");

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

            cache->clear();

            HashMap::Handle hMap = HashMap::create();
            hMap->put(keys[0], values[0]);
            hMap->put(keys[1], values[1]);
            hMap->put(keys[2], values[2]);
            hMap->put(keys[3], values[3]);
            cache->putAll(hMap);

            Collection::Handle hValues = cache->values();
            TS_ASSERT(hValues->size() == 4);


            Iterator::Handle hIter = hValues->iterator();
            while (hIter->hasNext())
                {
                Object::Holder vEntry = cast<Object::Holder>(hIter->next());

                TS_ASSERT(hMap->containsValue(vEntry));
                }
            }

        /**
        * Test named cache's entry collection.
        */
        void testNearCacheEntryCollection()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key3");
            keys[3] = String::create("key4");

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

            cache->clear();

            HashMap::Handle hMap = HashMap::create();
            hMap->put(keys[0], values[0]);
            hMap->put(keys[1], values[1]);
            hMap->put(keys[2], values[2]);
            hMap->put(keys[3], values[3]);
            cache->putAll(hMap);
            }

        /**
        * Test named cache index.
        */
        void testNearCacheIndex()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            IdentityExtractor::View hIdentityExtractor = IdentityExtractor::getInstance();
            cache->addIndex(hIdentityExtractor, false, NULL);

            cache->removeIndex(hIdentityExtractor);

            KeyExtractor::View hKeyExtractor = KeyExtractor::create(IdentityExtractor::getInstance());
            SafeComparator::View vComparator = SafeComparator::create();
            cache->addIndex(hKeyExtractor, true, vComparator);

            cache->removeIndex(hIdentityExtractor);

            //TODO: test index(es) functionality in cache
            }

        /**
        * Test named cache getAll with same keys.
        */
        void testNearCacheGetAllWithSameKeys()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            cache->clear();

            HashMap::Handle hMap = HashMap::create();
            hMap->put(String::create("key1"), String::create("value1"));
            hMap->put(String::create("key2"), String::create("value2"));
            hMap->put(String::create("key3"), String::create("value3"));
            hMap->put(String::create("key4"), String::create("value4"));
            hMap->put(String::create("key5"), String::create("value5"));
            cache->putAll(hMap);

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key1");
            keys[3] = String::create("key1");

            ArrayList::Handle list = ArrayList::create();
            for(size32_t i = 0, c = keys->length; i < c; ++i)
                {
                list->add(keys[i]);
                }

            Map::View hResults = cache->getAll(list);
            TS_ASSERT((int) hResults->size() == (int) 2);
            }

        /**
        * Test named cache getAll with different keys.
        */
        void testNearCacheGetAll()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // turn on near caching
            cache->get(NULL);

            HashMap::Handle hMap = HashMap::create(5);
            hMap->put(String::create("key1"), String::create("value1"));
            hMap->put(String::create("key2"), String::create("value2"));
            hMap->put(String::create("key3"), String::create("value3"));
            hMap->put(String::create("key4"), String::create("value4"));
            hMap->put(String::create("key5"), String::create("value5"));
            cache->putAll(hMap);

            ObjectArray::Handle keys = ObjectArray::create(5);
            keys[0] = String::create("key1");
            keys[1] = String::create("key2");
            keys[2] = String::create("key3");
            keys[3] = String::create("key4");
            keys[4] = String::create("key5");

            ArrayList::Handle list = ArrayList::create();
            for(size32_t i = 0, c = keys->length; i < c; ++i)
                {
                list->add(keys[i]);
                }

            CacheMap::Handle  hFront   = cache->getFrontMap();
            Map::View         hResults = hFront->getAll(list);
            TS_ASSERT_EQUALS((int) hResults->size(), 0);

            hResults = cache->getAll(list);
            TS_ASSERT_EQUALS((int) hResults->size(), 5);
            TS_ASSERT_EQUALS((int) hFront->size(), 1);
            }

        /**
        * Test expiry functionality.
        */
        void testNearCacheExpiry()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            cache->clear();

            cache->put(String::create("key1"), Float32::create(100.0F), 384);
            cache->put(String::create("key2"), Float32::create(80.5F), 384);
            cache->put(String::create("key3"), Float32::create(19.5F), 384);
            cache->put(String::create("key4"), Float32::create(2.0F), 384);
            TS_ASSERT(cache->size() == 4);

            ArrayList::Handle list = ArrayList::create();
            list->add(String::create("key1"));
            list->add(String::create("key2"));
            list->add(String::create("key3"));
            list->add(String::create("key4"));

            Thread::sleep(768);
            int64_t nInitialTime = System::currentTimeMillis();
            bool fResult = false;
            while (!fResult &&
                 System::currentTimeMillis() - nInitialTime < 1000)
              {
              Map::View results = cache->getAll(list);
              fResult = (results->size() == 0);
              Thread::sleep(5);
              }
            TS_ASSERT(fResult);
            }

        void testNearCacheClear()
            {
            NearCache::Handle cache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            cache->get(NULL);

            Integer32::Handle iTest = Integer32::create(1);
            cache->put(iTest, iTest);
            TS_ASSERT(cache->get(iTest)->equals(iTest));
            TS_ASSERT(cache->size() == 1);

            cache->clear();

            TS_ASSERT(cache->size() == 0);
            TS_ASSERT(cache->get(iTest) == NULL);
            }

        void testNearCacheInvoke()
            {
            NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

            // Turn on near caching
            hCache->get(NULL);

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

        void testInvokeAll()
            {
            NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

             // Turn on near caching
            hCache->get(NULL);

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

        void testInvokeAllWithFilter()
            {
            NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

             // Turn on near caching
            hCache->get(NULL);

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

         void testAggregate()
            {
            NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

             // Turn on near caching
            hCache->get(NULL);

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

         void testAggregateWithFilter()
             {
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

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

         void testKeySet_CachingValues()
             {
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

             Object::View vKey0  = String::create("key0");
             Object::View vVal0  = String::create("val0");
             Object::View vKey1  = String::create("key1");
             Object::View vVal1  = String::create("val1");
             Object::View vKey2  = String::create("key2");
             Object::View vVal2  = String::create("val2");
             Object::View vKey3  = String::create("key3");
             Object::View vVal3  = String::create("val3");

             hCache->put(vKey0, vVal0);
             hCache->put(vKey1, vVal1);
             hCache->put(vKey2, vVal2);
             hCache->put(vKey3, vVal3);

             Set::Handle hsetKeys = hCache->keySet();
             TS_ASSERT(!hsetKeys->isEmpty() && hsetKeys->size() == 4);
             TS_ASSERT(hsetKeys->contains(vKey0));
             TS_ASSERT(hsetKeys->contains(vKey1));
             TS_ASSERT(hsetKeys->contains(vKey2));
             TS_ASSERT(hsetKeys->contains(vKey3));

             ObjectArray::Handle ha = hsetKeys->toArray();
             TS_ASSERT(ha->length == 4);
             for (size32_t i = 0; i < 4; ++i)
                 {
                 TS_ASSERT(hCache->get(ha[i]) != NULL);
                 }
             // remove and assert
             TS_ASSERT(hsetKeys->remove(vKey0));
             // try removing non-existent key
             TS_ASSERT(!hsetKeys->remove(String::create("bogus")));
             ha = hsetKeys->toArray();
             TS_ASSERT(hCache->size() == 3 &&
                     hsetKeys->toArray()->length == 3);

             hsetKeys->clear();

             TS_ASSERT(hCache->size() == 0 && hsetKeys->toArray()->length == 0);
             }

         void testEntrySet()
             {
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

             Object::View vKey0  = String::create("key0");
             Object::View vVal0  = String::create("val0");
             Object::View vKey1  = String::create("key1");
             Object::View vVal1  = String::create("val1");
             Object::View vKey2  = String::create("key2");
             Object::View vVal2  = String::create("val2");
             Object::View vKey3  = String::create("key3");
             Object::View vVal3  = String::create("val3");

             hCache->put(vKey0, vVal0);
             hCache->put(vKey1, vVal1);
             hCache->put(vKey2, vVal2);
             hCache->put(vKey3, vVal3);

             Set::Handle hsetEntries = hCache->entrySet();
             TS_ASSERT(!hsetEntries->isEmpty() && hsetEntries->size() == 4);

             // pass in bad type should throw ClassCastException
             try
                 {
                 hsetEntries->contains(String::create("bogus"));
                 TS_FAIL("Expected ClassCastException");
                 }
             catch(ClassCastException::View)
                 {
                 // expected
                 }
             Map::Entry::View vEntry = cast<Map::Entry::View>(hsetEntries->toArray()[0]);
             TS_ASSERT(hsetEntries->contains(vEntry));

             TS_ASSERT(hsetEntries->remove(vEntry));
             TS_ASSERT(!hsetEntries->contains(vEntry));

             ObjectArray::Handle ha = hsetEntries->toArray();
             TS_ASSERT(ha->length == 3);
             vEntry = cast<Map::Entry::View>(ha[0]);
             TS_ASSERT(hCache->get(vEntry->getKey())->equals(vEntry->getValue()));
             vEntry = cast<Map::Entry::View>(ha[1]);
             TS_ASSERT(hCache->get(vEntry->getKey())->equals(vEntry->getValue()));
             vEntry = cast<Map::Entry::View>(ha[2]);
             TS_ASSERT(hCache->get(vEntry->getKey())->equals(vEntry->getValue()));

             hsetEntries->clear();

             TS_ASSERT(hCache->size() == 0 &&
                     hsetEntries->size() == 0 && hsetEntries->isEmpty());
             }

        void testCoh8796()
            {
            NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-logical"));
            CacheMap::Handle  hFront = hCache->getFrontMap();
            CacheMap::Handle  hBack  = hCache->getBackMap();
            size32_t          cPuts  = 1000;
            bool              fResult;

            for(size32_t i = 0, c = cPuts; i < c; ++i)
                {
                hBack->put(Integer32::create(i), Integer32::create(i), 5000);
                if (i % 2 == 0)
                    {
                    Object::Holder oh = hCache->get(Integer32::create(i));
                    }
                }

            TS_ASSERT_EQUALS(hFront->size(), cPuts / 2);

            // expire the back map
            try
                {
                COH_TIMEOUT_AFTER(15000)
                    {
                    // calling size() expires the entries in the back and sends out synthetic deletes
                    while (hBack->size() > 0)
                        {
                        Thread::sleep(500);
                        }
                    }
                }
            catch (InterruptedException::View ve)
                {
                COH_THROW_STREAM(InterruptedException, "Timeout waiting for back cache size to reach 0."
                        << " Current size=" << hBack->size());
                }

            // ensure that synthetic deletes were filtered out;
            // front map values for evens are still there
            for (size32_t i = 0; i < cPuts; i++)
                {
                Integer32::Handle hInteger   = Integer32::create(i);
                Object::Holder    oHolderVal = hFront->get(hInteger);
                if (i % 2 == 0)
                    {
                    fResult = oHolderVal->equals(hInteger);
                    TS_ASSERT(fResult);
                    }
                else
                    {
                    fResult = oHolderVal == NULL;
                    TS_ASSERT(fResult);
                    }
                }

            fResult = hFront->size() == cPuts / 2;       // 0, 2, 4, ...
            TS_ASSERT(fResult);

            fResult = hBack->size() == 0;
            TS_ASSERT(fResult);

            // ensure that put works, and that a value update is properly
            // distributed to both the front and back map
            for (size32_t i = 0, c = cPuts; i < c; ++i)
                {
                Integer32::Handle hKey   = Integer32::create(i * 4);
                Integer32::Handle hValue = Integer32::create(i * 4);

                hCache->put(hKey, hValue);

                fResult = (hFront->get(hKey))->equals(hValue);
                TS_ASSERT(fResult);

                fResult = (hBack->get(hKey))->equals(hValue);
                TS_ASSERT(fResult);

                hBack->put(hKey, Integer32::create(i * 4 + 1));

                fResult = !hFront->containsKey(hKey);
                TS_ASSERT(fResult);

                fResult = hFront->get(hKey) == NULL;
                TS_ASSERT(fResult);

                hCache->put(hKey, hValue);

                fResult = (hFront->get(hKey))->equals(hValue);
                TS_ASSERT(fResult);

                fResult = (hBack->get(hKey))->equals(hValue);
                TS_ASSERT(fResult);

                hBack->remove(hKey);

                fResult = !hFront->containsKey(hKey);
                TS_ASSERT(fResult);

                fResult = hFront->get(hKey) == NULL;
                TS_ASSERT(fResult);

                fResult = !hBack->containsKey(hKey);
                TS_ASSERT(fResult);

                fResult = hBack->get(hKey) == NULL;
                TS_ASSERT(fResult);
                }
            }

         void testListener()
             {
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

             // the mock listener that receives the event notifications
             MockMapListener::Handle hMockListener = MockMapListener::create();
             // the mock used in verifying the MapEvent which is passed to the listener
             MockMapEvent::Handle hMockMapEvent = MockMapEvent::create();

             hCache->addFilterListener(hMockListener);

             Object::View vKey   = String::create("key1");
             Object::View vVal   = String::create("val1");
             Object::View vVal2  = String::create("val2");

             hMockListener->setStrict(true);
             hMockListener->entryInserted(hMockMapEvent);
             hMockListener->setMatcher(&nearCacheMatchListenerEvent);
             hMockListener->entryUpdated(hMockMapEvent);
             hMockListener->setMatcher(&nearCacheMatchListenerEvent);
             hMockListener->entryDeleted(hMockMapEvent);
             hMockListener->setMatcher(&nearCacheMatchListenerEvent);
             hMockListener->replay();

             // setup mock for pattern matcher.
             hMockMapEvent->setStrict(true);
             // insert
             hMockMapEvent->getKey();
             hMockMapEvent->setObjectReturn(vKey);
             hMockMapEvent->getId();
             hMockMapEvent->setInt32Return(MapEvent::entry_inserted);
             hMockMapEvent->getOldValue();
             hMockMapEvent->setObjectReturn(NULL);
             hMockMapEvent->getNewValue();
             hMockMapEvent->setObjectReturn(vVal);
             // update
             hMockMapEvent->getKey();
             hMockMapEvent->setObjectReturn(vKey);
             hMockMapEvent->getId();
             hMockMapEvent->setInt32Return(MapEvent::entry_updated);
             hMockMapEvent->getOldValue();
             hMockMapEvent->setObjectReturn(vVal);
             hMockMapEvent->getNewValue();
             hMockMapEvent->setObjectReturn(vVal2);
             //delete
             hMockMapEvent->getKey();
             hMockMapEvent->setObjectReturn(vKey);
             hMockMapEvent->getId();
             hMockMapEvent->setInt32Return(MapEvent::entry_deleted);
             hMockMapEvent->getOldValue();
             hMockMapEvent->setObjectReturn(vVal2);
             hMockMapEvent->getNewValue();
             hMockMapEvent->setObjectReturn(NULL);
             hMockMapEvent->replay();

             // insert
             hCache->put(vKey, vVal);
             // update
             hCache->put(vKey, vVal2);
             // delete
             hCache->remove(vKey);

             int64_t nInitialTime = System::currentTimeMillis();
             bool fResult = false;
             while (!fResult &&
                     System::currentTimeMillis() - nInitialTime < 10000)
                 {
                 fResult = hMockListener->verifyAndReturnResult();
                 Thread::sleep(250);
                 }
             TS_ASSERT(fResult);
             }

         /**
         * Test getOrDefault method
         */
         void testGetOrDefault()
             {
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

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
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

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
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

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
             NearCache::Handle hCache = cast<NearCache::Handle>(ensureCleanCache("near-extend"));

              // Turn on near caching
             hCache->get(NULL);

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

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }
    };
