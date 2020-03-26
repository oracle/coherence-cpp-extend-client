/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/ConfigurableAddressProviderFactory.hpp"

#include "coherence/net/AddressProvider.hpp"
#include "coherence/net/AddressProviderFactory.hpp"
#include "coherence/net/ConfigurableAddressProvider.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"


COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlElement;
using coherence::run::xml::XmlHelper;


// ----- constructors -------------------------------------------------------

ConfigurableAddressProviderFactory::ConfigurableAddressProviderFactory()
        : f_vXmlConfig(self())
    {
    }


// ----- AddressProviderFactory interface -----------------------------------

AddressProvider::Handle ConfigurableAddressProviderFactory::createAddressProvider(ClassLoader::View vLoader)
    {
    ClassLoader::View vClassLoader = vLoader;
    if (vClassLoader == NULL)
        {
        vClassLoader = SystemClassLoader::getInstance();
        }

    XmlElement::View vXmlConfig    = getConfig();
    String::View     vsElementName = vXmlConfig->getName();

    if (vXmlConfig->getElement("address-provider") != NULL)
        {
        vXmlConfig = vXmlConfig->getElement("address-provider");
        }

    if (vsElementName->equals("address-provider"))
        {
        XmlElement::View vXmlSocketAddress = vXmlConfig->getElement("socket-address");
        if (vXmlSocketAddress != NULL)
            {
            return ConfigurableAddressProvider::create(vXmlConfig);
            }

        return cast<AddressProvider::Handle>(XmlHelper::createInstance(
                vXmlConfig,
                vClassLoader,
                vClassLoader->loadByType(typeid(AddressProvider))));
        }
    else
        {
        return ConfigurableAddressProvider::create(vXmlConfig);
        }
    }


// ----- XmlConfigurable ----------------------------------------------------

void ConfigurableAddressProviderFactory::setConfig(XmlElement::View vXml)
    {
    initialize(f_vXmlConfig, vXml);
    }

XmlElement::View ConfigurableAddressProviderFactory::getConfig() const
    {
    return f_vXmlConfig;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ConfigurableAddressProviderFactory::toString() const
    {
    return COH_TO_STRING("ConfigurableAddressProviderFactory{Xml=" << getConfig() << "}");
    }

COH_CLOSE_NAMESPACE2
