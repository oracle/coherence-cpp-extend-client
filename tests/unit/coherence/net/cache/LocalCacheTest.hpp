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
#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"

#include "common/TestListener.hpp"

class LocalCacheSuite : public CxxTest::TestSuite
    {
    public:
        class testLoader
            : public class_spec<testLoader,
                extends<SafeHashMap>,
                implements<IterableCacheLoader, CacheStore> >
            {
            friend class factory<testLoader>;

            virtual void store(Object::View vKey, Object::Holder ohValue)
                {
                put(vKey, ohValue);
                }

            virtual void storeAll(Map::View vMapEntries)
                {
                putAll(vMapEntries);
                }

            virtual Object::Holder load(Object::View vKey)
                {
                return get(vKey);
                }

            virtual void erase(Object::View vKey)
                {
                remove(vKey);
                }

            virtual void eraseAll(Collection::View vColKeys)
                {
                Iterator::Handle hIter = vColKeys->iterator();
                while(hIter->hasNext())
                    {
                    remove(hIter->next());
                    }
                }

            virtual Map::View loadAll(Collection::View vColKeys)
                {
                HashMap::Handle hMap = HashMap::create();
                Iterator::Handle hIter = vColKeys->iterator();
                while (hIter->hasNext())
                    {
                    Object::View   vKey    = hIter->next();
                    Object::Holder ohValue = get(vKey);

                    if (NULL != ohValue)
                        {
                        hMap->put(vKey, ohValue);
                        }
                    }

                return hMap;
                }

            virtual Iterator::Handle keys()
                {
                return keySet()->iterator();
                }
            };

        void testGetSetCacheLoader()
            {
            LocalCache::Handle hCache = LocalCache::create();
            testLoader::Handle hLoader = testLoader::create();
            hCache->setCacheLoader(hLoader);
            TS_ASSERT(hLoader == hCache->getCacheLoader());
            hCache->setCacheLoader(NULL);
            TS_ASSERT(NULL == hCache->getCacheLoader());
            }

        void testLoaderBasedGetPut()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);
            TS_ASSERT(h1 == hCache->get(h1));

            Integer32::Handle h2 = Integer32::create(2);
            hCache->put(h2, h2);
            TS_ASSERT(h2 == hCache->get(h2));
            TS_ASSERT(h2 == hLoader->get(h2));
            }

        void testLoaderBasedRemove()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);

            // Get the entry so it's in the cache before we call remove
            // Otherwise LocalCache won't remove it from the store
            hCache->get(h1);
            hCache->remove(h1);
            TS_ASSERT(hCache->get(h1) == NULL);
            TS_ASSERT(hLoader->get(h1) == NULL);
            }

        void testLoaderBasedClear()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);

            for(int x = 2; x < 26; ++ x)
                {
                Integer32::Handle h = Integer32::create(x);
                hCache->put(h,h);
                }

            hCache->get(h1);
            TS_ASSERT(hLoader->size() == 25);
            TS_ASSERT(hCache->size() == 25);

            hCache->clear();
            TS_ASSERT(hCache->size() == 0);
            TS_ASSERT(hLoader->size() == 0);
            }

        void testLoaderBasedgetEntry()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);

            Map::Entry::Handle hEntry = hCache->getEntry(h1);
            TS_ASSERT(hEntry != NULL);
            TS_ASSERT(hEntry->getKey() == h1);
            TS_ASSERT(hEntry->getValue() == h1);
            TS_ASSERT(hCache->getCacheMisses() == 1);
            }

        void testLoaderBasedGetAll()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();
            ArrayList::Handle hList   = ArrayList::create();

            //Pre-poulate the loader with a value
            for (int x = 0; x < 25; ++ x)
                {
                Integer32::Handle h = Integer32::create(x);
                hLoader->put(h, h);
                hList->add(h);
                }

            hCache->setCacheLoader(hLoader);

            Map::View vResult = hCache->getAll(hList);
            TS_ASSERT(vResult->size() == 25);
            TS_ASSERT(hList->containsAll(vResult->keySet()));
            TS_ASSERT(hList->containsAll(vResult->values()));
            TS_ASSERT(hCache->getCacheMisses() == 25);
            }

        void testLoad()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);
            hCache->load(h1);
            hCache->get(h1);
            TS_ASSERT(hCache->getCacheMisses()   == 0);
            }

        void testLoadEvent()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();
            TestListener::Handle hListener = TestListener::create();
            hCache->addFilterListener(hListener);

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);
            hCache->setCacheLoader(hLoader);

            hCache->load(h1);
            TS_ASSERT(cast<CacheEvent::View>(hListener->getEvent())->isSynthetic());

            Integer32::Handle h2 = Integer32::create(2);
            hCache->put(h2, h2);
            TS_ASSERT(hListener->getEvent() != NULL);
            }

        void testLoaderBasedLoadAll()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();
            ArrayList::Handle hList   = ArrayList::create();

            //Pre-poulate the loader with a value
            for (int x = 0; x < 25; ++ x)
                {
                Integer32::Handle h = Integer32::create(x);
                hLoader->put(h, h);
                hList->add(h);
                }

            hCache->setCacheLoader(hLoader);
            hCache->loadAll();

            Iterator::Handle hIter = hList->iterator();
            while(hIter->hasNext())
                {
                hCache->get(hIter->next());
                }

            TS_ASSERT(hCache->getCacheMisses() == 0);
            }

        void testPeek()
            {
            testLoader::Handle hLoader = testLoader::create();
            LocalCache::Handle hCache  = LocalCache::create();

            //Pre-poulate the loader with a value
            Integer32::Handle h1 = Integer32::create(1);
            hLoader->put(h1, h1);

            hCache->setCacheLoader(hLoader);

            TS_ASSERT(hCache->peek(h1) == NULL);
            hCache->get(h1);
            TS_ASSERT(hCache->peek(h1) == h1);
            }
    };
