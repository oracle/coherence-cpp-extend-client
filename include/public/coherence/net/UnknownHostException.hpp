/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNKNOWN_HOST_EXCEPTION_HPP
#define COH_UNKNOWN_HOST_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::IOException;


/**
* UnknownHostException is thrown as a result of a failed hostname resolution.
*
* @author mf  2008.03.17
*/
class COH_EXPORT UnknownHostException
    : public throwable_spec<UnknownHostException,
        extends<IOException> >
    {
    friend class factory<UnknownHostException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an UnknownHostException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new UnknownHostException
        */
        UnknownHostException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_UNKNOWN_HOST_EXCEPTION_HPP
