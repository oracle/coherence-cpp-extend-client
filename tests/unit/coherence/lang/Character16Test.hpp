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
* Test Suite for the Character16 class.
*/
class Character16Suite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test getValue().
        */
        void testGetValue()
            {
            wchar16_t ch = 'c';
            Character16::Handle hch = Character16::create('c');
            TS_ASSERT(hch->getValue() == ch);
            }

        /**
        * Test isASCII().
        */
        void testIsASCII()
            {
            Character16::Handle hch1 = Character16::create(0);
            Character16::Handle hch2 = Character16::create('j');
            Character16::Handle hch3 = Character16::create(128);
            TS_ASSERT(!hch1->isASCII());
            TS_ASSERT(hch2->isASCII());
            TS_ASSERT(!hch3->isASCII());
            }

        /**
        * Test compareTo().
        */
        void testCompareTo()
            {
            Character16::Handle hch1 = Character16::create('a');
            Character16::Handle hch2 = Character16::create('z');
            TS_ASSERT(hch1->compareTo(hch2) <  0);
            TS_ASSERT(hch2->compareTo(hch1) >  0);
            TS_ASSERT(hch1->compareTo(hch1) == 0);
            }

        /**
        * Test clone().
        */
        void testClone()
            {
            Character16::Handle hch1 = Character16::create(true);
            Character16::Handle hch2 = cast<Character16::Handle>(hch1->clone());
            TS_ASSERT(hch1 != hch2);
            TS_ASSERT(hch1->equals(hch2));
            TS_ASSERT(hch2->equals(hch1));
            }
    };
