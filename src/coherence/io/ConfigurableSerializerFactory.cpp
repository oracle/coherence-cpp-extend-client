/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/ConfigurableSerializerFactory.hpp"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/SerializerFactory.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"


COH_OPEN_NAMESPACE2(coherence,io)

using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;
using coherence::run::xml::XmlHelper;


// ----- constructors -------------------------------------------------------

ConfigurableSerializerFactory::ConfigurableSerializerFactory()
        : f_vXmlConfig(self())
    {
    }

// ----- SerializerFactory interface ----------------------------------------

Serializer::View ConfigurableSerializerFactory::createSerializer(ClassLoader::View vLoader) const
    {
    ClassLoader::View vClassLoader = vLoader;
    if (vClassLoader == NULL)
        {
        vClassLoader = SystemClassLoader::getInstance();
        }
    return cast<Serializer::View>(XmlHelper::createInstance(
            f_vXmlConfig,
            vClassLoader,
            vClassLoader->loadByType(typeid(Serializer))));
    }


// ----- XmlConfigurable ----------------------------------------------------

void ConfigurableSerializerFactory::setConfig(XmlElement::View vXml)
    {
    initialize(f_vXmlConfig, vXml);
    }

XmlElement::View ConfigurableSerializerFactory::getConfig() const
    {
    return f_vXmlConfig;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ConfigurableSerializerFactory::toString() const
    {
    return COH_TO_STRING("ConfigurableSerializerFactory{Xml=" << getConfig() << "}");
    }

COH_CLOSE_NAMESPACE2






