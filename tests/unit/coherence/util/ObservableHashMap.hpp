/*
 * Copyright (c) 2000, 2026, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableHashMap.hpp"

#include "common/TestListener.hpp"

using coherence::net::cache::ContinuousQueryCache;
using coherence::util::ObservableHashMap;

class SizedInternalMapContinuousQueryCache
    : public class_spec<SizedInternalMapContinuousQueryCache,
        extends<ContinuousQueryCache> >
    {
    friend class factory<SizedInternalMapContinuousQueryCache>;

    protected:
        SizedInternalMapContinuousQueryCache(
                coherence::net::NamedCache::Handle hCache,
                coherence::util::Filter::View vFilter)
            : super(hCache, vFilter, true,
                    (coherence::util::MapListener::Handle) NULL,
                    (coherence::util::ValueExtractor::View) NULL)
            {
            }

        virtual coherence::util::ObservableMap::Handle
                instantiateInternalCache() const
            {
            return coherence::util::ObservableHashMap::create(
                    1000003, 1.0F, 3.0F);
            }
    };

class ObservableHashMapSuite : public CxxTest::TestSuite
    {
    public:
        void testSizedCreate()
            {
            ObservableHashMap::Handle hMap =
                    ObservableHashMap::create(1000003, 1.0F, 3.0F);
            Integer32::Handle hVal = Integer32::create(1);

            hMap->put(hVal, hVal);

            TS_ASSERT_EQUALS(hMap->get(hVal), hVal);
            }

        void testInsert()
            {
            ObservableHashMap::Handle hMap = ObservableHashMap::create();
            TestListener::Handle hListener = TestListener::create();

            hMap->addFilterListener(hListener);

            Integer32::Handle hVal = Integer32::create(1);
            hMap->put(hVal, hVal);

            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getOldValue() == NULL);
            TS_ASSERT(hListener->getEvent()->getNewValue() == hVal);
            }

        void testUpdate()
            {
            ObservableHashMap::Handle hMap = ObservableHashMap::create();
            TestListener::Handle hListener = TestListener::create();

            hMap->addFilterListener(hListener);

            Integer32::Handle hVal = Integer32::create(1);
            Integer32::Handle hVal1 = Integer32::create(2);
            hMap->put(hVal, hVal);
            hMap->put(hVal, hVal1);

            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getOldValue() == hVal);
            TS_ASSERT(hListener->getEvent()->getNewValue() == hVal1);
            }

        void testRemove()
            {
            ObservableHashMap::Handle hMap = ObservableHashMap::create();
            TestListener::Handle hListener = TestListener::create();

            hMap->addFilterListener(hListener);

            Integer32::Handle hVal = Integer32::create(1);
            hMap->put(hVal, hVal);
            hMap->remove(hVal);

            TS_ASSERT(hListener->getEvent() != NULL);
            TS_ASSERT(hListener->getEvent()->getOldValue() == hVal);
            TS_ASSERT(hListener->getEvent()->getNewValue() == NULL);
            }

        void testRemoveKeyListener()
            {
            ObservableHashMap::Handle hMap = ObservableHashMap::create();
            TestListener::Handle hListener = TestListener::create();
            String::View vKey = String::create("key");

            hMap->addKeyListener(hListener, vKey, false);
            hMap->removeKeyListener(hListener, vKey);

            hMap->put(vKey, String::create("val"));
            TS_ASSERT(hListener->getEvent() == NULL);
            }

        void testRemoveFilterListener()
            {
            ObservableHashMap::Handle hMap = ObservableHashMap::create();
            TestListener::Handle hListener = TestListener::create();
            String::View vKey = String::create("key");

            hMap->addFilterListener(hListener);
            hMap->removeFilterListener(hListener);

            hMap->put(vKey, String::create("val"));
            TS_ASSERT(hListener->getEvent() == NULL);
            }
    };
