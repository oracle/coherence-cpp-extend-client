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
* Test suite for POF methods that work with arrays.
*/
class PofStreamPrimitiveArrayTest : public CxxTest::TestSuite
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
        * Test the PofBufferWriter::writeBooleanArray() method.
        */
        void testBooleanArray()
            {
            initPofWriter();
            Array<bool>::Handle haf = Array<bool>::create(8);
            haf[0] = false;
            haf[1] = true;
            haf[2] = false;
            haf[3] = true;
            haf[4] = false;
            haf[5] = true;
            haf[6] = false;
            haf[7] = true;

            hWriter->writeBooleanArray(-1, haf);

            initPofReader();
            TS_ASSERT(hReader->readBooleanArray(-1)->equals(haf));
            }

        /**
        * Test the PofBufferWriter::writeOctetArray() method.
        */
        void testOctetArray()
            {
            initPofWriter();
            Array<octet_t>::Handle hab1 = Array<octet_t>::create(4);
            hab1[0] = 1;
            hab1[1] = 128;
            hab1[2] = numeric_limits<octet_t>::max();
            hab1[3] = 0;

            Array<octet_t>::Handle hab2 = Array<octet_t>::create(0);

            hWriter->writeOctetArray(-1, hab1);
            hWriter->writeOctetArray(-1, hab2);

            initPofReader();
            TS_ASSERT(hReader->readOctetArray(-1)->equals(hab1));
            TS_ASSERT(hReader->readOctetArray(-1)->equals(hab2));
            }

        /**
        * Test the PofBufferWriter::readOctetArray() method with exception
        * expected.
        */
        void testOctetArrayWithException()
            {
            initPofWriter();
            String::View str = "string_bytearray";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readOctetArray(-1) );
            }

        /**
        * Test the PofBufferWriter::writeCharArray() method.
        */
        void testCharArray()
            {
            initPofWriter();
            Array<wchar16_t>::Handle hach1 = Array<wchar16_t>::create(4);
            hach1[0] = 'a';
            hach1[1] = (wchar16_t) numeric_limits<char>::max();
            hach1[2] = (wchar16_t) 0x0080;
            hach1[3] = (wchar16_t) numeric_limits<char>::min();

            Array<wchar16_t>::Handle hach2 = Array<wchar16_t>::create(0);

            hWriter->writeChar16Array(-1, hach1);
            hWriter->writeChar16Array(-1, hach2);

            initPofReader();
            TS_ASSERT(hReader->readChar16Array(-1)->equals(hach1));
            TS_ASSERT(hReader->readChar16Array(-1)->equals(hach2));
            }

        /**
        * Test the PofBufferWriter::readCharArray() method with exception
        * expected.
        */
        void testCharArrayWithException()
            {
            initPofWriter();
            hWriter->writeFloat32(-1, 11.0);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readChar16Array(-1) );
            }

        /**
        * Test the PofBufferWriter::writeInt16Array() method.
        */
        void testInt16Array()
            {
            initPofWriter();
            Array<short>::Handle has1 = Array<short>::create(4);
            has1[0] = 0;
            has1[1] = numeric_limits<short>::max();
            has1[2] = 12222;
            has1[3] = numeric_limits<short>::min();

            Array<short>::Handle has2 = Array<short>::create(0);

            hWriter->writeInt16Array(-1, has1);
            hWriter->writeInt16Array(-1, has2);

            initPofReader();
            TS_ASSERT(hReader->readInt16Array(-1)->equals(has1));
            TS_ASSERT(hReader->readInt16Array(-1)->equals(has2));
            }

        /**
        * Test the PofBufferWriter::readInt16Array() method with exception
        * expected.
        */
        void testInt16ArrayWithException()
            {
            initPofWriter();
            String::View str = "string_shortarray";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readInt16Array(-1) );
            }

        /**
        * Test the PofBufferWriter::writeInt32Array() method.
        */
        void testInt32Array()
            {
            initPofWriter();
            Array<int32_t>::Handle hai1 = Array<int32_t>::create(5);
            hai1[0] = 0;
            hai1[1] = numeric_limits<int32_t>::max();
            hai1[2] = 12222;
            hai1[3] = -1;
            hai1[4] = numeric_limits<int32_t>::min();

            Array<int32_t>::Handle hai2 = Array<int32_t>::create(0);

            hWriter->writeInt32Array(-1, hai1);
            hWriter->writeInt32Array(-1, hai2);

            initPofReader();
            TS_ASSERT(hReader->readInt32Array(-1)->equals(hai1));
            TS_ASSERT(hReader->readInt32Array(-1)->equals(hai2));
            }

        /**
        * Test the PofBufferWriter::readInt32Array() method with exception
        * expected.
        */
        void testInt32ArrayWithException()
            {
            initPofWriter();
            String::View str = "string_intarray";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readInt32Array(-1) );
            }

        /**
        * Test the PofBufferWriter::writeInt64Array() method.
        */
        void testInt64Array()
            {
            Array<int64_t>::Handle hal1     = Array<int64_t>::create(5);
            Array<int64_t>::Handle hal2     = Array<int64_t>::create(0);
            ObjectArray::Handle    objArray = ObjectArray::create(5);
            LongArray::Handle      hala     = SparseArray::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hal1[0] = 0;
            hal1[1] = numeric_limits<int64_t>::max();
            hal1[2] = 88888888;
            hal1[3] = -1;
            hal1[4] = numeric_limits<int64_t>::min();

            objArray[0] = Integer32::create(1);
            objArray[1] = String::create("test");
            objArray[2] = Octet::create(20);
            objArray[3] = NULL;
            objArray[4] = Integer64::create(100);

            hala->add(String::create("A"));
            hala->add(String::create("B"));
            hala->add(NULL);
            hala->add(String::create("1"));

            initPofWriter();
            hWriter->writeInt64Array(-1, hal1);
            hWriter->writeInt64Array(-1, hal2);
            hWriter->writeInt64Array(-1, NULL);
            hWriter->writeObjectArray(-1, objArray);
            hWriter->writeLongArray(-1, hala, vLoader->loadByType(typeid(String)));

            initPofReader();
            TS_ASSERT(hReader->readInt64Array(-1)->equals(hal1));
            TS_ASSERT(hReader->readInt64Array(-1)->equals(hal2));
            TS_ASSERT(NULL == hReader->readInt64Array(-1));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(objArray));
            TS_ASSERT(hReader->readLongArray(-1)->equals(hala));
            }

        /**
        * Test the PofBufferWriter::readInt64Array() method with exception
        * expected.
        */
        void testInt64ArrayWithException()
            {
            initPofWriter();
            String::View str = "string_int64array";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readInt64Array(-1) );
            }

        /**
        * Test the PofBufferWriter::writeFloat32Array() method.
        */
        void testFloat32Array()
            {
            initPofWriter();
            Array<float32_t>::Handle hafl1 = Array<float32_t>::create(4);
            hafl1[0] = 0;
            hafl1[1] = numeric_limits<float32_t>::max();
            hafl1[2] = -1;
            hafl1[3] = numeric_limits<float32_t>::min();

            Array<float32_t>::Handle hafl2 = Array<float32_t>::create(0);

            hWriter->writeFloat32Array(-1, hafl1);
            hWriter->writeFloat32Array(-1, hafl2);

            initPofReader();
            TS_ASSERT(hReader->readFloat32Array(-1)->equals(hafl1));
            TS_ASSERT(hReader->readFloat32Array(-1)->equals(hafl2));
            }

        /**
        * Test the PofBufferWriter::readFloat32Array() method with exception
        * expected.
        */
        void testFloat32ArrayWithException()
            {
            initPofWriter();
            String::View str = "string_float32array";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readFloat32Array(-1) );
            }

        /**
        * Test the PofBufferWriter::writeFloat64Array() method.
        */
        void testFloat64Array()
            {
            initPofWriter();
            Array<float64_t>::Handle hafl1 = Array<float64_t>::create(5);
            hafl1[0] = 0;
            hafl1[1] = numeric_limits<float64_t>::max();
            hafl1[2] = 888888.8888;
            hafl1[3] = -1;
            hafl1[4] = numeric_limits<float64_t>::min();

            Array<float64_t>::Handle hafl2 = Array<float64_t>::create(0);

            hWriter->writeFloat64Array(-1, hafl1);
            hWriter->writeFloat64Array(-1, hafl2);

            initPofReader();
            TS_ASSERT(hReader->readFloat64Array(-1)->equals(hafl1));
            TS_ASSERT(hReader->readFloat64Array(-1)->equals(hafl2));
            }

        /**
        * Test the PofBufferWriter::readFloat64Array() method with exception
        * expected.
        */
        void testFloat64ArrayWithException()
            {
            initPofWriter();
            String::View str = "string_float64array";
            hWriter->writeString(-1, str);

            initPofReader();
            TS_ASSERT_THROWS_ANYTHING( hReader->readFloat64Array(-1) );
            }

        void testPofWriterWriteUniformObjectArrayWithNull()
            {
            ObjectArray::Handle hao1        = ObjectArray::create(5);
            ObjectArray::Handle hao2        = ObjectArray::create(3);
            ObjectArray::Handle hao3        = ObjectArray::create(5);
            ObjectArray::Handle hao4        = ObjectArray::create(3);
            ObjectArray::Handle hao5        = ObjectArray::create(5);
            ObjectArray::Handle hao6        = ObjectArray::create(5);
            ObjectArray::Handle hao7        = ObjectArray::create(2);
            ObjectArray::Handle hao8        = ObjectArray::create(5);
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hao1[0] = Float64::create(32);
            hao1[1] = Float64::create(numeric_limits<float64_t>::max());
            hao1[2] = Float64::create(-1);
            hao1[3] = NULL;
            hao1[4] = Float64::create(0);

            hao2[0] = Boolean::create(true);
            hao2[1] = NULL;
            hao2[2] = Boolean::create(false);

            hao3[0] = Octet::create(65);
            hao3[1] = NULL;
            hao3[2] = Octet::create(-37);
            hao3[3] = Octet::create(0);
            hao3[4] = NULL;

            hao4[0] = Character16::create('A');
            hao4[1] = Character16::create('B');
            hao4[2] = NULL;

            hao5[0] = Float32::create((float32_t)32);
            hao5[1] = Float32::create(numeric_limits<float32_t>::max());
            hao5[2] = Float32::create((float32_t)-1);
            hao5[3] = Float32::create((float32_t)0);
            hao5[4] = NULL;

            hao6[0] = Integer32::create(32);
            hao6[1] = Integer32::create(numeric_limits<int32_t>::max());
            hao6[2] = Integer32::create(-1);
            hao6[3] = Integer32::create(0);
            hao6[4] = NULL;

            hao7[0] = RawDate::create(2006, 12, 2); 
            hao7[1] = NULL;

            hao8[0] = String::create("test");
            hao8[1] = String::create("test3");
            hao8[2] = NULL;
            hao8[3] = NULL;
            hao8[4] = String::create("test4");

            initPofWriter();
            hWriter->writeObjectArray(0, hao1, vLoader->loadByType(typeid(Float64)));
            hWriter->writeObjectArray(0, hao2, vLoader->loadByType(typeid(bool)));
            hWriter->writeObjectArray(0, hao3, vLoader->loadByType(typeid(Octet)));
            hWriter->writeObjectArray(0, hao4, vLoader->loadByType(typeid(Character16)));
            hWriter->writeObjectArray(0, hao5, vLoader->loadByType(typeid(Float32)));
            hWriter->writeObjectArray(0, hao6, vLoader->loadByType(typeid(Integer32)));
            hWriter->writeObjectArray(0, hao7, vLoader->loadByType(typeid(RawDate)));
            hWriter->writeObjectArray(0, hao8, vLoader->loadByType(typeid(String)));
            hWriter->writeObjectArray(0, NULL, vLoader->loadByType(typeid(Object)));

            initPofReader();
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao1));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao2));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao3));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao4));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao5));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao6));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao7));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao8));
            TS_ASSERT(hReader->readObjectArray(-1) == NULL);
            }
    };
