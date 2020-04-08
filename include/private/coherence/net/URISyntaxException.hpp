/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_URI_SYNTAX_EXCEPTION_HPP
#define COH_URI_SYNTAX_EXCEPTION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* Checked exception thrown to indicate that a string could not be parsed as a
* URI reference.
*
* @author jh  2008.02.11
*/
class COH_EXPORT URISyntaxException
    : public throwable_spec<URISyntaxException,
        extends<Exception> >
    {
    friend class factory<URISyntaxException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a URISyntaxException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new URISyntaxException
        */
        URISyntaxException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_URI_SYNTAX_EXCEPTION_HPP
