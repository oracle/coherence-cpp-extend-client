/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_HELPER_HPP
#define COH_XML_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlDocument.hpp"
#include "coherence/run/xml/XmlElement.hpp"

#include <iostream>

COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* XML related static utility functions.
*
* @author js  2007.12.18
* @author tb  2007.12.18
*/
class COH_EXPORT XmlHelper
    : public abstract_spec<XmlHelper>
    {
    // ----- Xml loading helpers --------------------------------------------

    public:
        /**
        * Load XML from a stream.
        *
        * @param stream  the stream from which to read the XML content
        *
        * @return the XML content
        *
        * @since Coherence 3.7
        */
        static XmlDocument::Handle loadXml(std::istream& stream);

        /**
        * Load XML from a string
        *
        * @param vsXml  the string from which to read the XML content
        *
        * @return the XML content
        *
        * @since Coherence 12.2.1
        */
        static XmlDocument::Handle loadXml(String::View vsXml);

        /**
        * Load the configuration from a file.
        *
        * @param vsName   the name of the file
        * @param vsDescr  a description of the file being loaded (e.g.
        *                 "cache configuration").  The description is only
        *                 used in logging and error messages related to
        *                 loading the file
        *
        * @return the configuration XML
        *
        * @since Coherence 3.7
        */
        static XmlDocument::Handle loadFile(String::View vsName, String::View vsDescr);


    // ----- utility functions ----------------------------------------------

    public:
        /**
        * Encode an attribute value so that it can be quoted and made part of
        * a valid and well formed XML document.
        *
        * @param vsValue  the attribute value to encode
        * @param chQuote  the character that will be used to quote the
        *                 attribute
        *
        * @return the attribute value in its encoded form (but not quoted)
        */
        static String::View encodeAttribute(String::View vsValue,
                char chQuote);

        /**
        * XML quote the passed string.
        *
        * @param vsVal  the string to quote
        *
        * @return the quoted string
        */
        static String::View quote(String::View vsVal);

        /**
        * Encode an element's content value so that it can be made part of a
        * valid and well formed XML document.
        *
        * @param vsValue             the content value to encode
        * @param fPreferBlockEscape  pass true to use the CDATA escape if two
        *                            conditions are met:  that escaping is
        *                            required, and that the value does not
        *                            contain the string "]]>"
        *
        * @return the attribute value in its encoded form (but not quoted)
        */
        static String::View encodeContent(String::View vsValue,
                bool fPreferBlockEscape);

        /**
        * Ensure that a child element exists.
        *
        * If any part of the path does not exist create new child
        * elements to match the path.
        *
        * @param hXml   an XmlElement
        * @param sPath  element path
        *
        * @return the existing or new XmlElement object
        *
        * @throws IllegalArgumentException  if the name is null or if any part
        *         of the path is not a legal XML tag name
        * @throws UnsupportedOperationException if any element in the path
        *         is immutable or otherwise can not add a child element
        *
        * @see findElement(XmlElement, String)
        */
        static XmlElement::Handle ensureElement(XmlElement::Handle hXml,
                String::View vsPath);

        /**
        * Test if the specified character is XML whitespace.
        *
        * @param ch  a character
        *
        * @return true if the passed character is XML whitespace
        */
        static bool isWhitespace(char ch);

        /**
        * Encode a System Identifier as per the XML 1.0 Specification
        * second edition, section 4.2.2.
        * <p/>
        * From the XML 1.0 specification, section 4.2.2, URI references
        * require encoding and escaping of certain characters. The disallowed
        * characters include all non-ASCII characters, plus the excluded
        * characters listed in Section 2.4 of [IETF RFC 2396], except for the
        * number sign (#) and percent sign (%) characters and the square
        * bracket characters re-allowed in [IETF RFC 2732].
        *
        * @param vsUri  the URI to encode
        *
        * @return the encoded URI
        */
        static String::View encodeUri(String::View vsUri);

        /**
        * Breaks the specified string into a multi-line string.
        *
        * @param vsText   the string to break
        * @param nWidth   the max width of resulting lines, including
         *                the indent
        * @param nIndent  the number of spaces to indent
        *
        * @return the string, broken and indented
        */
        static String::View breakLines(String::View vsText, size32_t nWidth,
                size32_t nIndent = 0);

        /**
        * Validate the passed encoding.  Encodings are strings defined as:
        * <p/>
        * [A-Za-z] ([A-Za-z0-9._] | '-')*
        *
        * @param vsEncoding  the document encoding
        *
        * @return true if the encoding is valid, false otherwise
        */
        static bool isEncodingValid(String::View vsEncoding);

        /**
        * Decode an attribute value that was quoted.
        *
        * @param vsValue  the attribute value to decode
        *
        * @return the attribute value in its decoded form
        */
        static String::View decodeAttribute(String::View vsValue);

        /**
        * Validate the passed public identifier.
        *
        * @param vsName  the public identifier of the XML document
        *
        * @return true if the identifier is valid, false otherwise
        */
        static bool isPublicIdentifierValid(String::View vsName);

        /**
        * Decode a System Identifier as per the XML 1.0 Specification 2nd ed
        * section 4.2.2.
        *
        * @param vsUri  the URI to decode
        *
        * @return the decoded URI
        */
        static String::View decodeUri(String::View vsUri);

        /**
        * Validate the passed system identifier.
        *
        * @param vsName  the system identifier of the XML document
        *
        * @return true if the identifier is valid, false otherwise
        */
        static bool isSystemIdentifierValid(String::View vsName);

        /**
        * Validate the passed name.  Currently, this does not allow the
        * "CombiningChar" or "Extender" characters that are allowed by the
        * XML specification 1.0 2ed section 2.3 [4].
        *
        * @param vsName  the XML name to validate
        *
        * @return true if the name is valid, false otherwise
        */
        static bool isNameValid(String::View vsName);

        /**
        * Decode an element's content value.
        *
        * @param vsValue  the content value to decode
        *
        * @return the attribute value in its decoded form
        */
        static String::View decodeContent(String::View vsValue);

        /**
        * Trim XML whitespace.  See XML 1.0 2ed section 2.3.
        *
        * @param vs  the original string
        *
        * @return the passed string minus any leading or trailing whitespace
        */
        static String::View trim(String::View vs);

        /**
        * Write a newline followed by the specified number of spaces to a
        * stream.
        *
        * @param out      the std::ostream object to write to
        * @param nIndent  the number of spaces in the indent
        */
        static void writeNewLineAndIndent(std::ostream& out, size32_t nIndent);

        /**
        * Parse the specified "init-params" element of the following structure:
        * <pre>
        * &lt;!ELEMENT init-params (init-param*)&gt;
        * &lt;!ELEMENT init-param ((param-name | param-type), param-value, description?)&gt;
        * </pre>
        * into an object array.
        * 
        * For the purpose of this method only the parameters that have the "param-type"
        * element specified are processed. The following types are supported:
        * <ul>
        * <li> string   (a.k.a. coherence::lang::String)
        * <li> boolean  (a.k.a. coherence::lang::Boolean)
        * <li> int      (a.k.a. coherence::lang::Integer32)
        * <li> xml      (a.k.a. coherence::run::xml::XmlElement)
        * </ul>
        * For any other [explicitly specified] types the corresponding
        * "init-param" XmlElement itself is placed into the returned array.
        *
        * @param vXmlParams  the "init-params" XmlElement to parse
        *
        * @return an array of parameters
        */
        static ObjectArray::View parseInitParams(XmlElement::View vXmlParams);

        /**
        * Transform the specified "init-params" element of the following
        * structure:
        * <pre>
        * [!ELEMENT init-params (init-param*)]
        * [!ELEMENT init-param ((param-name | param-type), param-value,
        * description?)]
        * </pre>
        * into an XML element composed of the corrsponding names.  For
        * example, the the "init-params" element of the following structure:
        * <pre>
        *   &lt;init-param&gt;
        *      &lt;param-name&gt;NameOne&lt;/param-name&gt;
        *      &lt;param-value&gt;ValueOne&lt;/param-value&gt;
        *   &lt;/init-param&gt;
        *   &lt;init-param&gt;
        *      &lt;param-name&gt;NameTwo&lt;/param-name&gt;
        *      &lt;param-value&gt;ValueTwo&lt;/param-value&gt;
        *   &lt;/init-param&gt;
        * </pre>
        * will transform into
        * <pre>
        *   &lt;NameOne&gt;ValueOne&lt;/NameOne&gt;
        *   &lt;NameTwo&gt;ValueTwo&lt;/NameTwo&gt;
        * </pre>
        * For the purpose of this method only the parameters that have the
        * "param-name" element specified are processed.
        *
        * @param hXmlParent  the XML element to insert the transformed
        *                    elements into
        * @param vXmlParams  the "init-params" XmlElement to parse
        *
        * @return the resulting XmlElement (xmlParent)
        */
        static XmlElement::Handle transformInitParams(XmlElement::Handle hXmlParent,
                XmlElement::View vXmlParams);

        /**
         * Process possible macro(s) with in a XmlElement's value.
         * The macro syntax is "${system-property default-value}".
         * Thus, a value of "${coherence.remote.client near}-remote" is macro expanded by default to "near-remote".
         * If system property "coherence.remote.client" is set to "thin", then the value is expanded to "thin-remote".
         *
         * @param hXml  the XmlElement to process
         *
         * @return true if the XmlElement's value contains one or more macros and the element's value was updated by this method.
         */
        static bool processMacros(XmlElement::Handle hXml);

        /**
        * Replace the values of the XmlElement and all its children that
        * contain the specified attribute with the values returned by the
        * <tt>System.getProperty()</tt> call.
        * <p/>
        * This method iterates the specified XmlElement tree and for each
        * element that contains the attribute with the specified name
        * replaces its value with the value of the corresponding system
        * property (if exists).
        *
        * @param hXml                 the XmlElement to process
        * @param vsPropertyAttribute  name of the attribute that supplies the
        *                             corresponding system property name
        */
        static void replaceSystemProperties(XmlElement::Handle hXml,
                String::View vsPropertyAttribute);

        /**
        * Remove all immediate child elements with the given name
        *
        * @param hXml    an XmlElement
        * @param vsName  child element name
        *
        * @return the number of removed child elements
        *
        * @throws UnsupportedOperationException if the element is immutable
        *         or otherwise cannot remove a child element
        *
        * @since Coherence 12.1.2
        */
        static int removeElement(XmlElement::Handle hXml, String::View vsName);

        /**
        * Replace a child element with the same name as the specified element.
        * If the child element does not exist the specified element is just
        * added.
        *
        * @param hXmlParent   parent XmlElement
        * @param vXmlReplace  element to replace with
        *
        * @return true if matching child element has been found and replaced;
        *         false otherwise
        *
        * @throws UnsupportedOperationException if the parent element is
        *         immutable or otherwise cannot remove a child element
        */
        static bool replaceElement(XmlElement::Handle hXmlParent,
                XmlElement::View vXmlReplace);

        /**
        * Check whether or not this element or any of its children elements
        * have any content such as values or attributes.
        *
        * @param vXml  an XmlElement
        *
        * @return true iff the element itself and all of its children have
        *         neither values nor attributes
        */
        static bool isEmpty(XmlElement::View vXml);

        /**
        * Converts a string to upper case.
        *
        * @param vs  the string to convert to upper case
        *
        * @return the new upper case string
        */
        static String::View toUpperCase(String::View vs);

        /**
        * Create an instance of the class configured using an XmlElement of the
        * following structure:
        * <pre>
        *   &lt;!ELEMENT ... (class-name | (class-factory-name, method-name), init-params?&gt;
        *   &lt;!ELEMENT init-params (init-param*)&gt;
        *   &lt;!ELEMENT init-param ((param-name | param-type), param-value, description?)&gt;
        * </pre>
        *
        * As of Coherence 3.6 the supplied element may also be of the following format:
        * <pre>
        * &lt;!ELEMENT instance&gt;
        * </pre>
        * where the "instance" format is the same as above.
        *
        * @param vXml            the XML element that contains the instantiation info
        * @param vLoader         a ClassLoader that should be used to load necessary
        *                        classes (optional)
        * @param vClzAssignable  if non-NULL, this method will validate that
        *                        the Class is assignable from the loaded Class
        *
        * @return an object instantiated or obtained based on the class configuration
        */
        static Object::Handle createInstance(XmlElement::View vXml,
                ClassLoader::View vLoader = NULL,
                Class::View vClzAssignable = NULL);

        // ----- type conversions -----------------------------------------------
    
        /**
        * Convert the passed Object to the specified type.
        *
        * @param v      the object value or NULL
        * @param nType  the enumerated type to convert to
        *
        * @return an object of the specified type
        */
        static Object::View convert(Object::View v, int32_t nType);


    // ----- protected class functions --------------------------------------

    protected:
        /**
        * Determines if a character is a control character.
        *
        * @param ch  the character to check
        *
        * @return true if the character is a control character, false
        *         otherwise.
        */
        static bool isControlChar(char ch);

        /**
        * Encodes a character and writes it to a provided std::ostream.
        *
        * @param out  the std::ostream that the encoded char is written to
        * @param ch   the character to be encoded
        */
        static void writeEncodedChar(std::ostream& out, char ch);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_HELPER_HPP
