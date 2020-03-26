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
* Test Suite for the Float32 class.
*/
class Float32Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            float32_t fl = (float32_t) 1.1;
            Float32::Handle hfl1 = Float32::create((float32_t) 1.1);
            TS_ASSERT(Float32::compare(hfl1->getValue(), fl) == 0);

            Number::Handle h = hfl1;
            TS_ASSERT(h->getInt16Value() == (int16_t) fl);
            TS_ASSERT(h->getInt32Value() == (int32_t) fl);
            TS_ASSERT(h->getInt64Value() == (int64_t) fl);
            TS_ASSERT(Float32::compare(h->getFloat32Value(), (float32_t) fl) == 0);
            TS_ASSERT(Float64::compare(h->getFloat64Value(), (float64_t) fl) == 0);
            }

        /**
        * Test compareTo().
        */
        void testCompareTo()
            {
            Float32::Handle hfl11 = Float32::create((float32_t) 42.1);
            Float32::Handle hfl12 = Float32::create((float32_t) 101010.1);
            TS_ASSERT(hfl11->compareTo(hfl12) < 0);
            TS_ASSERT(hfl12->compareTo(hfl11) > 0);
            TS_ASSERT(hfl11->compareTo(hfl11) == 0);
            }

        void testCompare()
            {
            float32_t flNegInf = Float32::getNegativeInfinity();
            float32_t flPosInf = Float32::getPositiveInfinity();
            float32_t flNaN    = Float32::getNaN();

            TS_ASSERT(Float32::compare(flNegInf, -1.0f)  < 0);
            TS_ASSERT(Float32::compare(flNegInf, flNaN)  < 0);
            TS_ASSERT(Float32::compare(-1.0f, -0.0f)     < 0);
            TS_ASSERT(Float32::compare(-0.0f, 0.0f)      < 0);
            TS_ASSERT(Float32::compare(0.0f, 1.0f)       < 0);
            TS_ASSERT(Float32::compare(1.0f, flPosInf)   < 0);
            TS_ASSERT(Float32::compare(flPosInf, flNaN)  < 0);
            TS_ASSERT(Float32::compare(flNaN, flPosInf)  > 0);
            TS_ASSERT(Float32::compare(flNaN, flNaN)    == 0);
            TS_ASSERT(Float32::compare(1.0f, 1.0f)      == 0);
            }

        void testTests()
            {
            float32_t flNegInf = Float32::getNegativeInfinity();
            float32_t flPosInf = Float32::getPositiveInfinity();
            float32_t flNaN    = Float32::getNaN();
            float32_t flZero   = Float32::valueOf(0.0f)->getValue();

            TS_ASSERT( Float32::isInfinite(flNegInf));
            TS_ASSERT( Float32::isInfinite(flPosInf));
            TS_ASSERT(!Float32::isInfinite(1.0f));
            TS_ASSERT( Float32::isZero(0.0f));
            TS_ASSERT( Float32::isZero(-0.0f));
            TS_ASSERT(!Float32::isZero(1.0f));
            TS_ASSERT(!Float32::isNaN(1.0f));
            TS_ASSERT( Float32::isNaN(flNaN));
            TS_ASSERT( Float32::isInfinite(1.0f / flZero));
            TS_ASSERT( Float32::isInfinite(-1.0f / flZero));
            TS_ASSERT( Float32::isNaN(flZero / flZero ));
            }

        /**
        * Test compareTo().
        */
        void testEquals()
            {
            Float32::Handle hfl11 = Float32::create((float32_t) 1.1);
            Float32::Handle hfl12 = Float32::create((float32_t) 1.1);
            Float32::Handle hfl13 = Float32::create((float32_t) 42.1);
            TS_ASSERT(hfl11->equals(hfl11));
            TS_ASSERT(hfl11->equals(hfl12));
            TS_ASSERT(hfl12->equals(hfl11));
            TS_ASSERT(!hfl11->equals(hfl13));
            TS_ASSERT(!hfl13->equals(hfl11));
            TS_ASSERT(!hfl11->equals(String::create("1.1")));
            }

        /**
        * Test compareTo().
        */
        void testHashCode()
            {
            Float32::Handle hfl11 = Float32::create((float32_t) 1.1);
            Float32::Handle hfl12 = Float32::create((float32_t) 1.1);
            Float32::Handle hfl13 = Float32::create((float32_t) 42.1);
            TS_ASSERT(hfl11->hashCode() == hfl12->hashCode());
            TS_ASSERT(hfl11->hashCode() != hfl13->hashCode());
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Float32::Handle hfl11 = Float32::create((float32_t) 1.1);
            Float32::Handle hfl12 = cast<Float32::Handle>(hfl11->clone());
            TS_ASSERT(hfl11 != hfl12);
            TS_ASSERT(hfl11->equals(hfl12));
            TS_ASSERT(hfl12->equals(hfl11));
            }

        /**
        * Test float32ToBits() and bitsToFloat32().
        */
        void testFloat32Bits()
            {
            Float32::Handle hfl1 = Float32::create(42.0F);

            uint32_t  n  = Float32::float32ToBits(hfl1->getValue());
            float32_t fl = Float32::bitsToFloat32(n);

            Float32::Handle hfl2 = Float32::create(fl);
            TS_ASSERT(0 == hfl1->compareTo(hfl2));
            TS_ASSERT(hfl1->equals(hfl2));

            fl = Float32::getNaN();
            TS_ASSERT(Float32::isNaN(fl));

            n = Float32::float32ToBits(fl);
            TS_ASSERT(0x7FC00000 == n);
            }
    };
