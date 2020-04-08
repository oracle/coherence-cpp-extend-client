/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ALLOCATOR_HPP
#define COH_ALLOCATOR_HPP

#include "coherence/lang/compatibility.hpp"

#include <string.h>

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* Custom heap allocator.
*
* @author mf 2008.11.14
*/
class COH_EXPORT Allocator
    {
    public:
        /**
        * Allocate a block of heap memory.
        *
        * The memory will be zeroed out before returning
        *
        * @param cb  the required block size.
        *
        * @return the block
        *
        * @throws OutOfMemoryError if the request cannot be satisified
        */
        static void* allocate(size_t cb);

        /**
        * Release a block of heap memory.
        *
        * @param p  the block to free.
        *
        * @throws IllegalStateException upon memory corruption detection
        */
        static void release(void* ab);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ALLOCATOR_HPP
