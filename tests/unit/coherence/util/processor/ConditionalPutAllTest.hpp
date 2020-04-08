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
#include "coherence/util/processor/ConditionalPutAll.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;
using namespace coherence::util::processor;
using namespace coherence::util::filter;
using namespace mock;

/**
* Test Suite for the ConditionalPutAll object.
*/
class ConditionalPutAllSuite : public CxxTest::TestSuite
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

            Object::Holder hKey = String::create("key");
            Object::Holder hValue = String::create("foo");

            hMap->put(hKey, hValue);

            //set expectations
            hMockEntry->getKey();
            hMockEntry->lastExpectation()->setObjectReturn(hKey);
            hMockEntry->setValue(hValue, false);

            ConditionalPutAll::Handle hProc =
                    ConditionalPutAll::create(AlwaysFilter::create(),
                            hMap);
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
            HashMap::Handle hMap = HashMap::create();

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
            Object::View   vKey2           = String::create("key1");

            hMap->put(vKey1, vValue1);
            hMap->put(vKey2, vValue2);

            //set expectations
            hMockEntry->getKey();
            hMockEntry->setObjectReturn(vKey1);
            ((MockInvocableMapEntry::View)hMockEntry)->getValue();
            hMockEntry->setObjectReturn(vValue1);
            hMockFilter->evaluate(vValue1);
            hMockFilter->setBoolReturn(false);

            hMockEntry2->getKey();
            hMockEntry2->setObjectReturn(vKey2);
            ((MockInvocableMapEntry::View)hMockEntry2)->getValue();
            hMockEntry2->setObjectReturn(vValue2);
            hMockFilter->evaluate(vValue2);
            hMockFilter->setBoolReturn(true);
            hMockEntry2->setValue(vValue2, false);

            ConditionalPutAll::Handle hProc =
                    ConditionalPutAll::create(hMockFilter, hMap);

            //replay
            hMockFilter->replay();
            hMockEntry->replay();
            hMockEntry2->replay();

            Map::View vResults = hProc->processAll(hSet);

            TS_ASSERT(vResults->size()==0);

            //verify
            hMockFilter->verify();
            hMockEntry->verify();
            hMockEntry2->verify();

            }

        /**
        * Test equals
        */
        void testEquals()
            {
            HashMap::Handle hMap = HashMap::create();
            Object::Holder hKey = String::create("key");
            Object::Holder hValue = String::create("foo");

            hMap->put(hKey, hValue);
            AlwaysFilter::View vFilter = AlwaysFilter::create();

            ConditionalPutAll::Handle hProc =
                    ConditionalPutAll::create(vFilter, hMap);

            TS_ASSERT(hProc->equals(hProc));

            ConditionalPutAll::Handle hProc2 =
                    ConditionalPutAll::create(vFilter, hMap);

            TS_ASSERT(hProc->equals(hProc2));
            TS_ASSERT(hProc2->equals(hProc));

            NeverFilter::View vFilter2 = NeverFilter::create();
            ConditionalPutAll::Handle hProc3 =
                    ConditionalPutAll::create(vFilter2, hMap);

            TS_ASSERT(!hProc3->equals(hProc2));
            TS_ASSERT(!hProc2->equals(hProc3));
            }

        };

