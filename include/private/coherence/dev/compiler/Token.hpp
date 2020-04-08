/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TOKEN_HPP
#define COH_TOKEN_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* Represents a language token.  This interface is implemented for each
* specific language, and potentially for each token or category of
* tokens within each language.  This interface is intended only as the
* basis for a token object's actual interface; other methods will be
* necessary to describe tokens that have values of a particular type,
* for example.
*
* @author tb  2007.12.13
*/
class COH_EXPORT Token
    : public interface_spec<Token>
    {
    // ----- Token interface ------------------------------------------------

    public:
        /**
        * Determines the category that a token belongs to.
        *
        * @return the category of the token
        */
        virtual int32_t getCategory() const = 0;

        /**
        * Determines the sub-category that a token belongs to.
        *
        * @return the sub-category of the token or 0 if sub-categorization is
        *         not applicable to this token
        */
        virtual int32_t getSubCategory() const = 0;

        /**
        * Determines the unique identifier of a predefined token.  In Java,
        * the keywords, operators, separators, boolean and null literals are
        * the only pre-defined tokens.
        *
        * @return the token identifier, or 0 if the token does not have an ID
        */
        virtual int32_t getID() const = 0;

        /**
        * Provides the value of the token if applicable.  This is used for
        * for Java literals.
        *
        * @return the value of the token or null if the token has no value
        */
        virtual Object::View getValue() const = 0;

        /**
        * Determines the line of the script which contains the token.  The
        * line number is zero-based and corresponds to the line in which the
        * first character of the token was encountered.
        *
        * @return the token's line number
        */
        virtual size32_t getLine() const = 0;

        /**
        * Determines the offset of the token within the line of the script.
        * The offset is zero-based and specifies the location of the first
        * character of the token.
        *
        * @return the token's offset within the line
        */
        virtual size32_t getOffset() const = 0;

        /**
        * Determines the length, in characters, of the token.  The length of
        * the token is calculated as the difference between the offset of the
        * first character following the token and the offset of the first
        * character of the token itself.
        *
        * @return the length of the token
        */
        virtual size32_t getLength() const = 0;

        /**
        * Adjusts the location (line/offset) of the token relative to its
        * current location.
        *
        * @param iLine  the lines to adjust
        * @param of     the offset to adjust
        */
        virtual void adjust(int32_t iLine, int32_t of) = 0;

        /**
        * Provides a string representation of the token as it would appear in
        * a script.  In other words, this method reverse-engineers the token.
        *
        * @return the Java token as it would appear in a script
        */
        virtual String::View getText() const = 0;
     };

COH_CLOSE_NAMESPACE3

#endif // COH_TOKEN_HPP
