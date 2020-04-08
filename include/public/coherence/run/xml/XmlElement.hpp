/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_ELEMENT_HPP
#define COH_XML_ELEMENT_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlValue.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

#include <ostream>

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::List;


/**
* An interface for XML element access. The XmlElement interface represents
* both the element and its content (through the underlying XmlValue
* interface).
*
* @author js  2007.12.14
*/
class COH_EXPORT XmlElement
    : public interface_spec<XmlElement,
        implements<XmlValue> >
    {
    // ----- XmlElement interface -------------------------------------------

    public:
        /**
        * Get the name of the element.
        *
        * @return the element name
        */
        virtual String::View getName() const = 0;

        /**
        * Set the name of the element. This method is intended primarily
        * to be utilized to configure a newly instantiated element
        * before adding it as a child element to another element.
        *
        * @param vsName  the new element name
        */
        virtual void setName(String::View vsName) = 0;

        /**
        * Get the root element.
        *
        * @return the root element for this element
        */
        virtual Handle getRoot() = 0;

        /**
        * Get the root element.
        *
        * @return the root element for this element
        */
        virtual View getRoot() const = 0;

        /**
        * Get the '/'-delimited path of the element starting from the root
        * element.
        *
        * @return the element path
        */
        virtual String::View getAbsolutePath() const = 0;

        /**
        * Get an array of all child elements. The contents of the array
        * implement the XmlValue interface.
        *
        * @return an array containing all child elements
        */
        virtual ObjectArray::Handle getAllElements() const = 0;

        /**
        * Get the list of all child elements.  The contents of the list
        * implement the XmlValue interface.  If this XmlElement is mutable,
        * then the list returned from this method is expected to be mutable
        * as well.
        *
        * @return a List containing all elements of this XmlElement
        */
        virtual List::Handle getElementList() = 0;

        /**
        * Get the list of all child elements.  The contents of the list
        * implement the XmlValue interface.  If this XmlElement is mutable,
        * then the list returned from this method is expected to be mutable
        * as well.
        *
        * @return a List containing all elements of this XmlElement
        */
        virtual List::View getElementList() const = 0;

        /**
        * Get a child element.  If multiple child elements exist that
        * have the specified name, then any matching element may be
        * returned.
        *
        * @param vsName  the name of the element to get
        *
        * @return an element with the specified name or NULL if no
        *         matches are found
        */
        virtual Handle getElement(String::View vsName) = 0;

        /**
        * Get a View to a child element.  If multiple child elements
        * exist that have the specified name, then any matching element
        * may be returned.
        *
        * @param vsName  the name of the element to get
        *
        * @return an element with the specified name or NULL if no
        *         matches are found
        */
        virtual View getElement(String::View vsName) const = 0;

        /**
        * Return the specified child element using the same path notation as
        * supported by findElement, but return a read-only element if the
        * specified element does not exist.
        *
        * <b>This method never returns NULL.</b>
        *
        * This is a convenience method.  Elements are accessed and manipulated
        * via the list returned from getElementList().
        *
        * If multiple child elements exist that have the specified name, then
        * the behavior of this method is undefined, and it is permitted to return
        * any one of the matching elements, to return null, or to throw an
        * arbitrary runtime exception.
        *
        * @param  vsPath element path
        *
        * @return the specified element (never  NULL) as an object implementing
        *         XmlElement for read-only use
        */
        virtual View getSafeElement(String::View vsPath) const = 0;

        /**
        * Get an iterator of child elements that have a specific name.
        *
        * @param vsName  the name of the element to get
        *
        * @return an iterator containing all child elements of the specified
        *         name
        */
        virtual Iterator::Handle getElements(String::View vsName) = 0;

        /**
        * Get an iterator of child elements that have a specific name.
        * Note: This method returns only read-only Views of the elements.
        *
        * @param vsName  the name of the element to get
        *
        * @return an iterator containing all child elements of the specified
        *         name
        */
        virtual Iterator::Handle getElements(String::View vsName) const = 0;

        /**
        * Ensure that a child element exists.
        *
        * This method combines the functionality of findElement() and
        * addElement(). If any part of the path does not exist create new
        * child elements to match the path.
        *
        * @param vsPath  element path
        *
        * @return the existing or new XmlElement object
        *
        * @throws IllegalArgumentException  if the name is NULL or if any
        *         part of the path is not a legal XML tag name
        * @throws UnsupportedOperationException  if unable to add a child
        *         element
        */
        virtual XmlElement::Handle ensureElement(String::View vsPath) = 0;

        /**
        * Create a new element and add it as a child element to this element.
        *
        * @param vsName  the name for the new element
        *
        * @return a handle to the new XmlElement object
        */
        virtual Handle addElement(String::View vsName) = 0;

        /**
        * Find a child element with the specified '/'-delimited path. This is
        * based on a subset of the XPath specification, supporting:
        *
        * <ul>
        *   <li>Leading '/' to specify root</li>
        *   <li>Use of '/' as a path delimiter</li>
        *   <li>Use of '..' to specify parent</li>
        * </ul>
        * </p>
        * If multiple child elements exist that have the specified path,
        * then the behavior of this method is undefined, and it is
        * permitted to return any one of the matching elements.
        *
        * @param vsPath  the element path to search for
        *
        * @return the specified element or NULL if the specified child
        *         element does not exist
        */
        virtual Handle findElement(String::View vsPath) = 0;

        /**
        * Find a child element with the specified '/'-delimited path. This is
        * based on a subset of the XPath specification, supporting:
        *
        * <ul>
        *   <li>Leading '/' to specify root</li>
        *   <li>Use of '/' as a path delimiter</li>
        *   <li>Use of '..' to specify parent</li>
        * </ul>
        * </p>
        * If multiple child elements exist that have the specified path,
        * then the behavior of this method is undefined, and it is
        * permitted to return any one of the matching elements.
        *
        * @param vsPath  the element path to search for
        *
        * @return the specified element or NULL if the specified child
        *         element does not exist
        */
        virtual View findElement(String::View vsPath) const = 0;

        /**
        * Get an attribute value.
        *
        * @param vsName  the name of the attribute
        *
        * @return the attribute corresponding to the specified name or NULL
        *         if the attribute does not exist
        */
        virtual XmlValue::Handle getAttribute(String::View vsName) const = 0;

        /**
        * Get the map of all attributes.  The map is keyed by attribute names.
        * The corresponding values are non-NULL objects that implement the
        * XmlValue interface.
        *
        * @return a Map containing all attributes of this XmlElement; the
        *         return value will never be NULL, although it may be an
        *         empty map
        */
        virtual HashMap::Handle getAttributeMap() = 0;

        /**
        * Get the map of all attributes.  The map is keyed by attribute names.
        * The corresponding values are non-NULL objects that implement the
        * XmlValue interface.
        *
        * @return a Map containing all attributes of this XmlElement; the
        *         return value will never be NULL, although it may be an
        *         empty map
        */
        virtual HashMap::View getAttributeMap() const = 0;

        /**
        * Set an attribute value. If the attribute does not already exist,
        * and the new value is non-NULL, then the attribute is added and its
        * value is set to the passed value. If the attribute does exist, and
        * the new value is non-NULL, then the attribute's value is updated to
        * the passed value. If the attribute does exist, but the new value is
        * NULL, then the attribute and its corresponding value are removed.
        *
        * @param vsName  name of the attribute
        * @param hValue  the new value for the attribute; NULL indicates that
        *                the attribute should be removed
        */
        virtual void setAttribute(String::View vsName,
                XmlValue::Handle hValue) = 0;

        /**
        * Provides a means to add a new attribute value. If the attribute of
        * the same name already exists, it is returned, otherwise a new value
        * is created and added as an attribute.
        *
        * @param vsName  the name of the attribute
        *
        * @return the value of the existing attribute if the attribute
        *         of the same name exist; otherwise the value of a newly
        *         created attribute
        */
        virtual XmlValue::Handle addAttribute(String::View vsName) = 0;

        /**
        * Get an attribute value, and return a temporary value if the attribute
        * does not exist.
        *
        * This is a convenience method.  Attributes are accessed and manipulated
        * via the map returned from getAttributeMap.
        *
        * @param vsName  the name of the attribute
        *
        * @return the value of the specified attribute, or a temporary value if
        *         the attribute does not exist
        *
        * @since Coherence 12.1.2
        */
        virtual XmlValue::Handle getSafeAttribute(String::View vsName) const = 0;

        /**
        * Get the text of any comments that are in the XML element. An
        * element can contain many comments interspersed randomly with
        * textual values and child elements. In reality, comments are rarely
        * used. The purpose of this method and the corresponding mutator are
        * to ensure that if comments do exist, that their text will be
        * accessible through this interface and not lost through a transfer
        * from one instance of this interface to another.
        *
        * @return the comment text from this element (not including the
        *         "\<!--" and "--\>") or NULL if there was no comment
        */
        virtual String::View getComment() const = 0;

        /**
        * Set the text of this element's comment. This interface allows
        * a single comment to be associated with the element. The XML
        * specification does not allow a comment to contain the string "--".
        *
        * @param vsComment  the comment text
        *
        * @throws coherence::lang::IllegalArgumentException
        *         if the comment contains "--"
        */
        virtual void setComment(String::View vsComment) = 0;

        /**
        * Format the element as it will appear in XML.
        *
        * @param fPretty  true to specify that the output is intended to
        *                 be as human readable as possible
        * @param cIndent  the number of spaces to indent each line
        *
        * @return the XML formatted string.
        */
        virtual String::View formatXml(bool fPretty = false, size32_t cIndent = 0) const = 0;

        /**
        * Write the element as it will appear in XML.
        *
        * @param out      an std::ostream object to use to write to
        * @param fPretty  true to specify that the output is intended to
        *                 be as human readable as possible
        * @param cIndent  the number of spaces to indent each line
        */
        COH_INLINE void writeXml(std::ostream& out, bool fPretty = false, size32_t cIndent = 0) const
            {
            out << formatXml(fPretty, cIndent);
            }
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_ELEMENT_HPP
