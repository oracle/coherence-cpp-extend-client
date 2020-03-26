/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RUNTIME_EXCEPTION_HPP
#define COH_RUNTIME_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* RuntimeException is the superclass for exceptions which are not normally
* handled by user code.
*/
class COH_EXPORT RuntimeException
    : public throwable_spec<RuntimeException,
        extends<Exception, std::runtime_error> >
    {
    friend class factory<RuntimeException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an RuntimeException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new RuntimeException
        */
        RuntimeException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_RUNTIME_EXCEPTION_HPP
