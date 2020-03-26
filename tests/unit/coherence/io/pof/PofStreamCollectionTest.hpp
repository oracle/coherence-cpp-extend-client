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
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::SystemPofContext;
using coherence::util::ArrayList;
using coherence::util::Collection;

#include <limits>

/**
* Test suite for the POF collection methods.
*/
class PofStreamCollectionTest : public CxxTest::TestSuite
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
        * Test the PofBufferWriter::writeObjectArray() method.
        */
        void testPofWriterWriteObjectArray()
            {
            ObjectArray::Handle hao1 = ObjectArray::create(3);
            ObjectArray::Handle hao2 = ObjectArray::create(3);
            ObjectArray::Handle hao3 = ObjectArray::create(0);
            ObjectArray::Handle hao  = ObjectArray::create(3);

            hao1[0] = String::create("test");
            hao1[1] = String::create("test3");
            hao1[2] = String::create("testPOF1");

            hao2[0] = String::create("test");
            hao2[1] = String::create("test1");
            hao2[2] = String::create("testPOF2");

            hao[0] = Integer32::create(11);
            hao[1] = String::create("test");
            hao[2] = Boolean::create(true);

            initPofWriter();
            hWriter->writeObjectArray(-1, NULL);
            hWriter->writeObjectArray(-1, hao1);
            hWriter->writeObjectArray(-1, hao2);
            hWriter->writeObjectArray(-1, hao3);
            hWriter->writeObjectArray(-1, hao);

            initPofReader();
            TS_ASSERT(NULL == hReader->readObjectArray(-1));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao1));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao2));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao3));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao));
            }

        /**
        * Test the PofBufferWriter::writeObjectArray() method.
        */
        void testPofWriterWriteObjectArray2()
            {
            ObjectArray::Handle hao1 = ObjectArray::create(3);
            ObjectArray::Handle hao2 = ObjectArray::create(3);
            ObjectArray::Handle hao3 = ObjectArray::create(0);
            ObjectArray::Handle hao  = ObjectArray::create(3);

            hao1[0] = String::create("test");
            hao1[1] = String::create("test3");
            hao1[2] = String::create("testPOF1");

            hao2[0] = String::create("test");
            hao2[1] = String::create("test1");
            hao2[2] = String::create("testPOF2");

            hao[0] = Integer32::create(11);
            hao[1] = String::create("test");
            hao[2] = Boolean::create(true);

            initPofWriter();
            hWriter->writeObjectArray(-1, NULL);
            hWriter->writeObjectArray(-1, hao1);
            hWriter->writeObjectArray(-1, hao2);
            hWriter->writeObjectArray(-1, hao3);
            hWriter->writeObjectArray(-1, hao);

            initPofReader();
            TS_ASSERT(NULL == hReader->readObjectArray(-1));
            TS_ASSERT(hReader->readObjectArray(-1, ObjectArray::create(3))->equals(hao1));
            TS_ASSERT(hReader->readObjectArray(-1, ObjectArray::create(0))->equals(hao2));
            TS_ASSERT(hReader->readObjectArray(-1, ObjectArray::create(0))->equals(hao3));
            TS_ASSERT(hReader->readObjectArray(-1, ObjectArray::create(1))->equals(hao));
            }

        /**
        * Test the PofBufferWriter::writeObjectArray() method with uniform
        * object array.
        */
        void testPofWriterWriteUniformObjectArray()
            {
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();
            ObjectArray::Handle hao1 = ObjectArray::create(3);
            hao1[0] = String::create("test");
            hao1[1] = String::create("test3");
            hao1[2] = String::create("testPOF1");

            ObjectArray::Handle hao2 = ObjectArray::create(3);
            hao2[0] = Integer32::create(32);
            hao2[1] = Integer32::create(-1);
            hao2[2] = Integer32::create(0);

            initPofWriter();
            hWriter->writeObjectArray(-1, hao1, vLoader->loadByType(typeid(String)));
            hWriter->writeObjectArray(-1, hao2, vLoader->loadByType(typeid(Integer32)));

            initPofReader();
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao1));
            TS_ASSERT(hReader->readObjectArray(-1)->equals(hao2));
            }

        /**
        * Test the PofBufferWriter::writeCollection() method.
        */
        void testPofWriterWriteCollection()
            {
            Collection::Handle hCol1 = ArrayList::create();
            hCol1->add(String::create("A"));
            hCol1->add(String::create("Z"));
            hCol1->add(String::create("7"));

            Collection::Handle hCol2 = ArrayList::create();
            hCol2->add(Integer32::create(32));
            hCol2->add(Integer32::create(numeric_limits<int>::min()));
            hCol2->add(Integer32::create(numeric_limits<int>::max()));

            initPofWriter();
            hWriter->writeCollection(-1, NULL);
            hWriter->writeCollection(-1, hCol1);
            hWriter->writeCollection(-1, hCol2);
            hWriter->writeCollection(-1, hCol1);
            hWriter->writeRawDate(-1, RawDate::create(2006, 8, 8));

            initPofReader();
            TS_ASSERT(NULL == hReader->readCollection(-1));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol1));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol2));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol1));
            TS_ASSERT(hReader->readRawDate(-1)->equals(RawDate::create(2006, 8, 8)));
            }

        /**
        * Test the PofBufferWriter::writeCollection() method with readObject.
        */
        void testWriteCollectionEx()
            {
            Collection::Handle hCol1 = ArrayList::create();
            hCol1->add(String::create("A"));
            hCol1->add(Integer32::create(321));
            hCol1->add(String::create("Z"));

            initPofWriter();
            hWriter->writeCollection(-1, hCol1);
            hWriter->writeCollection(-1, ArrayList::create());

            initPofReader();
            Object::View colObj = (Object::View) hReader->readObject(-1);
            TS_ASSERT(instanceof<Collection::View>(colObj));
            Collection::View hCol = cast<Collection::View>(colObj);
            TS_ASSERT(hCol->size() == 3);
            ObjectArray::Handle hOba = hCol->toArray();
            ObjectArray::Handle hObaOrig = hCol1->toArray();
            TS_ASSERT(hOba[0]->equals(hObaOrig[0]));
            TS_ASSERT(hOba[1]->equals(hObaOrig[1]));
            TS_ASSERT(hOba[2]->equals(hObaOrig[2]));

            colObj = cast<Object::View>(hReader->readObject(-1));
            TS_ASSERT(instanceof<Collection::View>(colObj));
            hCol = cast<Collection::View>(colObj);
            TS_ASSERT(hCol->size() == 0);
            }

        /**
        * Test the PofBufferWriter::writeCollection() method with uniform 
        * data type.
        */
        void testPofWriterWriteUniformCollection()
            {
            Collection::Handle hCol1 = ArrayList::create();
            Collection::Handle hCol2 = ArrayList::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hCol1->add(String::create("A"));
            hCol1->add(String::create("Z"));
            hCol1->add(String::create("7"));
            hCol2->add(Integer32::create(32));
            hCol2->add(Integer32::create(numeric_limits<int>::min()));
            hCol2->add(Integer32::create(numeric_limits<int>::max()));

            initPofWriter();
            hWriter->writeCollection(-1, hCol1, vLoader->loadByType(typeid(String)));
            hWriter->writeCollection(-1, hCol2, vLoader->loadByType(typeid(Integer32)));
            hWriter->writeCollection(-1, ArrayList::create(0), vLoader->loadByType(typeid(String)));
            hWriter->writeCollection(-1, NULL, vLoader->loadByType(typeid(String)));

            initPofReader();
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol1));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol2));
            TS_ASSERT(hReader->readCollection(-1, ArrayList::create(0))->equals(ArrayList::create(0)));
            }

        // test case for COH-3370
        /**
        * Test the PofBufferWriter::writeCollection() method with uniform 
        * data type that contains NULL values.
        */
        void testPofWriterWriteUniformCollectionWithNulls()
            {
            Collection::Handle hCol1        = ArrayList::create();
            Collection::Handle hCol2        = ArrayList::create();
            Collection::Handle hCol3        = ArrayList::create();
            Collection::Handle hCol4        = ArrayList::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hCol1->add(String::create("A"));
            hCol1->add(String::create("Z"));
            hCol1->add(String::create("7"));
            hCol1->add(NULL);
            hCol2->add(Integer32::create(32));
            hCol2->add(Integer32::create(-37));
            hCol2->add(Integer32::create(numeric_limits<int>::min()));
            hCol2->add(Integer32::create(numeric_limits<int>::max()));
            hCol2->add(Integer32::create(-1));
            hCol2->add(NULL);
            hCol3->add(Integer64::create(64));
            hCol3->add(Integer64::create(numeric_limits<long>::min()));
            hCol3->add(Integer64::create(numeric_limits<long>::max()));
            hCol3->add(Integer64::create(-1));
            hCol3->add(NULL);
            hCol4->add(Integer16::create(16));
            hCol4->add(Integer16::create(numeric_limits<short>::min()));
            hCol4->add(Integer16::create(numeric_limits<short>::max()));
            hCol4->add(Integer16::create(-1));
            hCol4->add(NULL);

            initPofWriter();
            hWriter->writeCollection(-1, hCol1, vLoader->loadByType(typeid(String)));
            hWriter->writeCollection(-1, hCol2, vLoader->loadByType(typeid(Integer32)));
            hWriter->writeCollection(-1, hCol3, vLoader->loadByType(typeid(Integer64)));
            hWriter->writeCollection(-1, hCol4, vLoader->loadByType(typeid(Integer16)));
            hWriter->writeCollection(-1, ArrayList::create(0), vLoader->loadByType(typeid(String)));
            hWriter->writeCollection(-1, NULL, vLoader->loadByType(typeid(String)));

            initPofReader();
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol1));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol2));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol3));
            TS_ASSERT(hReader->readCollection(-1)->equals(hCol4));
            TS_ASSERT(hReader->readCollection(-1, ArrayList::create(0))->equals(ArrayList::create(0)));
            TS_ASSERT(NULL == hReader->readCollection(-1));
            }

        /**
        * Test the PofBufferWriter::writeMap() method.
        */
        void testPofWriterWriteMap()            
            {
            Map::Handle hMap1 = HashMap::create();
            Map::Handle hMap2 = HashMap::create();
            hMap1->put(Integer32::create(0), String::create("A"));
            hMap1->put(Integer32::create(1), String::create("Z"));
            hMap1->put(Integer32::create(2), String::create("7"));
            hMap2->put(Integer32::create(5), Integer32::create(32));
            hMap2->put(Integer32::create(10), Integer32::create(numeric_limits<int>::min()));
            hMap2->put(Integer32::create(15), Integer32::create(numeric_limits<int>::max()));

            initPofWriter();
            hWriter->writeMap(-1, hMap1);
            hWriter->writeMap(-1, hMap2);
            hWriter->writeMap(-1, NULL);

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(hMap2));
            TS_ASSERT(NULL == hReader->readMap(-1));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with uniform key.
        */
        void testPofWriterWriteMapEx1()
            {
            Map::Handle hMap1 = HashMap::create();
            Map::Handle hMap2 = HashMap::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hMap1->put(Integer16::create((short) 0), String::create("A"));
            hMap1->put(Integer16::create((short) 1), String::create("Z"));
            hMap1->put(Integer16::create((short) 2), String::create("7"));

            hMap2->put(Integer32::create(5), Integer32::create(32));
            hMap2->put(Integer32::create(10), Integer32::create(numeric_limits<int>::min()));
            hMap2->put(Integer32::create(15), Integer32::create(numeric_limits<int>::max()));

            initPofWriter();
            hWriter->writeMap(-1, hMap1, vLoader->loadByType(typeid(Integer16)));
            hWriter->writeMap(-1, hMap2, vLoader->loadByType(typeid(Integer32)));
            hWriter->writeMap(-1, NULL, vLoader->loadByType(typeid(Integer32)));

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(hMap2));
            TS_ASSERT(NULL == hReader->readMap(-1));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with uniform key and 
        * value.
        */
        void testPofWriterWriteUniformMap()
            {
            Map::Handle hMap1 = HashMap::create();
            Map::Handle hMap2 = HashMap::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hMap1->put(Integer16::create((short) 0), String::create("A"));
            hMap1->put(Integer16::create((short) 1), String::create("Z"));
            hMap1->put(Integer16::create((short) 2), String::create("7"));

            hMap2->put(Integer32::create(5), Integer32::create(32));
            hMap2->put(Integer32::create(10), Integer32::create(numeric_limits<int>::min()));
            hMap2->put(Integer32::create(15), Integer32::create(numeric_limits<int>::max()));

            initPofWriter();
            hWriter->writeMap(-1, hMap1, vLoader->loadByType(typeid(Integer16)), vLoader->loadByType(typeid(String)));
            hWriter->writeMap(-1, hMap2, vLoader->loadByType(typeid(Integer32)), vLoader->loadByType(typeid(Integer32)));

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(hMap2));
            }

        // test case for COH-3370
        /**
        * Test the PofBufferWriter::writeMap() method with uniform key and 
        * value that are NULLs.
        */
        void testPofWriterWriteUniformMapWithNulls()
            {
            Map::Handle hMap1 = HashMap::create();
            Map::Handle hMap2 = HashMap::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hMap1->put(Integer16::create((short) 0), String::create("A"));
            hMap1->put(Integer16::create((short) 1), String::create("Z"));
            hMap1->put(Integer16::create((short) 2), String::create("7"));
            hMap1->put(Integer16::create((short) 3), NULL);
            hMap1->put(NULL, NULL);

            hMap2->put(Integer32::create(5), Integer32::create(32));
            hMap2->put(Integer32::create(10), Integer32::create(numeric_limits<int>::min()));
            hMap2->put(Integer32::create(15), Integer32::create(numeric_limits<int>::max()));
            hMap2->put(Integer32::create(20), NULL);
            hMap2->put(NULL, Integer32::create(77));

            initPofWriter();
            hWriter->writeMap(-1, hMap1, vLoader->loadByType(typeid(Integer16)), vLoader->loadByType(typeid(String)));
            hWriter->writeMap(-1, hMap2, vLoader->loadByType(typeid(Integer32)), vLoader->loadByType(typeid(Integer32)));

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(hMap2));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with various data types.
        */
        void testPofWriterWriteMapEx3()
            {
            Map::Handle hMap1 = HashMap::create();
            Map::Handle hMap2 = HashMap::create();

            hMap1->put(Integer32::create(0), String::create("A"));
            hMap1->put(Integer32::create(1), String::create("G"));
            hMap1->put(Integer32::create(2), String::create("7"));

            hMap2->put(Integer32::create(0), RawDate::create(106, 7, 7));
            hMap2->put(Integer32::create(1), RawTime::create(7, 7, 7, 8, true));
            hMap2->put(Integer32::create(2), RawDate::create(106, 7, 8));
            hMap2->put(Integer32::create(10), hMap1);
            hMap2->put(Integer32::create(30), Float64::create(Float64::getPositiveInfinity()));
            hMap2->put(Integer32::create(35), Float64::create(Float64::getNegativeInfinity()));
            hMap2->put(Integer32::create(40), Float64::create(Float64::getNaN()));

            initPofWriter();
            hWriter->writeMap(-1, hMap2);

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap2));
            }

        /**
        * Test the PofBufferWriter::writeMap() method with various data types.
        */
        void testPofWriterWriteMapEx4()
            {
            Map::Handle hMap1 = HashMap::create();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            hMap1->put(Integer32::create(0), String::create("A"));
            hMap1->put(Integer32::create(1), String::create("G"));
            hMap1->put(Integer32::create(2), String::create("7"));

            initPofWriter();
            hWriter->writeMap(-1, hMap1, vLoader->loadByType(typeid(Integer32)));
            hWriter->writeMap(-1, hMap1, vLoader->loadByType(typeid(Integer32)), 
                    vLoader->loadByType(typeid(String)));
            hWriter->writeMap(-1, HashMap::create(0), vLoader->loadByType(typeid(String)));
            hWriter->writeMap(-1, HashMap::create(0), vLoader->loadByType(typeid(String)),
                    vLoader->loadByType(typeid(String)));
            hWriter->writeMap(-1, NULL, vLoader->loadByType(typeid(String)), 
                    vLoader->loadByType(typeid(String)));

            initPofReader();
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(hMap1));
            TS_ASSERT(hReader->readMap(-1)->equals(HashMap::create(0)));
            TS_ASSERT(hReader->readMap(-1)->equals(HashMap::create(0)));
            TS_ASSERT(NULL == hReader->readMap(-1));
            }
    };
