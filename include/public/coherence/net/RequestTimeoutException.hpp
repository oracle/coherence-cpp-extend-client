/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REQUEST_TIMEOUT_EXCEPTION_HPP
#define COH_REQUEST_TIMEOUT_EXCEPTION_HPP

#include "coherence/lang.ns"

#include "coherence/net/RequestIncompleteException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* Signals that a request execution in a clustered environment did not
* complete in a pre-determined amount of time. For some specific requests
* this exception could carry a partial execution result.
*
* @author jh  2008.02.14
*/
class COH_EXPORT RequestTimeoutException
    : public throwable_spec<RequestTimeoutException,
        extends<RequestIncompleteException> >
    {
    friend class factory<RequestTimeoutException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a RequestTimeoutException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        RequestTimeoutException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);

    };

COH_CLOSE_NAMESPACE2

#endif // COH_REQUEST_TIMEOUT_EXCEPTION_HPP
