/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/UUID.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include "cxxtest/TestSuite.h"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::Serializer;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::io::pof::PortableObjectSerializer;
using coherence::io::pof::SystemPofContext;
using coherence::util::StringHelper;
using coherence::util::UUID;

namespace test {
class TestPortableObject
    : public class_spec<TestPortableObject,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<TestPortableObject>;

    private:
        MemberView<String> m_hStr;

    protected:
        TestPortableObject()
            : m_hStr(self())
        {}

    public:
        void readExternal(PofReader::Handle hReader)
            {
                m_hStr = hReader->readString(0);
            }

        void writeExternal(PofWriter::Handle hWriter) const
            {
                hWriter->writeString(0, m_hStr);
            }

        void setStr(String::View vStr)
            {
                m_hStr = vStr;
            }

        String::View getStr() const
        {
            return m_hStr;
        }
    };
}
/**
* Test suite for the PortableObjectSerializer.
*/
class PortableObjectSerializerTest : public CxxTest::TestSuite
    {
    public:
        void testPortableObjectSerialize()
            {
            // registering TestPortableObject in context
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            SystemClassLoader::Handle vLoader = SystemClassLoader::getInstance();
            vLoader->registerClass(coherence::lang::TypedClass<test::TestPortableObject>::create());
            hCtx->registerUserType(12345, vLoader->loadByType(typeid(test::TestPortableObject)), PortableObjectSerializer::create(12345));
            TS_ASSERT(hCtx->getPofSerializer(12345) != NULL);

            // serializing TestPortableObject into context
            PofSerializer::View hs = hCtx->getPofSerializer(12345);
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(32768);
            test::TestPortableObject::Handle tt = test::TestPortableObject::create();
            tt->setStr("testing serialization");
            hCtx->serialize(hBuf->getBufferOutput(), tt);

            // deserializing TestPortableObject from context
            Object::Holder hObj = hCtx->deserialize(hBuf->getReadBuffer()->getBufferInput());
            TS_ASSERT(hObj != NULL);
            TS_ASSERT(instanceof<test::TestPortableObject::View>(hObj));
            test::TestPortableObject::View vPortable = cast<test::TestPortableObject::View>(hObj);
            TS_ASSERT(vPortable->getStr()->equals(tt->getStr()));
            }
    };
