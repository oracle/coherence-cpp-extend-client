/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDateTime.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SparseArray.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/net/LocalMember.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::PofHelper;
using coherence::io::pof::RawDate;
using coherence::io::pof::RawDateTime;
using coherence::io::pof::RawDayTimeInterval;
using coherence::io::pof::RawTime;
using coherence::io::pof::RawTimeInterval;
using coherence::io::pof::RawYearMonthInterval;
using coherence::io::pof::SystemPofContext;
using coherence::net::LocalMember;
using coherence::util::ArrayList;
using coherence::util::Binary;
using coherence::util::Collection;
using coherence::util::HashMap;
using coherence::util::LongArray;
using coherence::util::Map;
using coherence::util::SparseArray;
using coherence::util::UUID;


/**
* Test suite for the Event class.
*/
class PofBufferWriterTestSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the PofBufferWriter::writeBoolean() method.
        */
        void testWriteBoolean()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test true
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeBoolean(-1, true);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readBoolean(-1) == true);

            // test false
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeBoolean(-1, false);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readBoolean(-1) == false);
            }

        /**
        * Test the PofBufferWriter::writeOctet() method.
        */
        void testWriteOctet()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctet(-1, 0x02);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readOctet(-1) == 0x02);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctet(-1, 0x70);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readOctet(-1) == 0x70);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctet(-1, 0x77);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readOctet(-1) == 0x77);
            }

        /**
        * Test the PofBufferWriter::writeChar16() method.
        */
        void testWriteChar16()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test the 1-byte format
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16(-1, 0x007F);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readChar16(-1) == 0x007F);

            // test the 2-byte format
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16(-1, 0x07FF);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readChar16(-1) == 0x07FF);

            // test the 3-byte format
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16(-1, 0x0800);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readChar16(-1) == 0x0800);
            }

        /**
        * Test the PofBufferWriter::writeInt16() method.
        */
        void testWriteInt16()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a "compact" integer value
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16(-1, 1);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt16(-1) == 1);

            // test a negative integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16(-1, -32767);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt16(-1) == -32767);

            // test a positive integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16(-1, 32767);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt16(-1) == 32767);
            }

        /**
        * Test the PofBufferWriter::writeInt32() method.
        */
        void testWriteInt32()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a "compact" integer value
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32(-1, 1);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt32(-1) == 1);

            // test a negative integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32(-1, -2147483647);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt32(-1) == -2147483647);

            // test a positive integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32(-1, 2147483647);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt32(-1) == 2147483647);
            }

        /**
        * Test the PofBufferWriter::writeInt64() method.
        */
        void testWriteInt64()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a "compact" integer value
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64(-1, 1);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt64(-1) == 1);

            int64_t l = 2147483647;
            l += l;

            // test a negative integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64(-1, -l);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt64(-1) == -l);

            // test a positive integer value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64(-1, l);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt64(-1) == l);
            }

        /**
        * Test the PofBufferWriter::writeFloat32() method.
        */
        void testWriteFloat32()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test 0.0
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, (float32_t) 0.0);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::isZero(hReader->readFloat32(-1)));

            // test a negative floating point value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, (float32_t) -1.1);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), (float32_t) -1.1) == 0);

            // test a positive floating point value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, (float32_t) 1.1);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), (float32_t) 1.1) == 0);

            // test NaN
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, Float32::getNaN());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::create(hReader->readFloat32(-1))->equals(
                    Float32::create(Float32::getNaN())));

            // test negative infinity
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, Float32::getNegativeInfinity());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), Float32::getNegativeInfinity()) == 0);

            // test positive infinity
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32(-1, Float32::getPositiveInfinity());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), Float32::getPositiveInfinity()) == 0);
            }

        /**
        * Test the PofBufferWriter::writeFloat64() method.
        */
        void testWriteFloat64()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test 0.0
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, 0.0);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::isZero(hReader->readFloat64(-1)));

            // test a negative floating point value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, -1.1);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), -1.1) == 0);

            // test a positive floating point value
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, 1.1);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), 1.1) == 0);

            // test NaN
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, Float64::getNaN());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::create(hReader->readFloat64(-1))->equals(
                    Float64::create(Float64::getNaN())));

            // test negative infinity
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, Float64::getNegativeInfinity());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), Float64::getNegativeInfinity()) == 0);

            // test positive infinity
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64(-1, Float64::getPositiveInfinity());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), Float64::getPositiveInfinity()) == 0);
            }

        /**
        * Test the PofBufferWriter::writeBinary() method.
        */
        void testWriteBinary()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL Binary
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeBinary(-1, (Binary::View) NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readBinary(-1));

            // test an empty Binary
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeBinary(-1, PofHelper::getEmptyBinary());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readBinary(-1)->length());

            // test a populated Binary
            Array<octet_t>::Handle hab = Array<octet_t>::create(8);
            Array<octet_t>::View   vab = hab;
            hab[0] = (octet_t) 0;
            hab[1] = (octet_t) 1;
            hab[2] = (octet_t) 2;
            hab[3] = (octet_t) 3;
            hab[4] = (octet_t) 4;
            hab[5] = (octet_t) 5;
            hab[6] = (octet_t) 6;
            hab[7] = (octet_t) 7;
            hab = NULL;

            Binary::View vBin = Binary::create(vab, 0, vab->length);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeBinary(-1, vBin);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readBinary(-1)->equals(vBin));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vBin);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vBin));
            }

        /**
        * Test the PofBufferWriter::writeString() method.
        */
        void testWriteString()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL String
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeString(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readString(-1));

            // test an empty String
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeString(-1, "");
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readString(-1)->length());

            // test an ASCII String
            String::View vs = "this is a test";

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeString(-1, vs);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readString(-1)->equals(vs));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vs);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vs));
            }

        /**
        * Test the PofBufferWriter::writeRawDate() method.
        */
        void testWriteRawDate()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawDate
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDate(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawDate(-1));

            // test an initialized RawDate
            RawDate::View vDate = RawDate::create(1975, 12, 31);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDate(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawDate(-1)->equals(vDate));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vDate));
            }

        /**
        * Test the PofBufferWriter::writeRawTime() method.
        */
        void testWriteRawTime()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawTime
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTime(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawTime(-1));

            // test a RawTime without timezone
            RawTime::View vDate = RawTime::create(1, 2, 3, 4, false);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTime(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawTime(-1)->equals(vDate));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vDate));

            // test a RawTime with UTC timezone
            vDate = RawTime::create(1, 2, 3, 4, true);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTime(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawTime(-1)->equals(vDate));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vDate));

            // test a RawTime with EST timezone
            vDate = RawTime::create(1, 2, 3, 4, -5, 0);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTime(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawTime(-1)->equals(vDate));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vDate);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vDate));
            }

        /**
        * Test the PofBufferWriter::writeRawDateTime() method.
        */
        void testWriteRawDateTime()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawDateTime
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDateTime(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawDateTime(-1));

            // test an initialized RawDateTime without time zone
            RawDateTime::View vdt = RawDateTime::create(
                    RawDate::create(1975, 12, 31),
                    RawTime::create(1, 2, 3, 4, false));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDateTime(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawDateTime(-1)->equals(vdt));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vdt));

            // test an initialized RawDateTime with UTC timezone
            vdt = RawDateTime::create(
                    RawDate::create(1975, 12, 31),
                    RawTime::create(1, 2, 3, 4, true));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDateTime(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawDateTime(-1)->equals(vdt));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vdt));

            // test an initialized RawDateTime with EST timezone
            vdt = RawDateTime::create(
                    RawDate::create(1975, 12, 31),
                    RawTime::create(1, 2, 3, 4, -5, 0));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDateTime(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawDateTime(-1)->equals(vdt));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vdt);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vdt));
            }

        /**
        * Test the PofBufferWriter::writeRawDayTimeInterval() method.
        */
        void testWriteRawDayTimeInterval()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawDayTimeInterval
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDayTimeInterval(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawDayTimeInterval(-1));

            // test an initialized RawDayTimeInterval
            RawDayTimeInterval::View vInterval =
                    RawDayTimeInterval::create(1, 2, 3, 4, 5);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawDayTimeInterval(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawDayTimeInterval(-1)->equals(vInterval));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vInterval));
            }

        /**
        * Test the PofBufferWriter::writeRawTimeInterval() method.
        */
        void testWriteRawTimeInterval()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawTimeInterval
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTimeInterval(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawTimeInterval(-1));

            // test an initialized RawTimeInterval
            RawTimeInterval::View vInterval =
                    RawTimeInterval::create(1, 2, 3, 4);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawTimeInterval(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawTimeInterval(-1)->equals(vInterval));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vInterval));
            }

        /**
        * Test the PofBufferWriter::writeRawYearMonthInterval() method.
        */
        void testWriteRawYearMonthInterval()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL RawYearMonthInterval
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawYearMonthInterval(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readRawYearMonthInterval(-1));

            // test an initialized RawYearMonthInterval
            RawYearMonthInterval::View vInterval =
                    RawYearMonthInterval::create(1, 2);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeRawYearMonthInterval(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readRawYearMonthInterval(-1)->equals(vInterval));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, vInterval);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(vInterval));
            }

        /**
        * Test the PofBufferWriter::writeBooleanArray() method.
        */
        void testWriteBooleanArray()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeBooleanArray(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readBooleanArray(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeBooleanArray(-1, Array<bool>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readBooleanArray(-1)->length);

            // test a populated array
            Array<bool>::Handle haf = Array<bool>::create(2);
            haf[0] = false;
            haf[1] = true;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeBooleanArray(-1, haf);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readBooleanArray(-1)->equals(haf));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, haf);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(haf));
            }

        /**
        * Test the PofBufferWriter::writeOctetArray() method.
        */
        void testWriteOctetArray()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctetArray(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readOctetArray(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctetArray(-1, Array<octet_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readOctetArray(-1)->length);

            // test a populated array
            Array<octet_t>::Handle hab = Array<octet_t>::create(2);
            hab[0] = 0;
            hab[1] = 1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeOctetArray(-1, hab);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readOctetArray(-1)->equals(hab));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hab);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hab));
            }

        /**
        * Test the PofBufferWriter::writeChar16Array() method.
        */
        void testWriteChar16Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readChar16Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16Array(-1, Array<wchar16_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readChar16Array(-1)->length);

            // test a populated array
            Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(2);
            hach[0] = 0;
            hach[1] = 1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeChar16Array(-1, hach);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readChar16Array(-1)->equals(hach));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hach);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hach));
            }

        /**
        * Test the PofBufferWriter::writeInt16Array() method.
        */
        void testWriteInt16Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readInt16Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16Array(-1, Array<int16_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readInt16Array(-1)->length);

            // test a populated array
            Array<int16_t>::Handle han = Array<int16_t>::create(2);
            han[0] = 0;
            han[1] = 1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt16Array(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt16Array(-1)->equals(han));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(han));
            }

        /**
        * Test the PofBufferWriter::writeInt32Array() method.
        */
        void testWriteInt32Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readInt32Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32Array(-1, Array<int32_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readInt32Array(-1)->length);

            // test a populated array
            Array<int32_t>::Handle han = Array<int32_t>::create(2);
            han[0] = 0;
            han[1] = 1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt32Array(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt32Array(-1)->equals(han));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(han));
            }

        /**
        * Test the PofBufferWriter::writeInt64Array() method.
        */
        void testWriteInt64Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readInt64Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64Array(-1, Array<int64_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readInt64Array(-1)->length);

            // test a populated array
            Array<int64_t>::Handle han = Array<int64_t>::create(2);
            han[0] = 0;
            han[1] = 1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeInt64Array(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readInt64Array(-1)->equals(han));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, han);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(han));
            }

        /**
        * Test the PofBufferWriter::writeFloat32Array() method.
        */
        void testWriteFloat32Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readFloat32Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32Array(-1, Array<float32_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readFloat32Array(-1)->length);

            // test a populated array
            Array<float32_t>::Handle hafl = Array<float32_t>::create(2);
            hafl[0] = (float32_t) 0.0;
            hafl[1] = (float32_t) 1.1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat32Array(-1, hafl);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readFloat32Array(-1)->equals(hafl));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hafl);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hafl));
            }

        /**
        * Test the PofBufferWriter::writeFloat64Array() method.
        */
        void testWriteFloat64Array()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64Array(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readFloat64Array(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64Array(-1, Array<float64_t>::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readFloat64Array(-1)->length);

            // test a populated array
            Array<float64_t>::Handle hafl = Array<float64_t>::create(2);
            hafl[0] = 0.0;
            hafl[1] = 1.1;

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeFloat64Array(-1, hafl);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readFloat64Array(-1)->equals(hafl));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hafl);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hafl));
            }

        /**
        * Test the PofBufferWriter::writeObjectArray() method.
        */
        void testWriteObjectArray()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeObjectArray(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readObjectArray(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObjectArray(-1, ObjectArray::create(0));
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readObjectArray(-1)->length);

            // test a populated array
            ObjectArray::Handle ha = ObjectArray::create(2);
            ha[0] = String::create("This is a test");
            ha[1] = Integer32::create(123);

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObjectArray(-1, ha);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObjectArray(-1)->equals(ha));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, ha);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(ha));
            }

        /**
        * Test the PofBufferWriter::writeLongArray() method.
        */
        void testWriteLongArray()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readLongArray(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, SparseArray::create());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readLongArray(-1)->getSize());

            // test a populated array
            LongArray::Handle hla = SparseArray::create();
            hla->set(0, Integer32::create(123));
            hla->set(1, String::create("321"));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, hla);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readLongArray(-1)->equals(hla));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hla);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hla));
            }

        /**
        * Test the PofBufferWriter::writeLongArray() method with a uniform
        * array.
        */
        void testWriteUniformLongArray()
            {
            OctetArrayWriteBuffer::Handle hBuf    = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::View       vLoader = SystemClassLoader::getInstance();
            Class::View                   vClz    = vLoader->loadByType(typeid(Integer32));

            // test a NULL array
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, NULL, vClz);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readLongArray(-1));

            // test an empty array
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, SparseArray::create(), vClz);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readLongArray(-1)->getSize());

            // test a populated array
            LongArray::Handle hla = SparseArray::create();
            hla->set(123, Integer32::create(123));
            hla->set(321, Integer32::create(321));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeLongArray(-1, hla, vClz);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readLongArray(-1)->equals(hla));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hla);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hla));
            }

        /**
        * Test the PofBufferWriter::writeObject() method with a POF user type.
        */
        void testWriteUserType()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            UUID::Handle hUUID = UUID::create();

            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hUUID);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hUUID->equals(hReader->readObject(-1)));
            }

        /**
        * Test the PofBufferWriter::writeCollection() method.
        */
        void testWriteCollection()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL collection
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readCollection(-1));

            // test an empty collection
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, ArrayList::create());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readCollection(-1)->size());

            // test a populated collection
            Collection::Handle hCol = ArrayList::create();
            hCol->add(Integer32::create(123));
            hCol->add(String::create("321"));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, hCol);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hCol);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hCol));
            }

        /**
        * Test the PofBufferWriter::writeCollection() method with a uniform
        * collection.
        */
        void testWriteUniformCollection()
            {
            OctetArrayWriteBuffer::Handle hBuf    = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::View       vLoader = SystemClassLoader::getInstance();
            Class::View                   vClz    = vLoader->loadByType(typeid(Integer32));

            // test a NULL collection
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, NULL, vClz);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readCollection(-1));

            // test an empty collection
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, ArrayList::create(), vClz);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readCollection(-1)->size());

            // test a populated collection
            Collection::Handle hCol = ArrayList::create();
            hCol->add(Integer32::create(123));
            hCol->add(Integer32::create(321));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeCollection(-1, hCol, vClz);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hCol);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hCol));
            }

        /**
        * Test the PofBufferWriter::writeMap() method.
        */
        void testWriteMap()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            // test a NULL map
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, NULL);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readMap(-1));

            // test an empty map
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, HashMap::create());
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readMap(-1)->size());

            // test a populated map
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(123), String::create("123"));
            hMap->put(Integer64::create(321), Integer16::create(111));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, hMap);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readMap(-1)->equals(hMap));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hMap);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hMap));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with a uniform key map.
        */
        void testWriteUniformKeyMap()
            {
            OctetArrayWriteBuffer::Handle hBuf    = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::View       vLoader = SystemClassLoader::getInstance();
            Class::View                   vClzKey = vLoader->loadByType(typeid(Integer32));

            // test a NULL map
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, NULL, vClzKey);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readMap(-1));

            // test an empty map
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, HashMap::create(), vClzKey);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readMap(-1)->size());

            // test a populated map
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(123), String::create("123"));
            hMap->put(Integer32::create(321), Integer16::create(111));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, hMap, vClzKey);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readMap(-1)->equals(hMap));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hMap);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hMap));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with a uniform map.
        */
        void testWriteUniformMap()
            {
            OctetArrayWriteBuffer::Handle hBuf    = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::View       vLoader = SystemClassLoader::getInstance();
            Class::View                   vClzKey = vLoader->loadByType(typeid(Integer32));
            Class::View                   vClzVal = vLoader->loadByType(typeid(String));

            // test a NULL map
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, NULL, vClzKey, vClzVal);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(NULL == hReader->readMap(-1));

            // test an empty map
            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, HashMap::create(), vClzKey, vClzVal);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(0 == hReader->readMap(-1)->size());

            // test a populated map
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(123), String::create("123"));
            hMap->put(Integer32::create(321), String::create("321"));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeMap(-1, hMap, vClzKey, vClzVal);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readMap(-1)->equals(hMap));

            hBuf->clear();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hMap);
            hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            TS_ASSERT(hReader->readObject(-1)->equals(hMap));
            }

        /**
        * Test the LocalMember Pof method.
        */
        void testWriteLocalMember()
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            SystemPofContext::View        vCtx = SystemPofContext::getInstance();

            LocalMember::Handle     hMember  = LocalMember::create();
            hMember->setSiteName(String::create("site1"));
            hMember->setRackName(String::create("rack1"));
            hMember->setMachineName(String::create("machine1"));
            hMember->setProcessName(String::create("process1"));
            hMember->setMemberName(String::create("member1"));
            hMember->setRoleName(String::create("role1"));

            hBuf->clear();
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
            hWriter->writeObject(-1, hMember);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), vCtx);
            LocalMember::View vCopy = cast<LocalMember::View>(hReader->readObject(-1)); 
            TS_ASSERT(vCopy->getSiteName()->equals(hMember->getSiteName()));
            TS_ASSERT(vCopy->getRackName()->equals(hMember->getRackName()));
            TS_ASSERT(vCopy->getMachineName()->equals(hMember->getMachineName()));
            TS_ASSERT(vCopy->getProcessName()->equals(hMember->getProcessName()));
            TS_ASSERT(vCopy->getMemberName()->equals(hMember->getMemberName()));
            TS_ASSERT(vCopy->getRoleName()->equals(hMember->getRoleName()));
            }
    };
