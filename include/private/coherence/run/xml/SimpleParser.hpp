/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_PARSER_HPP
#define COH_SIMPLE_PARSER_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlDocument.hpp"

#include "private/coherence/run/xml/XmlToken.hpp"
#include "private/coherence/run/xml/XmlTokenizer.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)


/**
* The XML parser that uses XmlTokenizer to produce an XmlDocument from
* XML text.
*
* @author tb  2007.12.13
*/
class COH_EXPORT SimpleParser
    : public class_spec<SimpleParser>
    {
    friend class factory<SimpleParser>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new SimpleParser.
        *
        * @return a handle to the new SimpleParser
        */
        SimpleParser();


    // ----- SimpleParser interface -----------------------------------------

    public:
        /**
        * Parse the specified String into an XmlDocument object.
        *
        * @param vsXml  the String to parse
        *
        * @return an XmlDocument object
        *
        * @throws IOException if an error occurs during parsing
        */
        virtual XmlDocument::Handle parseXml(String::View vsXml);

        /**
        * Parse the specified istream into an XmlDocument object.
        *
        * @param stream the istream object
        *
        * @return an XmlDocument object
        *
        * @throws IOException if an error occurs during parsing
        */
        virtual XmlDocument::Handle parseXml(std::istream& stream);


    // ----- implementation -------------------------------------------------

    protected :
        /**
        * Parse a given XML document.
        *
        * @param hXml  a blank XmlDocument
        */
        virtual void parseDocument(XmlDocument::Handle hXml);

        /**
        * Parse a processing instruction (PI) from the given element.
        *
        * @param hXml  an xml element containing a PI
        */
        virtual void parsePi(XmlElement::Handle hXml);

        /**
        * Parse a processing instruction (PI) from the given element.
        *
        * @param hXml             an xml element containing a PI
        * @param fXmlDeclAllowed  true if an XML declaration is allowed;
        *                         false otherwise
        */
        virtual void parsePi(XmlElement::Handle hXml, bool fXmlDeclAllowed);

        /**
        * Parse an XML declaration from a given XML document.
        *
        * @param hXml  an XmlDocument
        */
        virtual void parseXmlDecl(XmlDocument::Handle hXml);

        /**
        * Parse an XML declaration from a given XML document.
        *
        * @param hXml  an XmlDocument
        */
        virtual void parseDoctype(XmlDocument::Handle hXml);

        /**
        * Parse an XML element.
        * <p/>
        * Note: '<' and element name have already been parsed
        *
        * @param hXml  an xml element
        */
        virtual void parseElement(XmlElement::Handle hXml);

        /**
        * Parse an XML element.
        *
        * @param hXml  an xml element
        */
        virtual void parseMisc(XmlElement::Handle hXml);

        /**
        * Parse an XML comment from a given element.
        *
        * @param hXml  an xml element
        */
        virtual void parseComment(XmlElement::Handle hXml);

        /**
        * Parse an XML comment from a given element.
        *
        * @param hXml         an xml element
        * @param fIsDocument  true if the passed in element a document
        */
        virtual void parseComment(XmlElement::Handle hXml, bool fIsDocument);


    // ----- parsing helpers ------------------------------------------------

    protected :
        /**
        * Determine if there is a current token.
        *
        * @return true if there is a current token
        */
        virtual bool hasCurrent();

        /**
        * Returns the current token and advances to the next token.
        *
        * @return the current token
        *
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View current();

        /**
        * Determine if there is a next token.
        *
        * @return true if there is a next token
        */
        virtual bool hasNext();

        /**
        * Advances to and returns the next token.
        *
        * @return the next token
        *
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View next();

        /**
        * Verifies that the current token matches the passed token id and, if
        * so, advances to the next token.  Otherwise, a syntax exception is
        * thrown.
        *
        * @param id  the token nId to match
        *
        * @return the current token
        *
        * @exception SyntaxException    thrown if the token does not match
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View match(int32_t nId);

        /**
        * Verifies that the current token is a name token whose name matches
        * the passed String and, if so, advances to the next token.
        * Otherwise, a syntax exception is thrown.
        *
        * @param vsName  the name token text to match
        *
        * @return the matched token
        *
        * @exception SyntaxException    thrown if the token does not match
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View match(String::View vsName);

        /**
        * Tests if the current token matches the passed token id and, if so,
        * advances to the next token.
        *
        * @param nId the token id to peek for
        *
        * @return the current token, if matched, or NULL
        *
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View peek(int32_t nId);

        /**
        * Tests if the current token matches the passed token category and
        * sub-category.  If so, it returns the current token and advances
        * to the next token.
        *
        * @param nCat     the category to peek for
        * @param nSubCat  the sub-category to peek for
        *
        * @return the current token, if matched, or NULL
        *
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View peek(int32_t nCat, int32_t nSubCat);

        /**
        * Tests if the current token is a name that matches the passed String
        * and, if so, advances to the next token.
        *
        * @param vsName  the name token text to peek for
        *
        * @return id the current token, if matched, or null
        *
        * @exception CompilerException  potentially thrown by the tokenizer
        */
        virtual XmlToken::View peek(String::View vsName);

        /**
        * Marks the current position and returns it as a token.
        *
        * @return the current token
        */
        virtual XmlToken::View mark();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The lexical tokenizer.
        */
        FinalHandle<XmlTokenizer> f_hTokenizer;

        /**
        * The "current" token being evaluated.
        */
        MemberView<XmlToken> m_vToken;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_PARSER_HPP
