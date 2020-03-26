/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURABLE_ADDRESS_PROVIDER_FACTORY_HPP
#define COH_CONFIGURABLE_ADDRESS_PROVIDER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/net/AddressProvider.hpp"
#include "coherence/net/AddressProviderFactory.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/run/xml/XmlElement.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;


/**
* An AddressProviderFactory implementation that creates instances of a
* AddressProvider class configured using an XmlElement of the following structure:
* <pre>
*   &lt;!ELEMENT ... (socket-address+ | address-provider)&gt;
*   &lt;!ELEMENT address-provider 
*     (class-name | (class-factory-name, method-name), init-params?&gt;
*   &lt;!ELEMENT socket-address (address, port)&gt;
* </pre>
*
* @author wl  2012.04.04
* @since Coherence 12.1.2
*/
class COH_EXPORT ConfigurableAddressProviderFactory
    : public class_spec<ConfigurableAddressProviderFactory,
        extends<Object>,
        implements<AddressProviderFactory, XmlConfigurable> >
    {
    friend class factory<ConfigurableAddressProviderFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ConfigurableAddressProviderFactory instance.
        */
        ConfigurableAddressProviderFactory();


    // ----- ConfigurableAddressProviderFactory interface -------------------

    public:
        /**
        * Instantiate an AddressProvider configured according to the specified XML.
        * The passed XML has to conform to the following format:
        * <pre>
        *   &lt;!ELEMENT ... (socket-address+ | address-provider)&gt;
        *   &lt;!ELEMENT address-provider
        *     (class-name | (class-factory-name, method-name), init-params?&gt;
        *   &lt;!ELEMENT socket-address (address, port)&gt;
        * </pre>
        *
        * @param vLoader  the optional ClassLoader with which to configure the
        *                 new AddressProvider.
        *
        * @return an instance of the corresponding AddressProvider
        *         implementation.
        */
        virtual AddressProvider::Handle createAddressProvider(ClassLoader::View vLoader = NULL);
    

    // ----- XmlConfigurable interface---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void setConfig(XmlElement::View xml);

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View getConfig() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ---- data members ----------------------------------------------------

    protected:
        /**
        * XML configuration for this ConfigurableAddressProviderFactory.
        */
        FinalView<XmlElement> f_vXmlConfig;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONFIGURABLE_ADDRESS_PROVIDER_FACTORY_HPP
