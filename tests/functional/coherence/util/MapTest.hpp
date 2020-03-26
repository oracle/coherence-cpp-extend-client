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
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;
using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Muterator;
using coherence::util::ReadOnlyArrayList;
using coherence::util::Set;
using coherence::util::SimpleMapEntry;

/**
* Test Suite for the Map interface of RemoteNamedCache.
*/
class MapSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the size method.
        */
        void testSize()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");

            int32_t cEntries = 5;
            for(int32_t i = 0; i < cEntries; ++i)
                {
                hCache->put(Integer32::create(i), String::create("b"));
                }

            TS_ASSERT(hCache->size() == (size32_t) cEntries);
            }

        void testIsEmpty()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT(hCache->isEmpty() == true);

            hCache->put(String::create("a"), String::create("b"));
            TS_ASSERT(hCache->isEmpty() == false);
            }

        void testContainsKey()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            Integer32::Handle  hKey   = Integer32::create(42);
            String::Handle     hValue = String::create("test value");

            hCache->put(hKey, hValue);
            TS_ASSERT(hCache->containsKey(hKey) == true);
            }

        void testContainsValue()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            Integer32::Handle  hKey   = Integer32::create(42);
            String::Handle     hValue = String::create("test value");

            hCache->put(hKey, hValue);
            TS_ASSERT(hCache->containsValue(hValue) == true);
            }

        void testPut()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            Integer32::Handle  hKey   = Integer32::create(42);
            String::Handle     hValue = String::create("test value");

            hCache->put(hKey, hValue);
            TS_ASSERT(hCache->get(hKey)->equals(hValue));
            }

        void testRemove()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            Integer32::Handle  hKey   = Integer32::create(42);
            String::Handle     hValue = String::create("test value");

            hCache->put(hKey, hValue);
            TS_ASSERT(hCache->get(hKey)->equals(hValue));

            hCache->remove(hKey);
            TS_ASSERT(hCache->get(hKey) == NULL);
            TS_ASSERT(hCache->size() == 0);
            }

        void testPutAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == hMap->size());

            Iterator::Handle hIter = hMap->entrySet()->iterator();
            while (hIter->hasNext())
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>
                        (hIter->next());

                TS_ASSERT(hCache->get(
                        vEntry->getKey())->equals(vEntry->getValue()));
                }
            }

        void testClear()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");

            int32_t cEntries = 5;
            for(int32_t i = 0; i < cEntries; ++i)
                {
                hCache->put(Integer32::create(i), String::create("b"));
                }

            TS_ASSERT(hCache->size() == (size32_t) cEntries);

            hCache->clear();
            TS_ASSERT(hCache->size() == 0);
            }

        void testKeySet()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Iterator::Handle hIter = hCache->keySet()->iterator();
            while (hIter->hasNext())
                {
                Integer32::View vInt = cast<Integer32::View>(hIter->next());

                TS_ASSERT(hMap->containsKey(vInt));
                }
            }

        void testKeySetSize()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->keySet()->size() == 10);
            }

        void testKeySetIsEmpty()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            TS_ASSERT(hCache->keySet()->isEmpty() == true);

            hCache->put(Integer32::create(123), Integer32::create(42));
            TS_ASSERT(hCache->keySet()->isEmpty() == false);
            }

        void testKeySetContains()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::View        vKeys = hCache->keySet();
            Iterator::Handle hIter = hMap->keySet()->iterator();
            while (hIter->hasNext())
                {
                TS_ASSERT(vKeys->contains(hIter->next()));
                }
            }

        void testKeySetContainsAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->keySet()->containsAll(hMap->keySet()));
            }

        void testKeySetIterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }
            hCache->putAll(hMap);

            Iterator::Handle hIter  = hCache->keySet()->iterator();
            int32_t          cIters = 0;
            while(hIter->hasNext())
                {
                TS_ASSERT(hMap->containsKey(hIter->next()));
                cIters++;
                }
            TS_ASSERT(cIters == 10);
            }

        void testKeySetMuterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::Handle       hSet   = hCache->keySet();
            Muterator::Handle hIter  = hSet->iterator();
            int32_t           cIters = 0;

            while(hIter->hasNext())
                {
                TS_ASSERT(hMap->containsKey(hIter->next()));
                hIter->remove();
                cIters++;
                }

            TS_ASSERT(cIters == 10);
            TS_ASSERT(hSet->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

        void testKeySetToArray()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::Handle         hKeySet = hCache->keySet();
            ObjectArray::Handle hoa     = hKeySet->toArray();

            TS_ASSERT(hoa->length == hKeySet->size());
            for (size32_t i = 0; i < hoa->length; ++i)
                {
                TS_ASSERT(hKeySet->contains(hoa[i]));
                }
            }

        void testKeySetAdd()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->keySet()->add(Integer32::create(12)),
                    UnsupportedOperationException::View);
            }

        void testKeySetAddAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->keySet()->addAll(ArrayList::create()),
                    UnsupportedOperationException::View);
            }

        void testKeySetRemove()
           {
           NamedCache::Handle hCache = ensureCleanCache("dist-map");
           hCache->put(Integer32::create(42), Integer32::create(42));
           TS_ASSERT(hCache->size() == 1);

           Set::Handle      hSet  = hCache->keySet();
           Iterator::Handle hIter = hSet->iterator();

           while (hIter->hasNext())
               {
               hSet->remove(hIter->next());
               }

           TS_ASSERT(hSet->size() == 0);
           TS_ASSERT(hCache->size() == 0);
           }

        void testKeySetRemoveAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            hCache->keySet()->removeAll(hMap->keySet());
            TS_ASSERT(hCache->size() == 0);
            }

        void testKeySetRetainAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Set::Handle hMapKeys = hMap->keySet();
            hMapKeys->remove(Integer32::create(1));
            hMapKeys->remove(Integer32::create(2));

            Set::Handle hCacheKeys = hCache->keySet();
            hCacheKeys->retainAll(ReadOnlyArrayList::create(hMapKeys->toArray()));

            TS_ASSERT(hCacheKeys->size() == 8);
            TS_ASSERT(hCache->size() == 8);
            }

        void testKeySetClear()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }
            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Set::Handle hSet = hCache->keySet();
            TS_ASSERT(hSet->size() == 10);

            hSet->clear();
            TS_ASSERT(hSet->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

        void testValues()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Iterator::Handle hIter = hCache->values()->iterator();
            while (hIter->hasNext())
                {
                Integer32::View vInt = cast<Integer32::View>(hIter->next());

                TS_ASSERT(hMap->containsValue(vInt));
                }
            }

        void tesetValuesSize()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->values()->size() == 10);
            }

        void testValuesIsEmpty()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            TS_ASSERT(hCache->values()->isEmpty() == true);

            hCache->put(Integer32::create(123), Integer32::create(42));
            TS_ASSERT(hCache->values()->isEmpty() == false);
            }

        void testValuesContains()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Collection::View vValues = hCache->values();
            Iterator::Handle hIter   = hMap->values()->iterator();

            while (hIter->hasNext())
                {
                TS_ASSERT(vValues->contains(hIter->next()));
                }
            }

        void testValuesContainsAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->values()->containsAll(hMap->values()));
            }

        void testValuesIterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Iterator::Handle hIter  = hCache->values()->iterator();
            int32_t          cIters = 0;

            while(hIter->hasNext())
                {
                TS_ASSERT(hMap->containsValue(hIter->next()));
                cIters++;
                }
            TS_ASSERT(cIters == 10);
            }

        void testValuesMuterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Collection::Handle hValues = hCache->values();
            Muterator::Handle  hIter   = hValues->iterator();
            int32_t            cIters  = 0;

            while(hIter->hasNext())
                {
                TS_ASSERT(hCache->containsValue(hIter->next()));
                hIter->remove();
                cIters++;
                }

            TS_ASSERT(cIters == 10);
            TS_ASSERT(hValues->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

        void testValuesToArray()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Collection::Handle  hValues = hCache->values();
            ObjectArray::Handle hoa = hValues->toArray();

            TS_ASSERT(hoa->length == hValues->size());
            for (size32_t i = 0; i < hoa->length; ++i)
                {
                TS_ASSERT(hValues->contains(hoa[i]));
                }
            }

        void testValuesAdd()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->values()->add(Integer32::create(12)),
                    UnsupportedOperationException::View);
            }

        void testValuesAddAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->values()->addAll(ArrayList::create()),
                    UnsupportedOperationException::View);
            }

        void testValuesRemove()
           {
           NamedCache::Handle hCache = ensureCleanCache("dist-map");
           hCache->put(Integer32::create(42), Integer32::create(42));
           TS_ASSERT(hCache->size() == 1);

           Collection::Handle hCol  = hCache->values();
           Iterator::Handle   hIter = hCol->iterator();

           while (hIter->hasNext())
               {
               hCol->remove(hIter->next());
               }

           TS_ASSERT(hCol->size() == 0);
           TS_ASSERT(hCache->size() == 0);
           }

        void testValuesRemoveAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            hCache->values()->removeAll(hMap->keySet());
            TS_ASSERT(hCache->size() == 0);
            }

        void testValuesRetainAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Collection::Handle hMapValues = hMap->values();
            hMapValues->remove(Integer32::create(1));
            hMapValues->remove(Integer32::create(2));

            Collection::Handle hCacheValues = hCache->values();
            hCacheValues->retainAll(hMapValues);

            TS_ASSERT(hCacheValues->size() == 8);
            TS_ASSERT(hCache->size() == 8);
            }

        void testValuesClear()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }
            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Collection::Handle hCol = hCache->values();
            TS_ASSERT(hCol->size() == 10);

            hCol->clear();
            TS_ASSERT(hCol->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

        void testEntrySet()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Iterator::Handle hIter = hCache->entrySet()->iterator();
            while (hIter->hasNext())
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

                TS_ASSERT(hMap->containsKey(vEntry->getKey()));
                }
            }

        void tesetEntrySetSize()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            TS_ASSERT(hCache->entrySet()->size() == 10);
            }

        void testEntrySetIsEmpty()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            TS_ASSERT(hCache->entrySet()->isEmpty() == true);

            hCache->put(Integer32::create(123), Integer32::create(42));
            TS_ASSERT(hCache->entrySet()->isEmpty() == false);
            }

        void testEntrySetContains()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::View        vEntries = hCache->entrySet();
            Iterator::Handle hIter    = hMap->entrySet()->iterator();

            while (hIter->hasNext())
                {
                TS_ASSERT(vEntries->contains(hIter->next()));
                }
            }

        void testEntrySetContainsAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->entrySet()->containsAll(
                    ReadOnlyArrayList::create(hMap->entrySet()->toArray())));
            }

        void testEntrySetIterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Iterator::Handle hIter  = hCache->entrySet()->iterator();
            int32_t          cIters = 0;

            while(hIter->hasNext())
                {
                TS_ASSERT(hMap->containsKey(
                        cast<Map::Entry::View>(hIter->next())->getKey()));
                cIters++;
                }
            TS_ASSERT(cIters == 10);
            }

        void testEntrySetMuterator()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::Handle        hEntries = hCache->entrySet();
            Muterator::Handle  hIter    = hEntries->iterator();
            int32_t            cIters   = 0;

            while(hIter->hasNext())
                {
                TS_ASSERT(hCache->containsKey(
                        cast<Map::Entry::View>(hIter->next())->getKey()));
                hIter->remove();
                cIters++;
                }

            TS_ASSERT(cIters == 10);
            TS_ASSERT(hEntries->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

        void testEntrySetToArray()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);

            Set::Handle         hEntries = hCache->entrySet();
            ObjectArray::Handle hoa      = hEntries->toArray();

            TS_ASSERT(hoa->length == hEntries->size());
            for (size32_t i = 0; i < hoa->length; ++i)
                {
                TS_ASSERT(hEntries->contains(hoa[i]));
                }
            }

        void testEntrySetAdd()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->entrySet()->add(
                    SimpleMapEntry::create(Integer32::create(12),
                            Integer32::create(12))),
                    UnsupportedOperationException::View);
            }

        void testEntrySetAddAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            TS_ASSERT_THROWS(hCache->entrySet()->addAll(ArrayList::create()),
                    UnsupportedOperationException::View);
            }

        void testEntrySetRemove()
           {
           NamedCache::Handle hCache = ensureCleanCache("dist-map");
           hCache->put(Integer32::create(42), Integer32::create(42));
           TS_ASSERT(hCache->size() == 1);

           Set::Handle      hCol  = hCache->entrySet();
           Iterator::Handle hIter = hCol->iterator();

           while (hIter->hasNext())
               {
               hCol->remove(hIter->next());
               }

           TS_ASSERT(hCol->size() == 0);
           TS_ASSERT(hCache->size() == 0);
           }

        void testEntrySetRemoveAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();

            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            hCache->entrySet()->removeAll(hMap->entrySet());
            TS_ASSERT(hCache->size() == 0);
            }

        void testEntrySetRetainAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Set::Handle       hMapEntries = hMap->entrySet();
            Muterator::Handle hIter       = hMapEntries->iterator();
            for(size_t i = 0; i < 2; ++i)
                {
                hIter->next();
                hIter->remove();
                }

            Set::Handle hCacheEntries = hCache->entrySet();
            hCacheEntries->retainAll(hMapEntries);

            TS_ASSERT(hCacheEntries->size() == 8);
            TS_ASSERT(hCache->size() == 8);
            }

        void testEntrySetClear()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-map");
            HashMap::Handle    hMap   = HashMap::create();
            for (int32_t i = 0; i < 10; ++i)
                {
                hMap->put(Integer32::create(i), Integer32::create(i));
                }

            hCache->putAll(hMap);
            TS_ASSERT(hCache->size() == 10);

            Set::Handle hEntries = hCache->entrySet();
            TS_ASSERT(hEntries->size() == 10);

            hEntries->clear();
            TS_ASSERT(hEntries->size() == 0);
            TS_ASSERT(hCache->size() == 0);
            }

    // ----- Split Noah's Tests Above Mark's Below --------------------------
    };
