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
#include "coherence/util/processor/ConditionalProcessor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the ConditionalProcessor object.
*/
class ConditionalProcessorSuite : public CxxTest::TestSuite
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

            ConditionalProcessor::Handle hProc =
                    ConditionalProcessor::create(AlwaysFilter::create(),
                            hMockProcessor);
            //replay
            hMockProcessor->replay();
            hProc->process(hEntry);

            //verify
            hMockProcessor->verify();
            }

        /**
        * Test processAll
        */
        void testProcessAll()
            {
            MockProcessor::Handle hMockProcessor = MockProcessor::create();

            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();
            MockInvocableMapEntry::Handle hMockEntry2 =
                MockInvocableMapEntry::create();

            Set::Handle hSet = HashSet::create();
            hSet->add(hMockEntry);
            hSet->add(hMockEntry2);

            MockFilter::Handle hMockFilter = MockFilter::create();
            Object::View   vValue1         = String::create("foo1");
            Object::View   vValue2         = String::create("foo2");

            Object::View   vKey1           = String::create("key1");

            //set expectations

            ((MockInvocableMapEntry::View)hMockEntry)->getValue();
            hMockEntry->setObjectReturn(vValue1);
            hMockFilter->evaluate(vValue1);
            hMockFilter->setBoolReturn(true);
            hMockEntry->getKey();
            hMockEntry->setObjectReturn(vKey1);
            hMockProcessor->process(hMockEntry);
            hMockProcessor->setObjectReturn(vValue1);

            ((MockInvocableMapEntry::View)hMockEntry2)->getValue();
            hMockEntry2->setObjectReturn(vValue2);
            hMockFilter->evaluate(vValue2);
            hMockFilter->setBoolReturn(false);

            ConditionalProcessor::Handle hProc =
                    ConditionalProcessor::create(hMockFilter, hMockProcessor);

            //replay
            hMockProcessor->replay();
            hMockFilter->replay();
            hMockEntry->replay();
            hMockEntry2->replay();

            Map::View vResults = hProc->processAll(hSet);

            TS_ASSERT(vResults->size()==1);
            TS_ASSERT(vResults->containsKey(vKey1));

            //verify
            hMockEntry->verify();
            hMockEntry2->verify();
            hMockFilter->verify();
            hMockProcessor->verify();
            }


        /**
        * Test equals
        */
        void testEquals()
            {

            MockProcessor::Handle hMockProcessor = MockProcessor::create();

            AlwaysFilter::View vFilter = AlwaysFilter::create();

            ConditionalProcessor::Handle hProc =
                    ConditionalProcessor::create(vFilter, hMockProcessor);

            TS_ASSERT(hProc->equals(hProc));

            ConditionalProcessor::Handle hProc2 =
                    ConditionalProcessor::create(vFilter, hMockProcessor);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));

            NeverFilter::View vFilter2 = NeverFilter::create();
            ConditionalProcessor::Handle hProc3 =
                    ConditionalProcessor::create(vFilter2, hMockProcessor);

            TS_ASSERT(!hProc3->equals(hProc2));
            TS_ASSERT(!hProc2->equals(hProc3));
            }

        };

