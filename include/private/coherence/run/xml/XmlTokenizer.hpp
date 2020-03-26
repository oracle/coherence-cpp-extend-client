/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_TOKENIZER_HPP
#define COH_XML_TOKENIZER_HPP

#include "coherence/lang.ns"

#include "private/coherence/dev/compiler/CompilerException.hpp"
#include "private/coherence/dev/compiler/Script.hpp"
#include "private/coherence/dev/compiler/Tokenizer.hpp"
#include "private/coherence/run/xml/ErrorDescription.hpp"
#include "private/coherence/util/Dequeue.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::dev::compiler::CompilerException;
using coherence::dev::compiler::ErrorList;
using coherence::dev::compiler::ParsePosition;
using coherence::dev::compiler::Script;
using coherence::dev::compiler::Token;
using coherence::dev::compiler::Tokenizer;
using coherence::util::Dequeue;


/**
* Converts a character stream (Script object) into XML tokens.
*
* @author tb  2007.12.13
*/
class COH_EXPORT XmlTokenizer
    : public class_spec<XmlTokenizer,
        extends<Object>,
        implements<Tokenizer> >
    {
    friend class factory<XmlTokenizer>;

    // ----- enums ----------------------------------------------------------

    private:
        // the state of the tokenizer
        enum Context
            {
            ctx_outside,     // starting state ... outside of any element
            ctx_xmldecl,     // in an XML decl processing instruction
            ctx_doctype,     // in a DOCTYPE element
            ctx_comment,     // in a XML comment
            ctx_pi,          // in a processing instruction <? ...
            ctx_element,     // in an XML element
            ctx_dtd,         // in a DTD element
            ctx_dtd_comment  // in a DTD comment
            };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new XmlTokenizer instance
        */
        XmlTokenizer();

        /**
        * Create a new XmlTokenizer instance initialized with the given
        * script and error list.
        *
        * @param hScript   the XML script to tokenize
        * @param hErrList  the error list which the Tokenizer will log to
        */
        XmlTokenizer(Script::Handle hScript, ErrorList::Handle hErrList);


    // ----- Tokenizer interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void setScript(Script::Handle hScript,
                ErrorList::Handle hErrList);

        /**
        * {@inheritDoc}
        */
        virtual bool hasMoreTokens() const;

        /**
        * {@inheritDoc}
        */
        virtual Token::View nextToken();

        /**
        * {@inheritDoc}
        */
        virtual void putBackToken(Token::View vToken);

        /**
        * {@inheritDoc}
        */
        virtual ParsePosition::Handle savePosition() const;

        /**
        * {@inheritDoc}
        */
        virtual void restorePosition(ParsePosition::Handle hParsePos);


    // ----- script parsing -------------------------------------------------

    protected:
        /**
        * Eats whitespace and comments until the next XML token is
        * encountered or the end of the string is reached, whichever comes
        * first.
        *
        * @exception CompilerException  If a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual void eatFluff();

        /**
        * Eats the largest token possible, returning a Token interface which
        * describes the token.
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatToken();

        /**
        * Eat a token from the "outside of markup" context.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatOutside();

        /**
        * Eat a token from the XML declaration section.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatXmlDecl();

        /**
        * Eat a token from the DOCTYPE section.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatDocType();

        /**
        * Eat a token from the DOCTYPE's embedded DTD section.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatDtd();

        /**
        * Eat a token from a comment.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatComment();

        /**
        * Eat a token from a processing instruction (PI).
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatPi();

        /**
        * Eat a token from an element.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatElement();

        /**
        * Eat a name token.
        *
        * @return the next Token
        *
        * @exception CompilerException  if a lexical error is encountered
        *            which should stop compilation, like an unexpected
        *            end-of-file
        */
        virtual Token::View eatName();


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * @return true iff the passed character a starting character for an
        *         XML name
        */
        static bool isNameStartChar(char ch);

        /**
        * @return true iff the passed character a valid character in an XML
        *         name
        */
        static bool isNameChar(char ch);

        /**
        * Peek for the specified character in the data being streamed from
        * the Script object. Advance past if found. Otherwise return false.
        *
        * @param ch  the character to peek
        *
        * @return true if peek found it, otherwise false
        */
        virtual bool peek(char ch);

        /**
        * Match the specified character in the data being streamed from the
        * Script object.
        *
        * @param ch  the character to match
        *
        * @exception CompilerException if the character can't be matched
        */
        virtual void match(char ch);

        /**
        * Match the specified String in the data being streamed from the
        * Script object.
        *
        * @param vsString  the String to match
        *
        * @exception CompilerException if the character can't be matched
        */
        virtual void match(String::View vsString);

        /**
        * Match one whitespace character.
        */
        virtual void matchWhitespace();

        /**
        * Scan until the specified String is encountered.
        *
        * @param ch  the character to scan for
        */
        virtual void scan(char ch);

        /**
        * Scan until the specified String is encountered.
        *
        * @param vs  the String to scan for
        */
        virtual void scan(String::View vs);


    // ----- error handling -------------------------------------------------

    protected:
        /**
        * Logs an internal error and throws a CompilerException.
        *
        * @exception CompilerException thrown unconditionally
        */
        virtual void internalError();

        /**
        * Logs the passed error in the error list.
        *
        * @param iLine      Ending line containing the error.
        * @param ofInLine   Offset of the error within the line.
        * @param severity   Severity of the error
        * @param vsCode     Error code
        * @param vsMessage  Error message
        */
        virtual void logError(size32_t iLine, size32_t ofInLine,
                ErrorDescription::Severity severity, String::View vsCode,
                String::View vsMessage);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Get the error code for an internal error.
        *
        * @return the error code for an internal error.
        */
        static String::View getInternalErrorCode();

        /**
        * Get the error code for an unexpected EOF error.
        *
        * @return the error code for an unexpected EOF error.
        */
        static String::View getUnexpectedEofErrorCode();

        /**
        * Get the error code for an unexpected IO error.
        *
        * @return the error code for an unexpected IO error.
        */
        static String::View getUnexpectedIoErrorCode();

        /**
        * Get the error code for an XML format error.
        *
        * @return the error code for an XML format error.
        */
        static String::View getXmlFormatErrorCode();

        /**
        * Get the error code for an char format error.
        *
        * @return the error code for an char format error.
        */
        static String::View getCharExpectedErrorCode();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Context-sensitive parsing indicator.
        */
        enum Context m_context;

        /**
        * The script to parse.
        */
        FinalHandle<Script> f_hScript;

        /**
        * The "put back" queue.
        */
        MemberHandle<Dequeue> m_hDequeue;

        /**
        * The error list to log to.
        */
        FinalHandle<ErrorList> f_hErrList;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_TOKENIZER_HPP
