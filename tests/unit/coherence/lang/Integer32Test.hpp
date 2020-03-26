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
* Test Suite for the Integer32 class.
*/
class Integer32Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            int32_t n = 123;
            Integer32::Handle hn = Integer32::create(123);
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
            Integer32::Handle hn1 = Integer32::create(123);
            Integer32::Handle hn2 = cast<Integer32::Handle>(hn1->clone());
            TS_ASSERT(hn1 != hn2);
            TS_ASSERT(hn1->equals(hn2));
            TS_ASSERT(hn2->equals(hn1));
            }

        void testBoxing()
            {
            BoxHandle<Integer32> hn1 = 123;
            TS_ASSERT(hn1->getValue() == 123);
            hn1 = 456;
            TS_ASSERT(hn1->getValue() == 456);
            BoxHandle<Integer32> hn2 = 789;
            hn1 = hn2;
            TS_ASSERT(hn1 == hn2);
            TS_ASSERT(hn2 == hn1);
            Integer32::Handle hn3 = hn1;
            TS_ASSERT(hn3 == hn1);
            TS_ASSERT(hn1 == hn3);

            BoxHandle<Integer32> hn4 = 123;
            Object::View v = hn4;
            TS_ASSERT(hn4 == 123);
            TS_ASSERT(123 == hn4);
            hn4 = 0;
            v = hn4;
            TS_ASSERT(!is_null(hn4));
            TS_ASSERT(!is_null(hn4));
            TS_ASSERT(!is_null(v));

            clear_handle(hn4);
            TS_ASSERT(is_null(hn4));
            TS_ASSERT(is_null(hn4));
            v = hn4;
            TS_ASSERT(NULL == v);

            int32_t retVal;
            TS_ASSERT_THROWS(retVal = (int32_t) hn4, NullPointerException::View);
            (void) retVal; // avoid "unused-but-set-variable" gcc compilation warning
            }
    };
