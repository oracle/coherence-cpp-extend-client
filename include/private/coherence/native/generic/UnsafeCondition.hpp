/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNSAFE_CONDITION_HPP
#define COH_UNSAFE_CONDITION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"

#include "private/coherence/native/NativeCondition.hpp"
#include "private/coherence/native/NativeMutex.hpp"
#include "private/coherence/native/generic/UnsafeMutex.hpp"

COH_OPEN_NAMESPACE3(coherence,native,generic)


/**
* Unsafe implementation of Condition. This implementation is largely for
* testing purposes, though it would also be suitable for a single-threaded
* build of the Coherence library.
*/
class COH_EXPORT UnsafeCondition
    : public NativeCondition
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Create a UnsafeCondition associated with a given UnsafeMutex.
        */
        UnsafeCondition()
            : m_cNotifications(0)
            {
            }

        /**
        * @internal
        */
        virtual ~UnsafeCondition()
            {
            }


    // ----- Condition interface --------------------------------------------

    public:
        /**
        * @inheritDoc
        */
        virtual int32_t wait(int32_t cMillis)
            {
            return -1; // TODO: sleep
            }

        /**
        * @inheritDoc
        */
        virtual void notify()
            {
            ++m_cNotifications;
            }

        /**
        * @inheritDoc
        */
        virtual void notifyAll()
            {
            ++m_cNotifications;
            }

        /**
        * @inheritDoc
        */
        virtual NativeRecursiveMutex& getMutex()
            {
            return m_mutex;
            }

        /**
        * @inheritDoc
        */
        virtual int32_t getNotificationCount() const
            {
            return m_cNotifications;
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Mutex associated with this Condition.
        */
        UnsafeMutex m_mutex;

        /**
        * The Notification count.
        */
        int32_t m_cNotifications;
    };

COH_CLOSE_NAMESPACE3


// ---- NativeCondition factory methods -------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeCondition* NativeCondition::create()
    {
    return new coherence::native::generic::UnsafeCondition();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_UNSAFE_CONDITION_HPP
