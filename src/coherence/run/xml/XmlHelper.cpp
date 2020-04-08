/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/XmlHelper.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/run/xml/XmlConfigurable.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Binary.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Muterator.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/run/xml/SimpleParser.hpp"
#include "private/coherence/util/StringHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <fstream>
#include <string>
#include <sstream>

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::io::IOException;
using coherence::io::OctetArrayReadBuffer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::util::ArrayList;
using coherence::util::Binary;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::Muterator;
using coherence::util::StringHelper;
using coherence::util::logging::Logger;


// ----- Xml loading helpers ------------------------------------------------

XmlDocument::Handle XmlHelper::loadXml(std::istream& stream)
    {
    return SimpleParser::create()->parseXml(stream);
    }

XmlDocument::Handle XmlHelper::loadXml(String::View vsXml)
    {
    return SimpleParser::create()->parseXml(vsXml);
    }

XmlDocument::Handle XmlHelper::loadFile(String::View vsName, String::View vsDescr)
    {
    if (vsDescr == NULL)
        {
        // default to something meaningless and generic
        vsDescr = "configuration";
        }

    try
        {
        std::ifstream in(vsName->getCString(), std::ifstream::in);
        if (in.fail())
            {
            COH_THROW_STREAM (IOException, "The " << vsDescr << " is missing: \""
                    << vsName << "\"");
            }

        XmlDocument::Handle hXml = loadXml(in);

        COH_LOG("Loaded " << vsDescr << " from \"" <<
                vsName << '"', Logger::level_info);
        return hXml;
        }
    catch (Exception::View e)
        {
        COH_THROW_STREAM (RuntimeException, e <<
                "Failed to load " << vsDescr << ": " << vsName);
        }
    }


// ----- utility methods ----------------------------------------------------

namespace
    {
    void assertASCII(String::View vsStr)
        {
        if (!vsStr->isASCII())
            {
            COH_THROW (UnsupportedOperationException::create(
                "XmlHelper utilities only support ASCII strings."));
            }
        }
    }

String::View XmlHelper::encodeAttribute(String::View vs, char chQuote)
    {
    assertASCII(vs);
    const size32_t cLen = vs->length();
    if (cLen == 0)
        {
        return vs;
        }
    else if (chQuote != '\'' && chQuote != '"')
        {
        COH_THROW (IllegalArgumentException::create(
            "Quote character must be either '\"' or '\''."));
        }

    std::stringstream ss;
    const char *ach = vs->getCString();
    size32_t iPrev = 0;

    for (size32_t i = 0; i < cLen; ++i)
        {
        char c = ach[i];
        if (isControlChar(c) || c == '<' || c == '>' || c == '&')
            {
            ss << vs->substring(iPrev, i);
            writeEncodedChar(ss, c);
            iPrev = i + 1;
            }
        else if (c == '\'' || c == '"')
            {
            if (c == chQuote)
                {
                ss << vs->substring(iPrev, i);
                writeEncodedChar(ss, c);
                iPrev = i + 1;
                }
            }
        }
    // there may be a portion of the string left that does not require
    // encoding
    while (iPrev < cLen)
        {
        ss << ach[iPrev++];
        }

    return ss.str().empty() ? vs : (String::View) ss.str();
    }

XmlElement::Handle XmlHelper::ensureElement(XmlElement::Handle hXml,
        String::View vsPath)
    {
    COH_ENSURE(NULL != hXml && NULL != vsPath);

    if (vsPath->startsWith("/"))
        {
        hXml = hXml->getRoot();
        }

    ObjectArray::Handle haTokens = StringHelper::split(vsPath, "/");
    for (size32_t i = 0, c = haTokens->length; i < c; ++i)
        {
        String::View vsName = cast<String::View>(haTokens[i]);

        if (vsName->equals(".."))
            {
            hXml = hXml->getParent();
            if (NULL == hXml)
                {
                COH_THROW_STREAM(IllegalArgumentException, "Invalid path " << vsPath);
                }
            }
        else
            {
            XmlElement::Handle hChild = hXml->getElement(vsName);
            hXml = NULL == hChild ? hXml->addElement(vsName) : hChild;
            }
        }

    return hXml;
    }

String::View XmlHelper::quote(String::View vs)
    {
    std::stringstream ss;
    ss << '\'' << encodeAttribute(vs, '\'') << '\'';
    return ss.str();
    }

