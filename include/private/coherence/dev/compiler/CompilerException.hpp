/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPILER_EXCEPTION_HPP
#define COH_COMPILER_EXCEPTION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE3(coherence,dev,compiler)


/**
* The CompilerException exception is used to stop the compilation of
* a script.  It is thrown when an element of the compilation process
* determines that compilation cannot complete successfully and that
* compilation should be stopped.  Compilation is not necessarily stopped
* when the first compilation error is encountered (as in a syntax, lexical,
* or semantic error) because the user typically wants to know about as many
* errors as possible.
*
* @author tb  2007.12.13
*/
class COH_EXPORT CompilerException
    : public throwable_spec<CompilerException,
        extends<Exception> >
    {
    friend class factory<CompilerException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a CompilerException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        CompilerException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPILER_EXCEPTION_HPP
