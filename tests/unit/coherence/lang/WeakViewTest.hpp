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
class WeakViewTest : public CxxTest::TestSuite, public Object
    {
    public:
        /**
        * WeakView dereference test
        */
        void testHandleDereference()
            {
            Boolean::View     vfTest  = Boolean::create(true);
            WeakView<Boolean> wvfTest(self(), vfTest);
            TS_ASSERT(vfTest->getValue() == wvfTest->getValue());
            }

        /**
        * Test retrieving a view for a live object.
        */
        void testGetLiveView()
            {
            int64_t       cObj = System::getHeapAnalyzer()->getObjectCount();
            Boolean::View vfTest;
                {
                vfTest = Boolean::create(true);
                }
            WeakView<Boolean> wvfTest(self(), vfTest);
// TODO            TS_ASSERT(vfTest == wvfTest);
            System::assertAttachCount(vfTest, 0, 2);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            }

        /**
        * Test retrieving a view for a dead object.
        */
        void testGetDeadHandle()
            {
            int64_t               cObj    = System::getHeapAnalyzer()->getObjectCount();
            Boolean::View     fTest;
                {
                fTest = Boolean::create(true);
                }
            WeakView<Boolean> wvfTest(self(), fTest);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 2);
            fTest = NULL;
            TS_ASSERT(NULL == (Boolean::View) wvfTest);
            TS_ASSERT(System::getHeapAnalyzer()->getObjectCount() == cObj + 1);
            }
    };
