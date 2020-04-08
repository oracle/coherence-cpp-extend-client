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
#include "coherence/net/ServiceInfo.hpp"

#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/Filter.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"

#include "coherence/util/filter/NeverFilter.hpp"

#include "common/TestFilter.hpp"
#include "common/TestTransformer.hpp"
#include "common/TestUtils.hpp"

using namespace std;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::net::ServiceInfo;
using coherence::util::Filter;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::NeverFilter;
using coherence::net::cache::ContinuousQueryCache;
using common::test::TestFilter;
using common::test::TestTransformer;

/**
 * view-scheme configuration tests.
 * 
 * @author rl 2019.05.13
 * @since 12.2.1.4
 */ 
class ViewTest : public CxxTest::TestSuite
    {
    // ----- test methods ---------------------------------------------------

    public:
        void testUseCustomFilter()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCache("view-filter"));
            TS_ASSERT(NeverFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == false)
            }

        void testUseCustomFilterWithInitParams()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCache("view-filter-with-params"));
            TS_ASSERT(TestFilter::create("foo", 10)->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == false)
            }

        void testThrowIfConfiguredFilterIsInvalid()
            {
            TS_ASSERT_THROWS_ANYTHING(validateIsCqc(getCache("view-filter-invalid")))
            }

        void testUseCustomTransformer()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCacheNoClear("view-transformer"));
            TS_ASSERT(AlwaysFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(IdentityExtractor::getInstance()->equals(cache->getTransformer()));
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == true)
            }

        void testUseCustomTransformerWithInitParams()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCacheNoClear("view-transformer-with-params"));
            TS_ASSERT(AlwaysFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(TestTransformer::create(42)->equals(cache->getTransformer()));
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == true)
            }

        void testUseConfiguredReadOnly()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCacheNoClear("view-read-only"));
            TS_ASSERT(AlwaysFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == true)
            }

        void testUseConfiguredReconnectInterval()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCache("view-reconnect-interval"));
            TS_ASSERT(AlwaysFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 1000)
            TS_ASSERT(cache->isReadOnly() == false)
            }

        void testUseConfiguredListener()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCache("view-with-listener"));
            TS_ASSERT(AlwaysFilter::getInstance()->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() != NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == false)
            }

        void testUseCustomFilterWithInitParamsAndMacro()
            {
            ContinuousQueryCache::Handle cache = validateIsCqc(getCache("view-with-macro"));
            TS_ASSERT(TestFilter::create("foo", 50)->equals(cache->getFilter()));
            TS_ASSERT(cache->getTransformer() == NULL)
            TS_ASSERT(cache->getMapListener() == NULL)
            TS_ASSERT(cache->getReconnectInterval() == 0)
            TS_ASSERT(cache->isReadOnly() == false)
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }

    // ----- helper methods -------------------------------------------------

    protected:
        NamedCache::Handle getCache(String::View vsCacheName)
            {
            return ensureCleanCache(vsCacheName, "defaultconfig/extend-view-cache-config.xml");
            }

        NamedCache::Handle getCacheNoClear(String::View vsCacheName)
            {
            CacheFactory::configure(CacheFactory::loadXmlFile("defaultconfig/extend-view-cache-config.xml"));
            return CacheFactory::getCache(vsCacheName);
            }

        ContinuousQueryCache::Handle validateIsCqc(NamedCache::Handle hNamedCache)
            {
            TS_ASSERT(hNamedCache->getCacheService()->getInfo()->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(instanceof<ContinuousQueryCache::Handle>(hNamedCache));
            return cast<ContinuousQueryCache::Handle>(hNamedCache);
            }
    };