String::View XmlHelper::encodeContent(String::View vsValue,
        bool fPreferBlockEscape)
    {
    assertASCII(vsValue);
    size32_t cLen = vsValue->length();
    if (cLen == 0)
        {
        return vsValue;
        }

    const char *str = vsValue->getCString();
    std::stringstream ss;

    if (fPreferBlockEscape)
        {
        // scan to see if any escape is necessary, and if so, use CDATA
        // if possible (content must not contain "]]>")
        bool fUseCdataEscape = true;
        bool fRequiresEscape = isWhitespace(str[0]) ||
                isWhitespace(str[cLen - 1]);

        for (size32_t of = 0; of < cLen; ++of)
            {
            char nch = str[of];
            if (nch == '<' || nch == '&')
                {
                fRequiresEscape = true;
                }
            else if (nch == ']' && of + 2 < cLen && str[of + 1] == ']' &&
                    str[of + 2] == '>')
                {
                fUseCdataEscape = false;
                }
            }
        if (!fRequiresEscape)
            {
            return vsValue;
            }

        if (fUseCdataEscape)
            {
            ss << "<![CDATA[" << vsValue << "]]>";
            return ss.str();
            }
        }

    // encode leading whitespace
    size32_t  of         = 0;
    bool whitespace = true;
    while (of < cLen && whitespace)
        {
        char cWhite = str[of];
        if (isWhitespace(cWhite))
            {
            ++of;
            writeEncodedChar(ss, cWhite);
            }
        else
            {
            whitespace = false;
            }
        }

    // figure out the extent of trailing whitespace
    size32_t cLenNonWhite = cLen;
    while (cLenNonWhite > of && isWhitespace(str[cLenNonWhite - 1]))
        {
        --cLenNonWhite;
        }

    // encode portion between leading and trailing whitespace
    size32_t ofPrev    = of;
    size32_t cBrackets = 0;
    for (; of < cLenNonWhite; ++of)
        {
        char ch = str[of];
        switch(ch)
            {
            case ']':
                ++cBrackets;
                break;
            case '>':
                if (cBrackets < 2)
                    {
                    cBrackets = 0;
                    break;
                    }
            case '<':
            case '&':
                {
                // transfer up to but not including the current offset
                if (of > ofPrev)
                    {
                    ss << vsValue->substring(ofPrev, of);
                    }
                if (ch == '>' || ch == '<' || ch == '&')
                    {
                    writeEncodedChar(ss, ch);
                    }
                else
                    {
                    COH_THROW (IllegalArgumentException::create(
                            "Unexpected Exception occurred"));
                    }
                // the next character in the string is now the next
                // character to transfer/encode
                ofPrev    = of + 1;
                cBrackets = 0;
                break;
                }

            default:
                cBrackets = 0;
                break;
            }
        }
    // there may be a portion of the string left that does not require
    // encoding
    while (ofPrev < cLenNonWhite)
        {
        ss << str[ofPrev++];
        }

    // encode trailing whitespace
    for (; of < cLen; ++of)
        {
        char chWhite = str[of];
        if (isWhitespace(chWhite))
            {
            writeEncodedChar(ss, chWhite);
            }
        else
            {
            COH_THROW (IllegalArgumentException::create(
                    "Unexpected Exception occurred"));
            }
        }
    return ss.str();
    }

bool XmlHelper::isWhitespace(char ch)
    {
    switch (ch)
        {
        case '\t':
        case '\n':
        case '\r':
        case ' ' :
            return true;
        default:
            return false;
        }
    }

String::View XmlHelper::encodeUri(String::View vsUri)
    {
    assertASCII(vsUri);
    if (NULL == vsUri)
        {
        return StringHelper::getEmptyString();
        }

    size32_t cLen = vsUri->length();
    if (cLen == 0)
        {
        return vsUri;
        }

    const char *str = vsUri->getCString();
    std::stringstream ss;
    size32_t ofPrev = 0;

    for (size32_t of = 0; of < cLen; ++of)
        {
        char ch = str[of];
        switch (ch)
            {
            case '<' : case '>' : case '"' : case '{' : case '}' :
            case '|' : case '\\': case '^' : case '`' : case '%' :
            case '\'': case ' ' :
                {
                if (of > ofPrev)
                    {
                    ss << vsUri->substring(ofPrev, of);
                    }

                ss << '%' << std::hex << (int32_t)ch;
                ofPrev = of + 1;
                break;
                }

            default:
                break;
            }
        }

    while (ofPrev < cLen)
        {
        ss << str[ofPrev++];
        }

    if (!ss.str().empty())
        {
        return ss.str();
        }
    else
        {
        return vsUri;
        }
    }

