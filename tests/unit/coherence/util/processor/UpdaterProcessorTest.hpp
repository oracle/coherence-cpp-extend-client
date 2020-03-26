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
#include "coherence/util/processor/UpdaterProcessor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the UpdaterProcessor object.
*/
class UpdaterProcessorSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();

            Object::Handle hValue = String::create("foo");

            //set expectations
            hMockEntry->isPresent();
            hMockEntry->lastExpectation()->setBoolReturn(true);
            hMockEntry->getValue();
            hMockEntry->lastExpectation()->setObjectReturn(hValue);
            hMockEntry->setValue(hValue, true);

            MockValueUpdater::Handle hUpdater = MockValueUpdater::create();

            UpdaterProcessor::Handle hProc =
                    UpdaterProcessor::create(cast<ValueUpdater::View>(hUpdater), hValue);
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
            Object::Handle hValue = String::create("foo");
            MockValueUpdater::Handle hUpdater = MockValueUpdater::create();

            UpdaterProcessor::Handle hProc =
                    UpdaterProcessor::create(cast<ValueUpdater::View>(hUpdater), hValue);

            TS_ASSERT(hProc->equals(hProc));

            UpdaterProcessor::Handle hProc2 =
                    UpdaterProcessor::create(cast<ValueUpdater::View>(hUpdater), hValue);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

