/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_THREAD_LOCAL_HPP
#define COH_POSIX_THREAD_LOCAL_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeThreadLocal.hpp"

COH_OPEN_NAMESPACE3(coherence,native,posix)

#include <iostream>
#include <pthread.h>
#include <sstream>
#include <sys/errno.h>


/**
* Posix implementation of NativeThreadLocal
*
* @author mf 2007.12.12
*/
class COH_EXPORT PosixThreadLocal
    : public NativeThreadLocal
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create a posix thread local variable.
        */
        PosixThreadLocal(CleanupFunction func)
            {
            COH_ENSURE_EQUALITY(pthread_key_create(&m_key, func), 0);
            }

        virtual ~PosixThreadLocal()
            {
            COH_ENSURE_EQUALITY(pthread_key_delete(m_key), 0);
            }


    // ----- NativeThreadLocal interface ------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void set(void* pvValue)
            {
            COH_ENSURE_EQUALITY(pthread_setspecific(m_key, pvValue), 0);
            }

        /**
        * @inheritDoc
        */
        virtual void* get() const
            {
            return pthread_getspecific(m_key);
            }


    // ----- data members ---------------------------------------------------

    protected:

        /**
        * The posix key associated with the ThreadLocal.
        */
        pthread_key_t m_key;
    };

COH_CLOSE_NAMESPACE3


// ----- NativeThreadLocal factory methods ----------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeThreadLocal* NativeThreadLocal::create(CleanupFunction func)
    {
    return new coherence::native::posix::PosixThreadLocal(func);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_THREAD_LOCAL_HPP
