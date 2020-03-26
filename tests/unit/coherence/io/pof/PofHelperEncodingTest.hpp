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
#include "coherence/io/pof/RawTime.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofConstants;
using coherence::io::pof::RawDate;
using coherence::io::pof::RawTime;

#include <limits>

/**
* Test suite for the POF Helper encoding methods.
*/
class PofHelperEncodingTest : public CxxTest::TestSuite
    {
    private:
        OctetArrayWriteBuffer::Handle hBuf;

    public:
        /**
        * Test the PofHelper::encodeTinyInt32() method.
        */
        void testEncodeTinyInt()
            {
            TS_ASSERT(PofHelper::encodeTinyInt32(1) == PofConstants::v_int_1);
            TS_ASSERT(PofHelper::encodeTinyInt32(-1) == PofConstants::v_int_neg_1);
            TS_ASSERT(PofHelper::encodeTinyInt32(13) == PofConstants::v_int_13);
            }

        /**
        * Test the PofHelper::writeDate() method.
        */
        void testWriteDate()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            PofHelper::writeDate(hbout, 2006, 8, 10);
            PofHelper::writeDate(hbout, 2006, 8, 11);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readRawDate(hbin)->equals(RawDate::create(2006, 8, 10)));
            TS_ASSERT(PofHelper::readRawDate(hbin)->equals(RawDate::create(2006, 8, 11)));
            }

        /**
        * Test the PofHelper::writeTime() method.
        */
        void testWriteTime()
            {
            hBuf = OctetArrayWriteBuffer::create(1024);
            WriteBuffer::BufferOutput::Handle hbout = hBuf->getBufferOutput();
            PofHelper::writeTime(hbout, 12, 52, 14, 11, 0, 0, 0);
            PofHelper::writeTime(hbout, 12, 53, 23, 56, 0, 0, 0);

            ReadBuffer::BufferInput::Handle hbin = hBuf->getReadBuffer()->getBufferInput();
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12, 52, 14, 11, 0, 0)));
            TS_ASSERT(PofHelper::readRawTime(hbin)->equals(RawTime::create(12, 53, 23, 56, 0, 0)));
            }
    };
