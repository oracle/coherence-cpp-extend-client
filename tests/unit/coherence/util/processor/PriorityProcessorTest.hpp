/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/processor/PriorityProcessor.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace mock;

/**
* Test Suite for the PriorityProcessor object.
*/
class PriorityProcessorSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockProcessor::Handle hMockProcessor = MockProcessor::create();

            MockInvocableMapEntry::Handle hEntry =
                MockInvocableMapEntry::create();

            //set expectations
            hMockProcessor->process(hEntry);

            PriorityProcessor::Handle hProc =
                    PriorityProcessor::create(hMockProcessor);
            //replay
            hMockProcessor->replay();
            hProc->process(hEntry);

            //verify
            hMockProcessor->verify();
            }
        };

