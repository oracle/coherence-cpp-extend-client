/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/XmlTokenizer.hpp"

#include "private/coherence/run/xml/XmlScript.hpp"
#include "private/coherence/run/xml/XmlToken.hpp"

#include <sstream>

COH_OPEN_NAMESPACE3(coherence,run,xml)


// ----- local class: Position ----------------------------------------------

/**
* Stores all information required to later restore the current position in
* the script.
*
* @author tb  2007.01.17
*/
class Position
    : public class_spec<Position,
        extends<Object>,
        implements<ParsePosition> >
    {
    friend class factory<Position>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
        * Create a new Position instance.
        *
        * @param hDequeue    the associated dequeue
        * @param hScriptPos  the parse position from the script
        */
        Position(Dequeue::Handle hDequeue, ParsePosition::Handle hScriptPos)
                : f_hDequeue(self(), hDequeue),
                  f_hScriptPos(self(), hScriptPos)
            {
            }

    // ----- data members -----------------------------------------------

    public:
        /**
        * The associated dequeue.
        */
        FinalHandle<Dequeue> f_hDequeue;

        /**
        * The parse position from the script
        */
        FinalHandle<ParsePosition> f_hScriptPos;
    };


// ----- constructors -------------------------------------------------------

XmlTokenizer::XmlTokenizer()
    : m_context(ctx_outside),
      f_hScript(self()),
      m_hDequeue(self(), Dequeue::create()),
      f_hErrList(self())
    {
    }

XmlTokenizer::XmlTokenizer(Script::Handle hScript, ErrorList::Handle hErrlist)
    : m_context(ctx_outside),
      f_hScript(self(), hScript),
      m_hDequeue(self(), Dequeue::create()),
      f_hErrList(self(), hErrlist)
    {
    }


// ----- Tokenizer interface ------------------------------------------------

void XmlTokenizer::setScript(Script::Handle hScript,
        ErrorList::Handle hErrList)
    {
    COH_ENSURE_PARAM(hScript);
    // this method may only be called once
    COH_ENSURE(is_null(f_hScript));

    initialize(f_hErrList, hErrList);
    initialize(f_hScript, hScript);

    // eat up to the next token
    eatFluff();
    }

bool XmlTokenizer::hasMoreTokens() const
    {
    if (is_null(f_hScript))
        {
        return false;
        }
    // first check for any tokens that were "put back"
    if (m_hDequeue->hasNext())
        {
        return true;
        }
    // check if there is more to parse
    return f_hScript->hasMoreChars();
    }

Token::View XmlTokenizer::nextToken()
    {
    Token::View vToken;

    Dequeue::Handle hDequeue = m_hDequeue;
    if (hDequeue->hasNext())
        {
        vToken = cast<Token::View>(hDequeue->next());
        }
    else if (f_hScript->hasMoreChars())
        {
        vToken = eatToken();
        eatFluff();
        }
    else
        {
        COH_THROW (NoSuchElementException::create(
                "The tokens are exhausted."));
        }

    return vToken;
    }

void XmlTokenizer::putBackToken(Token::View vToken)
    {
    m_hDequeue->putBackElement(vToken);
    }

ParsePosition::Handle XmlTokenizer::savePosition() const
    {
    return Position::create(Dequeue::create(m_hDequeue),
            f_hScript->savePosition());
    }

void XmlTokenizer::restorePosition(ParsePosition::Handle hParsePos)
    {
    Position::Handle hPosition = cast<Position::Handle>(hParsePos);

    f_hScript->restorePosition(hPosition->f_hScriptPos);

    m_hDequeue = (Dequeue::Handle) hPosition->f_hDequeue;
    }


// ----- script parsing -----------------------------------------------------

void XmlTokenizer::eatFluff()
    {
    while (f_hScript->hasMoreChars())
        {
        char ch = f_hScript->nextChar();
        switch (ch)
            {
            // XML 1.0 spec 2nd ed section 2.3:
            // S ::= (#x20 | #x9 | #xD | #xA)+
            case 0x20:
            case 0x09:
            case 0x0D:
            case 0x0A:
                break;

            // a token has been encountered
            default:
                f_hScript->putBackChar();
                return;
            }
        }
    }

