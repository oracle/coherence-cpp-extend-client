/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/SimpleParser.hpp"

#include "coherence/io/IOException.hpp"

#include "private/coherence/dev/compiler/SyntaxException.hpp"
#include "private/coherence/run/xml/SimpleDocument.hpp"
#include "private/coherence/run/xml/XmlHelper.hpp"
#include "private/coherence/run/xml/XmlScript.hpp"
#include "private/coherence/util/StringHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::dev::compiler::SyntaxException;
using coherence::io::IOException;
using coherence::util::StringHelper;


// ----- constructors -------------------------------------------------------

SimpleParser::SimpleParser()
        : f_hTokenizer(self(), XmlTokenizer::create()),
          m_vToken(self())
    {
    }


// ----- SimpleParser interface ---------------------------------------------

XmlDocument::Handle SimpleParser::parseXml(String::View vsXml)
    {
    XmlDocument::Handle hXmlDoc  = SimpleDocument::create();
    ErrorList::Handle   hErrList = ErrorList::create();

    COH_SYNCHRONIZED(this)
        {
        try
            {
            f_hTokenizer->setScript(XmlScript::create(vsXml), hErrList);
            m_vToken = next();
            parseDocument(hXmlDoc);
            return hXmlDoc;
            }
        catch (SyntaxException::View s)
            {
            std::stringstream ss;
            ss << "Exception occurred during parsing: "
                    << s->getMessage()->getCString();
            COH_THROW(RuntimeException::create(ss.str(), s));
            }
        catch (Exception::View t)
            {
            std::stringstream ss;
            ss << "Exception occurred during parsing: "
                    << t->getMessage()->getCString();
            if (!hErrList->isEmpty())
                {
                ss << "\n";
                hErrList->toStream(ss);
                }
            COH_THROW (IOException::create(ss.str()));
            }
        }
    }

XmlDocument::Handle SimpleParser::parseXml(std::istream& stream)
    {
    if (stream.fail())
        {
        COH_THROW_STREAM (IOException,
                "Exception occurred during parsing: The stream "
                << " is in a failed state.");
        }
    std::stringstream ss;
    char              caLine[256];

    while (!stream.eof())
        {
        stream.read(caLine, 255);
        caLine[stream.gcount()] = 0x00;
        ss << caLine;
        }
    return parseXml(ss.str());
    }


// ----- implementation -----------------------------------------------------

void SimpleParser::parseDocument(XmlDocument::Handle hXml)
    {
    // check for "<?xml"
    if (peek(XmlToken::pi_start) != NULL)
        {
        parsePi(hXml, true);
        }
    // check for comments / other PIs
    parseMisc(hXml);

    // check for "<!DOCTYPE"
    if (peek(XmlToken::doctype_start) != NULL)
        {
        parseDoctype(hXml);
        }

    // check for comments / other PIs
    parseMisc(hXml);

    // root element is required
    match(XmlToken::element_start);
    if (hXml->getName() != NULL)
        {
        match(hXml->getName());
        }
    else
        {
        String::View vsName = match(XmlToken::name)->getText();
        hXml->setName(vsName);
        }
    parseElement(hXml);
    // check for comments / other PIs
    parseMisc(hXml);
    }

void SimpleParser::parsePi(XmlElement::Handle hXml)
    {
    parsePi(hXml, false);
    }

void SimpleParser::parsePi(XmlElement::Handle hXml, bool fXmlDeclAllowed)
    {
    XmlToken::View vTokenName = match(XmlToken::name);
    if (vTokenName->getText()->equals("xml"))
        {
        if (fXmlDeclAllowed)
            {
            parseXmlDecl(cast<XmlDocument::Handle>(hXml));
            }
        else
            {
            COH_THROW (SyntaxException::create(
                    "XML declaration can only appear at the beginning of a document"));
            }
        }
    else
        {
        // ignore all other PIs
        while (current()->getID() != XmlToken::pi_stop)
            {
            }
        }
    }

