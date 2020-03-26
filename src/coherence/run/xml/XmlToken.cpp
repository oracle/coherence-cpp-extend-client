/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/run/xml/XmlToken.hpp"

#include <sstream>

COH_OPEN_NAMESPACE3(coherence,run,xml)


// ----- constructors -------------------------------------------------------

XmlToken::XmlToken(XmlToken::View vToken, size32_t iLine, size32_t of,
        size32_t cLength)
        : m_category(vToken->m_category),
          m_subCategory(vToken->m_subCategory),
          m_tokenId(vToken->m_tokenId),
          f_vValue(self(), vToken->f_vValue),
          f_vsText(self(), vToken->f_vsText),
          m_iLine(iLine),
          m_of(of),
          m_cLength(cLength)
    {
    }

XmlToken::XmlToken(enum Category category, enum SubCategory subCategory,
        enum TokenId tokenID, Object::View vValue, String::View vsText,
        size32_t iLine, size32_t of, size32_t cLength)
        : m_category(category),
          m_subCategory(subCategory),
          m_tokenId(tokenID),
          f_vValue(self(), vValue),
          f_vsText(self(), vsText),
          m_iLine(iLine),
          m_of(of),
          m_cLength(cLength)
    {
    }


// ----- token functions ----------------------------------------------------

int32_t XmlToken::getCategory() const
    {
    return m_category;
    }

int32_t XmlToken::getSubCategory() const
    {
    return m_subCategory;
    }

int32_t XmlToken::getID() const
    {
    return m_tokenId;
    }

Object::View XmlToken::getValue() const
    {
    return f_vValue;
    }

size32_t XmlToken::getLine() const
    {
    return m_iLine;
    }

size32_t XmlToken::getOffset() const
    {
    return m_of;
    }

size32_t XmlToken::getLength() const
    {
    return m_cLength;
    }

void XmlToken::adjust(int32_t iLine, int32_t of)
    {
    int32_t iCurrentLine = (int32_t) m_iLine;
    COH_ENSURE((size32_t) iCurrentLine == m_iLine);
    int32_t ofCurrent    = (int32_t) m_of;
    COH_ENSURE((size32_t) iCurrentLine == m_iLine);

    iCurrentLine += iLine;
    ofCurrent    += of;

    m_iLine = iCurrentLine < 0 ? 0 : iCurrentLine;
    m_of = ofCurrent < 0 ? 0 : ofCurrent;
    }

