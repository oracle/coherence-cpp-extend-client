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
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/ValueManipulator.hpp"
#include "coherence/util/aggregator/Integer64Sum.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/comparator/EntryComparator.hpp"
#include "coherence/util/processor/NumberIncrementor.hpp"

#include "private/coherence/net/cache/LocalInvocableCache.hpp"
#include "private/coherence/net/cache/LocalQueryCache.hpp"

#include "common/TestListener.hpp"
#include "common/TestEvenFilter.hpp"

using coherence::util::LinkedList;
using coherence::util::comparator::EntryComparator;
using coherence::util::ValueManipulator;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::processor::NumberIncrementor;

class LocalInvocableCacheSuite : public CxxTest::TestSuite
    {
    public:
        void testInvoke()
            {
            LocalInvocableCache::Handle hCache = LocalInvocableCache::create();

            Integer32::View vKey = Integer32::create(1);
            Integer32::View vValue = Integer32::create(0);
            hCache->put(vKey, vValue);

            NumberIncrementor::Handle vNumberIncrementor =
                        NumberIncrementor::create((ValueManipulator::View) NULL,
                                Integer32::create(2), false);

            TS_ASSERT(hCache->get(vKey) == vValue);
            hCache->invoke(vKey, vNumberIncrementor);
            TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(2)) == true);
            }
        void testGrow()
            {
            LocalInvocableCache::Handle hCache = LocalInvocableCache::create();

            Integer32::View vKey = Integer32::create(1);
            Integer32::View vValue = Integer32::create(0);
            hCache->put(vKey, vValue);
            hCache->put(vValue, vKey);

            // Force a grow that will copy the current set
            Iterator::Handle hIter = hCache->entrySet()->iterator();
            TS_ASSERT(instanceof<LocalInvocableCache::Entry::View>(hIter->next()) == true);
            for (int x = 0; x < 100; ++x)
                {
                Integer32::Handle hInt = Integer32::create(x);
                hCache->put(hInt, hInt);
                }

            Iterator::Handle hIter2 = hCache->entrySet()->iterator();
            TS_ASSERT(instanceof<LocalInvocableCache::Entry::View>(hIter2->next()) == true);
            }
        void testInvokeAll()
            {
            LocalInvocableCache::Handle hCache = LocalInvocableCache::create();


            Integer32::View vValue = Integer32::create(0);
            for (int x = 0; x < 10; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                hCache->put(vKey, vValue);
                }

            NumberIncrementor::Handle vNumberIncrementor =
                        NumberIncrementor::create((ValueManipulator::View) NULL,
                                Integer32::create(2), false);


            Set::Handle      hSet = hCache->keySet();
            Iterator::Handle hIter = hSet->iterator();
            while (hIter->hasNext())
                {
                TS_ASSERT(hCache->get(hIter->next())->equals(vValue));
                }
            hCache->invokeAll((Collection::View)ReadOnlyArrayList::create(hSet->toArray()), vNumberIncrementor);
            Integer32::View vNewValue = Integer32::create(2);
            hIter = hSet->iterator();
            while (hIter->hasNext())
                {
                TS_ASSERT(hCache->get(hIter->next())->equals(vNewValue));
                }
            }
        void testAggregateCollection()
            {
            LocalInvocableCache::Handle hCache = LocalInvocableCache::create();
            LinkedList::Handle hList = LinkedList::create();

            Integer32::View vValue = Integer32::create(1);
            for (int x = 0; x < 10; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                hList->add(vKey);
                hCache->put(vKey, vValue);
                }

            Integer64Sum::Handle hAgg = Integer64Sum::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());

            TS_ASSERT(Integer64::create(10)->equals(hCache->aggregate(
                    (Collection::View)hList, hAgg)));
            }
        void testAggregateFilter()
            {
            LocalInvocableCache::Handle hCache = LocalInvocableCache::create();

            Integer32::View vValue = Integer32::create(2);
            for (int x = 0; x < 5; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                hCache->put(vKey, vValue);
                }

            vValue = Integer32::create(1);
            for (int x = 5; x < 10; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                hCache->put(vKey, vValue);
                }

            Integer64Sum::Handle hAgg = Integer64Sum::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());

            TS_ASSERT(Integer64::create(10)->equals(hCache->aggregate(
                    (Filter::View)TestEvenFilter::create(), hAgg)));
            }
    };