String::View XmlHelper::breakLines(String::View vsText, size32_t nWidth,
        size32_t nIndent)
    {
    assertASCII(vsText);

    const char* ach        = vsText->getCString();
    size32_t    cch        = vsText->length();
    bool        fBreak     = false;
    size32_t    iLastBreak = String::npos;
    size32_t    iLastSpace = String::npos;
    size32_t    iBreakPos  = String::npos;
    std::stringstream ss;

    if (nWidth <= nIndent)
        {
        COH_THROW (IllegalArgumentException::create(
                "Indent and width are incompatible"));
        }
    nWidth -= nIndent;

    for (size32_t i = 0; i < cch; ++i)
        {
        char ch = ach[i];
        if (isWhitespace(ch))
            {
            iLastSpace = i;
            }

        if (ch == '\n')
            {
            fBreak     = true;
            iBreakPos  = i;
            }
        else if (i == iLastBreak + nWidth)
            {
            fBreak = true;
            if (iLastSpace != String::npos &&
                    (iLastBreak == String::npos || iLastBreak < iLastSpace))
                {
                iBreakPos = iLastSpace;
                }
            else
                {
                iBreakPos = i;
                }
            }
        else if (i == cch - 1)
            {
            fBreak    = true;
            iBreakPos = i;
            }

        if (fBreak)
            {
            for (size32_t iPos = iLastBreak + 1; iPos < iBreakPos; ++iPos)
                {
                ss << ach[iPos];
                }
            if (iBreakPos < cch - 1)
                {
                if (! isWhitespace(ach[iBreakPos]))
                    {
                    ss << ach[iBreakPos];
                    }
                writeNewLineAndIndent(ss, nIndent);
                // strip whitespace from start of line
                while (isWhitespace(ach[iBreakPos + 1]))
                    {
                    ++iBreakPos;
                    }
                }
            else
                {
                ss << ach[cch - 1];
                }
            iLastBreak = iBreakPos;
            i          = iBreakPos;
            fBreak     = false;
            }
        }
    return ss.str();
    }

