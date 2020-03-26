/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_MUTEX_HPP
#define COH_WINDOWS_MUTEX_HPP

#include "coherence/lang.ns"

#include "private/coherence/native/NativeMutex.hpp"
#include "private/coherence/native/NativeRecursiveMutex.hpp"

#include <iostream>
#include <sstream>
#include <windows.h>

COH_OPEN_NAMESPACE3(coherence,native,windows)


/**
* Windows implementation of a Mutex and RecursiveMutex based on critical
* sections.
*
* Window's mutexs are always recursive.
*/
class COH_EXPORT WindowsMutex
    : public NativeRecursiveMutex
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * @internal
        */
        WindowsMutex()
            : m_cLock(0)
            {
            InitializeCriticalSection(&m_mutex);
            }

        /**
        * @internl
        */
        virtual ~WindowsMutex()
            {
            COH_ENSURE_EQUALITY(m_cLock, 0);
            DeleteCriticalSection(&m_mutex);
            }


    // ----- Mutex interface ------------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual void lock()
            {
            EnterCriticalSection(&m_mutex);
            ++m_cLock;
            }


        /**
        * @inheritDoc
        */
        virtual bool tryLock()
            {
            if (TryEnterCriticalSection(&m_mutex))
                {
                ++m_cLock;
                return true;
                }
            return false;
            }

        /**
        * @inheritDoc
        */
        virtual void unlock()
            {
            --m_cLock;
            LeaveCriticalSection(&m_mutex);
            }

        /**
        * @inheritDoc
        */
        virtual size32_t sizeOf() const
            {
            return (size32_t) sizeof(WindowsMutex);
            }


    // ----- WindowsMutex interface -----------------------------------------

    public:
        /**
        * Fully unlock the mutex, this can only be called by the mutex holder
        *
        * @return the number of times the Mutex had been locked
        */
        int32_t fullyUnlock()
            {
            int32_t cLock = m_cLock;
            for (int32_t i = 0; i < cLock; ++i)
                {
                unlock();
                }
            return cLock;
            }

        /**
        * Lock the mutex multiple times
        *
        * @param cLock  the number of times to lock the mutex, note this
        *               value is updatd based on the number of times it is
        *               actually gets locked
        */
        void fullyLock(int32_t& cLock)
            {
            while (cLock--)
                {
                lock();
                }
            }

        /**
        * Assert that the calling thead owns the Mutex.
        *
        * @throw IllegalStateException if the caller does not have the Mutex
        *        locked.
        */
        void assertOwnership()
            {
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
        * The windows critical section associated with this implementation.
        */
        CRITICAL_SECTION m_mutex;

        /**
        * The number times the Mutex is currently locked.
        */
        int32_t m_cLock;
    };

COH_CLOSE_NAMESPACE3


// ----- (Recursive)Mutex factory methods -----------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeMutex* NativeMutex::create()
    {
    return new coherence::native::windows::WindowsMutex();
    }

NativeRecursiveMutex* NativeRecursiveMutex::create()
    {
    // WindowsMutexs are always recursive
    return new coherence::native::windows::WindowsMutex();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_MUTEX_HPP
