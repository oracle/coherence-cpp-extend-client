/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_TOKEN_HPP
#define COH_XML_TOKEN_HPP

#include "coherence/lang.ns"

#include "private/coherence/dev/compiler/Token.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::dev::compiler::Token;


/**
* Represents an XML language token.
*
* @author tb  2007.12.13
*/
class COH_EXPORT XmlToken
    : public class_spec<XmlToken,
        extends<Object>,
        implements<Token> >
    {
    friend class factory<XmlToken>;

    // ----- enums ----------------------------------------------------------

    public:
        /**
        * Enum for XmlToken categories
        */
        enum Category
            {
            cat_name, cat_literal, cat_separator, cat_dtdstuff
            };

        /**
        * Enum for XmlToken categories
        */
        enum SubCategory
            {
            subcat_none, subcat_lit_pi, subcat_lit_comment,
            subcat_lit_quoted, subcat_lit_chardata
            };

        /**
        * Enum for XmlToken ids
        */
        enum TokenId
            {
            none, name, literal, chardata, chardata_raw, comment,
            pi_start, pi_stop, doctype_start, element_start,
            element_stop, endtag_start, empty_stop, comment_start,
            comment_stop, equals, dtd_decl_start, dtd_decl_stop,
            dtd_element, dtd_attlist, dtd_entity, dtd_notation,
            dtd_declsep
            };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new XmlToken instance from a given XmlToken.
        *
        * @param vToken   the token to clone from
        * @param iLine    the line number of the token
        * @param of       the offset of the token within the line
        * @param cLength  the length of the token
        */
        XmlToken(XmlToken::View vToken, size32_t iLine, size32_t of = 0,
                size32_t cLength = 0);

        /**
        * Create a new XmlToken instance.
        *
        * @param eCategory     the category of the token
        * @param eSubCategory  the subcategory of the token (or none)
        * @param eTokenID      the enumerated token ID (or none)
        * @param vValue        the value of the token (or null)
        * @param vsText        the text of the token
        * @param iLine         the line number of the token
        * @param of            the offset of the token within the line
        * @param cLength       the length of the token
        */
        XmlToken(enum Category eCategory, enum SubCategory eSubCategory,
                enum TokenId eTokenID, Object::View vValue,
                String::View vsText, size32_t iLine = 0, size32_t of = 0,
                size32_t cLength = 0);


    // ----- Token interface ------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getCategory() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getSubCategory() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getID() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View getValue() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getLine() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getOffset() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t getLength() const;

        /**
        * {@inheritDoc}
        */
        virtual void adjust(int32_t iLine, int32_t of);

        /**
        * {@inheritDoc}
        */
        virtual String::View getText() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return a static "abstract" PI Start token.  The static token can be
        * used with the copy constructor to create new token instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenPiStart();

        /**
        * Return a static "abstract" PI Stop token.  The static token can be
        * used with the copy constructor to create new token instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenPiStop();

        /**
        * Return a static "abstract" Doc type Start token.  The static
        * token can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDocTypeStart();

        /**
        * Return a static "abstract" Element Start token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenElementStart();

        /**
        * Return a static "abstract" Element Stop token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenElementStop();

        /**
        * Return a static "abstract" End Tag Start token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenEndTagStart();

        /**
        * Return a static "abstract" Empty Stop token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenEmptyStop();

        /**
        * Return a static "abstract" Comment Start token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenCommentStart();

        /**
        * Return a static "abstract" Comment Stop token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenCommentStop();

        /**
        * Return a static "abstract" Equals token.  The static token can be
        * used with the copy constructor to create new token instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenEquals();

        /**
        * Return a static "abstract" DTD Decl Start token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdDeclStart();

        /**
        * Return a static "abstract" DTD Decl Stop token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdDeclStop();

        /**
        * Return a static "abstract" DTD Element token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdElement();

        /**
        * Return a static "abstract" DTD Att List token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdAttList();

        /**
        * Return a static "abstract" DTD Entity token.  The static token can
        * be used with the copy constructor to create new token instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdEntity();

        /**
        * Return a static "abstract" DTD Notation token.  The static token
        * can be used with the copy constructor to create new token
        * instances.
        *
        * @return the static token
        */
        static XmlToken::View getTokenDtdNotation();

        /**
        * Get a token category description for a given Category.
        *
        * @param category  the enumeration token category value
        *
        * @return the string description for the given category
        */
        static String::View getCategoryDescription(Category category);

        /**
        * Get a token subcategory description for a given SubCategory.
        *
        * @param subCategory  the enumeration token subcategory value
        *
        * @return the string description for the given subcategory
        */
        static String::View getSubCategoryDescription(
                SubCategory subCategory);

        /**
        * Get a token id description for a given token id.
        *
        * @param tokenId  the enumeration token id value
        *
        * @return the string description for the given token id
        */
        static String::View getTokenIdDescription(TokenId tokenId);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * the token category
        */
        enum Category m_category;

        /**
        * the token sub category
        */
        enum SubCategory m_subCategory;

        /**
        * the token id
        */
        enum TokenId m_tokenId;

        /**
        * the token's value
        */
        FinalView<Object> f_vValue;

        /**
        * the string representation of the token as it would appear in
        * a script.
        */
        FinalView<String> f_vsText;

        /**
        * the line number of the script which contains the token
        */
        size32_t m_iLine;

        /**
        * the offset of the token within the line of the script
        */
        size32_t m_of;

        /**
        * the length, in characters, of the token
        */
        size32_t m_cLength;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_TOKEN_HPP
