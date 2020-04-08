/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OUT_OF_MEMORY_ERROR
#define COH_OUT_OF_MEMORY_ERROR

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Exception.hpp"
#include "coherence/lang/RuntimeException.hpp"
#include "coherence/lang/String.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Thrown to indicate that an attempt to allocate an object failed due to
* resource constraints.
*/
class COH_EXPORT OutOfMemoryError
    : public throwable_spec<OutOfMemoryError,
        extends<RuntimeException, std::bad_alloc> >
    {
    friend class factory<OutOfMemoryError>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a OutOfMemoryError.
        *
        * @param vsMsg   the detail message
        * @param vCause  the underlying cause of the exception
        */
        OutOfMemoryError(String::View vsMsg = String::null_string,
                Exception::View vCause = NULL);


    // ----- static helpers -------------------------------------------------

    public:
        /**
        * Ensure that the supplied pointer is non-NULL.  If NULL then an
        * OutOfMemoryErrorWill be thrown.
        *
        * @param pAlloc  the pointer to validate
        * @param cb      the size of the allocation request
        */
        static void ensure(void* pAlloc, size_t cb = 0);
    };


COH_CLOSE_NAMESPACE2

#endif // COH_OUT_OF_MEMORY_ERROR
