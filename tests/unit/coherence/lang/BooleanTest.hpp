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
* Test Suite for the Boolean class.
*/
class BooleanTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test that a Boolean set to true has a true value.
        */
        void testTrue()
            {
            Boolean::Handle hfTest = Boolean::create(true);
            TS_ASSERT(hfTest->getValue());
            }

        /**
        * Test that a Boolean set to false has a false value.
        */
        void testFalse()
            {
            Boolean::Handle hfTest = Boolean::create(false);
            TS_ASSERT(!hfTest->getValue());
            }

        /**
        * Test the two Boolean constants.
        */
        void testConstants()
            {
            TS_ASSERT( Boolean::valueOf(true)->getValue());
            TS_ASSERT(!Boolean::valueOf(false)->getValue());
            }

        /**
        * Test compareTo().
        */
        void testCompareTo()
            {
            Boolean::Handle hf1 = Boolean::create(false);
            Boolean::Handle hf2 = Boolean::create(true);
            TS_ASSERT(hf1->compareTo(hf2) <  0);
            TS_ASSERT(hf2->compareTo(hf1) >  0);
            TS_ASSERT(hf1->compareTo(hf1) == 0);
            }

        /**
        * Test equals().
        */
        void testEquals()
            {
            Boolean::Handle hf1 = Boolean::create(true);
            Boolean::Handle hf2 = Boolean::create(true);
            Boolean::Handle hf3 = Boolean::create(false);
            TS_ASSERT( hf1->equals(hf1));
            TS_ASSERT( hf1->equals(hf2));
            TS_ASSERT( hf2->equals(hf1));
            TS_ASSERT(!hf1->equals(hf3));
            TS_ASSERT(!hf3->equals(hf1));
            TS_ASSERT(!hf1->equals(String::create("true")));
            }

        /**
        * Test hashCode().
        */
        void testHashCode()
            {
            Boolean::Handle hf1 = Boolean::create(true);
            Boolean::Handle hf2 = Boolean::create(false);
            TS_ASSERT_EQUALS(hf1->hashCode(), size32_t(1231));
            TS_ASSERT_EQUALS(hf2->hashCode(), size32_t(1237));
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Boolean::Handle hf1 = Boolean::create(true);
            Boolean::Handle hf2 = cast<Boolean::Handle>(hf1->clone());
            TS_ASSERT(hf1 != hf2);
            TS_ASSERT(hf1->equals(hf2));
            TS_ASSERT(hf2->equals(hf1));
            }

        /**
        * Test that a Boolean is immutable.
        */
        void testImmutable()
            {
            Boolean::Handle hf = Boolean::create(true);
            TS_ASSERT(hf->isImmutable());
            }

        /**
        * Test toString(bool).
        */
        void testToString()
            {
            TS_ASSERT(Boolean::toString(true)->equals("true"));
            TS_ASSERT(Boolean::toString(false)->equals("false"));
            }
    };
