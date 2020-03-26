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
* Test Suite for the Volatile.
*/
class VolatileSuite : public CxxTest::TestSuite
    {
    public:
        void testAssign()
            {
            Volatile<bool> fVolatile(System::common());
            fVolatile = true;
            TS_ASSERT(fVolatile == true);
            }

        void testCompare()
            {
            Volatile<bool> fVolatile(System::common());
            fVolatile = true;
            TS_ASSERT(fVolatile == true);
            TS_ASSERT(fVolatile != false);
            TS_ASSERT(fVolatile == fVolatile)
            }

        void testDereference()
            {
            Volatile<bool> fVolatile(System::common());
            fVolatile = true;
            bool f = fVolatile;
            TS_ASSERT(f == true);
            }
    };
