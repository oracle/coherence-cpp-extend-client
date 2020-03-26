/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NO_SUCH_ELEMENT_EXCEPTION_HPP
#define COH_NO_SUCH_ELEMENT_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate that a object is not found in the %cache/map/collection
*/
class COH_EXPORT NoSuchElementException
    : public throwable_spec<NoSuchElementException,
        extends<RuntimeException, std::logic_error> >
    {
    friend class factory<NoSuchElementException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an NoSuchElementException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        NoSuchElementException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NO_SUCH_ELEMENT_EXCEPTION_HPP
