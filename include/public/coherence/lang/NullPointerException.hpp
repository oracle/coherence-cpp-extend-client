/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NULL_POINTER_EXCEPTION_HPP
#define COH_NULL_POINTER_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown when an application attempts to use <tt>NULL</tt> in a case where
* a non-NULL handle to Object is required. These include:
* <ul>
* <li>Calling the instance method through a <tt>NULL</tt> handle.</li>
* <li>Accessing or modifying the field through a <tt>NULL</tt> handle.</li>
* <li>Dereferencing a <tt>NULL</tt> handle.</li>
* </ul>
* Applications should throw instances of this class to indicate other illegal
* uses of <tt>NULL</tt> handles.
*/
class COH_EXPORT NullPointerException
    : public throwable_spec<NullPointerException,
        extends<RuntimeException> >
    {
    friend class factory<NullPointerException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a NullPointerException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        NullPointerException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NULL_POINTER_EXCEPTION_HPP
