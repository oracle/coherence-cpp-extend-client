/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TOKENIZER_HPP
#define COH_TOKENIZER_HPP

#include "coherence/lang.ns"

#include "private/coherence/dev/compiler/ErrorList.hpp"
#include "private/coherence/dev/compiler/ParsePosition.hpp"
#include "private/coherence/dev/compiler/Script.hpp"
#include "private/coherence/dev/compiler/Token.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* Represents a lexical tokenizer.  This interface is implemented for each
* specific language.
*
* @author tb  2007.12.13
*/
class COH_EXPORT Tokenizer
    : public interface_spec<Tokenizer>
    {
    // ----- Tokenizer interface --------------------------------------------

    public:
        /**
        * Initializes the Tokenizer.  This method must be called exactly
        * one time to initialize the Tokenizer object.
        *
        * @param hScript   the Script to tokenize
        * @param hErrlist  the ErrorList to log errors to
        *
        * @exception NoSuchElementException  if the tokens are exhausted
        * @exception CompilerException if a lexical error occurs that should
        *            stop the compilation process
        */
        virtual void setScript(Script::Handle hScript,
                ErrorList::Handle hErrlist) = 0;

        /**
        * Checks for more tokens in the script.
        *
        * @return true if tokenizing of the script is incomplete
        */
        virtual bool hasMoreTokens()const = 0;

        /**
        * Eats and returns the next token from the script.
        *
        * @return the next token
        *
        * @exception NoSuchElementException  if the tokens are exhausted
        * @exception CompilerException if a lexical error occurs that should
        *            stop the compilation process
        */
        virtual Token::View nextToken() = 0;

        /**
        * Regurgitates the last eaten token so that the next call to
        * nextToken will return the same token that was returned by the most
        * recent call to nextToken.  (This method can be called more than
        * once to regurgitate multiple tokens.)
        *
        * @param vToken  the token to put back
        *
        * @exception NoSuchElementException an attempt to back up past the
        *            beginning of the script -or- the dequeue was not
        *            constructed large enough to hold the number of entries
        *            that have been put back.
        */
        virtual void putBackToken(Token::View vToken) = 0;

        /**
        * Returns an object that can be used to restore the current position
        * in the script.  This method is similar to the mark method of the
        * Java stream classes, but by returning an object that identifies the
        * position, multiple positions can be saved and later returned to.
        *
        * @return an object which identifies the current position within the
        *         script
        */
        virtual ParsePosition::Handle savePosition() const = 0;

        /**
        * Restores the current parsing position that was returned from
        * the savePosition method.
        *
        * @param hParsePos  the return value from a previous call to
        *                   savePosition
        */
        virtual void restorePosition(ParsePosition::Handle hParsePos) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_TOKENIZER_HPP
