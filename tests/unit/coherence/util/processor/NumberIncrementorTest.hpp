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
#include "coherence/util/processor/NumberIncrementor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the NumberIncrementor object.
*/
class NumberIncrementorSuite : public CxxTest::TestSuite
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

            Integer32::Handle hNum = Integer32::create(10);

            //set expectations
            hMockEntry->isPresent();
            hMockEntry->lastExpectation()->setBoolReturn(true);

            hMockPropertyManipulator->getExtractor();
            hMockPropertyManipulator->lastExpectation()->setObjectReturn(hMockValueExtractor);

            hMockEntry->extract(hMockValueExtractor);
            hMockEntry->lastExpectation()->setObjectReturn(hNum);

            hMockPropertyManipulator->getUpdater();
            hMockPropertyManipulator->lastExpectation()->setObjectReturn(hMockValueUpdater);

            hMockEntry->update(hMockValueUpdater, Integer32::create(11));

            NumberIncrementor::Handle hProc =
                    NumberIncrementor::create(
                            (ValueManipulator::View) hMockPropertyManipulator,
                            Integer32::create(1), false);
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

            NumberIncrementor::Handle hProc =
                NumberIncrementor::create(
                        (ValueManipulator::View) hMockPropertyManipulator,
                        Integer32::create(1), false);

            TS_ASSERT(hProc->equals(hProc));

            NumberIncrementor::Handle hProc2 =
                NumberIncrementor::create(
                        (ValueManipulator::View) hMockPropertyManipulator,
                        Integer32::create(1), false);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

