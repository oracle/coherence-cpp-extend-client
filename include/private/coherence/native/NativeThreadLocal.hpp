/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_THREAD_LOCAL_HPP
#define COH_NATIVE_THREAD_LOCAL_HPP

#include "coherence/lang/compatibility.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

extern "C"
    {
    typedef void (*CleanupFunction)(void*);
    }


/**
* NativeThreadLocal provides platform independent thread local storage.
*
* The value type is void* which allows storage of simple integer values, or
* pointers. If pointers are stored then it is important to also register a
* cleanup function. Cleanup functions are automatically run when threads
* terminate.
*
* Direct use of this NativeThreadLocals is not recommended, instead see
* coherence::lang::Thread for its wrapper implementation. The underlying
* OS implementations specify a limitations on the total number of TLSs
* which can be created, generaly ~128.
*
* @author mf 2007.12.12
*/
class COH_EXPORT NativeThreadLocal
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create and return a new NativeThreadLocal.
        *
        * @param func the cleanup function associated with this key, or NULL
        *             if the key does not require cleanup.
        */
        static NativeThreadLocal* create(CleanupFunction func = NULL);


    // ----- destructor -----------------------------------------------------

    public:
        virtual ~NativeThreadLocal() {}


    // ----- NativeThreadLocal interface ------------------------------------

    public:
        /**
        * Set the value for the calling thread.
        *
        * @param pvValue the calling thread's value
        */
        virtual void set(void* pvValue) = 0;

        /**
        * Get the value for the calling thread.
        *
        * @return the calling thread's value, or NULL if the value has not be
        *         set.
        */
        virtual void* get() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_THREAD_LOCAL_HPP
