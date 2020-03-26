/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "mock/CommonMocks.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/HashSet.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

using coherence::net::cache::ContinuousQueryCache;
using coherence::util::ArrayList;
using coherence::util::Filter;
using coherence::util::HashSet;
using coherence::util::filter::AlwaysFilter;

namespace
    {
    bool matchAll(ArrayList::View /*vExpected*/, ArrayList::View /*vActual*/)
        {
        return true;
        }
    }



class ContinuousQueryCacheTest : public CxxTest::TestSuite
    {
    public:

    void testGetCache()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getCache() == hMockNamedCache);
        }


    void testGetFilter()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        Filter::Handle hFilter = DummyFilter::create();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(hCqc->getFilter() == hFilter);
        }


    void testIsCacheValues()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();

        //set expectations
        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //replay
        hMockNamedCache->replay();

        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(!hCqc->isCacheValues());

        hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, true);

        TS_ASSERT(hCqc->isCacheValues());
        }


    void testSetCacheValues()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        Filter::Handle hFilter = DummyFilter::create();
        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->entrySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        hMockNamedCache->keySet(hFilter);
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //set expectations
        hMockNamedCache->getAll(ArrayList::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(HashMap::create());

        //set expectations
        hMockNamedCache->getAll(ArrayList::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(HashMap::create());

        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, false);
        TS_ASSERT(!hCqc->isCacheValues());

        hCqc->setCacheValues(true);
        TS_ASSERT(hCqc->isCacheValues());

        hCqc->setCacheValues(false);
        TS_ASSERT(!hCqc->isCacheValues());
        }

    void testIsReadOnly()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(!hCqc->isReadOnly());
        }

    void testSetReadOnly()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        hCqc->setReadOnly(true);
        TS_ASSERT(hCqc->isReadOnly());
        try
            {
            hCqc->setReadOnly(false);
            TS_FAIL("Expected exception!");
            }
        catch( IllegalStateException::View )
            {
            //expected
            }
        }

    void testGetState()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getState()==ContinuousQueryCache::state_synchronized);
        }

    void testGetReconnectInterval()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        TS_ASSERT(hCqc->getReconnectInterval()==0);
        }

    void testSetReconnectInterval()
        {
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, DummyFilter::create(), false);
        hCqc->setReconnectInterval(999);
        TS_ASSERT(hCqc->getReconnectInterval()==999);
        }


    void testGetPutWithCacheValues()
        {
/*
        MockNamedCache::Handle hMockNamedCache = MockNamedCache::create();
        hMockNamedCache->keySet((Filter::View) NULL);
        hMockNamedCache->lastExpectation()->ignoreArguments();
        hMockNamedCache->setObjectReturn(HashSet::create());
        hMockNamedCache->replay();
        Filter::Handle hFilter = AlwaysFilter::create();

        //set expectations
        hMockNamedCache->entrySet(DummyFilter::create());
        hMockNamedCache->setMatcher(&matchAll);
        hMockNamedCache->setObjectReturn(Collections::emptySet());

        //replay
        hMockNamedCache->replay();

        ContinuousQueryCache::Handle hCqc = ContinuousQueryCache::create(hMockNamedCache, hFilter, true);

        String::View vsKey = String::create("key");
        String::Handle hsValue = String::create("value");

        TS_ASSERT(hCqc->get(vsKey)== NULL);

        hCqc->put(vsKey, hsValue);

        TS_ASSERT(hCqc->get(vsKey)->equals(hsValue));
        */
        }

    };


