/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "mock/CommonMocks.hpp"

#include "coherence/lang.ns"

#include "coherence/net/PriorityTask.hpp"

#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/aggregator/Count.hpp"
#include "coherence/util/aggregator/PriorityAggregator.hpp"

#include "private/coherence/component/net/extend/protocol/cache/AggregateAllRequest.hpp"

#include "AggregatorStub.hpp"

using coherence::component::net::extend::protocol::cache::AggregateAllRequest;
using coherence::net::PriorityTask;
using coherence::util::InvocableMap;
using coherence::util::aggregator::Count;
using coherence::util::aggregator::PriorityAggregator;

using test::AggregatorStub;

using namespace std;
using namespace mock;

class AggregateAllRequestTest : public CxxTest::TestSuite
    {
    public:

        void testGetSchedulingPriorityWhenAggregatorIsNotPriorityAggregator()
            {
            InvocableMap::EntryAggregator::Handle hAggregator = Count::create();
            AggregateAllRequest::Handle           hRequest    = AggregateAllRequest::create();

            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::schedule_standard, hRequest->getSchedulingPriority());
            }

        void testGetSchedulingPriorityWhenAggregatorIsPriorityAggregator()
            {
            PriorityAggregator::Handle  hAggregator = PriorityAggregator::create();
            AggregateAllRequest::Handle hRequest    = AggregateAllRequest::create();

            hAggregator->setSchedulingPriority(PriorityTask::schedule_first);
            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::schedule_first, hRequest->getSchedulingPriority());
            }

        void testGetExecutionTimeoutMillisWhenAggregatorIsNotPriorityAggregator()
            {
            InvocableMap::EntryAggregator::Handle hAggregator = Count::create();
            AggregateAllRequest::Handle           hRequest    = AggregateAllRequest::create();

            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::timeout_default, hRequest->getExecutionTimeoutMillis());
            }

        void testGetExecutionTimeoutWhenAggregatorIsPriorityAggregator()
            {
            PriorityAggregator::Handle  hAggregator = PriorityAggregator::create();
            AggregateAllRequest::Handle hRequest    = AggregateAllRequest::create();

            hAggregator->setExecutionTimeoutMillis(PriorityTask::timeout_none);
            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::timeout_none, hRequest->getExecutionTimeoutMillis());
            }

        void testGetRequestTimeoutMillisWhenAggregatorIsNotPriorityAggregator()
            {
            InvocableMap::EntryAggregator::Handle hAggregator = Count::create();
            AggregateAllRequest::Handle           hRequest    = AggregateAllRequest::create();

            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::timeout_default, hRequest->getRequestTimeoutMillis());
            }

        void testGetRequestTimeoutMillisWhenAggregatorIsPriorityAggregator()
            {
            PriorityAggregator::Handle  hAggregator = PriorityAggregator::create();
            AggregateAllRequest::Handle hRequest    = AggregateAllRequest::create();

            hAggregator->setRequestTimeoutMillis(PriorityTask::timeout_none);
            hRequest->setAggregator(hAggregator);

            TS_ASSERT_EQUALS(PriorityTask::timeout_none, hRequest->getRequestTimeoutMillis());
            }

        void testShouldCallRunCanceledWhenAggregatorIsPriorityAggregator()
            {
            AggregatorStub::Handle      hAggregator = AggregatorStub::create();
            AggregateAllRequest::Handle hRequest    = AggregateAllRequest::create();

            hRequest->setAggregator(hAggregator);
            hRequest->runCanceled(true);

            TS_ASSERT_EQUALS(true, hAggregator->wasRunCanceledCalled());
            }
    };
