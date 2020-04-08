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
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Collection.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::SystemPofContext;
using coherence::util::Collection;

#include <limits>

/**
* Test suite for POF methods that work with primitive values.
*/
class PofStreamPrimitiveValueTest : public CxxTest::TestSuite
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
        * Test the PofBufferWriter::writeBoolean() method.
        */
        void testWriteBoolean()
            {
            initPofWriter();
            hWriter->writeBoolean(-1, true);
            hWriter->writeBoolean(-1, false);
            hWriter->writeBoolean(-1, true);
            hWriter->writeBoolean(-1, true);

            initPofReader();
            TS_ASSERT(hReader->readBoolean(-1) == true);
            TS_ASSERT(hReader->readBoolean(-1) == false);
            TS_ASSERT(hReader->readBoolean(-1) == true);

            Object::View obool = (Object::View) hReader->readObject(-1);
            TS_ASSERT(instanceof<Boolean::View>(obool));
            Boolean::Handle fTest = Boolean::create(true);
            TS_ASSERT(fTest->equals(cast<Boolean::View>(obool)));
            }

        /**
        * Test the PofBufferWriter::writeOctet() method.
        */
        void testWriteOctet()
        {
            initPofWriter();
            hWriter->writeOctet(-1, 1);
            hWriter->writeOctet(-1, 2);
            hWriter->writeOctet(-1, 255);
            hWriter->writeOctet(-1, 0);

            initPofReader();
            TS_ASSERT(hReader->readOctet(-1) == 1);
            TS_ASSERT(hReader->readOctet(-1) == 2);
            TS_ASSERT(hReader->readOctet(-1) == 255);
            TS_ASSERT(hReader->readOctet(-1) == 0);
        }

        /**
        * Test the PofBufferWriter::writeChar16() method.
        */
        void testWriteChar16()
        {
            initPofWriter();
            hWriter->writeChar16(-1, numeric_limits<char>::min());
            hWriter->writeChar16(-1, 'a');
            hWriter->writeChar16(-1, numeric_limits<char>::max());
            hWriter->writeChar16(-1, 'A');

            initPofReader();
            TS_ASSERT(hReader->readChar16(-1) == (wchar16_t) numeric_limits<char>::min());
            TS_ASSERT(hReader->readChar16(-1) == 'a');
            TS_ASSERT(hReader->readChar16(-1) == (wchar16_t) numeric_limits<char>::max());
            TS_ASSERT(hReader->readChar16(-1) == 'A');
        }

        /**
        * Test the PofBufferWriter::writeInt16() method.
        */
        void testWriteInt16()
        {
            initPofWriter();
            hWriter->writeInt16(-1, 0);
            hWriter->writeInt16(-1, -1);
            hWriter->writeInt16(-1, 101);
            hWriter->writeInt16(-1, numeric_limits<short>::min());
            hWriter->writeInt16(-1, numeric_limits<short>::max());

            initPofReader();
            TS_ASSERT(hReader->readInt16(-1) == 0);
            TS_ASSERT(hReader->readInt16(-1) == -1);
            TS_ASSERT(hReader->readInt16(-1) == 101);
            TS_ASSERT(hReader->readInt16(-1) == numeric_limits<short>::min());
            TS_ASSERT(hReader->readInt16(-1) == numeric_limits<short>::max());
        }

        /**
        * Test the PofBufferWriter::writeInt32() method.
        */
        void testWriteInt32()
        {
            initPofWriter();
            hWriter->writeInt32(-1, 0);
            hWriter->writeInt32(-1, -1);
            hWriter->writeInt32(-1, 101);
            hWriter->writeInt32(-1, numeric_limits<int>::min());
            hWriter->writeInt32(-1, numeric_limits<int>::max());

            initPofReader();
            TS_ASSERT(hReader->readInt32(-1) == 0);
            TS_ASSERT(hReader->readInt32(-1) == -1);
            TS_ASSERT(hReader->readInt32(-1) == 101);
            TS_ASSERT(hReader->readInt32(-1) == numeric_limits<int>::min());
            TS_ASSERT(hReader->readInt32(-1) == numeric_limits<int>::max());
        }

        /**
        * Test the PofBufferWriter::writeInt64() method.
        */
        void testWriteInt64()
        {
            initPofWriter();
            hWriter->writeInt64(-1, 0);
            hWriter->writeInt64(-1, -1);
            hWriter->writeInt64(-1, 101);
            hWriter->writeInt64(-1, numeric_limits<long>::min());
            hWriter->writeInt64(-1, numeric_limits<long>::max());

            initPofReader();
            TS_ASSERT(hReader->readInt64(-1) == 0);
            TS_ASSERT(hReader->readInt64(-1) == -1);
            TS_ASSERT(hReader->readInt64(-1) == 101);
            TS_ASSERT(hReader->readInt64(-1) == numeric_limits<long>::min());
            TS_ASSERT(hReader->readInt64(-1) == numeric_limits<long>::max());
        }

        /**
        * Test the PofBufferWriter::writeFloat32() method.
        */
        void testWriteFloat32()
        {
            initPofWriter();
            hWriter->writeFloat32(-1, 0);
            hWriter->writeFloat32(-1, -1);
            hWriter->writeFloat32(-1, 101);
            hWriter->writeFloat32(-1, numeric_limits<float>::min());
            hWriter->writeFloat32(-1, numeric_limits<float>::max());

            initPofReader();
            TS_ASSERT(Float32::isZero(hReader->readFloat32(-1)));
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), -1) == 0);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), 101) == 0);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), numeric_limits<float>::min()) == 0);
            TS_ASSERT(Float32::compare(hReader->readFloat32(-1), numeric_limits<float>::max()) == 0);
        }

        /**
        * Test the PofBufferWriter::writeFloat64() method.
        */
        void testWriteFloat64()
        {
            initPofWriter();
            hWriter->writeFloat64(-1, 0);
            hWriter->writeFloat64(-1, -1);
            hWriter->writeFloat64(-1, 101);
            hWriter->writeFloat64(-1, numeric_limits<double>::min());
            hWriter->writeFloat64(-1, numeric_limits<double>::max());

            initPofReader();
            TS_ASSERT(Float64::isZero(hReader->readFloat64(-1)));
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), -1) == 0);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), 101) == 0);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), numeric_limits<double>::min()) == 0);
            TS_ASSERT(Float64::compare(hReader->readFloat64(-1), numeric_limits<double>::max()) == 0);
        }
    };
