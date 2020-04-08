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
* An interface for XML document access.  The XmlDocumnet interface represents
* the document as both the root element (through the underlying XmlElement
* interface) and the properties specific to a document, such as DOCTYPE.
*
* @author js  2007.12.13
*/
class COH_EXPORT XmlDocument
    : public interface_spec<XmlDocument,
        implements<XmlElement> >
    {
    // ----- XmlDocument interface ------------------------------------------

    public:
        /**
        * Get the URI of the DTD (DOCTYPE) for the document. This is referred
        * to as the System Identifier by the XML specification.
        *
        * For example:
        *   http://java.sun.com/j2ee/dtds/web-app_2_2.dtd
        *
        * @return the document type URI
        */
        virtual String::View getDtdUri() const = 0;

        /**
        * Set the URI of the DTD (DOCTYPE) for the document. This is referred
        * to as the System Identifier by the XML specification.
        *
        * @param vsUri  the document type URI
        */
        virtual void setDtdUri(String::View vsUri) = 0;

        /**
        * Get the public identifier of the DTD (DOCTYPE) for the document.
        *
        * For example:
        *   -//Sun Microsystems, Inc.//DTD Web Application 1.2//EN
        *
        * @return the DTD public identifier
        */
        virtual String::View getDtdName() const = 0;

        /**
        * Set the public identifier of the DTD (DOCTYPE) for the document.
        *
        * @param vsName  the DTD public identifier
        */
        virtual void setDtdName(String::View vsName) = 0;

        /**
        * Get the encoding string for the XML document. Documents that are
        * parsed may or may not have the encoding string from the persistent
        * form of the document.
        *
        * @return the encoding set for the document
        */
        virtual String::View getEncoding() const = 0;

        /**
        * Set the encoding string for the XML document.
        *
        * @param vsEncoding  the encoding that the document will use
        */
        virtual void setEncoding(String::View vsEncoding) = 0;

        /**
        * Get the XML comment that appears outside of the root element. This
        * differs from the comment property of this object, which refers to
        * the comment within the root element.
        *
        * @return the document comment
        */
        virtual String::View getDocumentComment() const = 0;

        /**
        * Set the XML comment that appears outside of the root element. This
        * differs from the Comment property of this object, which refers to
        * the comment within the root element.
        *
        * @param vsComment  the document comment
        */
        virtual void setDocumentComment(String::View vsComment) = 0;

        /**
        * @return the document as a string
        */
        virtual String::View getXml() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_DOCUMENT_HPP