Token::View XmlTokenizer::eatToken()
    {
    switch (m_context)
        {
        case ctx_outside:
            return eatOutside();

        case ctx_xmldecl:
            return eatXmlDecl();

        case ctx_doctype:
            return eatDocType();

        case ctx_comment:
        case ctx_dtd_comment:
            return eatComment();

        case ctx_pi:
            return eatPi();

        case ctx_element:
            return eatElement();

        case ctx_dtd:
            return eatDtd();

        default:
            logError(f_hScript->getLine(), f_hScript->getOffset(),
                    ErrorDescription::fatal, getInternalErrorCode(),
                    "Internal Error");
            COH_THROW (CompilerException::create("Internal Error"));
        }
    }

Token::View XmlTokenizer::eatOutside()
    {
    size32_t       iLine    = f_hScript->getLine();
    size32_t       ofInLine = f_hScript->getOffset();

    if ('<' == f_hScript->nextChar())
        {
        // could be "<?", "<!--", "<!DOCTYPE", "<!CDATA", or just "<"
        switch (f_hScript->nextChar())
            {
            case '/':
                m_context = ctx_element;
                return XmlToken::create(XmlToken::getTokenEndTagStart(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);

            case '?':
                m_context = ctx_pi;
                return XmlToken::create(XmlToken::getTokenPiStart(), iLine,
                        ofInLine, f_hScript->getOffset() - ofInLine);

            case '!':
                // match either "--", "[CDATA" or "DOCTYPE"
                switch (f_hScript->nextChar())
                    {
                    case '-':
                        match('-');
                        m_context = ctx_comment;
                        return XmlToken::create(
                                XmlToken::getTokenCommentStart(),
                                iLine, ofInLine,
                                f_hScript->getOffset()- ofInLine);
                    case '[':
                        {
                        match("CDATA[");
                        ParsePosition::Handle hPos = f_hScript->savePosition();
                        scan("]]>");
                        String::View vsData =
                                f_hScript->subScript(hPos)->getText();
                        f_hScript->nextChar(); // ]
                        f_hScript->nextChar(); // ]
                        f_hScript->nextChar(); // >
                        return XmlToken::create(XmlToken::cat_literal,
                                XmlToken::subcat_lit_chardata,
                                XmlToken::chardata_raw, vsData, vsData,
                                iLine, ofInLine,
                                f_hScript->getOffset() - ofInLine);
                        }

                    case 'D':
                        f_hScript->putBackChar();
                        match("DOCTYPE");
                        m_context = ctx_doctype;
                        return XmlToken::create(
                                XmlToken::getTokenDocTypeStart(),
                                iLine, ofInLine,
                                f_hScript->getOffset()- ofInLine);
                    default:
                        // pretend we were looking for a comment
                        // (easier than duplicating the error reporting)
                        f_hScript->putBackChar();
                        match('-'); // will throw CompilerException
                        logError(f_hScript->getLine(), f_hScript->getOffset(),
                                ErrorDescription::fatal,
                                getInternalErrorCode(), "Internal Error");
                        COH_THROW (CompilerException::create("Internal Error"));
                    }
            default:
                // assume it is an element
                f_hScript->putBackChar();
                m_context = ctx_element;
                return XmlToken::create(XmlToken::getTokenElementStart(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
            }
        }
    else
        {
        // character data - scan up to '<'
        f_hScript->putBackChar();
        ParsePosition::Handle hPosition = f_hScript->savePosition();
        while (f_hScript->nextChar() != '<')
            {
            }
        f_hScript->putBackChar();

        String::View vsData = f_hScript->subScript(hPosition)->getText();
        return XmlToken::create(XmlToken::cat_literal,
                XmlToken::subcat_lit_chardata, XmlToken::chardata, vsData,
                vsData, iLine, ofInLine, f_hScript->getOffset() - ofInLine);
        }
    }

Token::View XmlTokenizer::eatXmlDecl()
    {
    // use element-context parsing
    return eatElement();
    }

Token::View XmlTokenizer::eatDocType()
    {
    // check for embedded DTD
    size32_t iLine    = f_hScript->getLine();
    size32_t ofInLine = f_hScript->getOffset();

    if ('[' == f_hScript->nextChar())
        {
        m_context = ctx_dtd;
        return XmlToken::create(XmlToken::getTokenDtdDeclStart(), iLine,
                ofInLine, f_hScript->getOffset() - ofInLine);
        }
    f_hScript->putBackChar();

    // use element-context parsing
    return eatElement();
    }

Token::View XmlTokenizer::eatDtd()
    {
    size32_t iLine    = f_hScript->getLine();
    size32_t ofInLine = f_hScript->getOffset();

    char ch = f_hScript->nextChar();
    switch (ch)
        {
        case '<':
            {
            match('!');

            if (peek('-'))
                {
                match('-');
                m_context = ctx_dtd_comment;
                return XmlToken::create(XmlToken::getTokenCommentStart(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
                }

            Token::View vTokenName = eatName();
            String::View vsName = vTokenName->getText();
            scan('>');
            match('>');

            if (vsName->equals("ELEMENT"))
                {
                return XmlToken::create(XmlToken::getTokenDtdElement(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
                }
            else if (vsName->equals("ATTLIST"))
                {
                return XmlToken::create(XmlToken::getTokenDtdAttList(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
                }
            else if (vsName->equals("ENTITY"))
                {
                return XmlToken::create(XmlToken::getTokenDtdEntity(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
                }
            else if (vsName->equals("NOTATION"))
                {
                return XmlToken::create(XmlToken::getTokenDtdNotation(),
                        iLine, ofInLine, f_hScript->getOffset() - ofInLine);
                }
            else
                {
                logError(f_hScript->getLine(), f_hScript->getOffset(),
                        ErrorDescription::error, getXmlFormatErrorCode(),
                        vsName);
                COH_THROW (CompilerException::create(vsName));
                }
            }

        case '%':
            {
            // %name;
            Token::View vTokenName = eatName();
            String::View vsName = vTokenName->getText();
            match(';');
            return XmlToken::create(XmlToken::cat_dtdstuff,
                    XmlToken::subcat_none, XmlToken::dtd_declsep, vsName,
                    vsName, iLine, ofInLine, f_hScript->getOffset() - ofInLine);
            }

        default:
            f_hScript->putBackChar();
            match(']');
            m_context = ctx_doctype;
            return XmlToken::create(XmlToken::getTokenDtdDeclStop(), iLine,
                    ofInLine, f_hScript->getOffset() - ofInLine);
        }
    }

Token::View XmlTokenizer::eatComment()
    {
    // read until "-->" found
    size32_t              iLine     = f_hScript->getLine();
    size32_t              ofInLine  = f_hScript->getOffset();
    ParsePosition::Handle hPosition = f_hScript->savePosition();
    bool                  fFirst    = true;

    while (true)
        {
        switch (f_hScript->nextChar())
            {
            case '-':
                if ('-' == f_hScript->nextChar())
                    {
                    if ('>' == f_hScript->nextChar())
                        {
                        if (fFirst)
                            {
                            // return the end-of-commment
                            m_context = (ctx_dtd_comment == m_context ?
                                    ctx_dtd : ctx_outside);
                            return XmlToken::create(
                                    XmlToken::getTokenCommentStop(), iLine,
                                    ofInLine, f_hScript->getOffset()-ofInLine);
                            }
                        else
                            {
                            // restore the end of comment
                            f_hScript->putBackChar();
                            f_hScript->putBackChar();
                            f_hScript->putBackChar();

                            String::View vsComment =
                                    f_hScript->subScript(hPosition)->getText();
                            return XmlToken::create(XmlToken::cat_literal,
                                    XmlToken::subcat_lit_comment,
                                    XmlToken::comment, (Object::View) NULL,
                                    vsComment,
                                    iLine, ofInLine,
                                    f_hScript->getOffset()-ofInLine);
                            }
                        }
                    }
            }

        fFirst = false;
        }
    }

Token::View XmlTokenizer::eatPi()
    {
    Token::View vTokenName = eatName();
    if (vTokenName->getText()->equals("xml"))
        {
        // switch over to XML-specific PI
        m_context = ctx_xmldecl;
        }
    else
        {
        eatFluff();

        // eat body
        size32_t              iLine     = f_hScript->getLine();
        size32_t              ofInLine  = f_hScript->getOffset();
        ParsePosition::Handle hPosition = f_hScript->savePosition();

        scan("?>");
        String::View vsBody = f_hScript->subScript(hPosition)->getText();
        if (vsBody->length() > 0)
            {
            // queue body
            m_hDequeue->addElement(XmlToken::create(XmlToken::cat_literal,
                    XmlToken::subcat_lit_chardata, XmlToken::chardata_raw,
                    (Object::View) NULL, vsBody, iLine, ofInLine,
                    f_hScript->getOffset() - ofInLine));
            }

        // queue closing separator
        iLine    = f_hScript->getLine();
        ofInLine = f_hScript->getOffset();
        f_hScript->nextChar();
        f_hScript->nextChar();
        m_hDequeue->addElement(XmlToken::create(XmlToken::getTokenPiStop(),
                iLine, ofInLine, f_hScript->getOffset() - ofInLine));

        m_context = ctx_outside;
        }

    return vTokenName;
    }

Token::View XmlTokenizer::eatElement()
    {
    size32_t iLine    = f_hScript->getLine();
    size32_t ofInLine = f_hScript->getOffset();
    char     ch       = f_hScript->nextChar();

    switch (ch)
        {
        // "?>" is supported here because PI and XMLDECL delegate
        // to this method for parsing
        case '?':
            match('>');
            m_context = ctx_outside;
            return XmlToken::create(XmlToken::getTokenPiStop(), iLine,
                    ofInLine, f_hScript->getOffset() - ofInLine);

        case '=':
            return XmlToken::create(XmlToken::getTokenEquals(), iLine,
                    ofInLine, f_hScript->getOffset() - ofInLine);

        case '\'':
        case '\"':
            {
            // scan for closing quote (ch)
            ParsePosition::Handle hPosition = f_hScript->savePosition();
            scan(ch);
            String::View vsLit = f_hScript->subScript(hPosition)->getText();
            match(ch);
            return XmlToken::create(XmlToken::cat_literal,
                    XmlToken::subcat_lit_quoted, XmlToken::literal,
                    (Object::View) NULL,
                    vsLit, iLine, ofInLine, f_hScript->getOffset() - ofInLine);
            }

        case '/':
            match('>');
            m_context = ctx_outside;
            return XmlToken::create(XmlToken::getTokenEmptyStop(), iLine,
                    ofInLine, f_hScript->getOffset() - ofInLine);

        case '>':
            m_context = ctx_outside;
            return XmlToken::create(XmlToken::getTokenElementStop(), iLine,
                    ofInLine, f_hScript->getOffset() - ofInLine);

        default:
            f_hScript->putBackChar();
            return eatName();
        }
    }

Token::View XmlTokenizer::eatName()
    {
    size32_t iLine    = f_hScript->getLine();
    size32_t ofInLine = f_hScript->getOffset();
    std::stringstream   ss;

    char ch = f_hScript->nextChar();
    if (!isNameStartChar(ch))
        {
        f_hScript->putBackChar();
        logError(f_hScript->getLine(), f_hScript->getOffset(),
                ErrorDescription::error, getXmlFormatErrorCode(),
                "Invalid Name");
        COH_THROW (CompilerException::create("Invalid Name"));
        }
    ss << ch;

    while (isNameChar(ch = f_hScript->nextChar()))
        {
        ss << ch;
        }
    f_hScript->putBackChar();

    return XmlToken::create(XmlToken::cat_name, XmlToken::subcat_none,
            XmlToken::name, (Object::View) NULL, ss.str(), iLine, ofInLine,
            f_hScript->getOffset() - ofInLine);
    }

bool XmlTokenizer::isNameStartChar(char ch)
    {
    // close enough (checking for the most probable first)
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || '_' == ch
            || ':' == ch || (ch >= '0' && ch <= '9');
    }

bool XmlTokenizer::isNameChar(char ch)
    {
    // close enough (checking for the most probable first)
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || '_' == ch
            || ':' == ch  || '-' == ch  || '.' == ch
            || (ch >= '0' && ch <= '9');
    }


// ----- helpers ------------------------------------------------------------

bool XmlTokenizer::peek(char ch)
    {
    char chActual = f_hScript->nextChar();
    if (ch == chActual)
        {
        return true;
        }
    else
        {
        f_hScript->putBackChar();
        return false;
        }
    }

void XmlTokenizer::match(char ch)
    {
    char chActual = f_hScript->nextChar();
    if (ch != chActual)
        {
        f_hScript->putBackChar();
        logError(f_hScript->getLine(), f_hScript->getOffset(),
                ErrorDescription::error, getCharExpectedErrorCode(),
                "Invalid character");
        f_hScript->nextChar();
        COH_THROW (CompilerException::create("Invalid character"));
        }
    }

void XmlTokenizer::match(String::View vs)
    {
    const char* ach = vs->getCString();
    size32_t    cch = vs->length();

    for (size32_t of = 0; of < cch; ++of)
        {
        char chActual = f_hScript->nextChar();
        if (ach[of] != chActual)
            {
            f_hScript->putBackChar();
            logError(f_hScript->getLine(), f_hScript->getOffset(),
                    ErrorDescription::error, getCharExpectedErrorCode(),
                    "Invalid character");
            f_hScript->nextChar();
            COH_THROW (CompilerException::create("Invalid character"));
            }
        }
    }

void XmlTokenizer::matchWhitespace()
    {
    char ch = f_hScript->nextChar();
    switch (ch)
        {
        // XML 1.0 spec 2nd ed section 2.3:
        // S ::= (#x20 | #x9 | #xD | #xA)+
        case 0x20:
        case 0x09:
        case 0x0D:
        case 0x0A:
            return;

        default:
            // easy way to log an error for no whitespace
            f_hScript->putBackChar();
            match(' ');
        }
    }

void XmlTokenizer::scan(char ch)
    {
    while (true)
        {
        if (f_hScript->nextChar() == ch)
            {
            f_hScript->putBackChar();
            return;
            }
        }
    }

void XmlTokenizer::scan(String::View vs)
    {
    const char* ach     = vs->getCString();
    size32_t    cch     = vs->length();
    char        cSentry = ach[0];
    while (true)
        {
        if (f_hScript->nextChar() == cSentry)
            {
            // determine if the entire search string has been found
            bool fFound  = true;
            size32_t  cTested = 1;
            for (size32_t of = 1; of < cch; ++of)
                {
                fFound = f_hScript->nextChar() == ach[of];
                ++cTested;
                if (!fFound)
                    {
                    break;
                    }
                }
            // restore the search string if it were found; otherwise
            // restore everything but the sentry character (to avoid
            // finding the same one again)
            for (size32_t i = 0, c = fFound ? cTested : cTested - 1; i < c; ++i)
                {
                f_hScript->putBackChar();
                }

            if (fFound)
                {
                break;
                }
            }
        }
    }


// ----- error handling -----------------------------------------------------

void XmlTokenizer::internalError()
    {
    logError(f_hScript->getLine(), f_hScript->getOffset(),
            ErrorDescription::fatal, getInternalErrorCode(),
            "Internal Error");
    COH_THROW (CompilerException::create("Internal Error"));
    }

void XmlTokenizer::logError(size32_t iLine, size32_t ofInLine,
        ErrorDescription::Severity severity, String::View vsCode,
        String::View vsMessage)
    {
    ErrorList::Handle hErrList = f_hErrList;
    if (NULL != hErrList)
        {
        hErrList->add(ErrorDescription::create(iLine, iLine, ofInLine, 0,
                severity, vsCode, vsMessage));
        }
    }


// ----- constants ----------------------------------------------------------

String::View XmlTokenizer::getInternalErrorCode()
    {
    static FinalView<String> ERR_INTERNAL(System::common(),
            String::create("XT-001"));
    return ERR_INTERNAL;
    }
COH_STATIC_INIT(XmlTokenizer::getInternalErrorCode());

String::View XmlTokenizer::getUnexpectedEofErrorCode()
    {
    static FinalView<String> ERR_UNEXPECTED_EOF(System::common(),
            String::create("XT-002"));
    return ERR_UNEXPECTED_EOF;
    }
COH_STATIC_INIT(XmlTokenizer::getUnexpectedEofErrorCode());

String::View XmlTokenizer::getUnexpectedIoErrorCode()
    {
    static FinalView<String> ERR_UNEXPECTED_IO(System::common(),
            String::create("XT-003"));
    return ERR_UNEXPECTED_IO;
    }
COH_STATIC_INIT(XmlTokenizer::getUnexpectedIoErrorCode());

String::View XmlTokenizer::getXmlFormatErrorCode()
    {
    static FinalView<String> ERR_XML_FORMAT(System::common(),
            String::create("XT-004"));
    return ERR_XML_FORMAT;
    }
COH_STATIC_INIT(XmlTokenizer::getXmlFormatErrorCode());

String::View XmlTokenizer::getCharExpectedErrorCode()
    {
    static FinalView<String> ERR_CHAR_EXPECTED(System::common(),
            String::create("XT-005"));
    return ERR_CHAR_EXPECTED;
    }
COH_STATIC_INIT(XmlTokenizer::getCharExpectedErrorCode());

COH_CLOSE_NAMESPACE3