String::View XmlToken::getText() const
    {
    return f_vsText;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> XmlToken::toString() const
    {
    // location/length info
    String::View vs = "(";

    // add the category (always applicable)
    vs = COH_TO_STRING(vs << getCategoryDescription(m_category)->getCString() << ':'
            << getText()->getCString() << " on Line:" << getLine()
            << " at Offset:" << getOffset() << ')');

    // add the sub-category (if applicable)
    if (getSubCategory() != subcat_none)
        {
        vs = COH_TO_STRING(vs << " sub-token:" << getSubCategoryDescription(m_subCategory)->getCString());
        }

    return vs;
    }


// ----- constants ----------------------------------------------------------

String::View XmlToken::getCategoryDescription(XmlToken::Category category)
    {
    static FinalView<String> vName(System::common(), String::create("Name"));
    static FinalView<String> vLiteral(System::common(), String::create("Literal"));
    static FinalView<String> vSeperator(System::common(), String::create("Separator"));
    static FinalView<String> vDTD(System::common(), String::create("DTD Stuff"));

    switch(category)
        {
        case cat_name:
            return vName;
        case cat_literal:
            return vLiteral;
        case cat_separator:
            return vSeperator;
        case cat_dtdstuff:
            return vDTD;
        default:
            COH_THROW_STREAM (IllegalArgumentException, "invalid category"
                    << category);
        };
    }
COH_STATIC_INIT(XmlToken::getCategoryDescription(XmlToken::cat_name));

String::View XmlToken::getSubCategoryDescription(
        XmlToken::SubCategory subCategory)
    {
    static FinalView<String> vNone(System::common(), String::create("None"));
    static FinalView<String> vPI(System::common(), String::create("PI"));
    static FinalView<String> vComment(System::common(), String::create("Comment"));
    static FinalView<String> vQuoted(System::common(), String::create("Quoted"));
    static FinalView<String> vChar(System::common(), String::create("Character Data"));

    switch (subCategory)
        {
        case subcat_none:
            return vNone;
        case subcat_lit_pi:
            return vPI;
        case subcat_lit_comment:
            return vComment;
        case subcat_lit_quoted:
            return vQuoted;
        case subcat_lit_chardata:
            return vChar;
        default:
            COH_THROW_STREAM (IllegalArgumentException, "invalid sub-category"
                    << subCategory);
        }
    }
COH_STATIC_INIT(XmlToken::getSubCategoryDescription(XmlToken::subcat_none));

String::View XmlToken::getTokenIdDescription(XmlToken::TokenId tokenId)
    {
    static FinalView<String> vsNone(System::common(), String::create("none"));
    static FinalView<String> vsName(System::common(), String::create("name"));
    static FinalView<String> vsLiteral(System::common(), String::create("literal"));
    static FinalView<String> vsChar(System::common(), String::create("character data"));
    static FinalView<String> vsCData(System::common(), String::create("CDATA"));
    static FinalView<String> vsComment(System::common(), String::create("comment"));
    static FinalView<String> vsPIStart(System::common(), String::create("<?"));
    static FinalView<String> vsPIStop(System::common(), String::create("?>"));
    static FinalView<String> vsDocStart(System::common(), String::create("<!DOCTYPE"));
    static FinalView<String> vsElemStart(System::common(), String::create("<"));
    static FinalView<String> vsElemStop(System::common(), String::create(">"));
    static FinalView<String> vsEndTagStart(System::common(), String::create("</"));
    static FinalView<String> vsEmptyStop(System::common(), String::create("/>"));
    static FinalView<String> vsCommentStart(System::common(), String::create("<!--"));
    static FinalView<String> vsCommentStop(System::common(), String::create("-->"));
    static FinalView<String> vsEquals(System::common(), String::create("="));
    static FinalView<String> vsDeclStart(System::common(), String::create("["));
    static FinalView<String> vsDeclStop(System::common(), String::create("]"));
    static FinalView<String> vsElement(System::common(), String::create("<!ELEMENT"));
    static FinalView<String> vsAttList(System::common(), String::create("<!ATTLIST"));
    static FinalView<String> vsEntity(System::common(), String::create("<!ENTITY"));
    static FinalView<String> vsNotation(System::common(), String::create("<!NOTATION"));
    static FinalView<String> vsDeclSep(System::common(), String::create("%name;"));

    switch (tokenId)
        {
        case none:
            return vsNone;
        case name:
            return vsName;
        case literal:
            return vsLiteral;
        case chardata:
            return vsChar;
        case chardata_raw:
            return vsCData;
        case comment:
            return vsComment;
        case pi_start:
            return vsPIStart;
        case pi_stop:
            return vsPIStop;
        case doctype_start:
            return vsDocStart;
        case element_start:
            return vsElemStart;
        case element_stop:
            return vsElemStop;
        case endtag_start:
            return vsEndTagStart;
        case empty_stop:
            return vsEmptyStop;
        case comment_start:
            return vsCommentStart;
        case comment_stop:
            return vsCommentStop;
        case equals:
            return vsEquals;
        case dtd_decl_start:
            return vsDeclStart;
        case dtd_decl_stop:
            return vsDeclStop;
        case dtd_element:
            return vsElement;
        case dtd_attlist:
            return vsAttList;
        case dtd_entity:
            return vsEntity;
        case dtd_notation:
            return vsNotation;
        case dtd_declsep:
            return vsDeclSep;
        default:
            COH_THROW_STREAM (IllegalArgumentException, "invalid tokenid"
                    << tokenId);
        }
    }
COH_STATIC_INIT(XmlToken::getTokenIdDescription(XmlToken::none));

XmlToken::View XmlToken::getTokenPiStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::pi_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::pi_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenPiStart());

XmlToken::View XmlToken::getTokenPiStop()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::pi_stop,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::pi_stop)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenPiStop());

XmlToken::View XmlToken::getTokenDocTypeStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::doctype_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::doctype_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDocTypeStart());

XmlToken::View XmlToken::getTokenElementStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::element_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::element_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenElementStart());

XmlToken::View XmlToken::getTokenElementStop()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::element_stop,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::element_stop)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenElementStop());

XmlToken::View XmlToken::getTokenEndTagStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::endtag_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::endtag_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenEndTagStart());

XmlToken::View XmlToken::getTokenEmptyStop()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::empty_stop,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::empty_stop)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenEmptyStop());

XmlToken::View XmlToken::getTokenCommentStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::comment_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::comment_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenCommentStart());

XmlToken::View XmlToken::getTokenCommentStop()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::comment_stop,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::comment_stop)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenCommentStop());

XmlToken::View XmlToken::getTokenEquals()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::equals,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::equals)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenEquals());

XmlToken::View XmlToken::getTokenDtdDeclStart()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::dtd_decl_start,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_decl_start)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdDeclStart());

XmlToken::View XmlToken::getTokenDtdDeclStop()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_separator,
                XmlToken::subcat_none,
                XmlToken::dtd_decl_stop,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_decl_stop)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdDeclStop());

XmlToken::View XmlToken::getTokenDtdElement()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_dtdstuff,
                XmlToken::subcat_none,
                XmlToken::dtd_element,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_element)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdElement());

XmlToken::View XmlToken::getTokenDtdAttList()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_dtdstuff,
                XmlToken::subcat_none,
                XmlToken::dtd_attlist,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_attlist)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdAttList());

XmlToken::View XmlToken::getTokenDtdEntity()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_dtdstuff,
                XmlToken::subcat_none,
                XmlToken::dtd_entity,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_entity)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdEntity());

XmlToken::View XmlToken::getTokenDtdNotation()
    {
    static FinalView<XmlToken> vToken(System::common(),
            XmlToken::create(
                XmlToken::cat_dtdstuff,
                XmlToken::subcat_none,
                XmlToken::dtd_notation,
                (Object::View) NULL,
                XmlToken::getTokenIdDescription(XmlToken::dtd_notation)));
    return vToken;
    }
COH_STATIC_INIT(XmlToken::getTokenDtdNotation());

COH_CLOSE_NAMESPACE3
