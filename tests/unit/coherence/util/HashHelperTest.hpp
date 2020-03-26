/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashHelper.hpp"
#include "coherence/util/List.hpp"

using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::HashHelper;
using coherence::util::List;

/**
* Test Suite for the HashHelper static helper class.
*/
class HashHelperTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test boolean
        */
        void testBoolean()
            {
            TS_ASSERT(0x04DF == HashHelper::hash(true, 1));
            TS_ASSERT(0x0525 == HashHelper::hash(false, 31));

            // array
            Array<bool>::Handle hafGeorge = Array<bool>::create(1);
            hafGeorge[0] = true;
            TS_ASSERT(0x000005CF == HashHelper::hash<bool>(hafGeorge, 1));
            }

        /**
        * Test octet (byte)
        */
        void testOctet()
            {
            TS_ASSERT(0x11 == HashHelper::hash((octet_t) 0x1, 1));
            TS_ASSERT(0x01D0 == HashHelper::hash((octet_t) 0x20, 31));

            // array
            Array<octet_t>::Handle habOctet = Array<octet_t>::create(1);
            habOctet[0] = 0x1;
            TS_ASSERT(0x00000101 == HashHelper::hash<octet_t>(habOctet, 1));
            }

        /**
        * Test wchar16_t
        */
        void testChar()
            {
            TS_ASSERT(0x01D0 == HashHelper::hash(' ', 31));

            // array
            Array<wchar16_t>::Handle hachChar = Array<wchar16_t>::create(1);
            hachChar[0] = ' ';
            TS_ASSERT(0x00000120 == HashHelper::hash<wchar16_t>(hachChar, 1));
            }

        /**
        * Test float64_t
        */
        void testDouble()
            {
            TS_ASSERT(0x3FF00010 == HashHelper::hash((float64_t) 1.0, 1));
            TS_ASSERT(0x8C8CC1F1 == HashHelper::hash((float64_t) 32.1, 31));

            // array
            Array<float64_t>::Handle hadflDouble = Array<float64_t>::create(1);
            hadflDouble[0] = 1.0;
            TS_ASSERT(0x3FF00100 == HashHelper::hash(hadflDouble, 1));
            }

        /**
        * Test float32_t
        */
        void testFloat()
            {
            TS_ASSERT(0x3F800010 == HashHelper::hash((float32_t) 1.0, 1));
            TS_ASSERT(0x42006796 == HashHelper::hash((float32_t) 32.1, 31));

            // array
            Array<float32_t>::Handle haflFloat = Array<float32_t>::create(1);
            haflFloat[0] = 1.0;
            TS_ASSERT(0x3F800100 == HashHelper::hash<float32_t>(haflFloat, 1));
            }

        /**
        * Test size32_t
        */
        void testInt()
            {
            TS_ASSERT(0x0011 == HashHelper::hash((size32_t)1, 1));
            TS_ASSERT(0x01D0 == HashHelper::hash((size32_t)32, 31));

            // array
            Array<size32_t>::Handle hanInt = Array<size32_t>::create(1);
            hanInt[0] = 1;
            TS_ASSERT(0x00000101 == HashHelper::hash<size32_t>(hanInt, 1));
            }

        /**
        * Test size64_t
        */
        void testLong()
            {
            TS_ASSERT(0x0011 == HashHelper::hash((size64_t) 1, 1));
            TS_ASSERT(0x01D0 == HashHelper::hash((size64_t) 32, 31));

            // array
            Array<size64_t>::Handle halLong = Array<size64_t>::create(1);
            halLong[0] = 1;
            TS_ASSERT(0x00000101 == HashHelper::hash<size64_t>(halLong, 1));
            }

        /**
        * Test int16_t
        */
        void testShort()
            {
            TS_ASSERT(0x0011 == HashHelper::hash((int16_t) 1, 1));
            TS_ASSERT(0x01D0 == HashHelper::hash((int16_t) 32, 31));

            // array
            Array<int16_t>::Handle hashShort = Array<int16_t>::create(1);
            hashShort[0] = 1;
            TS_ASSERT(0x00000101 == HashHelper::hash<int16_t>(hashShort, 1));
            }

        /**
        * Test Object
        */
        void testObject()
            {
            ObjectArray::Handle ah    = ObjectArray::create(1);
            List::Handle        hList = ArrayList::create();
            hList->add(Integer32::create(1));

            ah[0] = Boolean::valueOf(true);
            TS_ASSERT(0x000014DF == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Octet::valueOf(0x1);
            TS_ASSERT(0x00001011 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Character16::valueOf(' ');
            TS_ASSERT(0x00001030 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Float64::valueOf(1.0);
            TS_ASSERT(0x3FF01010 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Float32::valueOf(1.0);
            TS_ASSERT(0x3F801010 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Integer32::create(1);
            TS_ASSERT(0x00001011 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Integer64::valueOf(1);
            TS_ASSERT(0x00001011 == HashHelper::hash(cast<Object::View>(ah), 1));
            ah[0] = Integer16::valueOf(1);
            TS_ASSERT(0x00001011 == HashHelper::hash(cast<Object::View>(ah), 1));
            TS_ASSERT(0x00000011 == HashHelper::hash(cast<Collection::View>(hList), 1));
            }
        };
