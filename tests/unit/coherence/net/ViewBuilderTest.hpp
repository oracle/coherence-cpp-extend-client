/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "mock/CommonMocks.hpp"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/net/ViewBuilder.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"
#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

#include "common/TestFilter.hpp"
#include "common/TestMapListener.hpp"
#include "common/TestTransformer.hpp"

using coherence::net::NamedCache;
using coherence::net::ViewBuilder;
using coherence::net::cache::ContinuousQueryCache;
using coherence::net::cache::LocalCache;
using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::filter::AlwaysFilter;
using common::test::TestFilter;
using common::test::TestMapListener;
using common::test::TestTransformer;

using namespace coherence::lang;
using namespace mock;

/**
 * Unit tests for ViewBuilder.
 *
 * @author rl 6.3.19
 * @since 12.2.1.4
 */
class ViewBuilderTest : public CxxTest::TestSuite
    {
    // ----- test methods ---------------------------------------------------

    public:
        void testViewBuilderDefaults()
            {
            NamedCache::Handle hCache = ViewBuilder::create(cast<NamedCache::Handle>(createCache("defaults")))->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(AlwaysFilter::getInstance()->equals(hQueryCache->getFilter()))
            TS_ASSERT(hQueryCache->getMapListener()       == NULL)
            TS_ASSERT(hQueryCache->getTransformer()       == NULL)
            TS_ASSERT(hQueryCache->getReconnectInterval() == 0)
            TS_ASSERT(hQueryCache->getCacheNameSupplier() == NULL)
            TS_ASSERT(!hQueryCache->isReadOnly())
            }

        void testViewFromNamedCache()
            {
            NamedCache::Handle view = createCache("fromNamedCache")->view()->build();

            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(view))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(view);

            TS_ASSERT(AlwaysFilter::getInstance()->equals(hQueryCache->getFilter()))
            TS_ASSERT(hQueryCache->getMapListener()       == NULL)
            TS_ASSERT(hQueryCache->getTransformer()       == NULL)
            TS_ASSERT(hQueryCache->getReconnectInterval() == 0)
            TS_ASSERT(hQueryCache->getCacheNameSupplier() == NULL)
            TS_ASSERT(!hQueryCache->isReadOnly())
            }

        void testViewBuilderWithFilter()
            {
            Filter::Handle     hFilter = TestFilter::create("foo", 0);
            NamedCache::Handle hCache  = ViewBuilder::create(cast<NamedCache::Handle>(createCache("filter")))->filter(hFilter)->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(hFilter->equals(hQueryCache->getFilter()))
            }

        void testViewBuilderWithListener()
            {
            MapListener::Handle hListener = TestMapListener::create();
            NamedCache::Handle  hCache    = ViewBuilder::create(cast<NamedCache::Handle>(createCache("listener")))->listener(hListener)->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(hListener->equals(hQueryCache->getMapListener()))
            }

        void testViewBuilderWithTransformer()
            {
            ValueExtractor::Handle hTransformer = TestTransformer::create();
            NamedCache::Handle     hCache       = ViewBuilder::create(cast<NamedCache::Handle>(createCache("transformer")))->map(hTransformer)->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(hTransformer->equals(hQueryCache->getTransformer()))
            }

        void testViewBuilderCacheValues()
            {
            NamedCache::Handle hCache = ViewBuilder::create(cast<NamedCache::Handle>(createCache("cachingValues")))->values()->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(hQueryCache->isCacheValues())
            }

        void testViewBuilderKeysOnly()
            {
            NamedCache::Handle hCache = ViewBuilder::create(cast<NamedCache::Handle>(createCache("keysOnly")))->keys()->build();
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hCache))

            ContinuousQueryCache::Handle hQueryCache = cast<ContinuousQueryCache::Handle>(hCache);

            TS_ASSERT(!hQueryCache->isCacheValues())
            }
    
    // ----- helper methods -------------------------------------------------

    protected:
        NamedCache::Handle createCache(String::View sName)
            {
            MockNamedCache::Handle hCache = MockNamedCache::create();

            //set expectations
            hCache->getCacheName();
            hCache->setObjectReturn(sName);
            hCache->view();
            hCache->setObjectReturn(ViewBuilder::create(cast<NamedCache::Handle>(hCache)));
            hCache->keySet((Filter::View) NULL);
            hCache->lastExpectation()->ignoreArguments();
            hCache->setObjectReturn(HashSet::create());
            hCache->entrySet((Filter::View) NULL);
            hCache->lastExpectation()->ignoreArguments();
            hCache->setObjectReturn(HashSet::create());

            //replay
            hCache->replay();

            return hCache;
            }
    };
