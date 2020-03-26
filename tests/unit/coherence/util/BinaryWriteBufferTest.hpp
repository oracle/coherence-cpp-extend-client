/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/util/Binary.hpp"
#include "coherence/util/BinaryWriteBuffer.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::util::Binary;
using coherence::util::BinaryWriteBuffer;


/**
* Test suite for the BinaryWriteBuffer class.
*/
class BinaryWriteBufferTest : public CxxTest::TestSuite
    {
    public:
        Array<octet_t>::Handle createByteArray(size32_t c, octet_t b)
            {
            Array<octet_t>::Handle hab  = Array<octet_t>::create(c);
            for (size32_t i = 0; i < c; ++i)
                {
                hab[i] = (octet_t) b;
                }
            return hab;
            }

        /**
        * Test BinaryWriteBuffer creation.
        */
        void testCreate()
            {
            Array<octet_t>::View      vab = createByteArray(10, 7);
            BinaryWriteBuffer::Handle hwb = BinaryWriteBuffer::create(10);

            TS_ASSERT(hwb->length() == 0);
            hwb->write(0, vab);
            TS_ASSERT(hwb->length() == 10);

            hwb = BinaryWriteBuffer::create(0);
            TS_ASSERT(hwb->length() == 0);
            hwb->write(0, vab);
            TS_ASSERT(hwb->length() == 10);

            hwb = BinaryWriteBuffer::create(0, 10);
            TS_ASSERT(hwb->length() == 0);
            hwb->write(0, vab);
            TS_ASSERT(hwb->length() == 10);

            hwb = BinaryWriteBuffer::create(0, 7);
            TS_ASSERT_THROWS(hwb->write(0, vab), IndexOutOfBoundsException::View);

            TS_ASSERT_THROWS(BinaryWriteBuffer::create(25, 5), IllegalArgumentException::View);
            }

        /**
        * Test toBinary().
        */
        void testToBinary()
            {
            Array<octet_t>::View      vab = createByteArray(10, 7);
            BinaryWriteBuffer::Handle hwb = BinaryWriteBuffer::create(10);

            hwb->write(0, vab);

            Binary::View vBin = hwb->toBinary();
            TS_ASSERT(vBin->length() == 10);
            TS_ASSERT(vBin->equals(Binary::create(vab)));

            // BinaryWriteBuffer is now immutable
            TS_ASSERT_THROWS(hwb->write(0, vab), IllegalStateException::View);
            }
    };
