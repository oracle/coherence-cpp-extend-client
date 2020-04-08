/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_CAST_EXCEPTION_HPP
#define COH_CLASS_CAST_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown when an attempt is made to cast to an incompatible type.
*
* @author mf  2008.05.14
*/
class COH_EXPORT ClassCastException
    : public throwable_spec<ClassCastException,
        extends<RuntimeException, std::bad_cast> >
    {
    friend class factory<ClassCastException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an ClassCastException.
        *
        * @param vsMessage  the exception message
        * @param vCause     the underlying cause of the exception;
        *                   can be <tt>NULL</tt>
        */
        ClassCastException(String::View vsMessage = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CLASS_CAST_EXCEPTION_HPP
