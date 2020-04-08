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
* Test Suite for the Integer64 class.
*/
class Integer64Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            int64_t n = 123;
            Integer64::Handle hn = Integer64::create(123);
            TS_ASSERT(hn->getValue() == n);

            Number::Handle h = hn;
            TS_ASSERT(h->getInt16Value() == (int16_t) n);
            TS_ASSERT(h->getInt32Value() == (int32_t) n);
            TS_ASSERT(h->getInt64Value() == (int64_t) n);
            TS_ASSERT(Float32::compare(h->getFloat32Value(), (float32_t) n) == 0);
            TS_ASSERT(Float64::compare(h->getFloat64Value(), (float64_t) n) == 0);
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Integer64::Handle hn1 = Integer64::create(123);
            Integer64::Handle hn2 = cast<Integer64::Handle>(hn1->clone());
            TS_ASSERT(hn1 != hn2);
            TS_ASSERT(hn1->equals(hn2));
            TS_ASSERT(hn2->equals(hn1));
            }

        void testMin()
            {
            int64_t lMin = Integer64::min_value;
            TS_ASSERT(lMin + 1 > Integer64::min_value);
            COH_ENSURE_RELATION(int64_t, lMin - 1, >, lMin); // rollover
            }

        void testMax()
            {
            int64_t lMax = Integer64::max_value;
            TS_ASSERT(lMax - 1 < Integer64::max_value);
            COH_ENSURE_RELATION(int64_t, lMax + 1, <, lMax); // rollover
            }
    };
