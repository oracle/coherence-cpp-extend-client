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
* Test Suite for the Integer16 class.
*/
class Integer16Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            int16_t n = 123;
            Integer16::Handle hn = Integer16::create(123);
            TS_ASSERT(hn->getValue() == n);

            Number::Handle h = hn;
            TS_ASSERT(h->getInt16Value() == (int16_t) n);
            TS_ASSERT(h->getInt32Value() == (int32_t) n);
            TS_ASSERT(h->getInt64Value() == (int64_t) n);
            TS_ASSERT(Float32::compare(h->getFloat32Value(), (float32_t) n) == 0);
            TS_ASSERT(Float64::compare(h->getFloat64Value(), (float64_t) n) == 0);
            }

        /**
        * Test compareTo().
        */
        void testCompareTo()
            {
            Integer16::Handle hn1 = Integer16::create(42);
            Integer16::Handle hn2 = Integer16::create(1010);
            TS_ASSERT(hn1->compareTo(hn2) <  0);
            TS_ASSERT(hn2->compareTo(hn1) >  0);
            TS_ASSERT(hn1->compareTo(hn1) == 0);
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Integer16::Handle hn1 = Integer16::create(123);
            Integer16::Handle hn2 = cast<Integer16::Handle>(hn1->clone());
            TS_ASSERT(hn1 != hn2);
            TS_ASSERT(hn1->equals(hn2));
            TS_ASSERT(hn2->equals(hn1));
            }
    };
