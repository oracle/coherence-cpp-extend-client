/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_ELEMENT_HPP
#define COH_SIMPLE_ELEMENT_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/List.hpp"

#include "private/coherence/run/xml/SimpleValue.hpp"

#include <map>


COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::util::List;


/**
* A simple implementation of the XmlElement interface.
*
* @author js  2007.12.18
*/

class COH_EXPORT SimpleElement
    : public cloneable_spec<SimpleElement,
        extends<SimpleValue>,
        implements<XmlElement> >
    {
    friend class factory<SimpleElement>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SimpleElement with an optional name.
        *
        * @param vsName  name of the element, defaults to NULL
        */
        SimpleElement(String::View vsName = String::null_string);

        /**
        * Create a SimpleElement with a name and string value.
        *
        * @param vsName   name of the element
        * @param vsValue  value as a String
        */
        SimpleElement(String::View vsName, String::View vsValue);

        /**
        * Create a SimpleElement with a name and boolean value.
        *
        * @param vsName  name of the element
        * @param fValue  value as a bool
        */
        SimpleElement(String::View vsName, int32_t nValue);

        /**
        * Create a SimpleElement with a name and integer value.
        *
        * @param vsName  name of the element
        * @param nValue  value as an int32_t
        */
        SimpleElement(String::View vsName, bool fValue);

        /**
        * Copy constructor.
        */
        SimpleElement(const SimpleElement& that);


    // ----- XmlElement interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual XmlValue::Handle addAttribute(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::Handle addElement(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::Handle findElement(String::View vsPath);

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View findElement(String::View vsPath) const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getAbsolutePath() const;

        /**
        * {@inheritDoc}
        */
        virtual XmlValue::Handle getAttribute(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual XmlValue::Handle getSafeAttribute(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual HashMap::Handle getAttributeMap();

        /**
        * {@inheritDoc}
        */
        virtual HashMap::View getAttributeMap() const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getComment() const;

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::Handle getElement(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle getElements(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View getElement(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle getElements(String::View vsName) const;

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View getSafeElement(String::View vsPath) const;

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle getAllElements() const;

        /**
        * {@inheritDoc}
        */
        virtual List::Handle getElementList();

        /**
        * {@inheritDoc}
        */
        virtual List::View getElementList() const;

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::Handle ensureElement(String::View vsPath);

        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::Handle getRoot();

        /**
        * {@inheritDoc}
        */
        virtual XmlElement::View getRoot() const;

        /**
        * {@inheritDoc}
        */
        virtual void setAttribute(String::View vsName,
                XmlValue::Handle hValue);

        /**
        * {@inheritDoc}
        */
        virtual void setComment(String::View vsComment);

        /**
        * {@inheritDoc}
        */
        virtual void setName(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual String::View formatXml(bool fPretty = false, size32_t nIndent = 0) const;


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


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Write the element as a combined start/end tag.
        *
        * @param out  an std::ostream to write to
        */
        virtual void writeEmptyTag(std::ostream& out) const;

        /**
        * Write the element's start tag.
        *
        * @param out  an std::ostream to write to
        */
        virtual void writeStartTag(std::ostream& out) const;

        /**
        * Write the element's end tag.
        *
        * @param out  an std::ostream to write to
        */
        virtual void writeEndTag(std::ostream& out) const;

        /**
        * Write the attributes as part of a start tag.
        *
        * @param out  an std::ostream to write to
        */
        virtual void writeAttributes(std::ostream& out) const;

        /**
        * Write the element as it will appear in XML.
        *
        * @param out      an std::ostream object to use to write to
        * @param fPretty  true to specify that the output is intended to
        *                 be as human readable as possible
        * @param nIndent  the number of spaces to indent each line
        */
        virtual void writeChildren(std::ostream& out, bool fPretty,
                size32_t nIndent = 0) const;

        /**
        * Write the element as it will appear in XML.
        *
        * @param out      an std::ostream object to use to write to
        * @param fPretty  true to specify that the output is intended to
        *                 be as human readable as possible
        * @param nIndent  the number of spaces to indent each line
        */
        virtual void writeComment(std::ostream& out, bool fPretty,
                size32_t nIndent = 0) const;

        /**
        * Used internally to obtain an element which can be returned from
        * either a const or non-const public getter.
        *
        * @param vsName  the name of the element to get
        *
        * @return an element with the specified name or NULL if no
        *         matches are found
        */
        virtual Object::Holder getElementInternal(String::View vsName) const;


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
        * Name of the element.
        */
        MemberView<String> m_vsName;

        /**
        * Element comment.
        */
        MemberView<String> m_vsComment;

        /**
        * List holding XmlElement::Handle child elements.
        */
        MemberHandle<List> m_hListElements;

        /**
        * Map of attributes.
        */
        MemberHandle<HashMap> m_hMapAttributes;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_ELEMENT_HPP
