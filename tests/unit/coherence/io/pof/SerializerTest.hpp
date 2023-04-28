/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"

#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "common/ModuleSerializer.hpp"
#include "common/SerializerTestData.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;
using namespace common::test;

using coherence::io::OctetArrayWriteBuffer;

using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::PofWriter;
using coherence::io::pof::SystemPofContext;

using common::test::ModuleSerializer;
using common::test::SerializerTestData;

COH_REGISTER_POF_SERIALIZER(3001, \
        TypedClass<SerializerTestData::Balance>::create(), \
        ModuleSerializer::BalanceSerializer::create());

COH_REGISTER_POF_SERIALIZER(3002, \
        TypedClass<SerializerTestData::Customer>::create(), \
        ModuleSerializer::CustomerSerializer::create());

// add an extra blank line to avoid static macro line number conflicts
COH_REGISTER_POF_SERIALIZER(3003, \
        TypedClass<SerializerTestData::Product>::create(), \
        ModuleSerializer::ProductSerializer::create());

/**
* Test suite for the SerializerTest.
*
* @author lh  2011.07.15
*/
class SerializerTest : public CxxTest::TestSuite
    {
    private:
        OctetArrayWriteBuffer::Handle m_hBuf;
        SystemPofContext::Handle      m_hCtx;
        PofBufferWriter::Handle       m_hWriter;
        PofBufferReader::Handle       m_hReader;

        void initPofWriter()
            {
            m_hBuf = OctetArrayWriteBuffer::create(1024);
            m_hCtx = SystemPofContext::getInstance();
            m_hWriter = PofBufferWriter::create(m_hBuf->getBufferOutput(), m_hCtx);
            m_hWriter->enableReference();
            }

        void initPofReader()
            {
            m_hReader = PofBufferReader::create(m_hBuf->getReadBuffer()->getBufferInput(), m_hCtx);
            }

    public:
        void testSerialization()
            {
            SerializerTestData::Balance::Handle  hBal = SerializerTestData::Balance::create();
            SerializerTestData::Product::Handle  hP   = SerializerTestData::Product::create(hBal);
            SerializerTestData::Customer::Handle hC   = SerializerTestData::Customer::create(String::create("Customer"), hBal, hP);
            hBal->setBalance(2.0f);

            initPofWriter();
            m_hWriter->writeObject(0, hC);

            initPofReader();
            SerializerTestData::Customer::View vResult =
                    cast<SerializerTestData::Customer::View>(m_hReader->readObject(-1));
            SerializerTestData::Product::View  vP = vResult->getProduct();
            SerializerTestData::Balance::View  vB = vResult->getBalance();
            TS_ASSERT(vP->getBalance() == vB);
            }
    };
