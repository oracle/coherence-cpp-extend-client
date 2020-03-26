/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYNTAX_EXCEPTION_HPP
#define COH_SYNTAX_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "private/coherence/dev/compiler/CompilerException.hpp"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* The SyntaxException exception is equivalent to the "syntax error" message
* that a user sees when a script cannot compile.  It allows a parser to throw
* the exception from the depths of its syntactic analysis and catch it (if
* desired) and recover at a much higher level.
*
* @author tb  2007.12.13
*/
class COH_EXPORT SyntaxException
    : public throwable_spec<SyntaxException,
        extends<CompilerException> >
    {
    friend class factory<SyntaxException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SyntaxException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new SyntaxException
        */
        SyntaxException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SYNTAX_EXCEPTION_HPP
