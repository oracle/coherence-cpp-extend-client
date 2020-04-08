/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_CONDITION_HPP
#define COH_NATIVE_CONDITION_HPP

#include "coherence/lang/compatibility.hpp"

#include "private/coherence/native/NativeRecursiveMutex.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* Condition provides a mechanism for one thread to notify another then a
* logical condition has been satisfied.
*
* @author 2007.12.10 mf
*/
class COH_EXPORT NativeCondition
    : public virtual NativeMutex
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new Condition.
        *
        * This is not a managed object and the caller is responsible for
        * eventually deleting the returned object.
        *
        * @return the new Condition
        */
        static NativeCondition* create();


    // ----- destructor -----------------------------------------------------

    public:
        /**
        * @internal
        */
        virtual ~NativeCondition() {}


    // ----- NativeCondition interface --------------------------------------

    public:
        /**
        * Wait for the Condition to be signaled. The caller must be
        * synchronized on the Condition's Mutex when calling this method. The
        * Mutex will be released while waiting for notification and reacquired
        * before returning from this method, regadless of weather or not a
        * timeout occurs.
        *
        * Note spurious wakeups may occur so the caller must externally
        * validate the logical condition once this method returns.
        *
        * @param cMillis  the timeout duration
        *
        * @throws IllegalArgumenetException if cMillis <= 0
        */
        virtual void wait(int32_t cMillis) = 0;

        /**
        * Notify a thread that the Condition has been satisfied. If no
        * threads are currently waiting on this Condition then this is a
        * noop.  If any threads are waiting at least one will be notifed.
        *
        * The caller must be synchronized on the Mutex associated with
        * this Condition.
        */
        virtual void notify() = 0;

        /**
        * Notify all threads currently waiting on this Condition.
        *
        * The caller must be synchronized on the Mutex associated with
        * this Condition.
        */
        virtual void notifyAll() = 0;

        /**
        * Return the number of times this Condition has been notified.
        *
        * @return the notification count
        */
        virtual int32_t getNotificationCount() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_CONDITION_HPP
