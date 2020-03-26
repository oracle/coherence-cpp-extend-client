/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONCURRENT_MODIFICATION_EXCEPTION_HPP
#define COH_CONCURRENT_MODIFICATION_EXCEPTION_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Thrown to indicate that a non-permissible concurrent modification of an
* object has been detected.
*/
class COH_EXPORT ConcurrentModificationException
    : public throwable_spec<ConcurrentModificationException,
        extends<RuntimeException> >
    {
    friend class factory<ConcurrentModificationException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an ConcurrentModificationException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        *
        * @return a new ConcurrentModificationException
        */
        ConcurrentModificationException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONCURRENT_MODIFICATION_EXCEPTION_HPP
