/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/Binary.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::io::OctetArrayReadBuffer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::util::Binary;


/**
* Test suite for the OctetArrayReadBuffer class.
*/
class OctetArrayReadBufferSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the OctetArrayReadBuffer creation.
        */
        void testCreate()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(hab);
            TS_ASSERT(hrb->length() == 9);
            TS_ASSERT(hrb->read(0) == 0);
            TS_ASSERT(hrb->read(1) == 1);
            TS_ASSERT(hrb->read(2) == 2);
            TS_ASSERT(hrb->read(3) == 3);
            TS_ASSERT(hrb->read(4) == 4);
            TS_ASSERT(hrb->read(5) == 5);
            TS_ASSERT(hrb->read(6) == 6);
            TS_ASSERT(hrb->read(7) == 7);
            TS_ASSERT(hrb->read(8) == 8);

            hrb = OctetArrayReadBuffer::create(hab, 0);
            TS_ASSERT(hrb->length() == 9);
            TS_ASSERT(hrb->read(0) == 0);
            TS_ASSERT(hrb->read(1) == 1);
            TS_ASSERT(hrb->read(2) == 2);
            TS_ASSERT(hrb->read(3) == 3);
            TS_ASSERT(hrb->read(4) == 4);
            TS_ASSERT(hrb->read(5) == 5);
            TS_ASSERT(hrb->read(6) == 6);
            TS_ASSERT(hrb->read(7) == 7);
            TS_ASSERT(hrb->read(8) == 8);

            hrb = OctetArrayReadBuffer::create(hab, 1);
            TS_ASSERT(hrb->length() == 8);
            TS_ASSERT(hrb->read(0) == 1);
            TS_ASSERT(hrb->read(1) == 2);
            TS_ASSERT(hrb->read(2) == 3);
            TS_ASSERT(hrb->read(3) == 4);
            TS_ASSERT(hrb->read(4) == 5);
            TS_ASSERT(hrb->read(5) == 6);
            TS_ASSERT(hrb->read(6) == 7);
            TS_ASSERT(hrb->read(7) == 8);

            hrb = OctetArrayReadBuffer::create(hab, 9);
            TS_ASSERT(hrb->length() == 0);

            hrb = OctetArrayReadBuffer::create(hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);
            TS_ASSERT(hrb->read(0) == 0);
            TS_ASSERT(hrb->read(1) == 1);
            TS_ASSERT(hrb->read(2) == 2);
            TS_ASSERT(hrb->read(3) == 3);
            TS_ASSERT(hrb->read(4) == 4);
            TS_ASSERT(hrb->read(5) == 5);
            TS_ASSERT(hrb->read(6) == 6);
            TS_ASSERT(hrb->read(7) == 7);
            TS_ASSERT(hrb->read(8) == 8);

            hrb = OctetArrayReadBuffer::create(hab, 8, 0);
            TS_ASSERT(hrb->length() == 0);

            TS_ASSERT_THROWS(hrb = OctetArrayReadBuffer::create(hab, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hrb = OctetArrayReadBuffer::create(hab, 10, 1), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hrb = OctetArrayReadBuffer::create(hab, 1, 9), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hrb = OctetArrayReadBuffer::create(hab, 0, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(hrb = OctetArrayReadBuffer::create((Array<octet_t>::View) NULL), NullPointerException::View);
            }

        /**
        * Test the length() method.
        */
        void testLength()
            {
            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    Array<octet_t>::create(0), 0, 0);
            TS_ASSERT(hrb->length() == 0);
            }

        /**
        * Test the read(size32_t of) method.
        */
        void testReadOctet()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            TS_ASSERT(hrb->read(0) == 0);
            TS_ASSERT(hrb->read(1) == 1);
            TS_ASSERT(hrb->read(2) == 2);
            TS_ASSERT(hrb->read(3) == 3);
            TS_ASSERT(hrb->read(4) == 4);
            TS_ASSERT(hrb->read(5) == 5);
            TS_ASSERT(hrb->read(6) == 6);
            TS_ASSERT(hrb->read(7) == 7);
            TS_ASSERT(hrb->read(8) == 8);
            }

        /**
        * Test the read(size32_t ofBegin, size32_t ofEnd, Array<octet_t>::Handle
        * habDest, size32_t ofDest) method.
        */
        void testReadOctetArray()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            Array<octet_t>::Handle habDest = Array<octet_t>::create(6);
            hrb->read(1, 7, habDest, 0);

            TS_ASSERT(habDest[0] == 1);
            TS_ASSERT(habDest[1] == 2);
            TS_ASSERT(habDest[2] == 3);
            TS_ASSERT(habDest[3] == 4);
            TS_ASSERT(habDest[4] == 5);
            TS_ASSERT(habDest[5] == 6);

            habDest = Array<octet_t>::create(6);
            hrb->read(2, 6, habDest, 2);

            TS_ASSERT(habDest[2] == 2);
            TS_ASSERT(habDest[3] == 3);
            TS_ASSERT(habDest[4] == 4);
            TS_ASSERT(habDest[5] == 5);
            }

        /**
        * Test the getReadBuffer(size32_t of, size32_t cb) method.
        */
        void testGetReadBuffer()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::View vrb = hrb->getReadBuffer(1, 7);
            TS_ASSERT(vrb->length() == 7);

            TS_ASSERT(vrb->read(0) == 1);
            TS_ASSERT(vrb->read(1) == 2);
            TS_ASSERT(vrb->read(2) == 3);
            TS_ASSERT(vrb->read(3) == 4);
            TS_ASSERT(vrb->read(4) == 5);
            TS_ASSERT(vrb->read(5) == 6);
            TS_ASSERT(vrb->read(6) == 7);
            }

        /**
        * Test the toBinary() method.
        */
        void testToBinary()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            Binary::View vBin = hrb->toBinary();
            TS_ASSERT(vBin->length() == 9);

            TS_ASSERT(vBin->read(0) == 0);
            TS_ASSERT(vBin->read(1) == 1);
            TS_ASSERT(vBin->read(2) == 2);
            TS_ASSERT(vBin->read(3) == 3);
            TS_ASSERT(vBin->read(4) == 4);
            TS_ASSERT(vBin->read(5) == 5);
            TS_ASSERT(vBin->read(6) == 6);
            TS_ASSERT(vBin->read(7) == 7);
            TS_ASSERT(vBin->read(8) == 8);
            }

        /**
        * Test the toBinary(size32_t of, size32_t cb) method.
        */
        void testToBinaryLimited()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            Binary::View vBin = hrb->toBinary(1, 7);
            TS_ASSERT(vBin->length() == 7);

            TS_ASSERT(vBin->read(0) == 1);
            TS_ASSERT(vBin->read(1) == 2);
            TS_ASSERT(vBin->read(2) == 3);
            TS_ASSERT(vBin->read(3) == 4);
            TS_ASSERT(vBin->read(4) == 5);
            TS_ASSERT(vBin->read(5) == 6);
            TS_ASSERT(vBin->read(6) == 7);
            }

        /**
        * Test the toOctetArray() method.
        */
        void testToOctetArray()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            Array<octet_t>::View vab = hrb->toOctetArray();
            TS_ASSERT(vab->length == 9);

            TS_ASSERT(vab[0] == 0);
            TS_ASSERT(vab[1] == 1);
            TS_ASSERT(vab[2] == 2);
            TS_ASSERT(vab[3] == 3);
            TS_ASSERT(vab[4] == 4);
            TS_ASSERT(vab[5] == 5);
            TS_ASSERT(vab[6] == 6);
            TS_ASSERT(vab[7] == 7);
            TS_ASSERT(vab[8] == 8);
            }

        /**
        * Test the toOctetArray(size32_t of, size32_t cb) method.
        */
        void testToOctetArrayLimited()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            Array<octet_t>::View vab = hrb->toOctetArray(1, 7);
            TS_ASSERT(vab->length == 7);

            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            }

        /**
        * Test the equals() method.
        */
        void testEquals()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 0;
            hab[5] = 1;
            hab[6] = 2;
            hab[7] = 3;
            hab[8] = 4;

            OctetArrayReadBuffer::Handle hrb  = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            OctetArrayReadBuffer::Handle hrb2 = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->equals(hrb2));

            hrb2 = hrb;
            TS_ASSERT(hrb->equals(hrb2));

            hrb  = OctetArrayReadBuffer::create(hab, 0, 4);
            hrb2 = OctetArrayReadBuffer::create(hab, 4, 4);
            TS_ASSERT(hrb->equals(hrb2));

            hrb2 = OctetArrayReadBuffer::create(hab, 1, 4);
            TS_ASSERT(!hrb->equals(hrb2));
            }

        /**
        * Test the getBufferInput() method.
        */
        void testGetBufferInput()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);
            }

        /**
        * Test the BufferInput::setOffset(size32_t of) method.
        */
        void testBufferInputSetOffset()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            hbi->setOffset(1);
            TS_ASSERT(hbi->getOffset() == 1);
            TS_ASSERT(hbi->available() == 8);
            }

        /**
        * Test the BufferInput::skip(size32_t cb) method.
        */
        void testBufferInputSkipOctets()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            hbi->skip(0);
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->available() == 9);

            hbi->skip(1);
            TS_ASSERT(hbi->getOffset() == 1);
            TS_ASSERT(hbi->available() == 8);

            TS_ASSERT(hbi->read() == 1);
            TS_ASSERT(hbi->getOffset() == 2);
            TS_ASSERT(hbi->available() == 7);

            hbi->skip(2);
            TS_ASSERT(hbi->getOffset() == 4);
            TS_ASSERT(hbi->available() == 5);

            TS_ASSERT(hbi->read() == 4);
            TS_ASSERT(hbi->getOffset() == 5);
            TS_ASSERT(hbi->available() == 4);

            hbi->skip(3);
            TS_ASSERT(hbi->getOffset() == 8);
            TS_ASSERT(hbi->available() == 1);

            TS_ASSERT(hbi->read() == 8);
            TS_ASSERT(hbi->getOffset() == 9);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::read() method.
        */
        void testBufferInputReadOctet()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            TS_ASSERT(hbi->read() == 0);
            TS_ASSERT(hbi->getOffset() == 1);
            TS_ASSERT(hbi->available() == 8);

            TS_ASSERT(hbi->read() == 1);
            TS_ASSERT(hbi->getOffset() == 2);
            TS_ASSERT(hbi->available() == 7);

            TS_ASSERT(hbi->read() == 2);
            TS_ASSERT(hbi->getOffset() == 3);
            TS_ASSERT(hbi->available() == 6);

            TS_ASSERT(hbi->read() == 3);
            TS_ASSERT(hbi->getOffset() == 4);
            TS_ASSERT(hbi->available() == 5);

            TS_ASSERT(hbi->read() == 4);
            TS_ASSERT(hbi->getOffset() == 5);
            TS_ASSERT(hbi->available() == 4);

            TS_ASSERT(hbi->read() == 5);
            TS_ASSERT(hbi->getOffset() == 6);
            TS_ASSERT(hbi->available() == 3);

            TS_ASSERT(hbi->read() == 6);
            TS_ASSERT(hbi->getOffset() == 7);
            TS_ASSERT(hbi->available() == 2);

            TS_ASSERT(hbi->read() == 7);
            TS_ASSERT(hbi->getOffset() == 8);
            TS_ASSERT(hbi->available() == 1);

            TS_ASSERT(hbi->read() == 8);
            TS_ASSERT(hbi->getOffset() == 9);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::read(Array<octet_t>::Handle hab) method.
        */
        void testBufferInputReadOctetArray()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            Array<octet_t>::Handle habDest = Array<octet_t>::create(9);
            hbi->read(habDest);

            TS_ASSERT(hbi->getOffset() == 9);
            TS_ASSERT(hbi->available() == 0);

            TS_ASSERT(habDest[0] == 0);
            TS_ASSERT(habDest[1] == 1);
            TS_ASSERT(habDest[2] == 2);
            TS_ASSERT(habDest[3] == 3);
            TS_ASSERT(habDest[4] == 4);
            TS_ASSERT(habDest[5] == 5);
            TS_ASSERT(habDest[6] == 6);
            TS_ASSERT(habDest[7] == 7);
            TS_ASSERT(habDest[8] == 8);
            }

        /**
        * Test the BufferInput::read(Array<octet_t>::Handle hab, size32_t of,
        * size32_t cb) method.
        */
        void testBufferInputReadOctetArrayLimited()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            Array<octet_t>::Handle habDest = Array<octet_t>::create(8);
            hbi->read(habDest, 1, 7);

            TS_ASSERT(hbi->getOffset() == 7);
            TS_ASSERT(hbi->available() == 2);

            TS_ASSERT(habDest[1] == 0);
            TS_ASSERT(habDest[2] == 1);
            TS_ASSERT(habDest[3] == 2);
            TS_ASSERT(habDest[4] == 3);
            TS_ASSERT(habDest[5] == 4);
            TS_ASSERT(habDest[6] == 5);
            TS_ASSERT(habDest[7] == 6);
            }

        /**
        * Test the BufferInput::readBuffer(size32_t cb) method.
        */
        void testBufferInputReadBuffer()
            {
            Array<octet_t>::Handle hab = Array<octet_t>::create(9);
            hab[0] = 0;
            hab[1] = 1;
            hab[2] = 2;
            hab[3] = 3;
            hab[4] = 4;
            hab[5] = 5;
            hab[6] = 6;
            hab[7] = 7;
            hab[8] = 8;

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    hab, 0, 9);
            TS_ASSERT(hrb->length() == 9);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->getOffset() == 0);
            TS_ASSERT(hbi->getBuffer() == hrb);
            TS_ASSERT(hbi->available() == 9);

            ReadBuffer::View vrb = hbi->readBuffer(8);
            TS_ASSERT(hbi->available() == 1);

            TS_ASSERT(vrb->length() == 8);
            TS_ASSERT(vrb->read(0) == 0);
            TS_ASSERT(vrb->read(1) == 1);
            TS_ASSERT(vrb->read(2) == 2);
            TS_ASSERT(vrb->read(3) == 3);
            TS_ASSERT(vrb->read(4) == 4);
            TS_ASSERT(vrb->read(5) == 5);
            TS_ASSERT(vrb->read(6) == 6);
            TS_ASSERT(vrb->read(7) == 7);
            }

        /**
        * Test the BufferInput::readBoolean() method.
        */
        void testBufferInputReadBoolean()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeBoolean(true);
            hbo->writeBoolean(false);
            hbo->writeBoolean(true);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 3);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 3);
            TS_ASSERT(hrb->length() == 3);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readBoolean() == true);
            TS_ASSERT(hbi->readBoolean() == false);
            TS_ASSERT(hbi->readBoolean() == true);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::readChar16() method.
        */
        void testBufferInputReadChar16()
            {
            OctetArrayWriteBuffer::Handle      hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeChar16('A');
            hbo->writeChar16(0x07FF);
            hbo->writeChar16(0x0800);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 6);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 6);
            TS_ASSERT(hrb->length() == 6);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readChar16() == 'A');
            TS_ASSERT(hbi->readChar16() == 0x07FF);
            TS_ASSERT(hbi->readChar16() == 0x0800);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::readString() method.
        */
        void testBufferInputReadString()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeString(NULL);
            hbo->writeString("");

            String::View vs;

            hbo->writeString(vs);

            vs = "This is a test";

            hbo->writeString(vs);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 18);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 18);
            TS_ASSERT(hrb->length() == 18);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readString() == NULL);
            TS_ASSERT(hbi->readString()->equals(""));
            TS_ASSERT(hbi->readString() == NULL);
            TS_ASSERT(hbi->readString()->equals(vs));
            TS_ASSERT(hbi->available() == 0);

            // 2 character 4 byte UTF-8 string - 0xf0938080, 0xf09f8ebf
            char four[9];
            // 0xf0938080
            four[0] = (char) 0xf0;
            four[1] = (char) 0x93;
            four[2] = (char) 0x80;
            four[3] = (char) 0x80;
            // 0xf09f8ebf
            four[4] = (char) 0xf0;
            four[5] = (char) 0x9f;
            four[6] = (char) 0x8e;
            four[7] = (char) 0xbf;
            four[8] =        '\0';
            std::string data(four);

            vs  = String::create(data);
            TS_ASSERT_EQUALS(size32_t(2), vs->length());
            hwb = OctetArrayWriteBuffer::create(10);
            hbo = hwb->getBufferOutput();
            hbo->writeString(vs);
            vab = hwb->toOctetArray();

            TS_ASSERT_EQUALS(size32_t(9), vab->length);
            hrb = OctetArrayReadBuffer::create(vab, 0, 9);
            TS_ASSERT_EQUALS(size32_t(9), hrb->length());
            hbi = hrb->getBufferInput();

            String::View vsOut = hbi->readString();
            TS_ASSERT(vsOut->equals(vs));
            std::string conv = vsOut;
            TS_ASSERT_EQUALS(data, conv);
            }

        /**
        * Test the BufferInput::readInt16() method.
        */
        void testBufferInputReadInt16()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt16(0);
            hbo->writeInt16(1);
            hbo->writeInt16(2);
            hbo->writeInt16(99);
            hbo->writeInt16(9999);
            hbo->writeInt16(-1);
            hbo->writeInt16(-2);
            hbo->writeInt16(-99);
            hbo->writeInt16(-9999);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 15);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 15);
            TS_ASSERT(hrb->length() == 15);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readInt16() == 0);
            TS_ASSERT(hbi->readInt16() == 1);
            TS_ASSERT(hbi->readInt16() == 2);
            TS_ASSERT(hbi->readInt16() == 99);
            TS_ASSERT(hbi->readInt16() == 9999);
            TS_ASSERT(hbi->readInt16() == -1);
            TS_ASSERT(hbi->readInt16() == -2);
            TS_ASSERT(hbi->readInt16() == -99);
            TS_ASSERT(hbi->readInt16() == -9999);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::readInt32() method.
        */
        void testBufferInputReadInt32()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt32(0);
            hbo->writeInt32(1);
            hbo->writeInt32(2);
            hbo->writeInt32(99);
            hbo->writeInt32(9999);
            hbo->writeInt32(-1);
            hbo->writeInt32(-2);
            hbo->writeInt32(-99);
            hbo->writeInt32(-9999);
            hbo->writeInt32(1111111111);
            hbo->writeInt32(-1111111111);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 25);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 25);
            TS_ASSERT(hrb->length() == 25);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readInt32() == 0);
            TS_ASSERT(hbi->readInt32() == 1);
            TS_ASSERT(hbi->readInt32() == 2);
            TS_ASSERT(hbi->readInt32() == 99);
            TS_ASSERT(hbi->readInt32() == 9999);
            TS_ASSERT(hbi->readInt32() == -1);
            TS_ASSERT(hbi->readInt32() == -2);
            TS_ASSERT(hbi->readInt32() == -99);
            TS_ASSERT(hbi->readInt32() == -9999);
            TS_ASSERT(hbi->readInt32() == 1111111111);
            TS_ASSERT(hbi->readInt32() == -1111111111);
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::readInt64() method.
        */
        void testBufferInputReadInt64()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt64(0);
            hbo->writeInt64(1);
            hbo->writeInt64(2);
            hbo->writeInt64(99);
            hbo->writeInt64(9999);
            hbo->writeInt64(-1);
            hbo->writeInt64(-2);
            hbo->writeInt64(-99);
            hbo->writeInt64(-9999);
            hbo->writeInt64(1111111111);
            hbo->writeInt64(-1111111111);
            hbo->writeInt64(((int64_t) 1) << 62);
            hbo->writeInt64(-(((int64_t) 1) << 62));

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 44);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 44);
            TS_ASSERT(hrb->length() == 44);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(hbi->readInt64() == 0);
            TS_ASSERT(hbi->readInt64() == 1);
            TS_ASSERT(hbi->readInt64() == 2);
            TS_ASSERT(hbi->readInt64() == 99);
            TS_ASSERT(hbi->readInt64() == 9999);
            TS_ASSERT(hbi->readInt64() == -1);
            TS_ASSERT(hbi->readInt64() == -2);
            TS_ASSERT(hbi->readInt64() == -99);
            TS_ASSERT(hbi->readInt64() == -9999);
            TS_ASSERT(hbi->readInt64() == 1111111111);
            TS_ASSERT(hbi->readInt64() == -1111111111);
            TS_ASSERT(hbi->readInt64() == ((int64_t) 1) << 62);
            TS_ASSERT(hbi->readInt64() == -(((int64_t) 1) << 62));
            TS_ASSERT(hbi->available() == 0);
            }

        /**
        * Test the BufferInput::readFloat32() method.
        */
        void testBufferInputReadFloat32()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeFloat32((float32_t) 0.0);
            hbo->writeFloat32((float32_t) -1.1);
            hbo->writeFloat32((float32_t) 1.1);
            hbo->writeFloat32(Float32::getNaN());
            hbo->writeFloat32(Float32::getPositiveInfinity());
            hbo->writeFloat32(Float32::getNegativeInfinity());

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 24);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 24);
            TS_ASSERT(hrb->length() == 24);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(Float32::isZero(hbi->readFloat32()));
            TS_ASSERT(Float32::compare(hbi->readFloat32(), (float32_t) -1.1) == 0);
            TS_ASSERT(Float32::compare(hbi->readFloat32(), (float32_t) 1.1) == 0);
            TS_ASSERT(Float32::create(Float32::getNaN())->equals(
                    Float32::create(hbi->readFloat32())));
            TS_ASSERT(Float32::compare(hbi->readFloat32(), Float32::getPositiveInfinity()) == 0);
            TS_ASSERT(Float32::compare(hbi->readFloat32(), Float32::getNegativeInfinity()) == 0);
            }

        /**
        * Test the BufferInput::readFloat64() method.
        */
        void testBufferInputReadFloat64()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeFloat64(0.0);
            hbo->writeFloat64(-1.1);
            hbo->writeFloat64(1.1);
            hbo->writeFloat64(Float64::getNaN());
            hbo->writeFloat64(Float64::getPositiveInfinity());
            hbo->writeFloat64(Float64::getNegativeInfinity());

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 48);

            OctetArrayReadBuffer::Handle hrb = OctetArrayReadBuffer::create(
                    vab, 0, 48);
            TS_ASSERT(hrb->length() == 48);

            ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
            TS_ASSERT(Float64::isZero(hbi->readFloat64()));
            TS_ASSERT(Float64::compare(hbi->readFloat64(), -1.1) == 0);
            TS_ASSERT(Float64::compare(hbi->readFloat64(), 1.1) == 0);
            TS_ASSERT(Float64::create(Float64::getNaN())->equals(
                    Float64::create(hbi->readFloat64())));
            TS_ASSERT(Float64::compare(hbi->readFloat64(), Float64::getPositiveInfinity()) == 0);
            TS_ASSERT(Float64::compare(hbi->readFloat64(), Float64::getNegativeInfinity()) == 0);
            }
    };

