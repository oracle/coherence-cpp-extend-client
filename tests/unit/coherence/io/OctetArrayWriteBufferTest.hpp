/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
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
* Test suite for the OctetArrayWriteBuffer class.
*/
class OctetArrayWriteBufferSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the length() method.
        */
        void testLength()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(1024);
            TS_ASSERT(hwb->length() == 0);
            }

        /**
        * Test the getCapacity() method.
        */
        void testGetCapacity()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(1024);
            TS_ASSERT(hwb->getCapacity() == 1024);
            }

        /**
        * Test the getMaximumCapacity() method.
        */
        void testGetMaximumCapacity()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(1024, 2048);
            TS_ASSERT(hwb->getMaximumCapacity() == 2048);
            }

        /**
        * Test the write(size32_t ofDest, octet_t b) method.
        */
        void testWriteOctet()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

            hwb->write(0, 0);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(1, 1);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(2, 2);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(3, 3);
            TS_ASSERT(hwb->length() == 4);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(4, 4);
            TS_ASSERT(hwb->length() == 5);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(5, 5);
            TS_ASSERT(hwb->length() == 6);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(6, 6);
            TS_ASSERT(hwb->length() == 7);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(7, 7);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 8);

            hwb->write(8, 8);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the write(size32_t ofDest, Array<octet_t>::View vabSrc) method.
        */
        void testWriteOctetArray()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the write(size32_t ofDest, Array<octet_t>::View vabSrc,
        * size32_t ofSrc, size32_t cbSrc)write(size32_t ofDest, Array<octet_t>::View
        * vabSrc) method.
        */
        void testWriteOctetArrayLimited()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

            Array<octet_t>::Handle hab = Array<octet_t>::create(11);
            hab[0]  = 0;
            hab[1]  = 1;
            hab[2]  = 2;
            hab[3]  = 3;
            hab[4]  = 4;
            hab[5]  = 5;
            hab[6]  = 6;
            hab[7]  = 7;
            hab[8]  = 8;
            hab[9]  = 9;
            hab[10] = 10;

            hwb->write(0, cast<Array<octet_t>::View>(hab),
                       1, 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            TS_ASSERT(vab[7] == 8);
            TS_ASSERT(vab[8] == 9);
            }

        /**
        * Test the write(size32_t ofDest, ReadBuffer::View vBufSrc) method.
        */
        void testWriteBuffer()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            ReadBuffer::View vrb = OctetArrayReadBuffer::create(hab, 0, 9);

            hwb->write(0, vrb);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the write(size32_t ofDest, ReadBuffer::View vBufSrc,
        * size32_t ofSrc, size32_t cbSrc) method.
        */
        void testWriteBufferLimited()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

            Array<octet_t>::Handle hab = Array<octet_t>::create(11);
            hab[0]  = 0;
            hab[1]  = 1;
            hab[2]  = 2;
            hab[3]  = 3;
            hab[4]  = 4;
            hab[5]  = 5;
            hab[6]  = 6;
            hab[7]  = 7;
            hab[8]  = 8;
            hab[9]  = 9;
            hab[10] = 10;

            ReadBuffer::View vrb = OctetArrayReadBuffer::create(hab, 0, 11);

            hwb->write(0, vrb, 1, 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            TS_ASSERT(vab[7] == 8);
            TS_ASSERT(vab[8] == 9);
            }

        /**
        * Test the retain(size32_t of) method.
        */
        void testRetain()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            hwb->retain(1);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 8);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            TS_ASSERT(vab[7] == 8);
            }

        /**
        * Test the retain(size32_t of, size32_t cb) method.
        */
        void testRetainLimited()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            hwb->retain(1, 4);
            TS_ASSERT(hwb->length() == 4);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 4);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            }

        /**
        * Test the clear() method.
        */
        void testClear()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            hwb->clear();
            TS_ASSERT(hwb->length() == 0);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 0);
            }

        /**
        * Test the getWriteBuffer(size32_t of) method.
        */
        void testGetWriteBuffer()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            WriteBuffer::Handle hwb2 = hwb->getWriteBuffer(2);
            TS_ASSERT(hwb2->length() == 0);
            TS_ASSERT(hwb2->getCapacity() == hwb->getCapacity() - 2);
            TS_ASSERT(hwb2->getMaximumCapacity() == hwb->getMaximumCapacity() - 2);

            hwb2->write(1, 99);
            TS_ASSERT(hwb2->length() == 2);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 0);
            TS_ASSERT(vab[1] == 1);
            TS_ASSERT(vab[2] == 2);
            TS_ASSERT(vab[3] == 99);
            TS_ASSERT(vab[4] == 4);
            TS_ASSERT(vab[5] == 5);
            TS_ASSERT(vab[6] == 6);
            TS_ASSERT(vab[7] == 7);
            TS_ASSERT(vab[8] == 8);
            }

        /**
        * Test the getWriteBuffer(size32_t of, size32_t cb) method.
        */
        void testGetWriteBufferLimited()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            WriteBuffer::Handle hwb2 = hwb->getWriteBuffer(2, 6);
            TS_ASSERT(hwb2->length() == 0);
            TS_ASSERT(hwb2->getCapacity() == 6);
            TS_ASSERT(hwb2->getMaximumCapacity() == 6);

            hwb2->write(1, 99);
            TS_ASSERT(hwb2->length() == 2);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 0);
            TS_ASSERT(vab[1] == 1);
            TS_ASSERT(vab[2] == 2);
            TS_ASSERT(vab[3] == 99);
            TS_ASSERT(vab[4] == 4);
            TS_ASSERT(vab[5] == 5);
            TS_ASSERT(vab[6] == 6);
            TS_ASSERT(vab[7] == 7);
            TS_ASSERT(vab[8] == 8);
            }

        /**
        * Test the getReadBuffer() method.
        */
        void testGetReadBuffer()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            ReadBuffer::View vrb = hwb->getReadBuffer();
            TS_ASSERT(vrb->length() == 9);
            TS_ASSERT(vrb->read(0) == 0);
            TS_ASSERT(vrb->read(1) == 1);
            TS_ASSERT(vrb->read(2) == 2);
            TS_ASSERT(vrb->read(3) == 3);
            TS_ASSERT(vrb->read(4) == 4);
            TS_ASSERT(vrb->read(5) == 5);
            TS_ASSERT(vrb->read(6) == 6);
            TS_ASSERT(vrb->read(7) == 7);
            TS_ASSERT(vrb->read(8) == 8);
            }

        /**
        * Test the getUnsafeReadBuffer() method.
        */
        void testGetUnsafeReadBuffer()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            ReadBuffer::View vrb = hwb->getUnsafeReadBuffer();
            TS_ASSERT(vrb->length() == 9);
            TS_ASSERT(vrb->read(0) == 0);
            TS_ASSERT(vrb->read(1) == 1);
            TS_ASSERT(vrb->read(2) == 2);
            TS_ASSERT(vrb->read(3) == 3);
            TS_ASSERT(vrb->read(4) == 4);
            TS_ASSERT(vrb->read(5) == 5);
            TS_ASSERT(vrb->read(6) == 6);
            TS_ASSERT(vrb->read(7) == 7);
            TS_ASSERT(vrb->read(8) == 8);
            }

        /**
        * Test the toOctetArray() method.
        */
        void testToOctetArray()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));

            Array<octet_t>::View vab = hwb->toOctetArray();
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

            hwb->write(0, 1);
            TS_ASSERT(vab[0] == 1);
            }

        /**
        * Test the toBinary() method.
        */
        void testToBinary()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));

            Binary::View vBin = hwb->toBinary();
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

            hwb->write(0, 1);
            // Binary object should be unchanged
            TS_ASSERT(vBin->read(0) == 0);
            }

        /**
        * Test the getBufferOutput(size32_t of = 0) method.
        */
        void testGetBufferOutput()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput(2);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hbo->getBuffer() == hwb);
            }

        /**
        * Test the getAppendingBufferOutput() method.
        */
        void testGetAppendingBufferOutput()
            {
            OctetArrayWriteBuffer::Handle hwb = OctetArrayWriteBuffer::create(8);

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

            hwb->write(0, cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            WriteBuffer::BufferOutput::Handle hbo = hwb->getAppendingBufferOutput();
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hbo->getBuffer() == hwb);
            }

        /**
        * Test the BufferOutput::setOffset(size32_t of) method.
        */
        void testBufferOutputSetOffset()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->setOffset(10);
            TS_ASSERT(hbo->getOffset() == 10);

            hbo->write(1);
            TS_ASSERT(hbo->getOffset() == 11);
            TS_ASSERT(hwb->length() == 11);
            TS_ASSERT(hwb->getCapacity() == 1035);
            }

        /**
        * Test the BufferOutput::write(octet_t b) method.
        */
        void testBufferOutputWriteOctet()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->write(0);
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(1);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(2);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(3);
            TS_ASSERT(hbo->getOffset() == 4);
            TS_ASSERT(hwb->length() == 4);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(4);
            TS_ASSERT(hbo->getOffset() == 5);
            TS_ASSERT(hwb->length() == 5);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(5);
            TS_ASSERT(hbo->getOffset() == 6);
            TS_ASSERT(hwb->length() == 6);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(6);
            TS_ASSERT(hbo->getOffset() == 7);
            TS_ASSERT(hwb->length() == 7);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(7);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->write(8);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the BufferOutput::write(Array<octet_t>::View vab) method.
        */
        void testBufferOutputWriteOctetArray()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

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

            hbo->write(cast<Array<octet_t>::View>(hab));
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the BufferOutput::write(Array<octet_t>::View vab, size32_t of,
        * size32_t cb) method.
        */
        void testBufferOutputWriteOctetArrayLimited()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            Array<octet_t>::Handle hab = Array<octet_t>::create(11);
            hab[0]  = 0;
            hab[1]  = 1;
            hab[2]  = 2;
            hab[3]  = 3;
            hab[4]  = 4;
            hab[5]  = 5;
            hab[6]  = 6;
            hab[7]  = 7;
            hab[8]  = 8;
            hab[9]  = 9;
            hab[10] = 10;

            hbo->write(cast<Array<octet_t>::View>(hab), 1, 9);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            TS_ASSERT(vab[7] == 8);
            TS_ASSERT(vab[8] == 9);
            }

        /**
        * Test the BufferOutput::writeBuffer(ReadBuffer::View vBuf) method.
        */
        void testBufferOutputWriteBuffer()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

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

            ReadBuffer::View vrb = OctetArrayReadBuffer::create(hab, 0, 9);

            hbo->writeBuffer(vrb);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
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
        * Test the BufferOutput::writeBuffer(ReadBuffer::View vBuf,
        * size32_t of, size32_t cb) method.
        */
        void testBufferOutputWriteBufferLimited()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            Array<octet_t>::Handle hab = Array<octet_t>::create(11);
            hab[0]  = 0;
            hab[1]  = 1;
            hab[2]  = 2;
            hab[3]  = 3;
            hab[4]  = 4;
            hab[5]  = 5;
            hab[6]  = 6;
            hab[7]  = 7;
            hab[8]  = 8;
            hab[9]  = 9;
            hab[10] = 10;

            ReadBuffer::View vrb = OctetArrayReadBuffer::create(hab, 0, 11);

            hbo->writeBuffer(vrb, 1, 9);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1033);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 9);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 2);
            TS_ASSERT(vab[2] == 3);
            TS_ASSERT(vab[3] == 4);
            TS_ASSERT(vab[4] == 5);
            TS_ASSERT(vab[5] == 6);
            TS_ASSERT(vab[6] == 7);
            TS_ASSERT(vab[7] == 8);
            TS_ASSERT(vab[8] == 9);
            }

        /**
        * Test the BufferOutput::writeBoolean(bool f) method.
        */
        void testBufferOutputWriteBoolean()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeBoolean(true);
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeBoolean(false);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeBoolean(true);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 3);
            TS_ASSERT(vab[0] == 1);
            TS_ASSERT(vab[1] == 0);
            TS_ASSERT(vab[2] == 1);
            }

        /**
        * Test the BufferOutput::writeChar16(wchar16_t ch) method.
        */
        void testBufferOutputWriteChar16()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeChar16('A');
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeChar16(0x07FF);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeChar16(0x0800);
            TS_ASSERT(hbo->getOffset() == 6);
            TS_ASSERT(hwb->length() == 6);
            TS_ASSERT(hwb->getCapacity() == 8);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 6);
            TS_ASSERT(vab[0] == 'A');
            TS_ASSERT(vab[1] == 0xDF);
            TS_ASSERT(vab[2] == 0xBF);
            TS_ASSERT(vab[3] == 0xE0);
            TS_ASSERT(vab[4] == 0xA0);
            TS_ASSERT(vab[5] == 0x80);
            }

        /**
        * Test the BufferOutput::writeString(String::View vs) method.
        */
        void testBufferOutputWriteString()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeString(NULL);

            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeString("");
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 1034);

            String::View vs;

            hbo->writeString(vs);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 1034);

            vs = "This is a test";

            hbo->writeString(vs);
            TS_ASSERT(hbo->getOffset() == 18);
            TS_ASSERT(hwb->length() == 18);
            TS_ASSERT(hwb->getCapacity() == 1034);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 18);
            TS_ASSERT(vab[0]  == 0x40);
            TS_ASSERT(vab[1]  == 0x00);
            TS_ASSERT(vab[2]  == 0x40);
            TS_ASSERT(vab[3]  == 0x0E);
            TS_ASSERT(vab[4]  == 0x54); // T
            TS_ASSERT(vab[5]  == 0x68); // h
            TS_ASSERT(vab[6]  == 0x69); // i
            TS_ASSERT(vab[7]  == 0x73); // s
            TS_ASSERT(vab[8]  == 0x20); //
            TS_ASSERT(vab[9]  == 0x69); // i
            TS_ASSERT(vab[10] == 0x73); // s
            TS_ASSERT(vab[11] == 0x20); //
            TS_ASSERT(vab[12] == 0x61); // a
            TS_ASSERT(vab[13] == 0x20); //
            TS_ASSERT(vab[14] == 0x74); // t
            TS_ASSERT(vab[15] == 0x65); // e
            TS_ASSERT(vab[16] == 0x73); // s
            TS_ASSERT(vab[17] == 0x74); // t
            }

        /**
        * Test the BufferOutput::writeInt16(int16_t n) method.
        */
        void testBufferOutputWriteInt16()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt16(0);
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt16(1);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt16(2);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt16(99);
            TS_ASSERT(hbo->getOffset() == 5);
            TS_ASSERT(hwb->length() == 5);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt16(9999);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt16(-1);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1035);

            hbo->writeInt16(-2);
            TS_ASSERT(hbo->getOffset() == 10);
            TS_ASSERT(hwb->length() == 10);
            TS_ASSERT(hwb->getCapacity() == 1035);

            hbo->writeInt16(-99);
            TS_ASSERT(hbo->getOffset() == 12);
            TS_ASSERT(hwb->length() == 12);
            TS_ASSERT(hwb->getCapacity() == 1035);

            hbo->writeInt16(-9999);
            TS_ASSERT(hbo->getOffset() == 15);
            TS_ASSERT(hwb->length() == 15);
            TS_ASSERT(hwb->getCapacity() == 1035);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 15);
            TS_ASSERT(vab[0]  == 0x00);
            TS_ASSERT(vab[1]  == 0x01);
            TS_ASSERT(vab[2]  == 0x02);
            TS_ASSERT(vab[3]  == 0xA3);
            TS_ASSERT(vab[4]  == 0x01);
            TS_ASSERT(vab[5]  == 0x8F);
            TS_ASSERT(vab[6]  == 0x9C);
            TS_ASSERT(vab[7]  == 0x01);
            TS_ASSERT(vab[8]  == 0x40);
            TS_ASSERT(vab[9]  == 0x41);
            TS_ASSERT(vab[10] == 0xE2);
            TS_ASSERT(vab[11] == 0x01);
            TS_ASSERT(vab[12] == 0xCE);
            TS_ASSERT(vab[13] == 0x9C);
            TS_ASSERT(vab[14] == 0x01);
            }

        /**
        * Test the BufferOutput::writeInt32(int32_t n) method.
        */
        void testBufferOutputWriteInt32()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt32(0);
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt32(1);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt32(2);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt32(99);
            TS_ASSERT(hbo->getOffset() == 5);
            TS_ASSERT(hwb->length() == 5);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeInt32(9999);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(-1);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(-2);
            TS_ASSERT(hbo->getOffset() == 10);
            TS_ASSERT(hwb->length() == 10);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(-99);
            TS_ASSERT(hbo->getOffset() == 12);
            TS_ASSERT(hwb->length() == 12);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(-9999);
            TS_ASSERT(hbo->getOffset() == 15);
            TS_ASSERT(hwb->length() == 15);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(1111111111);
            TS_ASSERT(hbo->getOffset() == 20);
            TS_ASSERT(hwb->length() == 20);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt32(-1111111111);
            TS_ASSERT(hbo->getOffset() == 25);
            TS_ASSERT(hwb->length() == 25);
            TS_ASSERT(hwb->getCapacity() == 1034);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 25);
            TS_ASSERT(vab[0]  == 0x00);
            TS_ASSERT(vab[1]  == 0x01);
            TS_ASSERT(vab[2]  == 0x02);
            TS_ASSERT(vab[3]  == 0xA3);
            TS_ASSERT(vab[4]  == 0x01);
            TS_ASSERT(vab[5]  == 0x8F);
            TS_ASSERT(vab[6]  == 0x9C);
            TS_ASSERT(vab[7]  == 0x01);
            TS_ASSERT(vab[8]  == 0x40);
            TS_ASSERT(vab[9]  == 0x41);
            TS_ASSERT(vab[10] == 0xE2);
            TS_ASSERT(vab[11] == 0x01);
            TS_ASSERT(vab[12] == 0xCE);
            TS_ASSERT(vab[13] == 0x9C);
            TS_ASSERT(vab[14] == 0x01);
            TS_ASSERT(vab[15] == 0x87);
            TS_ASSERT(vab[16] == 0xD7);
            TS_ASSERT(vab[17] == 0xD1);
            TS_ASSERT(vab[18] == 0xA3);
            TS_ASSERT(vab[19] == 0x08);
            TS_ASSERT(vab[20] == 0xC6);
            TS_ASSERT(vab[21] == 0xD7);
            TS_ASSERT(vab[22] == 0xD1);
            TS_ASSERT(vab[23] == 0xA3);
            TS_ASSERT(vab[24] == 0x08);
            }

        /**
        * Test the BufferOutput::writeInt64(int64_t n) method.
        */
        void testBufferOutputWriteInt64()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeInt64(0);
            TS_ASSERT(hbo->getOffset() == 1);
            TS_ASSERT(hwb->length() == 1);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(1);
            TS_ASSERT(hbo->getOffset() == 2);
            TS_ASSERT(hwb->length() == 2);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(2);
            TS_ASSERT(hbo->getOffset() == 3);
            TS_ASSERT(hwb->length() == 3);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(99);
            TS_ASSERT(hbo->getOffset() == 5);
            TS_ASSERT(hwb->length() == 5);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(9999);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-1);
            TS_ASSERT(hbo->getOffset() == 9);
            TS_ASSERT(hwb->length() == 9);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-2);
            TS_ASSERT(hbo->getOffset() == 10);
            TS_ASSERT(hwb->length() == 10);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-99);
            TS_ASSERT(hbo->getOffset() == 12);
            TS_ASSERT(hwb->length() == 12);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-9999);
            TS_ASSERT(hbo->getOffset() == 15);
            TS_ASSERT(hwb->length() == 15);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(1111111111);
            TS_ASSERT(hbo->getOffset() == 20);
            TS_ASSERT(hwb->length() == 20);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-1111111111);
            TS_ASSERT(hbo->getOffset() == 25);
            TS_ASSERT(hwb->length() == 25);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(((int64_t) 1) << 62);
            TS_ASSERT(hbo->getOffset() == 35);
            TS_ASSERT(hwb->length() == 35);
            TS_ASSERT(hwb->getCapacity() == 1034);

            hbo->writeInt64(-((int64_t) (1) << 62));
            TS_ASSERT(hbo->getOffset() == 44);
            TS_ASSERT(hwb->length() == 44);
            TS_ASSERT(hwb->getCapacity() == 1034);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 44);
            TS_ASSERT(vab[0]  == 0x00);
            TS_ASSERT(vab[1]  == 0x01);
            TS_ASSERT(vab[2]  == 0x02);
            TS_ASSERT(vab[3]  == 0xA3);
            TS_ASSERT(vab[4]  == 0x01);
            TS_ASSERT(vab[5]  == 0x8F);
            TS_ASSERT(vab[6]  == 0x9C);
            TS_ASSERT(vab[7]  == 0x01);
            TS_ASSERT(vab[8]  == 0x40);
            TS_ASSERT(vab[9]  == 0x41);
            TS_ASSERT(vab[10] == 0xE2);
            TS_ASSERT(vab[11] == 0x01);
            TS_ASSERT(vab[12] == 0xCE);
            TS_ASSERT(vab[13] == 0x9C);
            TS_ASSERT(vab[14] == 0x01);
            TS_ASSERT(vab[15] == 0x87);
            TS_ASSERT(vab[16] == 0xD7);
            TS_ASSERT(vab[17] == 0xD1);
            TS_ASSERT(vab[18] == 0xA3);
            TS_ASSERT(vab[19] == 0x08);
            TS_ASSERT(vab[20] == 0xC6);
            TS_ASSERT(vab[21] == 0xD7);
            TS_ASSERT(vab[22] == 0xD1);
            TS_ASSERT(vab[23] == 0xA3);
            TS_ASSERT(vab[24] == 0x08);
            TS_ASSERT(vab[25] == 0x80);
            TS_ASSERT(vab[26] == 0x80);
            TS_ASSERT(vab[27] == 0x80);
            TS_ASSERT(vab[28] == 0x80);
            TS_ASSERT(vab[29] == 0x80);
            TS_ASSERT(vab[30] == 0x80);
            TS_ASSERT(vab[31] == 0x80);
            TS_ASSERT(vab[32] == 0x80);
            TS_ASSERT(vab[33] == 0x80);
            TS_ASSERT(vab[34] == 0x01);
            TS_ASSERT(vab[35] == 0xFF);
            TS_ASSERT(vab[36] == 0xFF);
            TS_ASSERT(vab[37] == 0xFF);
            TS_ASSERT(vab[38] == 0xFF);
            TS_ASSERT(vab[39] == 0xFF);
            TS_ASSERT(vab[40] == 0xFF);
            TS_ASSERT(vab[41] == 0xFF);
            TS_ASSERT(vab[42] == 0xFF);
            TS_ASSERT(vab[43] == 0x7F);
            }

        /**
        * Test the BufferOutput::writeFloat32(float32_t fl) method.
        */
        void testBufferOutputWriteFloat32()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeFloat32((float32_t) 0.0);
            TS_ASSERT(hbo->getOffset() == 4);
            TS_ASSERT(hwb->length() == 4);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeFloat32((float32_t) -1.1);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeFloat32((float32_t) 1.1);
            TS_ASSERT(hbo->getOffset() == 12);
            TS_ASSERT(hwb->length() == 12);
            TS_ASSERT(hwb->getCapacity() == 1036);

            hbo->writeFloat32(Float32::getNaN());
            TS_ASSERT(hbo->getOffset() == 16);
            TS_ASSERT(hwb->length() == 16);
            TS_ASSERT(hwb->getCapacity() == 1036);

            hbo->writeFloat32(Float32::getPositiveInfinity());
            TS_ASSERT(hbo->getOffset() == 20);
            TS_ASSERT(hwb->length() == 20);
            TS_ASSERT(hwb->getCapacity() == 1036);

            hbo->writeFloat32(Float32::getNegativeInfinity());
            TS_ASSERT(hbo->getOffset() == 24);
            TS_ASSERT(hwb->length() == 24);
            TS_ASSERT(hwb->getCapacity() == 1036);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 24);
            TS_ASSERT(vab[0]  == 0x00);
            TS_ASSERT(vab[1]  == 0x00);
            TS_ASSERT(vab[2]  == 0x00);
            TS_ASSERT(vab[3]  == 0x00);
            TS_ASSERT(vab[4]  == 0xBF);
            TS_ASSERT(vab[5]  == 0x8C);
            TS_ASSERT(vab[6]  == 0xCC);
            TS_ASSERT(vab[7]  == 0xCD);
            TS_ASSERT(vab[8]  == 0x3F);
            TS_ASSERT(vab[9]  == 0x8C);
            TS_ASSERT(vab[10] == 0xCC);
            TS_ASSERT(vab[11] == 0xCD);
            TS_ASSERT(vab[12] == 0x7F);
            TS_ASSERT(vab[13] == 0xC0);
            TS_ASSERT(vab[14] == 0x00);
            TS_ASSERT(vab[15] == 0x00);
            TS_ASSERT(vab[16] == 0x7F);
            TS_ASSERT(vab[17] == 0x80);
            TS_ASSERT(vab[18] == 0x00);
            TS_ASSERT(vab[19] == 0x00);
            TS_ASSERT(vab[20] == 0xFF);
            TS_ASSERT(vab[21] == 0x80);
            TS_ASSERT(vab[22] == 0x00);
            TS_ASSERT(vab[23] == 0x00);
            }

        /**
        * Test the BufferOutput::writeDouble(float64_t dfl) method.
        */
        void testBufferOutputWriteFloat64()
            {
            OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(8);
            WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();

            hbo->writeFloat64(0.0);
            TS_ASSERT(hbo->getOffset() == 8);
            TS_ASSERT(hwb->length() == 8);
            TS_ASSERT(hwb->getCapacity() == 8);

            hbo->writeFloat64(-1.1);
            TS_ASSERT(hbo->getOffset() == 16);
            TS_ASSERT(hwb->length() == 16);
            TS_ASSERT(hwb->getCapacity() == 1040);

            hbo->writeFloat64(1.1);
            TS_ASSERT(hbo->getOffset() == 24);
            TS_ASSERT(hwb->length() == 24);
            TS_ASSERT(hwb->getCapacity() == 1040);

            hbo->writeFloat64(Float64::getNaN());
            TS_ASSERT(hbo->getOffset() == 32);
            TS_ASSERT(hwb->length() == 32);
            TS_ASSERT(hwb->getCapacity() == 1040);

            hbo->writeFloat64(Float64::getPositiveInfinity());
            TS_ASSERT(hbo->getOffset() == 40);
            TS_ASSERT(hwb->length() == 40);
            TS_ASSERT(hwb->getCapacity() == 1040);

            hbo->writeFloat64(Float64::getNegativeInfinity());
            TS_ASSERT(hbo->getOffset() == 48);
            TS_ASSERT(hwb->length() == 48);
            TS_ASSERT(hwb->getCapacity() == 1040);

            Array<octet_t>::View vab = hwb->toOctetArray();
            TS_ASSERT(vab->length == 48);
            TS_ASSERT(vab[0]  == 0x00);
            TS_ASSERT(vab[1]  == 0x00);
            TS_ASSERT(vab[2]  == 0x00);
            TS_ASSERT(vab[3]  == 0x00);
            TS_ASSERT(vab[4]  == 0x00);
            TS_ASSERT(vab[5]  == 0x00);
            TS_ASSERT(vab[6]  == 0x00);
            TS_ASSERT(vab[7]  == 0x00);
            TS_ASSERT(vab[8]  == 0xBF);
            TS_ASSERT(vab[9]  == 0xF1);
            TS_ASSERT(vab[10] == 0x99);
            TS_ASSERT(vab[11] == 0x99);
            TS_ASSERT(vab[12] == 0x99);
            TS_ASSERT(vab[13] == 0x99);
            TS_ASSERT(vab[14] == 0x99);
            TS_ASSERT(vab[15] == 0x9A);
            TS_ASSERT(vab[16] == 0x3F);
            TS_ASSERT(vab[17] == 0xF1);
            TS_ASSERT(vab[18] == 0x99);
            TS_ASSERT(vab[19] == 0x99);
            TS_ASSERT(vab[20] == 0x99);
            TS_ASSERT(vab[21] == 0x99);
            TS_ASSERT(vab[22] == 0x99);
            TS_ASSERT(vab[23] == 0x9A);
            TS_ASSERT(vab[24] == 0x7F);
            TS_ASSERT(vab[25] == 0xF8);
            TS_ASSERT(vab[26] == 0x00);
            TS_ASSERT(vab[27] == 0x00);
            TS_ASSERT(vab[28] == 0x00);
            TS_ASSERT(vab[29] == 0x00);
            TS_ASSERT(vab[30] == 0x00);
            TS_ASSERT(vab[31] == 0x00);
            TS_ASSERT(vab[32] == 0x7F);
            TS_ASSERT(vab[33] == 0xF0);
            TS_ASSERT(vab[34] == 0x00);
            TS_ASSERT(vab[35] == 0x00);
            TS_ASSERT(vab[36] == 0x00);
            TS_ASSERT(vab[37] == 0x00);
            TS_ASSERT(vab[38] == 0x00);
            TS_ASSERT(vab[39] == 0x00);
            TS_ASSERT(vab[40] == 0xFF);
            TS_ASSERT(vab[41] == 0xF0);
            TS_ASSERT(vab[42] == 0x00);
            TS_ASSERT(vab[43] == 0x00);
            TS_ASSERT(vab[44] == 0x00);
            TS_ASSERT(vab[45] == 0x00);
            TS_ASSERT(vab[46] == 0x00);
            TS_ASSERT(vab[47] == 0x00);
            }
    };