void SimpleParser::parseXmlDecl(XmlDocument::Handle hXml)
    {
    if (peek("version") != NULL)
        {
        // version is assumed to be "1.0"
        match(XmlToken::equals);
        match(XmlToken::literal);
        }
    else
        {
        COH_THROW (SyntaxException::create(
                "The version value is missing from the XML declaration"));
        }

    if (peek("encoding") != NULL)
        {
        match(XmlToken::equals);
        String::View vsValue = match(XmlToken::literal)->getText();
        if (!XmlHelper::isEncodingValid(vsValue))
            {
            std::stringstream s;
            s << "The encoding value in"
                            << " the XML declaration is illegal (" << vsValue
                            << ")";
            COH_THROW (SyntaxException::create(s.str()));
            }
        hXml->setEncoding(vsValue);
        }

    if (peek("standalone") != NULL)
        {
        // standalone is discarded
        match(XmlToken::equals);
        String::View vsValue = match(XmlToken::literal)->getText();
        if (!(vsValue->equals("yes") || vsValue->equals("no")))
            {
            COH_THROW (SyntaxException::create(
                    "The value in the declaration must be 'yes' or 'no'"));
            }
        }

    match(XmlToken::pi_stop);
    }

void SimpleParser::parseDoctype(XmlDocument::Handle hXml)
    {
    // root element name
    hXml->setName(match(XmlToken::name)->getText());

    // ExternalID (optional): public identifier
    bool fPublic = (peek("PUBLIC") != NULL);
    if (fPublic)
        {
        String::View vsName = match(XmlToken::literal)->getText();
        vsName = XmlHelper::decodeAttribute(vsName);
        if (!XmlHelper::isPublicIdentifierValid(vsName))
            {
            std::stringstream s;
            s << "The public identifier in"
                    << " the XML DOCTYPE is invalid (" << vsName << ")";
            COH_THROW (SyntaxException::create(s.str()));
            }
        hXml->setDtdName(vsName);
        }

    // ExternalID (optional): system identifier
    if (fPublic || peek("SYSTEM") != NULL)
        {
        String::View vsUri = match(XmlToken::literal)->getText();
        vsUri = XmlHelper::decodeUri(XmlHelper::decodeAttribute(vsUri));
        if (!XmlHelper::isSystemIdentifierValid(vsUri))
            {
            std::stringstream s;
            s << "The system identifier in"
                    << " the XML DOCTYPE is invalid (" << vsUri << ")";
            COH_THROW (SyntaxException::create(s.str()));
            }
        hXml->setDtdUri(vsUri);
        }

    // ignore inline markup decl
    if (peek(XmlToken::dtd_decl_start) != NULL)
        {
        while (current()->getID() != XmlToken::dtd_decl_stop)
            {
            }
        }
    match(XmlToken::element_stop);
    }

void SimpleParser::parseElement(XmlElement::Handle hXml)
    {
    // parse attributes
    while (true)
        {
        XmlToken::View vToken = peek(XmlToken::name);
        if (NULL == vToken)
            {
            break;
            }
        String::View vsAttr = vToken->getText();
        if (!XmlHelper::isNameValid(vsAttr))
            {
            std::stringstream s;
            s << "Illegal attribute name: " << vsAttr << ")";
            COH_THROW (SyntaxException::create(s.str()));
            }
        match(XmlToken::equals);
        String::View vsValue = match(XmlToken::literal)->getText();
        vsValue = XmlHelper::decodeAttribute(vsValue);
        hXml->addAttribute(vsAttr)->setString(vsValue);
        }

    // check if this were an empty element
    if (peek(XmlToken::empty_stop) != NULL)
        {
        return;
        }

    // this element is the "content" type (not empty)
    match(XmlToken::element_stop);

    std::stringstream sValue;
    while (true)
        {
        XmlToken::View vToken = current();
        switch (vToken->getID())
            {
            case XmlToken::comment_start:
                parseComment(hXml);
                break;

            case XmlToken::pi_start:
                parsePi(hXml);
                break;

            case XmlToken::chardata:
                {
                String::View vsChunk = cast<String::View>(vToken->getValue());
                vsChunk = XmlHelper::trim(vsChunk);
                vsChunk = XmlHelper::decodeContent(vsChunk);
                if (vsChunk->length() > 0)
                    {
                    sValue << vsChunk->getCString();
                    }
                }
                break;

            case XmlToken::chardata_raw:
                {
                String::View vsChunk = cast<String::View>(vToken->getValue());
                sValue << vsChunk->getCString();
                }
                break;

            case XmlToken::element_start:
                {
                String::View vsName = match(XmlToken::name)->getText();
                parseElement(hXml->addElement(vsName));
                }
                break;

            case XmlToken::endtag_start:
                if (sValue.str().length() > 0)
                    {
                    hXml->setString(String::create(sValue.str()));
                    }
                match(hXml->getName());
                match(XmlToken::element_stop);
                return;
            }
        }
    }

