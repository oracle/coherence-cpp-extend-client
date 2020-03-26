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
#include "coherence/net/PriorityTask.hpp"

#include "coherence/net/RequestTimeoutException.hpp"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Filter.hpp"

#include "coherence/util/aggregator/PriorityAggregator.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"

#include "coherence/util/processor/PriorityProcessor.hpp"

#include "coherence/tests/SlowAggregator.hpp"
#include "coherence/tests/SlowProcessor.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using coherence::net::PriorityTask;
using coherence::net::RequestTimeoutException;
using coherence::util::Collection;
using coherence::util::Filter;
using coherence::util::aggregator::PriorityAggregator;
using coherence::util::filter::AlwaysFilter;
using coherence::util::processor::PriorityProcessor;
using coherence::tests::SlowAggregator;
using coherence::tests::SlowProcessor;

class PriorityTaskTest : public CxxTest::TestSuite
    {
    // ----- test fixtures --------------------------------------------------

public:

    void testShouldInvokePriorityProcessorWithTimeoutNone()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_none);

        TS_ASSERT(hCache->invoke(getKEY(), hPriorityProcessor)->equals(getRETURN_VALUE()));

        setDefaultCacheConfigure();
        }

    void testShouldInvokePriorityProcessorWithTimeoutDefault()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_default);

        TS_ASSERT_THROWS(hCache->invoke(getKEY(), hPriorityProcessor), RequestTimeoutException::View);

        setDefaultCacheConfigure();
        }

    void testShouldInvokeAllPriorityProcessorWithTimeoutNone()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_none);

        Collection::Handle hKeys = HashSet::create();
        hKeys->add(getKEY());

        TS_ASSERT(hCache->invoke((Collection::View)hKeys, hPriorityProcessor)->equals(getRETURN_VALUE()));

        setDefaultCacheConfigure();
        }

    void testShouldInvokeAllPriorityProcessorWithTimeoutDefault()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_default);

        Collection::Handle hKeys = HashSet::create();
        hKeys->add(getKEY());

        TS_ASSERT_THROWS(hCache->invoke((Collection::View)hKeys, hPriorityProcessor), RequestTimeoutException::View);

        setDefaultCacheConfigure();
        }

    void testShouldInvokeFilterPriorityProcessorWithTimeoutNone()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_none);

        TS_ASSERT(hCache->invoke(AlwaysFilter::create(), hPriorityProcessor)->equals(getRETURN_VALUE()));

        setDefaultCacheConfigure();
        }

    void testShouldInvokeFilterPriorityProcessorWithTimeoutDefault()
        {
        NamedCache::Handle        hCache             = setupCache();
        PriorityProcessor::Handle hPriorityProcessor = createProcessor(PriorityTask::timeout_default);

        TS_ASSERT_THROWS(hCache->invoke(AlwaysFilter::create(), hPriorityProcessor), RequestTimeoutException::View);

        setDefaultCacheConfigure();
        }

    void testShouldAggregateAllPriorityProcessorWithTimeoutNone()
        {
        NamedCache::Handle         hCache              = setupCache();
        PriorityAggregator::Handle hPriorityAggregator = createAggregator(PriorityTask::timeout_none);

        Collection::Handle hKeys = HashSet::create();
        hKeys->add(getKEY());

        TS_ASSERT_THROWS_NOTHING(hCache->aggregate((Collection::View)hKeys, hPriorityAggregator));

        setDefaultCacheConfigure();
        }

    void testShouldAggregateAllPriorityProcessorWithTimeoutDefault()
        {
        NamedCache::Handle         hCache              = setupCache();
        PriorityAggregator::Handle hPriorityAggregator = createAggregator(PriorityTask::timeout_default);

        Collection::Handle hKeys = HashSet::create();
        hKeys->add(getKEY());

        TS_ASSERT_THROWS(hCache->aggregate((Collection::View)hKeys, hPriorityAggregator),
                         RequestTimeoutException::View);

        setDefaultCacheConfigure();
        }

    void testShouldAggregateFilterPriorityProcessorWithTimeoutNone()
        {
        NamedCache::Handle         hCache              = setupCache();
        PriorityAggregator::Handle hPriorityAggregator = createAggregator(PriorityTask::timeout_none);

        TS_ASSERT_THROWS_NOTHING(hCache->aggregate((Filter::View) AlwaysFilter::create(), hPriorityAggregator));

        setDefaultCacheConfigure();
        }

    void testShouldAggregateFilterPriorityProcessorWithTimeoutDefault()
        {
        NamedCache::Handle         hCache              = setupCache();
        PriorityAggregator::Handle hPriorityAggregator = createAggregator(PriorityTask::timeout_default);

        TS_ASSERT_THROWS(hCache->aggregate((Filter::View) AlwaysFilter::create(), hPriorityAggregator),
                         RequestTimeoutException::View);

        setDefaultCacheConfigure();
        }


    /**
    * Clean up after the tests
    */
    void testCleanup()
        {
        CacheFactory::shutdown();
        }

    /**
     * Set-up and return the cache to use for tests.
     */
    NamedCache::Handle setupCache()
        {
        String::View vsCacheName   = String::create("dist-default");
        String::View vsCacheConfig = String::create("defaultconfig/extend-cache-config-short-timeout.xml");

        CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));

        NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);

        hCache->clear();

        hCache->put(getKEY(), Integer32::create(100));

        return hCache;
        }

    /**
     * Create a PriorityProcessor::Handle that wraps a SlowProcessor::Handle
     * and set the request timeout to the specified value.
     */
    PriorityProcessor::Handle createProcessor(int64_t lTimeout)
        {
        SlowProcessor::Handle     hSlowProcessor     = SlowProcessor::create(8000, getRETURN_VALUE());
        PriorityProcessor::Handle hPriorityProcessor = PriorityProcessor::create(hSlowProcessor);

        hPriorityProcessor->setRequestTimeoutMillis(lTimeout);

        return hPriorityProcessor;
        }

    /**
     * Create a PriorityAggregator::Handle that wraps a SlowAggregator::Handle.
     */
    PriorityAggregator::Handle createAggregator(int64_t lTimeout)
        {
        SlowAggregator::Handle     hSlowAggregator     = SlowAggregator::create();
        PriorityAggregator::Handle hPriorityAggregator = PriorityAggregator::create(hSlowAggregator);

        hPriorityAggregator->setRequestTimeoutMillis(lTimeout);

        return hPriorityAggregator;
        }

    /**
     * Obtain the key of the cache entry to test against
     */
    static String::View getKEY()
        {
        return String::create("key-1");
        }

    /**
     * Obtain the extepected return value.
     */
    static String::View getRETURN_VALUE()
        {
        return String::create("ReturnValue");
        }
    };
