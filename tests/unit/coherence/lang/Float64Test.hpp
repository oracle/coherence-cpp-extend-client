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
* Test Suite for the Float64 class.
*/
class Float64Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            float64_t fl = 1.1;
            Float64::Handle hfl = Float64::create(1.1);
            TS_ASSERT(Float64::compare(hfl->getValue(), fl) == 0);

            Number::Handle h = hfl;
            TS_ASSERT(h->getInt16Value() == (int16_t) fl);
            TS_ASSERT(h->getInt32Value() == (int32_t) fl);
            TS_ASSERT(h->getInt64Value() == (int64_t) fl);
            TS_ASSERT(Float64::compare(h->getFloat32Value(), (float32_t) fl) == 0);
            TS_ASSERT(Float64::compare(h->getFloat64Value(), (float64_t) fl) == 0);
            }

        /**
        * Test compareTo().
        */
        void testCompareTo()
            {
            Float64::Handle hfl1 = Float64::create(42.1);
            Float64::Handle hfl2 = Float64::create(101010.1);
            TS_ASSERT(hfl1->compareTo(hfl2) < 0);
            TS_ASSERT(hfl2->compareTo(hfl1) > 0);
            TS_ASSERT(hfl1->compareTo(hfl1) == 0);
            }

        void testCompare()
            {
            float64_t dflNegInf = Float64::getNegativeInfinity();
            float64_t dflPosInf = Float64::getPositiveInfinity();
            float64_t dflNaN    = Float64::getNaN();

            TS_ASSERT(Float64::compare(dflNegInf, -1.0)     < 0);
            TS_ASSERT(Float64::compare(dflNegInf, dflNaN)   < 0);
            TS_ASSERT(Float64::compare(-1.0, -0.0)          < 0);
            TS_ASSERT(Float64::compare(-0.0, 0.0)           < 0);
            TS_ASSERT(Float64::compare(0.0, 1.0)            < 0);
            TS_ASSERT(Float64::compare(1.0, dflPosInf)      < 0);
            TS_ASSERT(Float64::compare(dflPosInf, dflNaN)   < 0);
            TS_ASSERT(Float64::compare(dflNaN, dflPosInf)   > 0);
            TS_ASSERT(Float64::compare(dflNaN, dflNaN)     == 0);
            TS_ASSERT(Float64::compare(1.0, 1.0)           == 0);
            }

        void testTests()
            {
            float64_t dflNegInf = Float64::getNegativeInfinity();
            float64_t dflPosInf = Float64::getPositiveInfinity();
            float64_t dflNaN    = Float64::getNaN();
		    float64_t dflZero   = Float64::valueOf(0.0)->getValue();


            TS_ASSERT( Float64::isInfinite(dflNegInf));
            TS_ASSERT( Float64::isInfinite(dflPosInf));
            TS_ASSERT(!Float64::isInfinite(1.0));
            TS_ASSERT( Float64::isZero(0.0));
            TS_ASSERT( Float64::isZero(-0.0));
            TS_ASSERT(!Float64::isZero(1.0));
            TS_ASSERT(!Float64::isNaN(1.0));
            TS_ASSERT( Float64::isNaN(dflNaN));
            TS_ASSERT( Float64::isInfinite(1.0 / dflZero));
            TS_ASSERT( Float64::isInfinite(-1.0 / dflZero));
            TS_ASSERT( Float64::isNaN(dflZero / dflZero ));
            }

        /**
        * Test compareTo().
        */
        void testEquals()
            {
            Float64::Handle hfl1 = Float64::create(1.1);
            Float64::Handle hfl2 = Float64::create(1.1);
            Float64::Handle hfl3 = Float64::create(42.1);
            TS_ASSERT(hfl1->equals(hfl1));
            TS_ASSERT(hfl1->equals(hfl2));
            TS_ASSERT(hfl2->equals(hfl1));
            TS_ASSERT(!hfl1->equals(hfl3));
            TS_ASSERT(!hfl3->equals(hfl1));
            TS_ASSERT(!hfl1->equals(String::create("1.1")));
            }

        /**
        * Test compareTo().
        */
        void testHashCode()
            {
            Float64::Handle hfl1 = Float64::create(1.1);
            Float64::Handle hfl2 = Float64::create(1.1);
            Float64::Handle hfl3 = Float64::create(42.1);
            TS_ASSERT(hfl1->hashCode() == hfl2->hashCode());
            TS_ASSERT(hfl1->hashCode() != hfl3->hashCode());
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Float64::Handle hfl1 = Float64::create(1.1);
            Float64::Handle hfl2 = cast<Float64::Handle>(hfl1->clone());
            TS_ASSERT(hfl1 != hfl2);
            TS_ASSERT(hfl1->equals(hfl2));
            TS_ASSERT(hfl2->equals(hfl1));
            }

        /**
        * Test float64ToBits() and bitsToFloat64().
        */
        void testToFromBits()
            {
            Float64::Handle hdfl1 = Float64::create(42.0);

            uint64_t  l   = Float64::float64ToBits(hdfl1->getValue());
            float64_t dfl = Float64::bitsToFloat64(l);

            Float64::Handle hdfl2 = Float64::create(dfl);
            TS_ASSERT(0 == hdfl1->compareTo(hdfl2));
            TS_ASSERT(hdfl1->equals(hdfl2));

            dfl = Float64::getNaN();
            TS_ASSERT(Float64::isNaN(dfl));

            l = Float64::float64ToBits(dfl);
            TS_ASSERT(((int64_t) 0x7ff80000) << 32 == l);
            }
    };
