/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLONE_NOT_SUPPORTED_EXCEPTION_HPP
#define COH_CLONE_NOT_SUPPORTED_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* Thrown when an attempt is made to clone an object which does not support
* cloning.
*
* @author mf  2008.04.03
*/
class COH_EXPORT CloneNotSupportedException
    : public throwable_spec<CloneNotSupportedException,
        extends<Exception> >
    {
    friend class factory<CloneNotSupportedException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a CloneNotSupportedException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new CloneNotSupportedException
        */
        CloneNotSupportedException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif //COH_CLONE_NOT_SUPPORTED_EXCEPTION_HPP
