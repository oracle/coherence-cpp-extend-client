/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include <iomanip>
#include <iostream>
#include <limits>
#include <math.h> // cmath has linker errors on some machines (missing pow functions)
#include <sstream>

using namespace coherence::lang;

/**
* Test Suite for the Primitive class.
*/
class PrimitiveTest : public CxxTest::TestSuite
    {
    public:
        void testBooleanParse()
            {
            TS_ASSERT( Boolean::parse("true"));
            TS_ASSERT(!Boolean::parse("false"));
            TS_ASSERT(!Boolean::parse("foo"));
            TS_ASSERT(!Boolean::parse(NULL));
            TS_ASSERT(!Boolean::parse(String::null_string));
            }

        void testCharacter16Parse()
            {
            wchar_t awch[3] = { L'\xb5' , L'\xb5' , 0 };

            TS_ASSERT_EQUALS(Character16::parse(String::create(&awch[1])), wchar_t(L'\xb5'));               // wide char
            TS_ASSERT_THROWS(Character16::parse(String::create(awch)),     IllegalArgumentException::View); // 2 char string - illegal
            TS_ASSERT_EQUALS(Character16::parse(String::create("f")),      wchar_t('f'));                   // ASCII
            TS_ASSERT_THROWS(Character16::parse(NULL),                     IllegalArgumentException::View); // NULL - illegal

            // UTF-8 to UTF-16 conversion
            char car[4] = { '\xe2', '\x82', '\xac', 0 }; // e2 82 ac is UTF-8 Euro currency symbol
            TS_ASSERT_EQUALS(Character16::parse(String::create(car)), wchar_t(L'\x20ac')); // 20 ac is UTF-16 Euro currency symbol
            }

        void testInteger16Parse()
            {
            int16_t iTest = Integer16::parse("16000");

            TS_ASSERT(iTest == 16000);
            }

        void testInteger32Parse()
            {
            int32_t iTest = Integer32::parse("32767");

            TS_ASSERT(iTest == 32767);
            }

        void testInteger64Parse()
            {
            int64_t lExpected = 0x7FFFFFFF;
            lExpected = lExpected << 32;
            lExpected = lExpected | 0xFFFFFFFF;

            int64_t lTest = Integer64::parse("9223372036854775807");

            TS_ASSERT(lTest == lExpected);
            }

        void testFloat32Parse()
            {
            std::stringstream s;
            float32_t         flTest = Float32::parse("3.14159");
            float32_t         flTest2;

            s << "3.14159";
            s >> flTest2;

            // checking floats for equality is problematic
            TS_ASSERT(areNearlyEqual(flTest, flTest2, 6));

            // with precision
            float32_t flPrecision  = Float32::parse("13.14159", 2);
            float32_t flPrecision2 = (float32_t) 13.14;
            TS_ASSERT(areNearlyEqual(flPrecision, flPrecision2, 2));
            }

        void testFloat64Parse()
            {
            std::stringstream s;
            float64_t         flTest = Float64::parse("3.141592");
            float64_t         flTest2;

            s << "3.141592";
            s >> flTest2;

            TS_ASSERT(areNearlyEqual(flTest, flTest2, 7));

            // with precision
            float64_t flPrecision  = Float64::parse("13.141592", 4);
            float64_t flPrecision2 = 13.1416;
            TS_ASSERT(areNearlyEqual(flPrecision, flPrecision2, 4));

            flPrecision  = Float64::parse("13.141592653589793", 12);
            flPrecision2 = 13.1415926535900;
            TS_ASSERT(areNearlyEqual(flPrecision, flPrecision2, 12));
            }

        void testBadParse()
            {
            try
                {
                Integer32::parse("this is not a number");
                }
            catch (IllegalArgumentException::View)
                {
                return;
                }

            TS_FAIL("Exception not caught!");
            }

        void testBooleanToString()
            {
            TS_ASSERT(Boolean::valueOf(true)->toString()->equals("true"));
            TS_ASSERT(Boolean::valueOf(false)->toString()->equals("false"));
            }

        // ----- helper methods ----------------------------------------------

        /**
         * Check two floats for near equality.
         *
         * @param flVal1      a float
         * @param flVal2      another float
         * @param nPrecision  precision (1/10^nPrecision)
         *
         * @return true if the floats are equal to the specified precision
         */
        bool areNearlyEqual(float32_t flVal1, float32_t flVal2, int nPrecision)
            {
            float32_t flDiff = (float32_t) fabs(flVal1 - flVal2);
            if (Float32::compare(flDiff, 0) == 0)  // general case
                {
                return true;
                }
            // display the difference
            float64_t flEpsilon = 1.0 / pow((float32_t) 10, nPrecision); // 64 bit float for epsilon to avoid loss of precision warnings on VS2019
            std::cout << std::setprecision(std::numeric_limits<float32_t>::digits10 + 2)
                      << "\nflVal1=" << flVal1 << ", flVal2=" << flVal2 << ", flDiff="
                      << flDiff << ", flEpsilon=" << flEpsilon << std::endl;
            return flDiff < flEpsilon;
            }

        /**
         * Check two floats for near equality.
         *
         * @param flVal1      a float
         * @param flVal2      another float
         * @param nPrecision  precision (1/10^nPrecision)
         *
         * @return true if the floats are equal to the specified precision
         */
        bool areNearlyEqual(float64_t flVal1, float64_t flVal2, int nPrecision)
            {
            float64_t flDiff = (float64_t) fabs(flVal1 - flVal2);
            if (Float64::compare(flDiff, 0) == 0)  // general case
                {
                return true;
                }
            // display the difference
            float64_t flEpsilon = (float64_t) 1.0 / pow((float64_t) 10, nPrecision);
            std::cout << std::setprecision(std::numeric_limits<float64_t>::digits10 + 2)
                      << "\nflVal1=" << flVal1 << ", flVal2=" << flVal2 << ", flDiff="
                      << flDiff << ", flEpsilon=" << flEpsilon << std::endl;
            return flDiff < flEpsilon;
            }
    };
