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
#include "coherence/util/processor/NumberMultiplier.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the NumberMultiplier object.
*/
class NumberMultiplierSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                    MockInvocableMapEntry::create();

            MockPropertyManipulator::Handle hMockPropertyManipulator =
                    MockPropertyManipulator::create();

            MockValueExtractor::Handle hMockValueExtractor =
                    MockValueExtractor::create();
            MockValueUpdater::Handle hMockValueUpdater =
                    MockValueUpdater::create();

            Integer32::Handle hNum = Integer32::create(2);

            //set expectations
            hMockEntry->isPresent();
            hMockEntry->lastExpectation()->setBoolReturn(true);

            hMockPropertyManipulator->getExtractor();
            hMockPropertyManipulator->lastExpectation()->setObjectReturn(hMockValueExtractor);

            hMockEntry->extract(hMockValueExtractor);
            hMockEntry->lastExpectation()->setObjectReturn(hNum);

            hMockPropertyManipulator->getUpdater();
            hMockPropertyManipulator->lastExpectation()->setObjectReturn(hMockValueUpdater);

            hMockEntry->update(hMockValueUpdater, Integer32::create(42));

            NumberMultiplier::Handle hProc =
                    NumberMultiplier::create(
                            (PropertyManipulator::View)hMockPropertyManipulator,
                            Integer32::create(21), false);
            //replay
            hMockEntry->replay();
            hMockPropertyManipulator->replay();
            hMockValueExtractor->replay();
            hMockValueUpdater->replay();


            hProc->process(hMockEntry);


            //verify
            hMockEntry->verify();
            hMockPropertyManipulator->verify();
            hMockValueExtractor->verify();
            hMockValueUpdater->verify();
            }


        /**
        * Test equals
        */
        void testEquals()
            {
            MockPropertyManipulator::Handle hMockPropertyManipulator =
                    MockPropertyManipulator::create();

            NumberMultiplier::Handle hProc =
                NumberMultiplier::create(
                        (PropertyManipulator::View)hMockPropertyManipulator,
                        Integer32::create(1), false);

            TS_ASSERT(hProc->equals(hProc));

            NumberMultiplier::Handle hProc2 =
                NumberMultiplier::create(
                        (PropertyManipulator::View)hMockPropertyManipulator,
                        Integer32::create(1), false);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

