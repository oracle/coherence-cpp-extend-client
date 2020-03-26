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
#include "coherence/util/processor/ConditionalPut.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the ConditionalPut object.
*/
class ConditionalPutSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();

            Object::Holder hValue = String::create("foo");
            //set expectations
            hMockEntry->setValue(hValue, false);

            ConditionalPut::Handle hProc =
                    ConditionalPut::create(AlwaysFilter::create(),
                            hValue, true);
            //replay
            hMockEntry->replay();
            hProc->process(hMockEntry);

            //verify
            hMockEntry->verify();
            }

        /**
        * Test processAll
        */
        void testProcessAll()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();
            Set::Handle hSet = HashSet::create();
            hSet->add(hMockEntry);

            Object::Holder hValue = String::create("foo");
            //set expectations
            hMockEntry->setValue(hValue, false);

            ConditionalPut::Handle hProc =
                    ConditionalPut::create(AlwaysFilter::create(),
                            hValue, true);
            //replay
            hMockEntry->replay();
            hProc->processAll(hSet);

            //verify
            hMockEntry->verify();
            }

        /**
        * Test equals
        */
        void testEquals()
            {
            Object::Holder hValue = String::create("foo");
            AlwaysFilter::View vFilter = AlwaysFilter::create();

            ConditionalPut::Handle hProc =
                    ConditionalPut::create(vFilter, hValue, true);

            TS_ASSERT(hProc->equals(hProc));

            ConditionalPut::Handle hProc2 =
                    ConditionalPut::create(vFilter, hValue, true);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));

            NeverFilter::View vFilter2 = NeverFilter::create();
            ConditionalPut::Handle hProc3 =
                    ConditionalPut::create(vFilter2, hValue, true);

            TS_ASSERT(!hProc3->equals(hProc2));
            TS_ASSERT(!hProc2->equals(hProc3));
            }

        };

