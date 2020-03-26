/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ARITHMETIC_EXCEPTION_HPP
#define COH_ARITHMETIC_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/String.hpp"
#include "coherence/lang/RuntimeException.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* An exception that indicates exceptional condition related to
* arithmetic operations.
*/
class COH_EXPORT ArithmeticException
    : public throwable_spec<ArithmeticException,
        extends<RuntimeException, std::domain_error> >
    {
    friend class factory<ArithmeticException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an ArithmeticException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        ArithmeticException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ARITHMETIC_EXCEPTION_HPP
