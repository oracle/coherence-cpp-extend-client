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

#include "coherence/util/processor/ExtractorProcessor.hpp"
#include "coherence/util/processor/PriorityProcessor.hpp"

#include "private/coherence/component/net/extend/protocol/cache/InvokeRequest.hpp"

#include "ProcessorStub.hpp"

using coherence::component::net::extend::protocol::cache::InvokeRequest;
using coherence::net::PriorityTask;
using coherence::util::InvocableMap;
using coherence::util::processor::ExtractorProcessor;
using coherence::util::processor::PriorityProcessor;
using test::ProcessorStub;

using namespace std;
using namespace mock;

class InvokeRequestTest : public CxxTest::TestSuite
    {
    public:

        void testGetSchedulingPriorityWhenProcessorIsNotPriorityProcessor()
            {
            InvocableMap::EntryProcessor::Handle hProcessor = ExtractorProcessor::create();
            InvokeRequest::Handle                hRequest   = InvokeRequest::create();

            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::schedule_standard, hRequest->getSchedulingPriority());
            }

        void testGetSchedulingPriorityWhenProcessorIsPriorityProcessor()
            {
            PriorityProcessor::Handle hProcessor = PriorityProcessor::create();
            InvokeRequest::Handle     hRequest   = InvokeRequest::create();

            hProcessor->setSchedulingPriority(PriorityTask::schedule_first);
            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::schedule_first, hRequest->getSchedulingPriority());
            }

        void testGetExecutionTimeoutMillisWhenProcessorIsNotPriorityProcessor()
            {
            InvocableMap::EntryProcessor::Handle hProcessor = ExtractorProcessor::create();
            InvokeRequest::Handle                hRequest   = InvokeRequest::create();

            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::timeout_default, hRequest->getExecutionTimeoutMillis());
            }

        void testGetExecutionTimeoutWhenProcessorIsPriorityProcessor()
            {
            PriorityProcessor::Handle hProcessor = PriorityProcessor::create();
            InvokeRequest::Handle     hRequest   = InvokeRequest::create();

            hProcessor->setExecutionTimeoutMillis(PriorityTask::timeout_none);
            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::timeout_none, hRequest->getExecutionTimeoutMillis());
            }

        void testGetRequestTimeoutMillisWhenProcessorIsNotPriorityProcessor()
            {
            InvocableMap::EntryProcessor::Handle hProcessor = ExtractorProcessor::create();
            InvokeRequest::Handle                hRequest   = InvokeRequest::create();

            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::timeout_default, hRequest->getRequestTimeoutMillis());
            }

        void testGetRequestTimeoutMillisWhenProcessorIsPriorityProcessor()
            {
            PriorityProcessor::Handle hProcessor = PriorityProcessor::create();
            InvokeRequest::Handle     hRequest   = InvokeRequest::create();

            hProcessor->setRequestTimeoutMillis(PriorityTask::timeout_none);
            hRequest->setProcessor(hProcessor);

            TS_ASSERT_EQUALS(PriorityTask::timeout_none, hRequest->getRequestTimeoutMillis());
            }

        void testShouldCallRunCanceledWhenAggregatorIsPriorityAggregator()
            {
            ProcessorStub::Handle hProcessor = ProcessorStub::create();
            InvokeRequest::Handle hRequest   = InvokeRequest::create();

            hRequest->setProcessor(hProcessor);
            hRequest->runCanceled(true);

            TS_ASSERT_EQUALS(true, hProcessor->wasRunCanceledCalled());
            }
    };
