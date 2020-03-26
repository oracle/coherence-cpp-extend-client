/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EOF_EXCEPTION_HPP
#define COH_EOF_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

COH_OPEN_NAMESPACE2(coherence,io)


/**
* Signals that an end of file or stream has been reached unexpectedly during
* input.
*
* @author jh  2007.12.30
*/
class COH_EXPORT EOFException
    : public throwable_spec<EOFException,
        extends<IOException> >
    {
    friend class factory<EOFException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an EOFException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new EOFException
        */
        EOFException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EOF_EXCEPTION_HPP
