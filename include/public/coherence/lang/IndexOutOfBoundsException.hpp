/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
#define COH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate that an index of some sort (such as to an array, to a
* string, or to a vector) is out of range.
*
* @author jh  2007.12.30
*/
class COH_EXPORT IndexOutOfBoundsException
    : public throwable_spec<IndexOutOfBoundsException,
        extends<RuntimeException, std::out_of_range> >
    {
    friend class factory<IndexOutOfBoundsException>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an IndexOutOfBoundsException.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        IndexOutOfBoundsException(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INDEX_OUT_OF_BOUNDS_EXCEPTION_HPP
