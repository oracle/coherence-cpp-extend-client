/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_CONFIGURABLE_HPP
#define COH_XML_CONFIGURABLE_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* XmlConfigurable interface represents an {@link coherence::lang::Object
* Object} that can be configured by providing configuration information from
* XML structure.
*/
class COH_EXPORT XmlConfigurable
    : public interface_spec<XmlConfigurable>
    {
    // ----- XmlConfigurable interface --------------------------------------

    public:
        /**
        * If this {@link coherence::lang::Object Object} was previously
        * configured using some XML configuration, the corresponding
        * XmlElement is returned.
        *
        * @return a view of the XmlElement used to configure this Object
        */
        virtual XmlElement::View getConfig() const = 0;

        /**
        * Configure this {@link coherence::lang::Object Object} using the
        * provided XmlElement
        *
        * @param vXml  the XML configuration to set
        */
        virtual void setConfig(XmlElement::View vXml) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_CONFIGURABLE_HPP
