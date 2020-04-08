/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_RECURSIVE_MUTEX_HPP
#define COH_NATIVE_RECURSIVE_MUTEX_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativeMutex.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* Non-Object interface for a RecursiveMutex.
*
* This interface provides the building blocks for the Object based Monitor.
*
* There will be multiple implementations of this interface based on the
* facilities available to the environment.  Each implementation will implement
* this interfaces "create" method, and thus only one will ever be linked into
* a given distribution of the Coherence library.
*/
class COH_EXPORT NativeRecursiveMutex
        : public virtual NativeMutex
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new RecursiveMutex, as this is not an Object it is the
        * responsibility of the caller to eventually delete this object.
        *
        * @return a pointer to the new Mutex
        */
        static NativeRecursiveMutex* create();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_RECURSIVE_MUTEX_HPP
