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
* Test suite for generic POF methods that work with object.
*/
class PofStreamObjectValueTest : public CxxTest::TestSuite
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
        * Test the PofBufferWriter::writeString() method.
        */
        void testWriteString()
            {
            initPofWriter();
            hWriter->writeString(-1, "");
            hWriter->writeString(-1, String::create("test"));
            hWriter->writeString(-1, String::create("Test"));
            hWriter->writeString(-1, NULL);
            hWriter->writeString(-1, String::create(" Test@"));
            hWriter->writeString(-1, "");

            Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(2);
            hach[0] = ' ';
            hach[1] = '@';
            hWriter->writeChar16Array(-1, hach);

            ObjectArray::Handle hao = ObjectArray::create(3);
            hao[0] = Character16::create('=');
            hao[1] = Character16::create('b');
            hao[2] = Integer16::create(12);
            hWriter->writeObjectArray(-1, hao);

            initPofReader();
            TS_ASSERT(hReader->readString(-1)->equals(""));
            TS_ASSERT(hReader->readString(-1)->equals(String::create("test")));
            TS_ASSERT(hReader->readString(-1)->equals(String::create("Test")));
            TS_ASSERT(hReader->readString(-1) == NULL);
            TS_ASSERT(hReader->readString(-1)->equals(String::create(" Test@")));
            TS_ASSERT(hReader->readString(-1)->equals(""));
            TS_ASSERT(hReader->readString(-1)->equals(String::create((Array<wchar16_t>::View) hach)));

            Array<wchar16_t>::Handle hach1 = Array<wchar16_t>::create(3);
            hach1[0] = '=';
            hach1[1] = 'b';
            hach1[2] = (wchar16_t) 12;
            TS_ASSERT(hReader->readString(-1)->equals(String::create((Array<wchar16_t>::View) hach1)));
            }

        /**
        * Test the PofBufferWriter::writeObject() method.
        */
        void testWriteObject()
            {
            initPofWriter();
            hWriter->writeObject(-1, NULL);
            hWriter->writeObject(-1, Boolean::create(false));
            hWriter->writeBoolean(-1, true);
            hWriter->writeBoolean(-1, false);
            hWriter->writeObject(-1, Character16::create('a'));
            hWriter->writeObject(-1, Integer32::create(0));
            hWriter->writeObject(-1, Integer16::create(numeric_limits<short>::max()));
            hWriter->writeObject(-1, Integer16::create(-1));
            hWriter->writeObject(-1, Integer32::create(numeric_limits<int>::max()));
            hWriter->writeObject(-1, Integer64::create(-1));
            hWriter->writeObject(-1, Integer64::create(numeric_limits<long>::min()));
            hWriter->writeObject(-1, Float32::create(0.0F));
            hWriter->writeObject(-1, Float64::create(Float64::getNegativeInfinity()));
            hWriter->writeObject(-1, Float64::create(Float64::getNaN()));
            hWriter->writeObject(-1, Float64::create(Float64::getPositiveInfinity()));

            RawDateTime::View vdt = RawDateTime::create(
                    RawDate::create(1978, 4, 25),
                    RawTime::create(7, 5, 10, 4, false));
            hWriter->writeObject(-1, vdt);

            Array<bool>::Handle haf = Array<bool>::create(2);
            haf[0] = false;
            haf[1] = true;
            hWriter->writeObject(-1, haf);

            Array<octet_t>::Handle hab = Array<octet_t>::create(2);
            hab[0] = 0;
            hab[1] = 1;
            hWriter->writeObject(-1, hab);

            Array<wchar16_t>::Handle hach = Array<wchar16_t>::create(2);
            hach[0] = 0;
            hach[1] = 1;
            hWriter->writeObject(-1, hach);

            Array<int16_t>::Handle han16 = Array<int16_t>::create(2);
            han16[0] = 0;
            han16[1] = 1;
            hWriter->writeObject(-1, han16);

            Array<int32_t>::Handle han32 = Array<int32_t>::create(2);
            han32[0] = 0;
            han32[1] = 1;
            hWriter->writeObject(-1, han32);

            Array<int64_t>::Handle han64 = Array<int64_t>::create(2);
            han64[0] = 0;
            han64[1] = 1;
            hWriter->writeObject(-1, han64);

            ObjectArray::Handle hao = ObjectArray::create(2);
            hao[0] = String::create("This is a test");
            hao[1] = Integer32::create(123);
            hWriter->writeObject(-1, hao);

            Array<float32_t>::Handle hafl32 = Array<float32_t>::create(2);
            hafl32[0] = (float32_t) 0.0;
            hafl32[1] = (float32_t) 1.1;
            hWriter->writeObject(-1, hafl32);

            Array<float64_t>::Handle hafl64 = Array<float64_t>::create(2);
            hafl64[0] = 0.0;
            hafl64[1] = 1.1;
            hWriter->writeObject(-1, hafl64);

            RawYearMonthInterval::View vInterval =
                    RawYearMonthInterval::create(1, 2);
            hWriter->writeObject(-1, vInterval);

            initPofReader();
            TS_ASSERT(hReader->readObject(-1) == NULL);
            TS_ASSERT(hReader->readObject(-1)->equals(Boolean::create(false)));
            TS_ASSERT(hReader->readObject(-1)->equals(Boolean::create(true)));
            TS_ASSERT(hReader->readObject(-1)->equals(Boolean::create(false)));
            TS_ASSERT(hReader->readObject(-1)->equals(Character16::create('a')));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer32::create(0)));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer16::create(numeric_limits<short>::max())));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer16::create(-1)));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer32::create(numeric_limits<int>::max())));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer64::create(-1)));
            TS_ASSERT(hReader->readObject(-1)->equals(Integer64::create(numeric_limits<long>::min())));
            TS_ASSERT(hReader->readObject(-1)->equals(Float32::create(0.0F)));
            TS_ASSERT(hReader->readObject(-1)->equals(Float64::create(Float64::getNegativeInfinity())));
            TS_ASSERT(hReader->readObject(-1)->equals(Float64::create(Float64::getNaN())));
            TS_ASSERT(hReader->readObject(-1)->equals(Float64::create(Float64::getPositiveInfinity())));
            TS_ASSERT(hReader->readObject(-1)->equals(vdt));
            TS_ASSERT(hReader->readObject(-1)->equals(haf));
            TS_ASSERT(hReader->readObject(-1)->equals(hab));
            TS_ASSERT(hReader->readObject(-1)->equals(hach));
            TS_ASSERT(hReader->readObject(-1)->equals(han16));
            TS_ASSERT(hReader->readObject(-1)->equals(han32));
            TS_ASSERT(hReader->readObject(-1)->equals(han64));
            TS_ASSERT(hReader->readObject(-1)->equals(hao));
            TS_ASSERT(hReader->readObject(-1)->equals(hafl32));
            TS_ASSERT(hReader->readObject(-1)->equals(hafl64));
            TS_ASSERT(hReader->readObject(-1)->equals(vInterval));
            }
    };
