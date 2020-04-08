/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNSAFE_MUTEX_HPP
#define COH_UNSAFE_MUTEX_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeMutex.hpp"
#include "private/coherence/native/NativeRecursiveMutex.hpp"

COH_OPEN_NAMESPACE3(coherence,native,generic)


/**
* Unsafe implementation of a Mutex. This implementation is largely for
* testing purposes, though it would also be suitable for a single-threaded
* build of the Coherence library.
*/
class COH_EXPORT UnsafeMutex
    : public NativeRecursiveMutex
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * @internal
        */
        UnsafeMutex()
            {
            }

        /**
        * @internl
        */
        virtual ~UnsafeMutex()
            {
            }


    // ----- NativeMutex interface ------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void lock()
            {
            }

        /**
        * @inheritDoc
        */
        virtual bool tryLock()
            {
            return true;
            }

        /**
        * @inheritDoc
        */
        virtual void unlock()
            {
            }
    };

COH_CLOSE_NAMESPACE3


// ----- Native(Recursive)Mutex factory methods -----------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeMutex* NativeMutex::create()
    {
    return new coherence::native::generic::UnsafeMutex();
    }

NativeRecursiveMutex* NativeRecursiveMutex::create()
    {
    return new coherence::native::generic::UnsafeMutex();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_UNSAFE_MUTEX_HPP
