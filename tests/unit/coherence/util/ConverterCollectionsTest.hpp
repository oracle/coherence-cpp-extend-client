/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"

#include "coherence/util/ConverterCollections.hpp"
#include "coherence/util/MapEvent.hpp"

using namespace std;
using namespace coherence::util;
using namespace mock;

class ConverterCollectionsTest : public CxxTest::TestSuite
    {
    public:

        // ----- ConverterCacheEvent tests ----------------------------------

        void testConverterCacheEvent_getKey()
            {
            MockConverter::Handle hMockKeyConv = MockConverter::create();
            String::View vsKey = String::create("key");

            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    vsKey, String::create("old"),
                                    String::create("new"), false),
                            hMockKeyConv, MockConverter::create());

            hMockKeyConv->setStrict(true);
            // set mock expectations
            hMockKeyConv->convert(vsKey);
            Object::View vo = Object::create();
            hMockKeyConv->lastExpectation()->setObjectReturn(vo);
            hMockKeyConv->replay();

            // function being tested
            Object::View voReturned = vEvent->getKey();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getKey();
            TS_ASSERT(voReturned == vo);

            hMockKeyConv->verify();
            }

        void testConverterCacheEvent_getOldValue()
            {
            MockConverter::Handle hMockOldVal = MockConverter::create();
            String::View vsOldVal = String::create("old");

            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"), vsOldVal,
                                    String::create("new"), false),
                            MockConverter::create(), hMockOldVal);

            hMockOldVal->setStrict(true);
            // set mock expectations
            hMockOldVal->convert(vsOldVal);
            Object::View vo = Object::create();
            hMockOldVal->lastExpectation()->setObjectReturn(vo);
            hMockOldVal->replay();

            // function being tested
            Object::View voReturned = vEvent->getOldValue();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getOldValue();
            TS_ASSERT(voReturned == vo);

            hMockOldVal->verify();
            }

        void testConverterCacheEvent_getNewValue()
            {
            MockConverter::Handle hMockNewVal = MockConverter::create();
            String::View vsNewVal = String::create("new");

            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"),
                                    String::create("old"),
                                    vsNewVal, false),
                            MockConverter::create(), hMockNewVal);

            hMockNewVal->setStrict(true);
            // set mock expectations
            hMockNewVal->convert(vsNewVal);
            Object::View vo = Object::create();
            hMockNewVal->lastExpectation()->setObjectReturn(vo);
            hMockNewVal->replay();

            // function being tested
            Object::View voReturned = vEvent->getNewValue();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getNewValue();
            TS_ASSERT(voReturned == vo);

            hMockNewVal->verify();
            }

        void testConverteCacheEvent_getCacheEvent()
            {
            CacheEvent::View vCacheEvent = CacheEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"), false);

            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                        TestObservableMap::create(), vCacheEvent,
                        MockConverter::create(), MockConverter::create());

            TS_ASSERT(vEvent->getCacheEvent() == vCacheEvent);
            }

        void testConverterCacheEvent_getConverterKeyUp()
            {
            Converter::Handle hKeyConv = MockConverter::create();
            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"), String::create("old"),
                                    String::create("new"), false),
                            hKeyConv, MockConverter::create());

            TS_ASSERT(vEvent->getConverterKeyUp() == hKeyConv);
            }

        void testConverterCacheEvent_getConverterValueUp()
            {
            Converter::Handle hValueConv = MockConverter::create();
            ConverterCollections::ConverterCacheEvent::View vEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"), String::create("old"),
                                    String::create("new"), false),
                            MockConverter::create(), hValueConv);

            TS_ASSERT(vEvent->getConverterValueUp() == hValueConv);
            }

        void testConverterCacheEvent_setOldValue()
            {
            MockConverter::Handle hMockValueConv = MockConverter::create();

            ConverterCollections::ConverterCacheEvent::Handle hEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"), String::create("old-orig"),
                                    String::create("new"), false),
                            MockConverter::create(), hMockValueConv);

            hMockValueConv->setStrict(true);
            // set mock expectations.  No calls expected.
            hMockValueConv->replay();

            // function being tested
            String::View vsOld = String::create("old");
            hEvent->setOldValue(vsOld);
            Object::View voReturned = hEvent->getOldValue();
            TS_ASSERT(voReturned == vsOld);
            hMockValueConv->verify();
            }

        void testConverterCacheEvent_setNewValue()
            {
            MockConverter::Handle hMockValueConv = MockConverter::create();

            ConverterCollections::ConverterCacheEvent::Handle hEvent =
                    ConverterCollections::ConverterCacheEvent::create(
                            TestObservableMap::create(),
                            CacheEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"), String::create("old-orig"),
                                    String::create("new-orig"), false),
                            MockConverter::create(), hMockValueConv);

            hMockValueConv->setStrict(true);
            // set mock expectations.  No calls expected.
            hMockValueConv->replay();

            // function being tested
            String::View vsNew = String::create("new");
            hEvent->setNewValue(vsNew);
            Object::View voReturned = hEvent->getNewValue();
            TS_ASSERT(voReturned == vsNew);
            hMockValueConv->verify();
            }


        // ----- ConverterMapEvent tests ------------------------------------

        void testConverterMapEvent_getKey()
            {
            MockConverter::Handle hMockKeyConv = MockConverter::create();
            String::View vsKey = String::create("key");

            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    vsKey, String::create("old"),
                                    String::create("new")),
                            hMockKeyConv, MockConverter::create());

            hMockKeyConv->setStrict(true);
            // set mock expectations
            hMockKeyConv->convert(vsKey);
            Object::View vo = Object::create();
            hMockKeyConv->lastExpectation()->setObjectReturn(vo);
            hMockKeyConv->replay();

            // function being tested
            Object::View voReturned = vEvent->getKey();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getKey();
            TS_ASSERT(voReturned == vo);

            hMockKeyConv->verify();
            }

        void testConverterMapEvent_getOldValue()
            {
            MockConverter::Handle hMockOldVal = MockConverter::create();
            String::View vsOldVal = String::create("old");

            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"), vsOldVal,
                                    String::create("new")),
                            MockConverter::create(), hMockOldVal);

            hMockOldVal->setStrict(true);
            // set mock expectations
            hMockOldVal->convert(vsOldVal);
            Object::View vo = Object::create();
            hMockOldVal->lastExpectation()->setObjectReturn(vo);
            hMockOldVal->replay();

            // function being tested
            Object::View voReturned = vEvent->getOldValue();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getOldValue();
            TS_ASSERT(voReturned == vo);

            hMockOldVal->verify();
            }

        void testConverterMapEvent_getNewValue()
            {
            MockConverter::Handle hMockNewVal = MockConverter::create();
            String::View vsNewVal = String::create("new");

            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"),
                                    String::create("old"),
                                    vsNewVal),
                            MockConverter::create(), hMockNewVal);

            hMockNewVal->setStrict(true);
            // set mock expectations
            hMockNewVal->convert(vsNewVal);
            Object::View vo = Object::create();
            hMockNewVal->lastExpectation()->setObjectReturn(vo);
            hMockNewVal->replay();

            // function being tested
            Object::View voReturned = vEvent->getNewValue();
            TS_ASSERT(voReturned == vo);

            // ensure that same obj is returned and that converter is not invoked
            voReturned = vEvent->getNewValue();
            TS_ASSERT(voReturned == vo);

            hMockNewVal->verify();
            }

        void testConverterMapEvent_getMapEvent()
            {
            MapEvent::View vMapEvent = MapEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"));

            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                        TestObservableMap::create(), vMapEvent,
                        MockConverter::create(), MockConverter::create());

            TS_ASSERT(vEvent->getMapEvent() == vMapEvent);
            }

        void testConverterMapEvent_getConverterKeyUp()
            {
            Converter::Handle hKeyConv = MockConverter::create();
            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"),
                                    String::create("old"),
                                    String::create("new")),
                            hKeyConv, MockConverter::create());

            TS_ASSERT(vEvent->getConverterKeyUp() == hKeyConv);
            }

        void testConverterMapEvent_getConverterValueUp()
            {
            Converter::Handle hValueConv = MockConverter::create();
            ConverterCollections::ConverterMapEvent::View vEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"),
                                    String::create("old"),
                                    String::create("new")),
                            MockConverter::create(), hValueConv);

            TS_ASSERT(vEvent->getConverterValueUp() == hValueConv);
            }

        void testConverterMapEvent_setKey()
            {
            MockConverter::Handle hMockKeyConv = MockConverter::create();

            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"),
                                    String::create("old"),
                                    String::create("new")),
                            hMockKeyConv, MockConverter::create());

            hMockKeyConv->setStrict(true);
            // set mock expectations.  No calls expected.
            hMockKeyConv->replay();

            // function being tested
            String::View vsNewKey = String::create("newKey");
            hEvent->setKey(vsNewKey);
            Object::View voReturned = hEvent->getKey();
            TS_ASSERT(voReturned == vsNewKey);
            hMockKeyConv->verify();
            }

        void testConverterMapEvent_setOldValue()
            {
            MockConverter::Handle hMockValueConv = MockConverter::create();

            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"),
                                    String::create("old-orig"),
                                    String::create("new")),
                            MockConverter::create(), hMockValueConv);

            hMockValueConv->setStrict(true);
            // set mock expectations.  No calls expected.
            hMockValueConv->replay();

            // function being tested
            String::View vsOld = String::create("old");
            hEvent->setOldValue(vsOld);
            Object::View voReturned = hEvent->getOldValue();
            TS_ASSERT(voReturned == vsOld);
            hMockValueConv->verify();
            }

        void testConverterMapEvent_setNewValue()
            {
            MockConverter::Handle hMockValueConv = MockConverter::create();

            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"),
                                    String::create("old-orig"),
                                    String::create("new-orig")),
                            MockConverter::create(), hMockValueConv);

            hMockValueConv->setStrict(true);
            // set mock expectations.  No calls expected.
            hMockValueConv->replay();

            // function being tested
            String::View vsNew = String::create("new");
            hEvent->setNewValue(vsNew);
            Object::View voReturned = hEvent->getNewValue();
            TS_ASSERT(voReturned == vsNew);
            hMockValueConv->verify();
            }

        void testConverterMapEvent_isKeyConverted()
            {
            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("key"), String::create("old"),
                                    String::create("new")),
                            MockConverter::create(), MockConverter::create());


            TS_ASSERT(!hEvent->isKeyConverted());

            hEvent->setKey(String::create("new key"));
            TS_ASSERT(hEvent->isKeyConverted());
            }

        void testConverterMapEvent_isOldValueConverted()
            {
            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"),
                                    String::create("old-orig"),
                                    String::create("new")),
                            MockConverter::create(), MockConverter::create());

            TS_ASSERT(!hEvent->isOldValueConverted());

            hEvent->setOldValue(String::create("new old value"));
            TS_ASSERT(hEvent->isOldValueConverted());
            }

        void testConverterMapEvent_isNewValueConverted()
            {
            ConverterCollections::ConverterMapEvent::Handle hEvent =
                    ConverterCollections::ConverterMapEvent::create(
                            TestObservableMap::create(),
                            MapEvent::create(TestObservableMap::create(), 1,
                                    String::create("origKey"),
                                    String::create("old-orig"),
                                    String::create("new")),
                            MockConverter::create(), MockConverter::create());

            TS_ASSERT(!hEvent->isNewValueConverted());

            hEvent->setNewValue(String::create("new value"));
            TS_ASSERT(hEvent->isNewValueConverted());
            }


    };



