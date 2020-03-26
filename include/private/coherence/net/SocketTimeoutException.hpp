/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOCKET_TIMEOUT_EXCEPTION_HPP
#define COH_SOCKET_TIMEOUT_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/io/InterruptedIOException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::InterruptedIOException;


/**
* Signals that a timeout has occurred on a socket read or accept.
*/
class COH_EXPORT SocketTimeoutException
    : public throwable_spec<SocketTimeoutException,
        extends<InterruptedIOException> >
    {
    friend class factory<SocketTimeoutException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SocketTimeoutException.
        *
        * @param vsMsg             detail message
        * @param cBytesTransfered  the number of bytes transfered during the
        *                          blocked operation prior to the interrupt
        * @param vCause            the underlying cause of the exception
        *
        * @return a new SocketTimeoutException
        */
        SocketTimeoutException(String::View vsMsg = String::null_string,
                size32_t cBytesTransfered = 0,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SOCKET_TIMEOUT_EXCEPTION_HPP
