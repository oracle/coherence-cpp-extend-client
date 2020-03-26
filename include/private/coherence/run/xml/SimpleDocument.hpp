/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_DOCUMENT_HPP
#define COH_SIMPLE_DOCUMENT_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlDocument.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* A simple implementation of the XmlElement interface.
*
* @author js  2007.12.16
*/
class COH_EXPORT SimpleDocument
    : public cloneable_spec<SimpleDocument,
        extends<SimpleElement>,
        implements<XmlDocument> >
    {
    friend class factory<SimpleDocument>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a SimpleDocument.
        *
        * @param vsName  the name of the root element, NULL by default
        */
        SimpleDocument(String::View vsName = String::null_string);

        /**
        * Construct a SimpleDocument.
        *
        * @param vsName     the name of the root element
        * @param vsDtdUri   the URI of the DTD (system identifier)
        * @param vsDtdName  the name of the DTD (public identifier); may
        *                   be NULL
        */
        SimpleDocument(String::View vsName, String::View vsDtdUri,
                String::View vsDtdName);

        /**
        * Copy constructor.
        */
        SimpleDocument(const SimpleDocument& that);


    // ----- XmlDocument interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDtdUri() const;

        /**
        * {@inheritDoc}
        */
        virtual void setDtdUri(String::View vsUri);

        /**
        * {@inheritDoc}
        */
        virtual String::View getDtdName() const;

        /**
        * {@inheritDoc}
        */
        virtual void setDtdName(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual String::View getEncoding() const;

        /**
        * {@inheritDoc}
        */
        virtual void setEncoding(String::View vsEncoding);

        /**
        * {@inheritDoc}
        */
        virtual String::View getDocumentComment() const;

        /**
        * {@inheritDoc}
        */
        virtual void setDocumentComment(String::View vsComment);

        /**
        * {@inheritDoc}
        */
        virtual String::View getXml() const;


    // ----- XmlElement interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View formatXml(bool fPretty = false, size32_t cIndent = 0) const;

  
    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * URI of the DTD.
        */
        FinalView<String> f_vsDtdUri;

        /**
        * Name of the DTD.
        */
        FinalView<String> f_vsDtdName;

        /**
        * Document encoding.
        */
        FinalView<String> f_vsEncoding;

        /**
        * Document comment.
        */
        MemberView<String> m_vsDocComment;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_DOCUMENT_HPP
