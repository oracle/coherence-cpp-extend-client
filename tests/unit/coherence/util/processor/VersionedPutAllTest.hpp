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
#include "coherence/util/processor/VersionedPutAll.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the VersionedPutAll object.
*/
class VersionedPutAllSuite : public CxxTest::TestSuite
        {
        public:

        /**
        * Test process
        */
        void testProcess()
            {
            HashMap::Handle hMap = HashMap::create();

            MockInvocableMapEntry::Handle hMockEntry =
                  MockInvocableMapEntry::create();

            MockVersionable::Handle hMockVer = MockVersionable::create();

            MockComparable::Handle hMockComparable = MockComparable::create();

            Object::Holder hKey = String::create("key");
            hMap->put(hKey, hMockVer);

            //set expectations
            hMockEntry->getKey();
            hMockEntry->lastExpectation()->setObjectReturn(hKey);
            hMockEntry->getValue();
            hMockEntry->lastExpectation()->setObjectReturn(hMockVer);

            hMockVer->getVersionIndicator();
            hMockVer->lastExpectation()->setObjectReturn(hMockComparable);
            hMockVer->getVersionIndicator();
            hMockVer->lastExpectation()->setObjectReturn(hMockComparable);

            hMockComparable->compareTo(hMockComparable);
            hMockComparable->lastExpectation()->setIntegerReturn(0);

            hMockVer->incrementVersion();
            hMockEntry->setValue(hMockVer, false);

            VersionedPutAll::Handle hProc =
                    VersionedPutAll::create(hMap, true, false);
            //replay
            hMockEntry->replay();
            hMockVer->replay();
            hMockComparable->replay();

            hProc->process(hMockEntry);

            //verify
            hMockEntry->verify();
            hMockVer->verify();
            hMockComparable->verify();
            }

        /**
        * Test processAll
        */
        void testProcessAll()
            {
            HashMap::Handle hMap = HashMap::create();

            MockInvocableMapEntry::Handle hMockEntry =
                  MockInvocableMapEntry::create();
            Set::Handle hSet = HashSet::create();
            hSet->add(hMockEntry);

            MockVersionable::Handle hMockVer = MockVersionable::create();

            MockComparable::Handle hMockComparable = MockComparable::create();

            Object::Holder hKey = String::create("key");
            hMap->put(hKey, hMockVer);

            //set expectations
            hMockEntry->getKey();
            hMockEntry->lastExpectation()->setObjectReturn(hKey);
            hMockEntry->getValue();
            hMockEntry->lastExpectation()->setObjectReturn(hMockVer);

            hMockVer->getVersionIndicator();
            hMockVer->lastExpectation()->setObjectReturn(hMockComparable);
            hMockVer->getVersionIndicator();
            hMockVer->lastExpectation()->setObjectReturn(hMockComparable);

            hMockComparable->compareTo(hMockComparable);
            hMockComparable->lastExpectation()->setIntegerReturn(0);

            hMockVer->incrementVersion();
            hMockEntry->setValue(hMockVer, false);

            VersionedPutAll::Handle hProc =
                    VersionedPutAll::create(hMap, true, false);
            //replay
            hMockEntry->replay();
            hMockVer->replay();
            hMockComparable->replay();

            hProc->processAll(hSet);

            //verify
            hMockEntry->verify();
            hMockVer->verify();
            hMockComparable->verify();
            }

        /**
        * Test equals
        */
        void testEquals()
            {
            HashMap::Handle hMap = HashMap::create();

            VersionedPutAll::Handle hProc =
                    VersionedPutAll::create(hMap, true, false);

            TS_ASSERT(hProc->equals(hProc));

            VersionedPutAll::Handle hProc2 =
                    VersionedPutAll::create(hMap, true, false);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));
            }

        };

