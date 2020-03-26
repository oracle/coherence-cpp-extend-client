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
* Test Suite for the Octet class.
*/
class OctetSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            octet_t b = 123;
            Octet::Handle hb = Octet::create(123);
            TS_ASSERT(hb->getValue() == b);
            }

        /**
        * Test that compareTo returns the proper results
        */
        void testCompareTo()
            {
            Octet::Handle hb1 = Octet::create(42);
            Octet::Handle hb2 = Octet::create(101);
            TS_ASSERT(hb1->compareTo(hb2) <  0);
            TS_ASSERT(hb2->compareTo(hb1) >  0);
            TS_ASSERT(hb1->compareTo(hb1) == 0);
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Octet::Handle hb1 = Octet::create(123);
            Octet::Handle hb2 = cast<Octet::Handle>(hb1->clone());
            TS_ASSERT(hb1 != hb2);
            TS_ASSERT(hb1->equals(hb2));
            TS_ASSERT(hb2->equals(hb1));
            }
    };
