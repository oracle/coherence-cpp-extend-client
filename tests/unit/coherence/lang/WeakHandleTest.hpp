/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

using namespace coherence::lang;

/**
* Test Suite for the Handle.
*/
class WeakHandleTest : public CxxTest::TestSuite, public Object
    {
    public:
        /**
        * WeakHandle dereference test
        */
        void testHandleDereference()
            {
            Boolean::Handle     hfTest  = Boolean::create(true);
            WeakHandle<Boolean> whfTest(self(), hfTest);
            TS_ASSERT(hfTest->getValue() == whfTest->getValue());
            }

        /**
        * Test retrieving a handle for a live object.
        */
        void testGetLiveHandle()
            {
            int64_t             cObj    = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle     hfTest  = Boolean::create(true);
            WeakHandle<Boolean> whfTest(self(), hfTest);
            TS_ASSERT(hfTest == whfTest);
            TS_ASSERT(whfTest == hfTest);
            System::assertAttachCount(hfTest, 2, 0);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            }

        void testClearHandle()
            {
            Boolean::Handle     hfTest  = Boolean::create(true);
            WeakHandle<Boolean> whfTest(self(), hfTest);
            TS_ASSERT(whfTest == hfTest);
            whfTest = NULL;
            TS_ASSERT(whfTest != hfTest);
            TS_ASSERT(NULL == whfTest);
            }

        /**
        * Test retrieving a handle for a dead object.
        */
        void testGetDeadHandle()
            {
            int64_t             cObj    = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle     fTest   = Boolean::create(true);
            WeakHandle<Boolean> whfTest(self(), fTest);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            fTest = NULL;
            TS_ASSERT(NULL == (Boolean::Handle) whfTest);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 1);
            }

        /**
        * Test a weakly reference Object becoming immutable
        */
        void testImmutable()
            {
            int64_t         cObj  = System::getHeapAnalyzer()->getObjectCount();
            Boolean::Handle fTest = Boolean::create(true);
            Boolean::View   vfTest = fTest;
            WeakHandle<Boolean> whfTest(self(), fTest);
            System::assertAttachCount(vfTest, 2, 0);
            fTest = NULL;
            System::assertAttachCount(vfTest, 0, 2);
            TS_ASSERT(NULL == (Boolean::Handle) whfTest);
            const WeakHandle<Boolean>& wvfTest = whfTest;
            TS_ASSERT(vfTest == (Boolean::View) wvfTest);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            }

        void testEscape()
            {
            Object::Handle hGuard = Object::create(); // unescaped
            Object::Handle hEscape = Object::create(); // will escape
            System::assertAttachCount(hGuard, 1, 0);
            System::assertAttachCount(hEscape, 1, 0);
            TS_ASSERT(hGuard->_isEscaped() == false);
            TS_ASSERT(hEscape->_isEscaped() == false);
                {
                WeakHandle<Object> whEscape(*hGuard, hEscape); // unescaped WH to escaped WR
                System::assertAttachCount(hGuard, 1, 0);
                System::assertAttachCount(hEscape, 2, 0);
                TS_ASSERT(hGuard->_isEscaped() == false);
                TS_ASSERT(hEscape->_isEscaped() == false);

                // escape hEscape, which will cause it's WR to escape
                MemberView<Object> mvEscape(System::common(), hEscape);
                System::assertAttachCount(hGuard, 1, 0);

                System::assertAttachCount(hEscape, 3, 2);
                TS_ASSERT(hGuard->_isEscaped() == false);
                TS_ASSERT(hEscape->_isEscaped() == true);

                Object::View vEscape = hEscape;
                System::assertAttachCount(vEscape, 3, 2);
                TS_ASSERT(hEscape->_isEscaped() == true);

                hEscape = NULL;
                System::assertAttachCount(hGuard, 1, 0);
                System::assertAttachCount(vEscape, 0, 5);
                TS_ASSERT(hGuard->_isEscaped() == false);
                TS_ASSERT(vEscape->_isEscaped() == true);
                }
            }
    };


