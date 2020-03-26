/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_POF_SERIALIZER_HPP
#define COH_TEST_POF_SERIALIZER_HPP

#include "common/TestSerializer.hpp"

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(common,test)

using common::test::TestSerializer;

using namespace coherence::lang;

using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::PofContext;
using coherence::io::pof::SystemPofContext;
using coherence::io::pof::PofSerializer;
using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;
using coherence::util::Iterator;
using coherence::util::List;

/**
* Custom PofContext that delegates to the SystemPofContext
*
* This class can be defined in the config as a custom serializer.
* The Java proxy should just be defined to use Pof
* (no custom serializer on the Java proxy)
*
* Converts string "grid" to "TestPofSerializer" to verify this serializer
* is getting invoked.
*/
class TestPofSerializer
    : public class_spec<TestPofSerializer,
          extends<TestSerializer>,
          implements<PofContext> >
{
    friend class factory<TestPofSerializer>;

    public:
        virtual void serialize( WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const
            {
            // transform a well known string value to demonstrate that we are being called
            if (instanceof<String::View>(v) && cast<String::View>(v)->equals("grid"))
                {
                v = String::create("TestPofSerializer");
                }
            TestSerializer::serialize(hOut, v);
            }

        virtual PofSerializer::View getPofSerializer(int32_t nTypeId) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getPofSerializer(nTypeId);
            }

        virtual int32_t getUserTypeIdentifier(Object::View v) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getUserTypeIdentifier(v);
            }

        virtual int32_t getUserTypeIdentifier(Class::View vClass) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getUserTypeIdentifier(vClass);
            }

        virtual int32_t getUserTypeIdentifier(String::View vsClass) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getUserTypeIdentifier(vsClass);
            }

        virtual String::View getClassName(int32_t nTypeId) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getClassName(nTypeId);
            }

        virtual Class::View getClass(int32_t nTypeId) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->getClass(nTypeId);
            }

        virtual bool isUserType(Object::View v) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->isUserType(v);
            }

        virtual bool isUserType(Class::View vClass) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->isUserType(vClass);
            }

        virtual bool isUserType(String::View vsClass) const
            {
            PofContext::Handle hPofContext = SystemPofContext::getInstance();
            return hPofContext->isUserType(vsClass);
            }
    };

COH_REGISTER_TYPED_CLASS(TestPofSerializer);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_POF_SERIALIZER_HPP
