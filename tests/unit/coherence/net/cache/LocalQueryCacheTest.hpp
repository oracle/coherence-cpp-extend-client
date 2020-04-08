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
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/comparator/EntryComparator.hpp"
#include "coherence/util/filter/LimitFilter.hpp"

#include "private/coherence/net/cache/LocalConcurrentCache.hpp"

#include "common/TestListener.hpp"
#include "common/TestEvenFilter.hpp"

using coherence::util::comparator::EntryComparator;
using coherence::util::filter::LimitFilter;

class LocalQueryCacheSuite : public CxxTest::TestSuite
    {
    public:
        class InverseInt32Comparator
            : public class_spec<InverseInt32Comparator,
                extends<Object>,
                implements<Comparator> >
            {
            friend class factory<InverseInt32Comparator>;

            virtual int32_t compare(Object::View v1, Object::View v2) const
                {
                return -(cast<Integer32::View>(v1)->compareTo(cast<Integer32::View>(v2)));
                }
            };


        void testKeySet()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            for (int x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            Set::View vResults = hCache->keySet(TestEvenFilter::create());
            TS_ASSERT(vResults->size() == 32);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            // Test query with limit filter
            LimitFilter::Handle hFilter = LimitFilter::create(TestEvenFilter::create(), 16);
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 0);
            }

        void testKeySetIndex()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            IdentityExtractor::Handle hExtract = IdentityExtractor::create();

            hCache->addIndex(hExtract, false, NULL);

            for (int x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            Set::View vResults = hCache->keySet(TestEvenFilter::create());
            TS_ASSERT(vResults->size() == 32);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            // Test query with limit filter
            LimitFilter::Handle hFilter = LimitFilter::create(TestEvenFilter::create(), 16);
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                TS_ASSERT(cast<Integer32::View>(hIter->next())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->keySet(hFilter);
            TS_ASSERT(vResults->size() == 0);
            }

        void testEntrySet()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            for (int x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            Set::View vResults = hCache->entrySet(TestEvenFilter::create());
            TS_ASSERT(vResults->size() == 32);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            // Test query with limit filter
            LimitFilter::Handle hFilter = LimitFilter::create(TestEvenFilter::create(), 16);
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 0);
            }

        void testEntrySetIndex()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            IdentityExtractor::Handle hExtract = IdentityExtractor::create();

            hCache->addIndex(hExtract, false, NULL);

            for (int x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            Set::View vResults = hCache->entrySet(TestEvenFilter::create());
            TS_ASSERT(vResults->size() == 32);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            // Test query with limit filter
            LimitFilter::Handle hFilter = LimitFilter::create(TestEvenFilter::create(), 16);
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 16);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(cast<Integer32::View>(vEntry->getKey())->getInt32Value() % 2 == 0);
                TS_ASSERT(cast<Integer32::View>(vEntry->getValue())->getInt32Value() % 2 == 0);
                }

            hFilter->nextPage();
            vResults = hCache->entrySet(hFilter);
            TS_ASSERT(vResults->size() == 0);
            }

        void testEntrySetComparator()
            {
            LocalQueryCache::Handle hCache = LocalQueryCache::create();

            for (int32_t x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            Set::View vResults = hCache->entrySet(TestEvenFilter::create(),
                    InverseInt32Comparator::create());

            int32_t nCompare = 62;
            TS_ASSERT(vResults->size() == 32);
            for (Iterator::Handle hIter = vResults->iterator(); hIter->hasNext(); )
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                int32_t nCur =  cast<Integer32::View>(vEntry->getKey())->getInt32Value();
                TS_ASSERT(nCur == nCompare);
                nCompare -= 2;
                }
            }
    };
