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

/**
* Test suite for the POF empty array (de)serialization.
*/
class PofStreamEmptyArrayTest : public CxxTest::TestSuite
    {
    private:
        OctetArrayWriteBuffer::Handle hBuf;
        SystemPofContext::View        vCtx;
        PofBufferWriter::Handle       hWriter;
        PofBufferReader::Handle       hReader;

        void initPofWriter()
	        {
	        hBuf    = OctetArrayWriteBuffer::create(1024);
	        vCtx    = SystemPofContext::getInstance();
	        hWriter = PofBufferWriter::create(hBuf->getBufferOutput(), vCtx);
	        }

        void initPofReader()
	        {
			hReader = PofBufferReader::create(hBuf->getReadBuffer()->getBufferInput(), vCtx);
	        }

    public:
        /**
        * Test the PofBufferReader::readObjectArray() method when the
        * serialized object array is empty.
        */
        void testEmptyArray()
            {
            initPofWriter();
            hWriter->writeObjectArray(-1, ObjectArray::create(0));

            initPofReader();
            TS_ASSERT(hReader->readObjectArray(-1)->length == 0);
            }

        /**
        * Test the PofBufferReader::readOctetArray() method with empty array.
        */
        void testEmptyUniformArray()
            {
            initPofWriter();
            hWriter->writeOctetArray(-1, Array<octet_t>::create(0));

            initPofReader();
            TS_ASSERT(hReader->readOctetArray(-1)->length == 0);
            }

        /**
        * Test the PofBufferReader::readObjectArray() method with nested
        * empty array.
        */
        void testNestedEmptyArray()
            {
            initPofWriter();
            ObjectArray::Handle hoa = ObjectArray::create(1);
            hoa[0] = ObjectArray::create(0);
            hWriter->writeObjectArray(-1, hoa);

            initPofReader();
            ObjectArray::Handle hResult = hReader->readObjectArray(-1);
            TS_ASSERT(instanceof<ObjectArray::Handle>(hResult[0]));
            TS_ASSERT(cast<ObjectArray::Handle>(hResult[0])->length == 0);
            }

        /**
        * Test the PofBufferReader::readObjectArray() method with nested
        * empty uniform array.
        */
        void testNestedEmptyUniformArray()
	        {
	        initPofWriter();
	        ObjectArray::Handle hoa = ObjectArray::create(1);
	        hoa[0] = Array<octet_t>::create(0);
	        hWriter->writeObjectArray(-1, hoa);

	        initPofReader();
	        ObjectArray::Handle hResult = hReader->readObjectArray(-1);
	        TS_ASSERT(instanceof<Array<octet_t>::Handle>(hResult[0]));
	        TS_ASSERT(cast<Array<octet_t>::Handle>(hResult[0])->length == 0);
	        }
    };
