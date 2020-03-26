/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_DOCUMENT_HPP
#define COH_XML_DOCUMENT_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* An interface for XML document access. The XmlDocument interface represents
* the document as both the root element (through the underlying XmlElement
* interface) and the properties specific to a document, such as DOCTYPE.
*/
class COH_EXPORT XmlDocument
    : public interface_spec<XmlDocument,
        implements<XmlElement> >
    {
    // ----- XmlDocument interface ------------------------------------------
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_DOCUMENT_HPP
