/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_NAMED_CACHE_TEST
#define COH_REMOTE_NAMED_CACHE_TEST

#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PortableException.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/cache/ContinuousQueryCache.hpp"
#include "coherence/tests/IntegerComparator.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapTriggerListener.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/FilterTrigger.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"
#include "coherence/util/filter/MapEventFilter.hpp"
#include "common/TestMapListener.hpp"
#include "common/TestSyncListener.hpp"
#include "common/TestUtils.hpp"

#include "private/coherence/component/net/extend/RemoteCacheService.hpp"
#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"
#include "private/coherence/component/util/SafeCacheService.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Response.hpp"
#include "private/coherence/net/messaging/Request.hpp"
#include "private/coherence/util/StringHelper.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::component::net::extend::RemoteCacheService;
using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::net::extend::protocol::cache::NamedCacheProtocol;
using coherence::component::net::extend::protocol::cache::PutRequest;
using coherence::component::util::SafeCacheService;
using coherence::component::util::SafeNamedCache;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::Serializer;
using coherence::io::pof::PortableException;
using coherence::net::CacheFactory;
using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::net::cache::ContinuousQueryCache;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Response;
using coherence::net::messaging::Request;
using coherence::tests::IntegerComparator;
using coherence::util::ArrayList;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::Filter;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MapTriggerListener;
using coherence::util::Set;
using coherence::util::StringHelper;
using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::KeyExtractor;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::FilterTrigger;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::filter::NeverFilter;
using coherence::util::filter::MapEventFilter;
using common::test::TestMapListener;


