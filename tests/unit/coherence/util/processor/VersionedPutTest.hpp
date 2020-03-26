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
#include "coherence/util/processor/VersionedPut.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the VersionedPut object.
*/
class VersionedPutSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();
            MockVersionable::Handle hMockVer = MockVersionable::create();

            //set expectations
            hMockEntry->getValue();
            hMockEntry->lastExpectation()->setObjectReturn(NULL);

            hMockVer->incrementVersion();
            hMockEntry->setValue(hMockVer, false);


            VersionedPut::Handle hProc =
                    VersionedPut::create(hMockVer, true, false);
            //replay
            hMockVer->replay();
            hMockEntry->replay();
            hProc->process(hMockEntry);

            //verify
            hMockVer->verify();
            hMockEntry->verify();
            }

        /**
        * Test processAll
        */
        void testProcessAll()
            {
            MockInvocableMapEntry::Handle hMockEntry =
                MockInvocableMapEntry::create();
            MockVersionable::Handle hMockVer = MockVersionable::create();
            Set::Handle hSet = HashSet::create();
            hSet->add(hMockEntry);

            //set expectations
            hMockEntry->getValue();
            hMockEntry->lastExpectation()->setObjectReturn(NULL);

            hMockVer->incrementVersion();
            hMockEntry->setValue(hMockVer, false);


            VersionedPut::Handle hProc =
                    VersionedPut::create(hMockVer, true, false);
            //replay
            hMockVer->replay();
            hMockEntry->replay();
            hProc->processAll(hSet);

            //verify
            hMockVer->verify();
            hMockEntry->verify();
            }

        /**
        * Test equals
        */
        void testEquals()
            {
            MockVersionable::Handle hMockVer = MockVersionable::create();

            VersionedPut::Handle hProc =
                    VersionedPut::create(hMockVer, true, false);

            TS_ASSERT(hProc->equals(hProc));

            VersionedPut::Handle hProc2 =
                    VersionedPut::create(hMockVer, true, false);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

