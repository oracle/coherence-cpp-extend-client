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
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "common/NestedType.hpp"
#include "common/SimpleType.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::PortableObjectSerializer;
using coherence::io::pof::SystemPofContext;

using common::test::NestedType;
using common::test::SimpleType;


/**
* Test suite for the nested POF read/write operations
*/
class PofNestedBuffers : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the nested type.
        */
        void testNestedWriter()
            {
            // registering NestedType in context
            SystemPofContext::Handle hCtx     = SystemPofContext::getInstance();
            SystemClassLoader::Handle vLoader = SystemClassLoader::getInstance();

            vLoader->registerClass(TypedClass<NestedType>::create());
            hCtx->registerUserType(2000, vLoader->loadByType(typeid(NestedType)), PortableObjectSerializer::create(2000));
            TS_ASSERT(hCtx->getPofSerializer(2000) != NULL);

            PofSerializer::View hs             = hCtx->getPofSerializer(2000);
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1000);

            NestedType::Handle hNt = NestedType::create();
            hBuf->clear();
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), hCtx);
            hWriter->writeObject(-1, hNt);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), hCtx);
            NestedType::View vCopy = cast<NestedType::View>(hReader->readObject(-1));
            }

        /**
        * Test the nested type
        */
        void testNestedSimple()
            {
            // registering SimpleType in context
            SystemPofContext::Handle hCtx     = SystemPofContext::getInstance();
            SystemClassLoader::Handle vLoader = SystemClassLoader::getInstance();

            vLoader->registerClass(TypedClass<SimpleType>::create());
            hCtx->registerUserType(2001, vLoader->loadByType(typeid(SimpleType)), PortableObjectSerializer::create(2001));
            TS_ASSERT(hCtx->getPofSerializer(2001) != NULL);

            PofSerializer::View hs             = hCtx->getPofSerializer(2001);
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1000);

            SimpleType::Handle hSt = SimpleType::create();
            hBuf->clear();
            PofBufferWriter::Handle hWriter = PofBufferWriter::create(
                    hBuf->getBufferOutput(), hCtx);
            hWriter->writeObject(-1, hSt);
            PofBufferReader::Handle hReader = PofBufferReader::create(
                    hBuf->getUnsafeReadBuffer()->getBufferInput(), hCtx);
            SimpleType::View vCopy = cast<SimpleType::View>(hReader->readObject(-1));
            }
    };
