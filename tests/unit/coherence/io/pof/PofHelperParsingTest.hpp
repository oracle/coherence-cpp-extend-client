/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/pof/PofConstants.hpp"
#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/RawDayTimeInterval.hpp"
#include "coherence/io/pof/RawTime.hpp"
#include "coherence/io/pof/RawTimeInterval.hpp"
#include "coherence/io/pof/RawYearMonthInterval.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofConstants;
using coherence::io::pof::RawDate;
using coherence::io::pof::RawDayTimeInterval;
using coherence::io::pof::RawTime;
using coherence::io::pof::RawTimeInterval;
using coherence::io::pof::RawYearMonthInterval;

#include <limits>

/**
* Test suite for the POF Helper parsing methods.
*/
class PofHelperParsingTest : public CxxTest::TestSuite
    {
    private:
        OctetArrayWriteBuffer::Handle hBuf;
        SystemPofContext::View        vCtx;
        PofBufferWriter::Handle       hWriter;
        PofBufferReader::Handle       hReader;

        void initPofWriter()
        {
            hBuf = OctetArrayWriteBuffer::create(1024);
            vCtx = SystemPofContext::getInstance();
            hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
        }

        void initPofReader()
        {
            hReader = PofBufferReader::create(hBuf->getReadBuffer()->getBufferInput(), vCtx);
        }

        public:
        /**
        * Test the PofHelper::decodeTinyInt32() method.
        */
        void testDecodeTinyInt()
            {
            TS_ASSERT(PofHelper::decodeTinyInt32(PofConstants::v_int_0) == 0);
            TS_ASSERT(PofHelper::decodeTinyInt32(PofConstants::v_int_22) == 22);
            TS_ASSERT(PofHelper::decodeTinyInt32(PofConstants::v_int_neg_1) == -1);
            TS_ASSERT(PofHelper::decodeTinyInt32(PofConstants::v_int_17) == 17);
            }

        /**
        * Test the PofHelper::readAsChar() method.
        */
        void testReadAsChar()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeChar16('a');
            hbout->writeChar16('b');
            hbout->writeChar16('c');
            hbout->writeChar16(numeric_limits<char>::min());
            hbout->writeChar16((char) 0x0080);
            hbout->writeInt32(0x0080);
            hbout->writeChar16((wchar16_t) 0x0800);
            hbout->writeChar16(numeric_limits<wchar16_t>::max());

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readAsChar16(hbin, PofConstants::t_octet) == 'a');
            TS_ASSERT(PofHelper::readAsChar16(hbin, PofConstants::t_octet) == 'b');
            TS_ASSERT(PofHelper::readAsChar16(hbin, PofConstants::t_octet) == 'c');
            TS_ASSERT((char) PofHelper::readAsChar16(hbin, PofConstants::t_char) == (char) numeric_limits<char>::min());
            TS_ASSERT((char) PofHelper::readAsChar16(hbin, PofConstants::t_char) == (char) 0x0080);
            TS_ASSERT((char) PofHelper::readAsChar16(hbin, PofConstants::t_int32) == (char) 0x0080);
            TS_ASSERT((wchar16_t) PofHelper::readAsChar16(hbin, PofConstants::t_char) == (wchar16_t) 0x0800);
            TS_ASSERT((wchar16_t) PofHelper::readAsChar16(hbin, PofConstants::t_char) == (wchar16_t) numeric_limits<wchar16_t>::max());
            }

        /**
        * Test the PofHelper::readAsInt32() method.
        */
        void testReadAsInt32()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeInt32((int) 0);
            hbout->writeInt32(numeric_limits<int>::min());
            hbout->writeInt32(PofConstants::v_int_neg_1);
            hbout->writeInt32(numeric_limits<int>::max());

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == 0);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == numeric_limits<int>::min());
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == PofConstants::v_int_neg_1);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == numeric_limits<int>::max());

            initPofWriter();
            hWriter->writeFloat32(-1, 1000.123f);
            hWriter->writeFloat64(-1, 3000.123456);

            initPofReader();
            TS_ASSERT(hReader->readInt32(-1) == 1000);
            TS_ASSERT(hReader->readInt32(-1) == 3000);
            }

        /**
        * Test the PofHelper::readAsInt64() method.
        */
        void testReadAsInt64()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeInt64((long) 0);
            hbout->writeInt64(numeric_limits<long>::min());
            hbout->writeInt64((long) PofConstants::v_int_neg_1);
            hbout->writeInt64(numeric_limits<long>::max());

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readAsInt64(hbin, PofConstants::t_int64) == 0);
            TS_ASSERT(PofHelper::readAsInt64(hbin, PofConstants::t_int64) == numeric_limits<long>::min());
            TS_ASSERT(PofHelper::readAsInt64(hbin, PofConstants::t_int64) == PofConstants::v_int_neg_1);
            TS_ASSERT(PofHelper::readAsInt64(hbin, PofConstants::t_int64) == numeric_limits<long>::max());

            initPofWriter();
            hWriter->writeFloat32(-1, 1000.123f);
            hWriter->writeFloat64(-1, 3000.123456);

            initPofReader();
            TS_ASSERT(hReader->readInt64(-1) == 1000);
            TS_ASSERT(hReader->readInt64(-1) == 3000);
            }

        /**
        * Test the PofHelper::readAsFloat32() method.
        */
        void testReadAsFloat32()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeFloat32((float) 0);
            hbout->writeFloat32(numeric_limits<float>::min());
            hbout->writeFloat32((float) PofConstants::v_int_neg_1);
            hbout->writeFloat32(numeric_limits<float>::max());

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(Float32::isZero(PofHelper::readAsFloat32(hbin, PofConstants::t_float32)));
            TS_ASSERT(Float32::compare(PofHelper::readAsFloat32(hbin, PofConstants::t_float32), numeric_limits<float>::min()) == 0);
            TS_ASSERT(Float32::compare(PofHelper::readAsFloat32(hbin, PofConstants::t_float32), PofConstants::v_int_neg_1) == 0);
            TS_ASSERT(Float32::compare(PofHelper::readAsFloat32(hbin, PofConstants::t_float32), numeric_limits<float>::max()) == 0);

            initPofWriter();
            hWriter->writeFloat64(-1, 3000.123456);

            initPofReader();
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), (float) 3000.123456) == 0);
            }

        /**
        * Test the PofHelper::readAsFloat64() method.
        */
        void testReadAsFloat64()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeFloat64((double) 0);
            hbout->writeFloat64(numeric_limits<double>::min());
            hbout->writeFloat64((double) PofConstants::v_int_neg_1);
            hbout->writeFloat64(numeric_limits<double>::max());

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(Float64::isZero(PofHelper::readAsFloat64(hbin, PofConstants::t_float64)));
            TS_ASSERT(Float64::compare(PofHelper::readAsFloat64(hbin, PofConstants::t_float64), numeric_limits<double>::min()) == 0);
            TS_ASSERT(Float64::compare(PofHelper::readAsFloat64(hbin, PofConstants::t_float64), PofConstants::v_int_neg_1) == 0);
            TS_ASSERT(Float64::compare(PofHelper::readAsFloat64(hbin, PofConstants::t_float64), numeric_limits<double>::max()) == 0);

            initPofWriter();
            hWriter->writeFloat32(-1, 11.234F);

            initPofReader();
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), (float) 11.234) == 0);
            }

        /**
        * Test the PofHelper::readRawDate() method.
        */
        void testReadRawDate()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeInt32(2008); hbout->writeInt32(8); hbout->writeInt32(11);
            hbout->writeInt32(2008); hbout->writeInt32(8); hbout->writeInt32(12);
            hbout->writeInt32(2008); hbout->writeInt32(8); hbout->writeInt32(13);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readRawDate(hbin)->equals(RawDate::create(2008,8,11)));
            TS_ASSERT(PofHelper::readRawDate(hbin)->equals(RawDate::create(2008,8,12)));
            TS_ASSERT(PofHelper::readRawDate(hbin)->equals(RawDate::create(2008,8,13)));
            }

        /**
        * Test the PofHelper::readRawTime() method.
        */
        void testReadRawTime()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            hbout->writeInt32(12); hbout->writeInt32(45); hbout->writeInt32(56); hbout->writeInt32(0); hbout->writeInt32(2); hbout->writeInt32(0); hbout->writeInt32(0);
            hbout->writeInt32(12); hbout->writeInt32(45); hbout->writeInt32(57); hbout->writeInt32(0); hbout->writeInt32(2); hbout->writeInt32(0); hbout->writeInt32(0);
            hbout->writeInt32(12); hbout->writeInt32(45); hbout->writeInt32(58); hbout->writeInt32(0); hbout->writeInt32(2); hbout->writeInt32(0); hbout->writeInt32(0);
            hbout->writeInt32(12); hbout->writeInt32(45); hbout->writeInt32(59); hbout->writeInt32(0); hbout->writeInt32(1);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12,45,56,0,0,0)));
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12,45,57,0,0,0)));
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12,45,58,0,0,0)));
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12,45,59,0,true)));
            }

        /**
        * Test the PofHelper::skipValue() method.
        */
        void testSkipValue()
            {
            initPofWriter();
            hWriter->writeRawDate(-1, RawDate::create(2008, 1, 5));
            hWriter->writeRawTime(-1, RawTime::create(11, 11, 8, 0, true));
            Array<wchar16_t>::Handle hca = Array<wchar16_t>::create(3);
            hca[0] = 'g'; hca[1] = 's'; hca[2] = 't';
            hWriter->writeChar16Array(-1, hca);
            hWriter->writeString(-1, "euro");
            hWriter->writeInt32(-1, 11);
            hWriter->writeOctet(-1, 0x00F0);
            hWriter->writeInt32(-1, 300);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            PofHelper::skipValue(hbin);
            PofHelper::skipValue(hbin);
            PofHelper::skipValue(hbin);
            PofHelper::skipValue(hbin);
            PofHelper::skipValue(hbin);
            PofHelper::skipValue(hbin);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == PofConstants::t_int32);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == 300);
            }

        /**
        * Test the PofHelper::skipUniformValue() method.
        */
        void testSkipUniformValue()
            {
            initPofWriter();
            hWriter->writeInt32(-1, 100);
            hWriter->writeInt32(-1, 200);
            hWriter->writeInt32(-1, 300);

            initPofReader();
            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == PofConstants::t_int32);
            TS_ASSERT(PofHelper::readAsInt32(hbin, PofConstants::t_int32) == 300);

            initPofWriter();
            RawYearMonthInterval::Handle ymi = RawYearMonthInterval::create(2, 10);
            hWriter->writeRawYearMonthInterval(-1, ymi);
            hWriter->writeString(-1, "skipping to string1");
            hWriter->writeRawTimeInterval(-1, RawTimeInterval::create(4, 52, 10, 11));
            hWriter->writeString(-1, "skipping to string2");
            hWriter->writeRawDayTimeInterval(-1, RawDayTimeInterval::create(11, 12, 13, 14, 50) );
            hWriter->writeString(-1, "skipping to string3");
            hWriter->writeFloat32(-1, (float) 120.34);
            hWriter->writeString(-1, "skipping to string4");
            hWriter->writeFloat64(-1, (double) 1222.22);
            hWriter->writeString(-1, "skipping to string5");
            hWriter->writeChar16(-1, 'A');
            hWriter->writeString(-1, "skipping to string6");

            initPofReader();
            hbin = hBuf->getReadBuffer()->getBufferInput();
            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_year_month_interval);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string1"));

            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_time_interval);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string2"));

            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_day_time_interval);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string3"));

            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_float32);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string4"));

            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_float64);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string5"));

            PofHelper::skipUniformValue(hbin, PofConstants::t_int32);
            PofHelper::skipUniformValue(hbin, PofConstants::t_char);
            hReader = PofBufferReader::create(hbin, vCtx);
            TS_ASSERT(hReader->readString(-1)->equals("skipping to string6"));
            }
    };
