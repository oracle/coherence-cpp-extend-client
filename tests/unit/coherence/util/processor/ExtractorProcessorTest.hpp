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
#include "coherence/util/processor/ExtractorProcessor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the ExtractorProcessor object.
*/
class ExtractorProcessorSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                    MockInvocableMapEntry::create();

            MockValueExtractor::Handle hMockExtractor =
                    MockValueExtractor::create();


            //set expectations
            hMockEntry->extract(hMockExtractor);

            ExtractorProcessor::Handle hProc =
                    ExtractorProcessor::create((ValueExtractor::View)hMockExtractor);
            //replay
            hMockEntry->replay();

            hProc->process(hMockEntry);

            //verify
            hMockEntry->verify();
            }


        /**
        * Test equals
        */
        void testEquals()
            {
            MockValueExtractor::Handle hMockExtractor =
                    MockValueExtractor::create();

            ExtractorProcessor::Handle hProc =
                    ExtractorProcessor::create((ValueExtractor::View)hMockExtractor);

            TS_ASSERT(hProc->equals(hProc));

            ExtractorProcessor::Handle hProc2 =
                    ExtractorProcessor::create((ValueExtractor::View)hMockExtractor);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

