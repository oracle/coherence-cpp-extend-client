/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNSUPPORTED_OPERATION_EXCEPTION_HPP
#define COH_UNSUPPORTED_OPERATION_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate that the requested operation is not supported.
*
* @author jh  2007.11.07
*/
class COH_EXPORT UnsupportedOperationException
    : public throwable_spec<UnsupportedOperationException,
        extends<RuntimeException> >
    {
    friend class factory<UnsupportedOperationException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an UnsupportedOperationException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new UnsupportedOperationException
        */
        UnsupportedOperationException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_UNSUPPORTED_OPERATION_EXCEPTION_HPP
