/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_SERIALIZER_XML_CONFIGURABLE_HPP
#define COH_TEST_SERIALIZER_XML_CONFIGURABLE_HPP

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
using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;
using coherence::util::Iterator;
using coherence::util::List;

/**
* Custom XmlConfigurable Serializer that delegates to the SystemPofContext
*
* To use this class, you must have a similar delegating class for the
* Java proxy (the Java one does not have to be XmlConfigurable).)
*
* Converts string "grid" to the serializer name taken from config to verify this serializer
* is getting invoked.
*/
class TestSerializerXmlConfigurable
    : public class_spec<TestSerializerXmlConfigurable,
          extends<TestSerializer>,
          implements<XmlConfigurable> >
{
    friend class factory<TestSerializerXmlConfigurable>;

    protected:
        TestSerializerXmlConfigurable()
          : super(), m_vConfig(self()), m_vsSerializerName(self())
            {
            }

    public:
        virtual XmlElement::View getConfig() const
            {
            return m_vConfig;
            }

        virtual void setConfig(XmlElement::View vXml)
            {
            m_vConfig = vXml;
            if ( vXml != NULL )
                {
                List::View vList = vXml->getElementList();
                for (Iterator::Handle hIter = vList->iterator(); hIter->hasNext();)
                    {
                    XmlElement::View vParam = cast<XmlElement::View>(hIter->next());
                    if (vParam->getName()->equals("SerializerName"))
                        {
                        m_vsSerializerName =  vParam->getString();
                        }
                    }
                }
            }

        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const
            {
            // transform a well known string value to demonstrate that we are being called
            if (instanceof<String::View>(v) && cast<String::View>(v)->equals("grid"))
                {
                TestSerializer::serialize(hOut, m_vsSerializerName);
                }
            else
                {
                TestSerializer::serialize(hOut, v);
                }
            }

    private:
        MemberView<XmlElement> m_vConfig;
        MemberView<String>     m_vsSerializerName;
    };

COH_REGISTER_TYPED_CLASS(TestSerializerXmlConfigurable);

COH_CLOSE_NAMESPACE2

#endif // COH_TEST_SERIALIZER_XML_CONFIGURABLE_HPP
