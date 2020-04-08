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
#include "coherence/util/processor/CompositeProcessor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the CompositeProcessor object.
*/
class CompositeProcessorSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockProcessor::Handle hMockProcessor1 = MockProcessor::create();
            MockProcessor::Handle hMockProcessor2 = MockProcessor::create();

            ObjectArray::Handle oahProcs = ObjectArray::create(2);

            oahProcs[0] = hMockProcessor1;
            oahProcs[1] = hMockProcessor2;

            MockInvocableMapEntry::Handle hEntry =
                MockInvocableMapEntry::create();

            //set expectations
            hMockProcessor1->process(hEntry);
            hMockProcessor2->process(hEntry);

            CompositeProcessor::Handle hProc =
                    CompositeProcessor::create(oahProcs);
            //replay
            hMockProcessor1->replay();
            hMockProcessor2->replay();
            hProc->process(hEntry);

            //verify
            hMockProcessor1->verify();
            hMockProcessor2->verify();
            }

        /**
        * Test equals
        */
        void testEquals()
            {

            MockProcessor::Handle hMockProcessor1 = MockProcessor::create();
            MockProcessor::Handle hMockProcessor2 = MockProcessor::create();

            ObjectArray::Handle oahProcs = ObjectArray::create(2);

            oahProcs[0] = hMockProcessor1;
            oahProcs[1] = hMockProcessor2;

            CompositeProcessor::Handle hProc =
                    CompositeProcessor::create(oahProcs);

            TS_ASSERT(hProc->equals(hProc));

            CompositeProcessor::Handle hProc2 =
                    CompositeProcessor::create(oahProcs);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));

            MockProcessor::Handle hMockProcessor3 = MockProcessor::create();

            ObjectArray::Handle oahProcs2 = ObjectArray::create(3);

            oahProcs2[0] = hMockProcessor1;
            oahProcs2[1] = hMockProcessor2;
            oahProcs2[2] = hMockProcessor3;

            CompositeProcessor::Handle hProc3 =
                    CompositeProcessor::create(oahProcs2);

            TS_ASSERT(!hProc3->equals(hProc2));
            TS_ASSERT(!hProc2->equals(hProc3));
            }

        };

