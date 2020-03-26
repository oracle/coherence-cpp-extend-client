/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_MUTEX_HPP
#define COH_POSIX_MUTEX_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeMutex.hpp"
#include "private/coherence/native/NativeRecursiveMutex.hpp"

#include <iostream>
#include <pthread.h>
#include <sstream>
#include <sys/errno.h>

COH_OPEN_NAMESPACE3(coherence,native,posix)

/**
* Posix implementation of a Mutex and RecursiveMutex.
*/
class COH_EXPORT PosixMutex
    : public NativeRecursiveMutex
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * @internal
        */
        PosixMutex(bool fRecursive)
                : m_cLock(0)
            {
            COH_ENSURE_EQUALITY(pthread_mutexattr_init(&m_attr), 0);
            if (fRecursive)
                {
                COH_ENSURE_EQUALITY(pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE), 0);
                }
            COH_ENSURE_EQUALITY(pthread_mutex_init(&m_mutex, &m_attr), 0);
            }

        /**
        * @internl
        */
        virtual ~PosixMutex()
            {
            COH_ENSURE_EQUALITY(m_cLock, 0);
            // pthread docs show destroying the attr before the mutex
            COH_ENSURE_EQUALITY(pthread_mutexattr_destroy(&m_attr), 0);
            COH_ENSURE_EQUALITY(pthread_mutex_destroy(&m_mutex), 0);
            }


    // ----- NativeMutex interface ------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void lock()
            {
            COH_ENSURE_EQUALITY(pthread_mutex_lock(&m_mutex), 0);
            ++m_cLock;
            }

        /**
        * @inheritDoc
        */
        virtual bool tryLock()
            {
            int nResult = pthread_mutex_trylock(&m_mutex);
            switch (nResult)
                {
                case 0: // lock acquired
                    ++m_cLock;
                    return true;

                case EBUSY: // locked by another thread
                    return false;

                default: // error
                    COH_THROW_STREAM(IllegalStateException,
                        "Error while performing tryLock on posix mutex "
                            << &m_mutex << " result " << nResult);
                }
            }

        /**
        * @inheritDoc
        */
        virtual void unlock()
            {
            --m_cLock;
            COH_ENSURE_EQUALITY(pthread_mutex_unlock(&m_mutex), 0);
            }

        /**
        * @inheritDoc
        */
        virtual size32_t sizeOf() const
            {
            return (size32_t) sizeof(PosixMutex);
            }

    // ----- PosixMutex interface -------------------------------------------

    public:
        /**
        * Return a pointer to the native mutex which wrapped by this class.
        *
        * This class retains responsibility for deletion of the returned
        * mutex, and it will remain valid for the lifetime of this object.
        *
        * @return the underlying native mutex
        */
        virtual pthread_mutex_t* getPThreadMutex()
            {
            return &m_mutex;
            }

        /**
        * Assert that the calling thead owns the Mutex.
        *
        * @throw IllegalStateException if the caller does not have the Mutex
        *        locked.
        */
        void assertOwnership()
            {
            // NOTE: testing has shown this to have no signficant performance
            // cost
            // TODO: switch to throwing IllegalMonitorStateException
            int32_t cLock = 0;
            if (tryLock())
                {
                cLock = m_cLock;
                unlock();
                }
            COH_ENSURE(cLock > 1);
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The posix mutex associated with this implementation.
        */
        pthread_mutex_t m_mutex;

        /**
        * The mutex initialization attributes.
        */
        pthread_mutexattr_t m_attr;

        /**
        * The number times the Mutex is currently locked.
        */
        int32_t m_cLock;
    };

COH_CLOSE_NAMESPACE3


// ----- Native(Recursive)Mutex factory methods -----------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeMutex* NativeMutex::create()
    {
    return new coherence::native::posix::PosixMutex(false);
    }

NativeRecursiveMutex* NativeRecursiveMutex::create()
    {
    return new coherence::native::posix::PosixMutex(true);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_MUTEX_HPP
