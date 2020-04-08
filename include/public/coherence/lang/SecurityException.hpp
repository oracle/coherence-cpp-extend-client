/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SECURITY_EXCEPTION_HPP
#define COH_SECURITY_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate a security violation.
*/
class COH_EXPORT SecurityException
    : public throwable_spec<SecurityException,
        extends<RuntimeException> >
    {
    friend class factory<SecurityException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a SecurityException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        SecurityException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SECURITY_EXCEPTION_HPP