bool XmlHelper::isEncodingValid(String::View vsEncoding)
    {
    assertASCII(vsEncoding);
    if (NULL == vsEncoding)
        {
        return false;
        }

    const char* ach = vsEncoding->getCString();
    size32_t    cch = vsEncoding->length();

    if (cch == 0)
        {
        return false;
        }

    char ch = ach[0];
    if (!((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')))
        {
        return false;
        }

    for (size32_t of = 1; of < cch; ++of)
        {
        ch = ach[of];
        switch (ch)
            {
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z':

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z':

            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':

                // other legal characters
            case '.':
            case '_':
            case '-':
                break;

            default:
                return false;
            }
        }

    return true;
    }

String::View XmlHelper::decodeAttribute(String::View vsValue)
    {
    assertASCII(vsValue);
    if (vsValue->indexOf('&') == String::npos)
        {
        return vsValue;
        }

    const char*  ach = vsValue->getCString();
    size32_t     cch = vsValue->length();
    std::stringstream ss;

    size32_t ofPrev = 0;
    for (size32_t of = 0; of < cch; ++of)
        {
        if (ach[of] == '&')
            {
            // scan up to ';'
            size32_t ofSemi = of + 1;
            while (ofSemi < cch && ach[ofSemi] != ';')
                {
                ++ofSemi;
                }
            if (ofSemi >= cch || ofSemi == of + 1)
                {
                COH_THROW_STREAM(IllegalArgumentException,
                        "The XML attribute (" << vsValue <<
                        ") contains an unescaped '&'");
                }

            // transfer up to but not including the current offset
            if (of > ofPrev)
                {
                String::View subString = vsValue->substring(ofPrev, of);
                ss << subString->getCString();
                ofPrev = of;
                }

            // convert the escaped sequence to a character, ignoring
            // potential entity refs
            if (ach[of + 1] == '#')
                {
                bool         fHex = (ach[of + 2] == 'x');
                String::View sEsc = vsValue->substring(of
                        + (fHex ? 3 : 2), ofSemi);

                if (sEsc->length() < 1)
                    {
                    COH_THROW (IllegalArgumentException::create(
                            "not a number"));
                    }

                int32_t            n;
                std::istringstream iss(sEsc->getCString());
                bool               fConvertFailure =
                        (fHex ? ((iss >> std::hex >> n).fail()) :
                        ((iss >> n).fail()));

                if (fConvertFailure)
                    {
                    COH_THROW_STREAM(IllegalArgumentException,
                            "The XML attribute (" << vsValue
                            << ") contains an illegal escape (" <<
                            (fHex ? "hex" : "decimal") << " " << sEsc << ")");

                    }
                if (n < 0 || n > 0xFFFF)
                    {
                    COH_THROW ( IllegalArgumentException::create(
                            "out of range"));
                    }
                ss << (char)n;
                }
            else
                {
                String::View vsEsc = vsValue->substring(of + 1, ofSemi);
                if (vsEsc->equals("amp"))
                    {
                    ss << "&";
                    }
                else if (vsEsc->equals("apos"))
                    {
                    ss << "\'";
                    }
                else if (vsEsc->equals("gt"))
                    {
                    ss << ">";
                    }
                else if (vsEsc->equals("lt"))
                    {
                    ss << "<";
                    }
                else if (vsEsc->equals("quot"))
                    {
                    ss << "\"";
                    }
                else
                    {
                    // assume it is an entity ref etc.
                    continue;
                    }
                }

            of     = ofSemi;
            ofPrev = of + 1;
            }
        }

    if (ofPrev < cch)
        {
        String::View subString = vsValue->substring(ofPrev, cch);
        ss << subString->getCString();
        }

    return ss.str();
    }

bool XmlHelper::isPublicIdentifierValid(String::View vsName)
    {
    assertASCII(vsName);

    const char* ach = vsName->getCString();
    size32_t    cch = vsName->length();

    for (size32_t of = 0; of < cch; ++of)
        {
        switch (ach[of])
            {
            case 0x20:
            case 0x0D:
            case 0x0A:

            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z':

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z':

            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':

            case '-': case '\'': case '(': case ')': case '+': case ',':
            case '.': case '/': case ':': case '=': case '?': case ';':
            case '!': case '*': case '#': case '@': case '$': case '_':
            case '%':
                break;

            default:
                return false;
            }
        }

    return true;
    }

String::View XmlHelper::decodeUri(String::View vsUri)
    {
    assertASCII(vsUri);

    String::View vsOrig = String::create(vsUri->getCString());

    // scan for characters to unescape
    if (vsUri->indexOf("%") == String::npos)
        {
        const char*  ach    = vsUri->getCString();
        size32_t     cch    = vsUri->length();
        size32_t     ofPrev = 0;
        std::stringstream ss;

        for (size32_t of = 0; of < cch; ++of)
            {
            if (ach[of] == '%')
                {
                if (of + 2 >= cch)
                    {
                    COH_THROW_STREAM(IllegalArgumentException, "The URI (" <<
                            vsOrig << ") contains an unescaped '%'");
                    }

                // transfer up to but not including the current offset
                if (of > ofPrev)
                    {
                    String::View subString = vsUri->substring(ofPrev, of);
                    ss << subString->getCString();
                    }

                // convert the escaped sequence to a character
                int32_t n;
                String::View vsSub = vsUri->substring(of + 1, of + 3);
                std::istringstream iss(vsSub->getCString());
                if ((iss >> n).fail())
                    {
                    COH_THROW_STREAM(IllegalArgumentException, "The URI ("
                            << vsOrig << ") contains an illegal escape");
                    }
                if (n < 0 || n > 0xFF){
                COH_THROW ( IllegalArgumentException::create("out of range"));
                }

                ss << n;
                of += 2;
                ofPrev = of + 1;
                }
            }

        if (ofPrev < cch)
            {
            String::View subString = vsUri->substring(ofPrev, cch);
            ss << subString->getCString();
            }

        vsUri = ss.str();
        }
    return vsUri;
    }

bool XmlHelper::isSystemIdentifierValid(String::View /*vsName*/)
    {
    // TODO REVIEW: use vsName?
    return true;
    }

bool XmlHelper::isNameValid(String::View vsName)
    {
    assertASCII(vsName);

    if (NULL == vsName)
        {
        return false;
        }

    const char* ach = vsName->getCString();
    size32_t    cch = vsName->length();

    if (cch == 0)
        {
        return false;
        }

    char ch = ach[0];
    //TODO: use isAlpha().  Must ensure it is available on all platforms
    if (!(((ch >= 'A' && ch <= 'Z')||(ch >= 'a' && ch <= 'z')) || ch == '_'
            || ch == ':'))
        {
        return false;
        }

    for (size32_t of = 1; of < cch; ++of)
        {
        ch = ach[of];
        switch (ch)
            {
            // inline latin uppercase/lowercase letters and digits
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z':

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z':

            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':

                // other legal characters
            case '.':
            case '-':
            case '_':
            case ':':
                break;

            default:
                if (!(((ch >= 'A' && ch <= 'Z')||(ch >= 'a' && ch <= 'z'))
                        || (ch >= '0' && ch <= '9')))
                    {
                    return false;
                    }
            }
        }

    return true;
    }

String::View XmlHelper::decodeContent(String::View vsValue)
    {
    return decodeAttribute(vsValue);
    }

String::View XmlHelper::trim(String::View vs)
    {
    assertASCII(vs);

    const char* ach     = vs->getCString();
    size32_t    cch     = vs->length();
    size32_t    ofStart = 0;
    size32_t    ofEnd   = cch;

    while (ofStart < cch && isWhitespace(ach[ofStart]))
        {
        ++ofStart;
        }

    if (ofStart >= cch)
        {
        return StringHelper::getEmptyString();
        }

    while (isWhitespace(ach[ofEnd - 1]))
        {
        --ofEnd;
        }

    return ofStart > 0 || ofEnd < cch ? vs->substring(ofStart,
            ofEnd) : vs;
    }

bool XmlHelper::isControlChar(char ch)
    {
    return (ch <= 0x1F);
    }

void XmlHelper::writeEncodedChar(std::ostream& out, char ch)
    {
    switch (ch)
        {
        case '>':
            out << "&gt;";
            break;
        case '<':
            out << "&lt;";
            break;
        case '&':
            out << "&amp;";
            break;
        case '\'':
            out << "&apos;";
            break;
        case '\"':
            out << "&quot;";
            break;
        default:
            out << "&#x";
            if (ch <= 0x0F)
                {
                out << '0';
                }
            out << std::hex << std::uppercase << (int32_t)ch << ';';
        }
    }

void XmlHelper::writeNewLineAndIndent(std::ostream& out, size32_t nIndent)
    {
    out << '\n';
    while (nIndent != 0)
        {
        out << ' ';
        --nIndent;
        }
    }

ObjectArray::View XmlHelper::parseInitParams(XmlElement::View vXmlParams)
    {
    List::Handle     hListParam = ArrayList::create();
    Iterator::Handle hIter      = vXmlParams->getElements("init-param");
    while (hIter->hasNext())
        {
        XmlElement::View vXmlParam = cast<XmlElement::View>(hIter->next());
        String::View     vsType    = vXmlParam->getSafeElement("param-type")->getString();
        XmlElement::View vXmlValue = vXmlParam->getSafeElement("param-value");
        String::View     vsValue   = vXmlValue->getString();

        // process well-known types
        if (vsType->length() == 0)
            {
            COH_THROW_STREAM (RuntimeException, "missing parameter type:\n" << vXmlParam);
            }
        else if ((StringHelper::compare(vsType, "string", true) == 0)
                || (StringHelper::compare(vsType, "coherence::lang::String", true) == 0))
            {
            hListParam->add(vsValue);
            }
        else if ((StringHelper::compare(vsType,"int", true) == 0)
                || (StringHelper::compare(vsType, "coherence::lang::Integer32", true)  == 0))
            {
            hListParam->add(Integer32::valueOf(vXmlValue->getInt32()));
            }
        else if ((StringHelper::compare(vsType, "boolean", true) == 0)
                || (StringHelper::compare(vsType, "coherence::lang::Boolean", true) == 0))
            {
            hListParam->add(Boolean::valueOf(vXmlValue->getBoolean()));
            }
        else
            {
            // the type is unknown; pass the XmlElement itself as a parameter
            hListParam->add(vXmlParam->clone());
            }
        }
    return hListParam->toArray();
    } 

XmlElement::Handle XmlHelper::transformInitParams(XmlElement::Handle hXmlParent,
        XmlElement::View vXmlParams)
    {
    Iterator::Handle hIter = vXmlParams->getElements("init-param");
    while (hIter->hasNext())
        {
        XmlElement::View vXmlParam = cast<XmlElement::View>(hIter->next());
        String::View vsName = vXmlParam->getSafeElement(
              "param-name" )->getString();
        String::View vsValue = vXmlParam->getSafeElement(
              "param-value")->getString();

        if (vsName->length() != 0)
            {
            hXmlParent->ensureElement(vsName)->setString(vsValue);
            }
        }
    return hXmlParent;
    }

bool XmlHelper::processMacros(XmlElement::Handle hXml)
    {
    bool         fUpdated = false;
    String::View sValue   = hXml->getString();

    if (NULL != sValue)
        {
        // replace in-lined system-properties i.e. ${prop-name default-value}
        for (size32_t ofStart = sValue->indexOf("${"); ofStart != String::npos; ofStart = sValue->indexOf("${"))
            {
            size32_t ofEnd  = sValue->indexOf('}', ofStart);
            if (ofEnd == String::npos)
                {
                // missing closing } so no macro to process here
                break;
                }

            String::View sMacro = sValue->substring(ofStart, ofEnd + 1);
            String::View sDefault;
            String::View sProp;

            ofStart = sMacro->indexOf(' ');
            if (ofStart == String::npos)
                {
                sDefault = "";
                sProp    = sMacro->substring(2, sMacro->length() - 1);
                }
            else
                {
                sDefault = sMacro->substring(ofStart, sMacro->length() - 1)->trim();
                sProp    = sMacro->substring(2, ofStart);
                }
            sValue = StringHelper::replace(sValue, sMacro, System::getProperty(sProp, sDefault));
            fUpdated = true;
            }

        if (fUpdated)
            {
            hXml->setString(sValue);
            }
        }
    return fUpdated;
    }

void XmlHelper::replaceSystemProperties(XmlElement::Handle hXml,
        String::View vsPropertyAttribute)
    {
    XmlValue::View vAttr = hXml->getAttribute(vsPropertyAttribute);
    if (vAttr != NULL)
        {
        // remove the attribute
        hXml->setAttribute(vsPropertyAttribute, NULL);

        // set the element's value from the specified system property
        try
            {
            String::View vsValue = System::getProperty(vAttr->getString());
            if (vsValue != NULL)
                {
                hXml->setString(vsValue);
                }
            }
        catch (...)
            {
            // do nothing
            }
        }
    XmlHelper::processMacros(hXml);

    ObjectArray::Handle oaElements = hXml->getAllElements();
    for (size32_t nSize = oaElements->length, i = 0; i < nSize; ++i)
        {
        replaceSystemProperties(cast<XmlElement::Handle>(
                oaElements[i]), vsPropertyAttribute);

        }
    }

int XmlHelper::removeElement(XmlElement::Handle hXml, String::View vsName)
    {
    int cnt = 0;
    for (Muterator::Handle hIter = hXml->getElementList()->iterator(); hIter->hasNext();)
        {
        XmlElement::View vXml =
                cast<XmlElement::View>(hIter->next());

        if (vXml->getName()->equals(vsName))
            {
            hIter->remove();
            cnt++;
            }
        }
    return cnt;
    }

bool XmlHelper::replaceElement(XmlElement::Handle hXmlParent,
        XmlElement::View vXmlReplace)
    {
    List::Handle hList = hXmlParent->getElementList();
    for (Iterator::Handle hIter = hList->iterator(); hIter->hasNext();)
        {
        XmlElement::View vXml =
                cast<XmlElement::View>(hIter->next());

        if (vXml->getName()->equals(vXmlReplace->getName()))
            {
            hList->set(hList->indexOf(vXml),
                    cast<XmlElement::Handle>(vXmlReplace->clone()));
            return true;
            }
        }

        hList->add(cast<XmlElement::Handle>(vXmlReplace->clone()));
        return false;
    }

bool XmlHelper::isEmpty(XmlElement::View vXml)
    {
    if (!vXml->isEmpty())
        {
        return false;
        }

    Map::View vMapAttr = vXml->getAttributeMap();
    if (!vMapAttr->isEmpty())
        {
        return false;
        }

    List::View vListElements = vXml->getElementList();
    if (!vListElements->isEmpty())
        {
        return false;
        }

    //TODO: This is dead code since if there are elements, we will already
    //TODO: have returned false
    for (Iterator::Handle hIter = vListElements->iterator(); hIter->hasNext();)
        {
        XmlElement::View vXmlEl = cast<XmlElement::View>(hIter->next());
        if (!isEmpty(vXmlEl))
            {
            return false;
            }
        }
        return true;
    }

String::View XmlHelper::toUpperCase(String::View vs)
    {
    if (NULL == vs || vs->length() == 0)
        {
        return vs;
        }

    size32_t     cLen = vs->length();
    const char*  ach   = vs->getCString();
    std::stringstream ss;

    for (size32_t i = 0; i < cLen; ++i)
        {
        ss << (char) toupper(ach[i]);
        }

    return ss.str();
    }

Object::Handle XmlHelper::createInstance(XmlElement::View vXml,
        ClassLoader::View vLoader,
        Class::View vClzAssignable)
    {
    XmlElement::View vXmlInstance = vXml->getSafeElement("instance");
    if (vXmlInstance->getElementList()->isEmpty())
        {
        // pre 3.6 style, no outer instance element
        vXmlInstance = vXml;
        }

    String::View vsClass  = vXmlInstance->getSafeElement("class-name")->getString();
    String::View vsMethod = NULL;
    Class::View  vClz     = NULL;

    if (vsClass->length() == 0)
        {
        vsClass  = vXmlInstance->getSafeElement("class-factory-name")->getString();
        vsMethod = vXmlInstance->getSafeElement("method-name")->getString();
        if (vsClass->length() == 0)
            {
            COH_THROW_STREAM (IllegalArgumentException, "The configuration element \""
                << vXmlInstance->getName()
                << "\" does not specify a \"class-name\" or \"class-factory-name\"\n" << vXml);
            }
        else if (vsMethod->length() == 0)
            {
            COH_THROW_STREAM (IllegalArgumentException, "The configuration element \""
                << vXmlInstance->getName()
                << "\" specifies a \"class-factory-name\", but no \"method-name\"\n" << vXml);
            }
        }

    XmlElement::View  vXmlParams = vXmlInstance->getElement("init-params");
    ObjectArray::View vaoParam   = NULL;
    if (vXmlParams != NULL)
        {
        try {
            vaoParam   = parseInitParams(vXmlParams);
            vXmlParams = NULL;
            }
        catch (RuntimeException::View vE)
            {
            // ignored
            }
        }

    if (NULL == vLoader)
        {
        vLoader = SystemClassLoader::getInstance();
        }

    Object::Handle hInstance;
    try
        {
        vClz = vLoader->loadByName(vsClass);
        hInstance = (vsMethod == NULL) 
                ? vClz->newInstance(vaoParam)
                : cast<Object::Handle>(vClz->getMethod(vsMethod, vaoParam)->invoke(vClz->newInstance(), vaoParam));
        }
    catch (Exception::View ex)
        {
        String::Handle hMsg;
        if (vClz == NULL)
            {
            hMsg = COH_TO_STRING("Unable to load class \""
                    << vsClass
                    << "\" using "
                    << vLoader
                    << "\n"
                    << vXml);
            }
        else
            {
            String::Handle hsArgs = String::create();

            if (vaoParam != NULL)
                {
                for (int i = 0, c = vaoParam->length; i < c; i++)
                    {
                    hsArgs = COH_TO_STRING(hsArgs << Class::getClassName(vaoParam[i]));

                    if (i < c - 1)
                        {
                        hsArgs = COH_TO_STRING(hsArgs << ", ");
                        }
                    }
                }
            if (vsMethod == NULL)
                {
                hMsg = COH_TO_STRING("Missing or inaccessible constructor \""
                        << vsClass
                        << "(" << hsArgs << ")\n");
                }
            else
                {
                hMsg = COH_TO_STRING("Failed to instantiate class using factory method \""
                        << vsClass << "::" << vsMethod
                        << "(" << hsArgs << ")\n");
                }
            }

        COH_THROW (IllegalArgumentException::create(hMsg, ex));
        }

    // validate the object
    if (NULL == hInstance)
        {
        COH_THROW_STREAM (IllegalArgumentException, "Configuration element \""
                << vXml->getName() << "\" produced a NULL object");
        }
    else if (vClzAssignable != NULL && !vClzAssignable->isInstance(hInstance))
        {
        COH_THROW_STREAM (IllegalArgumentException, "The instance of class \""
                << vsClass
                << "\" produced from configuration element \""
                << vXml->getName()
                << "\" is not an instance of \""
                << vClzAssignable->getName()
                << '"');
        }

    // configure the object
    if (instanceof<XmlConfigurable::Handle>(hInstance) && vXmlParams != NULL)
        {
        cast<XmlConfigurable::Handle>(hInstance)->setConfig(
                XmlHelper::transformInitParams(SimpleElement::create("config"), vXmlParams));
        }

    return hInstance;
    }

Object::View XmlHelper::convert(Object::View v, int32_t nType)
    {
    if (NULL == v)
        {
        return NULL;
        }

    switch (nType)
        {
        case XmlValue::type_boolean:
            {
            if (instanceof<Boolean::View>(v))
                {
                return v;
                }

            String::View vs = cast<String::View>(convert(v, XmlValue::type_string));
            if (NULL != vs && vs->length() > 0)
                {
                const char* achValue = trim(vs)->getCString();
                switch (achValue[0])
                    {
                    case 'T':   // TRUE or True
                    case 't':   // true
                    case 'Y':   // YES or Yes
                    case 'y':   // yes or y
                        return Boolean::valueOf(true);

                    case '1':   // integer representation of true
                        if (vs->length() == 1)
                            {
                            return Boolean::valueOf(true);
                            }
                        break;

                    case 'F':   // FALSE or False
                    case 'f':   // false
                    case 'N':   // NO or No
                    case 'n':   // no
                        return Boolean::valueOf(false);

                    case '0':   // integer representation of false
                        if (vs->length() == 1)
                            {
                            return Boolean::valueOf(false);
                            }
                        break;
                    }
                }

            return NULL;
            }

        case XmlValue::type_int:
            {
            if (instanceof<Integer32::View>(v))
                {
                return v;
                }

            String::View vs = cast<String::View>(convert(v, XmlValue::type_string));
            if (NULL != vs && vs->length() > 0)
                {
                return Integer32::create(Integer32::parse(trim(vs)));
                }

            return NULL;
            }

        case XmlValue::type_long:
            {
            if (instanceof<Integer64::View>(v))
                {
                return v;
                }

            String::View vs = cast<String::View>(convert(v, XmlValue::type_string));
            if (vs != NULL && vs->length() > 0)
                {
                return Integer64::create(Integer64::parse(trim(vs)));
                }

            return NULL;
            }

        case XmlValue::type_double:
            {
            if (instanceof<Float64::View>(v))
                {
                return v;
                }

            String::View vs = cast<String::View>(convert(v, XmlValue::type_string));
            if (vs != NULL && vs->length() > 0)
                {
                return Float64::create(Float64::parse(trim(vs)));
                }

            return NULL;
            }

        case XmlValue::type_string:
            {
            if (instanceof<String::View>(v))
                {
                return v;
                }

            if (instanceof<Binary::View>(v))
                {
                Binary::View           vBin = cast<Binary::View>(v);
                int32_t                len  = vBin->length();
                Array<octet_t>::Handle hab  = Array<octet_t>::create(len + 1);
                hab[0] = len;
                Array<octet_t>::copy(vBin->toOctetArray(), 0, hab, 1, len);

                OctetArrayReadBuffer::Handle    hrb  = OctetArrayReadBuffer::create(hab, 0, hab->length);
                ReadBuffer::BufferInput::Handle hbi = hrb->getBufferInput();
                return hbi->readString();
                }

            return COH_TO_STRING(v);
            }

        case XmlValue::type_binary:
            {
            if (instanceof<Binary::View>(v))
                {
                return v;
                }

            String::View vs = cast<String::View>(convert(v, XmlValue::type_string));
            if (NULL != vs)
                {
                if (vs->length() == 0)
                    {
                    return Binary::getEmptyBinary();
                    }

                OctetArrayWriteBuffer::Handle     hwb = OctetArrayWriteBuffer::create(vs->length());
                WriteBuffer::BufferOutput::Handle hbo = hwb->getBufferOutput();
                hbo->writeString(vs);
                Array<octet_t>::View vab = hwb->toOctetArray();
                return Binary::create(vab);
                }

            return NULL;
            }

        case XmlValue::type_date:
        case XmlValue::type_time:
        case XmlValue::type_datetime:
            COH_THROW_STREAM (IllegalArgumentException,
                    "Unsupported data type: " << nType);

        default:
            COH_THROW_STREAM(IllegalStateException, "Unexpected execution of code at ");
        }

    return v;
    }

COH_CLOSE_NAMESPACE3
