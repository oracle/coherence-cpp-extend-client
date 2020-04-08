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
* Test Suite for the BoxHandle object.
*/
class BoxHandleTest : public CxxTest::TestSuite
    {
    public:
        void testBoxUnbox()
            {
            BoxHandle<Integer32> hn = 123;
            TS_ASSERT(hn->getValue() == 123);
            TS_ASSERT(hn == 123);
            }

        void testNonStrictNullUnbox()
            {
            BoxHandle<Integer32, int32_t, false> hn;
            TS_ASSERT(is_null(hn));
            TS_ASSERT(((int32_t) hn) == 0); // not sure if 0 is guaranteed, hmmmmm
            }

        void testStrictNullUnbox()
            {
            BoxHandle<Integer32> hn;
            TS_ASSERT(is_null(hn));
            int32_t nVal;
            TS_ASSERT_THROWS(nVal = (int32_t) hn, NullPointerException::View);
            (void) nVal; // avoid "unused-but-set-variable" gcc compilation warning
            }
    };
