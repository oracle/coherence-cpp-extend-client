/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURABLE_SERIALIZER_FACTORY_HPP
#define COH_CONFIGURABLE_SERIALIZER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/SerializerFactory.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/run/xml/XmlElement.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::run::xml::XmlConfigurable;
using coherence::run::xml::XmlElement;


/**
* A factory for Serializer objects.
*
* @author wl  2011.11.07
*
* @since Coherence 12.1.2
*/
class COH_EXPORT ConfigurableSerializerFactory
    : public class_spec<ConfigurableSerializerFactory,
        extends<Object>,
        implements<SerializerFactory, XmlConfigurable> >
    {
    friend class factory<ConfigurableSerializerFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ConfigurableSerializerFactory.
        *
        * @param vXmlConfig  the XML configuration for this
        *                    ConfigurableSerializerFactory.
        */
        ConfigurableSerializerFactory();


    // ----- ConfigurableSerializerFactory interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Serializer::View createSerializer(ClassLoader::View vLoader) const;
    

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
        * XML configuration for this ConfigurableSerializerFactory.
        */
        FinalView<XmlElement> f_vXmlConfig;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONFIGURABLE_SERIALIZER_FACTORY_HPP
