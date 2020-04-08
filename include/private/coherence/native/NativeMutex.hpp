/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_MUTEX_HPP
#define COH_NATIVE_MUTEX_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using namespace coherence::lang;


/**
* Non-Object interface for a Mutex.
*
* Note Mutex's are not required to support recursive locking, the result of
* a single thread locking a mutex it already has locked is platform dependent
* and may either succeede, deadlock, or result in an exception.  If recursive
* behavior is required then use RecursiveMutex.
*
* @author mf 2007.12.09
*/
class COH_EXPORT NativeMutex
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new Mutex, as this is not an managed Object it is the
        * responsibility of the caller to eventually delete this object.
        *
        * @return a pointer to the new Mutex
        */
        static NativeMutex* create();


    // ----- destructor -----------------------------------------------------

    public:
        /**
        * @internal
        */
        virtual ~NativeMutex() {};


    // ----- Mutex interface ------------------------------------------------

    public:
        /**
        * Lock the mutex, waiting as long as necessary.
        */
        virtual void lock() = 0;

        /**
        * Attempt to lock the mutex
        *
        * @return true if the calling thread acquires the mutex
        */
        virtual bool tryLock() = 0;

        /**
        * Unlock the mutex.
        *
        * This must be called by the thread which locked the mutex.
        */
        virtual void unlock() = 0;

        /**
         * Return the size of this object.
         */
        virtual size32_t sizeOf() const = 0;
    };

COH_CLOSE_NAMESPACE2

/**
* Macro for making more readable syncronized code blocks See the
* documentation of SynchronizedBlock for a useage example.
*/
#define COH_SYNCHRONIZED_NATIVE(PMUTEX) \
    if (coherence::lang::SynchronizedBlock<coherence::native::NativeMutex*> \
        _coh_sync_##__LINE__ = PMUTEX)

#endif // COH_NATIVE_MUTEX_HPP