void SimpleParser::parseMisc(XmlElement::Handle hXml)
    {
    while (true)
        {
        if (!hasCurrent())
            {
            return;
            }

        if (peek(XmlToken::comment_start) != NULL)
            {
            parseComment(hXml, true);
            }
        else if (peek(XmlToken::pi_start) != NULL)
            {
            parsePi(hXml);
            }
        else
            {
            return;
            }
        }
    }

void SimpleParser::parseComment(XmlElement::Handle hXml)
    {
    parseComment(hXml, false);
    }

void SimpleParser::parseComment(XmlElement::Handle hXml, bool fIsDocument)
    {
    std::stringstream sb;

    while (NULL == peek(XmlToken::comment_stop))
        {
        String::View vsComment = match(XmlToken::comment)->getText();
        sb << vsComment->getCString();
        }

    if (sb.str().length() > 0)
        {
        if (fIsDocument)
            {
            XmlDocument::Handle hDoc = cast<XmlDocument::Handle>(hXml);
            String::View sComment = hDoc->getDocumentComment();
            if (NULL == sComment || 0 == sComment->length())
                {
                hDoc->setDocumentComment(sb.str());
                }
            else
                {
                std::stringstream sbComment;
                sbComment << sComment->getCString() << "\n"
                        << sb.str();
                hDoc->setDocumentComment(sbComment.str());
                }
            }
        else
            {
            String::View vsComment = hXml->getComment();
            if (NULL == vsComment || 0 == vsComment->length())
                {
                hXml->setComment(sb.str());
                }
            else
                {
                std::stringstream sbComment;
                sbComment << vsComment->getCString() << "\n" <<
                        sb.str();
                hXml->setComment(sbComment.str());
                }
            }
        }
    }


// ----- parsing helpers ----------------------------------------------------

bool SimpleParser::hasCurrent()
    {
    return !is_null(m_vToken);
    }

XmlToken::View SimpleParser::current()
    {
    XmlToken::View vCurrentToken = m_vToken;
    next();
    return vCurrentToken;
    }

bool SimpleParser::hasNext()
    {
    return f_hTokenizer->hasMoreTokens();
    }

XmlToken::View SimpleParser::next()
    {
    if (f_hTokenizer->hasMoreTokens())
        {
        return m_vToken = cast<XmlToken::View>(f_hTokenizer->nextToken());
        }

    if (is_null(m_vToken))
        {
        COH_THROW (CompilerException::create("Invalid root element"));
        }

    m_vToken = (XmlToken::View) NULL;
    return NULL;
    }

XmlToken::View SimpleParser::match(int32_t nId)
    {
    if (m_vToken->getID() != nId)
        {
        std::stringstream s;
        s << "looking for id=" << nId << ", found id=" << m_vToken->getID()
                << "(";
        m_vToken->toStream(s);
        s << ")";
        COH_THROW (SyntaxException::create(s.str()));
        }
    return current();
    }

XmlToken::View SimpleParser::match(String::View vsName)
    {
    XmlToken::View vToken = peek(vsName);
    if (NULL == vToken)
        {
        std::stringstream s;
        s << "looking for id=" << vsName << ", found token=";
        m_vToken->toStream(s);
        s <<  "... It is possible that " << vsName
                << " is missing a closing tag";
        COH_THROW (SyntaxException::create(s.str()));
        }
    return vToken;
    }

XmlToken::View SimpleParser::peek(int32_t nId)
    {
    return (m_vToken->getID() == nId ? current() : (XmlToken::View) NULL);
    }

XmlToken::View SimpleParser::peek(int32_t nCat, int32_t nSubcat)
    {
    return (m_vToken->getCategory() == nCat &&
            m_vToken->getSubCategory()== nSubcat ? current() :
            (XmlToken::View) NULL);
    }

XmlToken::View SimpleParser::peek(String::View vsName)
    {
    return (XmlToken::name == m_vToken->getID() &&
            m_vToken->getText()->equals(vsName) ? current() :
            (XmlToken::View) NULL);
    }

XmlToken::View SimpleParser::mark()
    {
    return m_vToken;
    }

COH_CLOSE_NAMESPACE3