/**
* RemoteNamedCache test suite.
*/
class RemoteNamedCacheTest : public CxxTest::TestSuite
    {
    // ----- RemoteNamedCacheTest methods -----------------------------------

    protected:
        /**
        * Convert the passed in key to a (potentially) different object. This
        * allows classes which extend RemoteNamedCacheTest to use different
        * key classes.
        *
        * @param v  the initial key object
        *
        * @return the key object
        */
        virtual Object::View getKeyObject(Object::View v) const
            {
            return v;
            }


    // ----- RemoteNamedCacheTest tests -------------------------------------

    public:
        /**
        * Test initialization.
        */
        void testInitialize()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");
            TS_ASSERT(cache != NULL);
            TS_ASSERT(instanceof<SafeNamedCache::View>(cache));
            TS_ASSERT(instanceof<SafeCacheService::View>(cache->getCacheService()));
            TS_ASSERT(cache->getCacheName()->equals("dist-extend"));
            TS_ASSERT(cache->isActive());
            TS_ASSERT(cache->getCacheService()->isRunning());

            SafeNamedCache::Handle safeCache = cast<SafeNamedCache::Handle>(cache);
            TS_ASSERT(instanceof<RemoteNamedCache::View>(safeCache->getNamedCache()));
            TS_ASSERT(instanceof<RemoteCacheService::View>(safeCache->getSafeCacheService()->getCacheService()));

            cache->release();
            TS_ASSERT(cache->isActive() == false);
            }

        /**
        * Test named cache properties.
        */
        void testNamedCacheProperties()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");
            String::Handle key = "testNamedCachePropertiesKey";
            String::Handle value = "testNamedCachePropertiesValue";

            // NamedCache
            TS_ASSERT(cache->isActive());
            TS_ASSERT(cache->size() == 0);

            cache->put(getKeyObject(key), value);
            TS_ASSERT(cache->size() == 1);
            TS_ASSERT(cache->get(getKeyObject(key))->equals(value));

            // SafeNamedCache
            SafeNamedCache::View safeCache = cast<SafeNamedCache::View>(cache);
            TS_ASSERT(safeCache->isReleased() == false);

            // RemoteNamedCache
            RemoteNamedCache::View remoteCache = cast<RemoteNamedCache::View>(safeCache->getNamedCache());
            TS_ASSERT(remoteCache->getChannel()->isOpen());
            TS_ASSERT(instanceof<NamedCacheProtocol::View>(remoteCache->getProtocol()));
            }

        /**
        * Test named cache methods.
        */
        void testNamedCacheMethods()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");
            String::Handle key = "testNamedCachePropertiesKey";
            String::Handle value = "testNamedCachePropertiesValue";

            ObjectArray::Handle keys = ObjectArray::create(3);
            keys[0] = getKeyObject(String::create("key1"));
            keys[1] = getKeyObject(String::create("key2"));
            keys[2] = getKeyObject(String::create("key3"));

            ObjectArray::Handle values = ObjectArray::create(3);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");

            TS_ASSERT(cache->size() == 0);
            cache->put(getKeyObject(key), value);
            TS_ASSERT(cache->size() == 1);
            TS_ASSERT(cache->get(getKeyObject(key))->equals(value));
            TS_ASSERT(cache->containsKey(getKeyObject(key)));

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

            cache->remove(getKeyObject(key));
            TS_ASSERT(cache->get(getKeyObject(key)) == NULL);

            Array<octet_t>::Handle hab  = Array<octet_t>::create(3);
            hab[0] = 1;
            hab[1] = 2;
            hab[2] = 3;
            Binary::View vBin = Binary::create(hab, 0, 3);
            String::Handle key4 = String::create("key4");
            cache->put(getKeyObject(key4), vBin);
            Object::Holder o = cache->get(getKeyObject(key4));
            TS_ASSERT(o != NULL);

            TS_ASSERT(instanceof<Binary::View>(o));
            TS_ASSERT(vBin->length() == (cast<Binary::View>(o))->length());
            }

        /**
        * Test named cache's keys and collections.
        */
        void testNamedCacheKeysCollections()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = getKeyObject(String::create("key1"));
            keys[1] = getKeyObject(String::create("key2"));
            keys[2] = getKeyObject(String::create("key3"));
            keys[3] = getKeyObject(String::create("key4"));

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

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
        void testNamedCacheValuesCollections()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = getKeyObject(String::create("key1"));
            keys[1] = getKeyObject(String::create("key2"));
            keys[2] = getKeyObject(String::create("key3"));
            keys[3] = getKeyObject(String::create("key4"));

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

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
        void testNamedCacheEntryCollection()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = getKeyObject(String::create("key1"));
            keys[1] = getKeyObject(String::create("key2"));
            keys[2] = getKeyObject(String::create("key3"));
            keys[3] = getKeyObject(String::create("key4"));

            ObjectArray::Handle values = ObjectArray::create(4);
            values[0] = String::create("value1");
            values[1] = String::create("value2");
            values[2] = String::create("value3");
            values[3] = String::create("value4");

            HashMap::Handle hMap = HashMap::create();
            hMap->put(keys[0], values[0]);
            hMap->put(keys[1], values[1]);
            hMap->put(keys[2], values[2]);
            hMap->put(keys[3], values[3]);
            cache->putAll(hMap);

            SafeNamedCache::Handle hSafeCache = cast<SafeNamedCache::Handle>(cache);
            Iterator::Handle hIter = hSafeCache->getNamedCache()->entrySet()->iterator();
            while (hIter->hasNext())
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

                TS_ASSERT(hMap->containsValue(vEntry->getValue()));
                TS_ASSERT(hMap->containsKey(vEntry->getKey()));
                }
            }

        /**
        * Test named cache index.
        */
        void testNamedCacheIndex()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");
            IdentityExtractor::View hIdentityExtractor = IdentityExtractor::getInstance();
            cache->addIndex(hIdentityExtractor, false, NULL);

            cache->removeIndex(hIdentityExtractor);

            KeyExtractor::View hKeyExtractor = KeyExtractor::create(IdentityExtractor::getInstance());
            SafeComparator::View vComparator = SafeComparator::create();
            cache->addIndex(hKeyExtractor, true, vComparator);

            cache->removeIndex(hKeyExtractor);

            //TODO: test index(es) functionality in cache
           }

        /**
        * Test named cache getAll with same keys.
        */
        void testNamedCacheGetAllWithSameKeys()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            HashMap::Handle hMap = HashMap::create();
            hMap->put(getKeyObject(String::create("key1")), String::create("value1"));
            hMap->put(getKeyObject(String::create("key2")), String::create("value2"));
            hMap->put(getKeyObject(String::create("key3")), String::create("value3"));
            hMap->put(getKeyObject(String::create("key4")), String::create("value4"));
            hMap->put(getKeyObject(String::create("key5")), String::create("value5"));
            cache->putAll(hMap);

            ObjectArray::Handle keys = ObjectArray::create(4);
            keys[0] = getKeyObject(String::create("key1"));
            keys[1] = getKeyObject(String::create("key2"));
            keys[2] = getKeyObject(String::create("key1"));
            keys[3] = getKeyObject(String::create("key1"));

            ArrayList::Handle list = ArrayList::create();
            for(size32_t i = 0, c = keys->length; i < c; ++i)
                {
                list->add(keys[i]);
                }

            Map::View hResults = cache->getAll(list);
            TS_ASSERT((int) hResults->size() == (int) 2);
            }

        /**
        * Test remote named cache trigger listener.
        */
        void testCacheTriggerListener()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            FilterTrigger::Handle hFtRollback = FilterTrigger::create(NeverFilter::getInstance(), FilterTrigger::action_rollback);
            FilterTrigger::Handle hFtIgnore   = FilterTrigger::create(NeverFilter::getInstance(), FilterTrigger::action_ignore);
            FilterTrigger::Handle hFtRemove   = FilterTrigger::create(NeverFilter::getInstance(), FilterTrigger::action_remove);

            HashMap::Handle hMap = HashMap::create();
            for (int32_t i = 1; i <= 5; i++)
                {
                cache->put(getKeyObject(COH_TO_STRING("key" << i)), COH_TO_STRING("value" << i));
                }
            cache->putAll(hMap);

            MapTriggerListener::Handle hListener = MapTriggerListener::create(hFtIgnore);
            cache->addFilterListener(hListener);
            TS_ASSERT(cache->get(getKeyObject(String::create("key1")))->equals(String::create("value1")));
            cache->put(getKeyObject(String::create("key1")), String::create("newvalue"));
            TS_ASSERT(cache->get(getKeyObject(String::create("key1")))->equals(String::create("value1")));
            cache->removeFilterListener(hListener);

            hListener = MapTriggerListener::create(hFtRemove);
            cache->addFilterListener(hListener);
            TS_ASSERT(cache->containsKey(getKeyObject(String::create("key1"))));
            cache->put(getKeyObject(String::create("key1")), String::create("newvalue"));
            TS_ASSERT(cache->containsKey(getKeyObject(String::create("key1"))) == false);
            cache->removeFilterListener(hListener);

            hListener = MapTriggerListener::create(hFtRollback);
            cache->addFilterListener(hListener);
            TS_ASSERT(cache->get(getKeyObject(String::create("key3")))->equals(String::create("value3")));
            Exception::View ex;
            try
                {
                cache->put(getKeyObject(String::create("key3")), String::create("newvalue"));
                }
            catch (Exception::View exx)
                {
                ex = exx;
                }
            TS_ASSERT(ex != NULL);
            TS_ASSERT(cache->get(getKeyObject(String::create("key3")))->equals(String::create("value3")));
            cache->removeFilterListener(hListener);
            }

        /**
        * Test named cache keySet.
        */
        void testNamedCacheKeySet()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            HashMap::Handle hMap = HashMap::create();
            for (int32_t i = 1; i <= 10; i++)
                {
                cache->put(getKeyObject(COH_TO_STRING("key" << i)), Integer32::create(i));
                }
            cache->putAll(hMap);

            Filter::View vFilter = GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(5));
            Set::View vKeys = cache->keySet(vFilter);
            TS_ASSERT(vKeys->size() == 5);
            for (int32_t i = 6; i <= 10; i++)
                {
                TS_ASSERT(vKeys->contains(getKeyObject(COH_TO_STRING("key" << i))));
                }
            }

        /**
        * Test named cache entrySet.
        */
        void testNamedCacheEntrySet()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            HashMap::Handle hMap = HashMap::create();
            for (int32_t i = 1; i <= 10; i++)
                {
                cache->put(getKeyObject(COH_TO_STRING("key" << i)), Integer32::create(i));
                }
            cache->putAll(hMap);

            Filter::View vFilter = GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(5));
            Set::View vEntries   = cache->entrySet(vFilter);
            TS_ASSERT(vEntries->size() == 5);

            ArrayList::Handle  hKeys   = ArrayList::create();
            ArrayList::Handle  hValues = ArrayList::create();
            ObjectArray::Handle hoa     = vEntries->toArray();
            for (int i = 0; i < 5; i++)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hoa[i]);
                hKeys->add(vEntry->getKey());
                hValues->add(vEntry->getValue());
                }
            for (int i = 6; i <= 10; i++)
                {
                TS_ASSERT(hKeys->contains(getKeyObject(COH_TO_STRING("key" << i))));
                TS_ASSERT(hValues->contains(Integer32::create(i)));
                }

            vEntries = cache->entrySet(vFilter, SafeComparator::getInstance());
            TS_ASSERT(vEntries->size() == 5);
            hKeys   = ArrayList::create();
            hValues = ArrayList::create();
            hoa     = vEntries->toArray();
            for (int i = 0; i < 5; i++)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hoa[i]);
                hKeys->add(vEntry->getKey());
                hValues->add(vEntry->getValue());
                }
            for (int i = 6; i <= 10; i++)
                {
                TS_ASSERT(hKeys->contains(getKeyObject(COH_TO_STRING("key" << i))));
                TS_ASSERT(hValues->contains(Integer32::create(i)));
                }
            TS_ASSERT(hValues->get(0)->equals(Integer32::create(6)));
            TS_ASSERT(hValues->get(1)->equals(Integer32::create(7)));
            TS_ASSERT(hValues->get(2)->equals(Integer32::create(8)));
            TS_ASSERT(hValues->get(3)->equals(Integer32::create(9)));
            TS_ASSERT(hValues->get(4)->equals(Integer32::create(10)));
            }

        /**
        * Test Namedcache entrySet with LimitFilter.
        */
        void testEntrySetLimitFilterComparator()
            {
            const int32_t      pagesize     = 10;
            LimitFilter::View  vLimitFilter = LimitFilter::create(AlwaysFilter::create(), pagesize);
            NamedCache::Handle cache        = ensureCleanCache("dist-extend");
            HashMap::Handle    hMap         = HashMap::create();
            int32_t            i;

            for (i = 0; i < 1000; i++)
                {
                cache->put(Integer32::create(i), Integer32::create(i));
                }
            cache->putAll(hMap);

            IntegerComparator::View vComparator = IntegerComparator::create();
            Set::View               vEntries    = cache->entrySet(vLimitFilter,vComparator);

            TS_ASSERT(vEntries         != NULL);
            TS_ASSERT(vEntries->size() == (int)pagesize);

            ObjectArray::Handle hoa = vEntries->toArray();

            i = 1000;
            for (int32_t j = 0; j < (int)hoa->length; j++)
                {
                i--;
                Map::Entry::View vEntry = cast<Map::Entry::View>(hoa[j]);
                TS_ASSERT((cast<Integer32::View>(vEntry->getValue()))->equals(Integer32::create(i)));
                }
            }


        /**
        * Test expiry functionality.
        */
        void testNamedCacheExpiry()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

            hCache->put(getKeyObject(String::create("key1")), Float32::create(100.0F), 1000);
            hCache->put(getKeyObject(String::create("key2")), Float32::create(80.5F),  1000);
            hCache->put(getKeyObject(String::create("key3")), Float32::create(19.5F),  1000);
            hCache->put(getKeyObject(String::create("key4")), Float32::create(2.0F),   1000);
            TS_ASSERT_EQUALS(hCache->size(), size32_t(4));

            Collection::Handle hCol = ArrayList::create();
            hCol->add(getKeyObject(String::create("key1")));
            hCol->add(getKeyObject(String::create("key2")));
            hCol->add(getKeyObject(String::create("key3")));
            hCol->add(getKeyObject(String::create("key4")));

            Thread::sleep(1100);

            TS_ASSERT_EQUALS(hCache->getAll(hCol)->size(), size32_t(0));
            }

        /**
        * Test remote named cache synchronous listeners.
        */
        void testNamedCacheSynchronousListeners()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            HashMap::Handle hMap = HashMap::create();
            hMap->put(getKeyObject(String::create("key1")), Integer32::create(435));
            hMap->put(getKeyObject(String::create("key2")), Integer32::create(253));
            hMap->put(getKeyObject(String::create("key3")), Integer32::create(3));
            hMap->put(getKeyObject(String::create("key4")), Integer32::create(200));
            hMap->put(getKeyObject(String::create("key5")), Integer32::create(333));
            cache->putAll(hMap);

            Filter::View vGreaterThan300 = GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(300));
            Filter::View vListenerFilter = MapEventFilter::create(GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(390)));
            Filter::View vAlwaysFilter   = AlwaysFilter::create();
            ContinuousQueryCache::Handle hQueryCache = ContinuousQueryCache::create(cache, vGreaterThan300);
            TestSyncListener::Handle hListener = TestSyncListener::create();

            // always listener
            hQueryCache->addFilterListener(hListener, vAlwaysFilter, false);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key1")), Integer32::create(400));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_updated);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key7")), Integer32::create(350));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_inserted);

            hListener->setEvent(NULL);
            hQueryCache->remove(getKeyObject(String::create("key5")));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_deleted);

            hQueryCache->removeFilterListener(hListener, vAlwaysFilter);

            // listener for key5
            cache->clear();
            cache->putAll(hMap);
            hQueryCache->addKeyListener(hListener, getKeyObject(String::create("key5")), false);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(400));
            TS_ASSERT(hListener->getEvent() == NULL);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(400));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_updated);

            hListener->setEvent(NULL);
            hQueryCache->remove(getKeyObject(String::create("key1")));
            TS_ASSERT(hListener->getEvent() == NULL);

            hQueryCache->remove(getKeyObject(String::create("key5")));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_deleted);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(450));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_inserted);

            hQueryCache->removeKeyListener(hListener, getKeyObject(String::create("key5")));

            // listener for values greater than 390
            cache->clear();
            cache->putAll(hMap);
            hQueryCache->addFilterListener(hListener, vListenerFilter, false);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(320));
            TS_ASSERT(hListener->getEvent() == NULL);

            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(350));
            TS_ASSERT(hListener->getEvent() == NULL);

            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(400));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_updated);

            hListener->setEvent(NULL);
            hQueryCache->put(getKeyObject(String::create("key7")), Integer32::create(340));
            TS_ASSERT(hListener->getEvent() == NULL);

            hQueryCache->remove(getKeyObject(String::create("key7")));
            TS_ASSERT(hListener->getEvent() == NULL);

            hQueryCache->remove(getKeyObject(String::create("key1")));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_deleted);

            hQueryCache->put(getKeyObject(String::create("key8")), Integer32::create(1000));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_inserted);

            hQueryCache->remove(getKeyObject(String::create("key6")));
            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getId() == MapEvent::entry_deleted);

            hQueryCache->removeFilterListener(hListener, vListenerFilter);
            hQueryCache->release();
            }

        /**
        * Test remote named cache asynchronous listeners.
        */
        void testNamedCacheMapListeners()
            {
            NamedCache::Handle cache = ensureCleanCache("dist-extend");

            HashMap::Handle hMap = HashMap::create();
            hMap->put(getKeyObject(String::create("key1")), Integer32::create(435));
            hMap->put(getKeyObject(String::create("key2")), Integer32::create(253));
            hMap->put(getKeyObject(String::create("key3")), Integer32::create(3));
            hMap->put(getKeyObject(String::create("key4")), Integer32::create(200));
            hMap->put(getKeyObject(String::create("key5")), Integer32::create(333));
            cache->putAll(hMap);

            Filter::View vGreaterThan300 = GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(300));
            Filter::View vListenerFilter = MapEventFilter::create(GreaterFilter::create(IdentityExtractor::getInstance(), Integer32::create(390)));
            Filter::View vAlwaysFilter   = AlwaysFilter::create();

            ContinuousQueryCache::Handle hQueryCache  = ContinuousQueryCache::create(cache, vGreaterThan300);
            Object::Handle               hTestMonitor = Object::create();
            TestMapListener::Handle      hListener    = TestMapListener::create(hTestMonitor);

            // always listener
            hQueryCache->addFilterListener(hListener, vAlwaysFilter, false);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key1")), Integer32::create(400));
            MapEvent::View vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_updated);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key7")), Integer32::create(350));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_inserted);

            hListener->clearEvent();
            hQueryCache->remove(getKeyObject(String::create("key5")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_deleted);

            hQueryCache->removeFilterListener(hListener, vAlwaysFilter);

            // listener for key5
            cache->clear();
            cache->putAll(hMap);
            hQueryCache->addKeyListener(hListener, getKeyObject(String::create("key5")), false);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(400));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(400));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_updated);

            hListener->clearEvent();
            hQueryCache->remove(getKeyObject(String::create("key1")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hQueryCache->remove(getKeyObject(String::create("key5")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_deleted);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(450));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_inserted);

            hQueryCache->removeKeyListener(hListener, getKeyObject(String::create("key5")));

            // listener for values greater than 390
            cache->clear();
            cache->putAll(hMap);
            hQueryCache->addFilterListener(hListener, vListenerFilter, false);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(320));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hQueryCache->put(getKeyObject(String::create("key5")), Integer32::create(350));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hQueryCache->put(getKeyObject(String::create("key6")), Integer32::create(400));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_updated);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key7")), Integer32::create(340));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hQueryCache->remove(getKeyObject(String::create("key7")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent == NULL);

            hListener->clearEvent();
            hQueryCache->remove(getKeyObject(String::create("key1")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_deleted);

            hListener->clearEvent();
            hQueryCache->put(getKeyObject(String::create("key8")), Integer32::create(1000));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_inserted);

            hListener->clearEvent();
            hQueryCache->remove(getKeyObject(String::create("key6")));
            vEvent = hListener->waitForEvent();
            TS_ASSERT(vEvent != NULL);
            TS_ASSERT(vEvent->getId() == MapEvent::entry_deleted);

            hQueryCache->removeFilterListener(hListener, vListenerFilter);
            hQueryCache->release();
            }

        /**
        * Test PortableException.
        */
        void testPortableException()
            {
            SafeNamedCache::Handle hnc = cast<SafeNamedCache::Handle>(ensureCleanCache("dist-extend-direct"));
            RemoteNamedCache::Handle hRemoteCache = cast<RemoteNamedCache::Handle>(hnc->getNamedCache());
            Response::Handle hResponse;
            Object::Holder hResult;

            // open channel and create PutRequest
            Channel::Handle hChannel = hRemoteCache->getChannel();
            PutRequest::Handle hPutRequest = cast<PutRequest::Handle>(hChannel->getMessageFactory()->createMessage(PutRequest::type_id));

            // send PutRequest to the server
            Request::Status::Handle hStatus = cast<Request::Status::Handle>(hChannel->send((Request::Handle) hPutRequest));
            try
                {
                hResponse = cast<Response::Handle>(hStatus->waitForResponse(-1));
                }
            catch(Exception::View ex)
                {
                // get the PortableException
                TS_ASSERT(instanceof<PortableException::View>(ex));
                PortableException::View hPortableException = cast<PortableException::View>(ex);
                TS_ASSERT(hPortableException != NULL);

                // serialize/deserialize PortableException with channel's serializer
                OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
                Serializer::View vs = hChannel->getSerializer();
                vs->serialize(hBuf->getBufferOutput(), hPortableException);
                Object::Holder hObj = vs->deserialize(hBuf->getReadBuffer()->getBufferInput());
                TS_ASSERT(hObj != NULL);
                TS_ASSERT(instanceof<PortableException::View>(hObj));
                PortableException::View hPortableResult = cast<PortableException::View>(hObj);
                TS_ASSERT((int) hPortableResult->getMessage()->indexOf(hPortableException->getMessage(), 0) > -1);
                }
            }

        /**
        * Test getOrDefault method
        */
        void testGetOrDefault()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

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
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hMap->put(String::create("key6"), NULL);
            hCache->putAll(hMap);

            TS_ASSERT(hCache->putIfAbsent(vKey, vDefault)->equals(Integer32::create(398)));
            TS_ASSERT(NULL == hCache->putIfAbsent(String::create("key6"), vDefault));
            TS_ASSERT(NULL == hCache->putIfAbsent(String::create("key5"), vDefault));
            TS_ASSERT(6 == hCache->size());
            TS_ASSERT(hCache->get(String::create("key5"))->equals(vDefault));
            }

        /**
        * Test removeValue method
        */
        void testRemove()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hCache->putAll(hMap);

            bool result = hCache->removeValue(vKey, Integer32::create(200));
            TS_ASSERT(result == false);
            result = hCache->removeValue(vKey, Integer32::create(398));
            TS_ASSERT(result == true);
            TS_ASSERT(3 == hCache->size());
            }

        /**
        * Test replace method
        */
        void testReplace()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

            Map::Handle     hMap     = HashMap::create();
            String::View    vKey     = String::create("key1");
            Integer32::View vDefault = Integer32::create(400);

            hMap->put(String::create("key4"), Integer32::create(0));
            hMap->put(String::create("key3"), Integer32::create(-10));
            hMap->put(String::create("key2"), Integer32::create(45));
            hMap->put(vKey, Integer32::create(398));
            hMap->put(String::create("key6"), NULL);
            hCache->putAll(hMap);

            Object::View vResult = hCache->replace(vKey, vDefault);

            TS_ASSERT(Integer32::create(398)->equals(vResult));
            TS_ASSERT(vDefault->equals(hCache->get(vKey)));

            bool result = hCache->replace(vKey, Integer32::create(300), Integer32::create(450));
            TS_ASSERT(result == false);

            vResult = hCache->replace(String::create("key5"), Integer32::create(300));
            TS_ASSERT(NULL == vResult);

            vResult = hCache->replace(String::create("key6"), Integer32::create(300));
            TS_ASSERT(NULL == vResult);

            result = hCache->replace(vKey, vDefault, Integer32::create(450));
            TS_ASSERT(result == true);
            TS_ASSERT(Integer32::create(450)->equals(hCache->get(vKey)));
            }

        void testView()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-extend");

            hCache->put(String::create("key0"), Integer32::create(0));
            hCache->put(String::create("key1"), Integer32::create(1));
            hCache->put(String::create("key2"), Integer32::create(2));

            NamedCache::Handle hView  = hCache->view()->values()->build();

            TS_ASSERT(hView->size() == 3)
            TS_ASSERT(Integer32::create(0)->equals(hView->get(String::create("key0"))));
            TS_ASSERT(Integer32::create(1)->equals(hView->get(String::create("key1"))));
            TS_ASSERT(Integer32::create(2)->equals(hView->get(String::create("key2"))));
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

#endif // COH_REMOTE_NAMED_CACHE_TEST
