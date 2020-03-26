/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUEUE_HPP
#define COH_QUEUE_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The Queue provides a means to efficiently (and in a thread-safe manner)
* queue received messages and messages to be sent.
*
* @author nsa 2008.01.18
*/
class COH_EXPORT Queue
    : public interface_spec<Queue>
    {
    // ----- Queue interface ------------------------------------------------

    public:
        /**
        * Appends the specified element to the end of this queue. Queues may
        * place limitations on what types of elements may be added and should
        * clearly specify in their documentation any restrictions.
        *
        * @param oh  element to be appended to this Queue
        *
        * @return true if the collection changed as a result of this call
        *
        * @throw ClassCastException if the class of the specified element
        *        prevents it from being added to this Queue
        */
        virtual bool add(Object::Holder oh) = 0;

        /**
        * Insert the specified element to the front of this queue. Queues may
        * place limitations on what types of elements may be added and should
        * clearly specify in their documentation any restrictions.
        *
        * @param oh  element ot be prepended to this Queue
        *
        * @return true if the collection changed as a result of this call
        *
        * @throw ClassCastException if the class of the specified element
        *        prevents it from being added to this Queue
        */
        virtual bool addHead(Object::Holder oh) = 0;

        /**
        * Flush the queue.
        */
        virtual void flush() = 0;

        /**
        * Determine whether the Queue is empty or not.
        *
        * @return true if the Queue is empty; false if not
        */
        virtual bool isEmpty() const = 0;

        /**
        * Returns the first element from the front of this Queue.
        *
        * There is no blocking equivalent of this method as it would require
        * notification to wake up from an empty Queue, and this would mean
        * that the "add" and "addHead" methods would need to perform
        * notifyAll over notify which has performance implications.
        *
        * @return the first element in the front of this Queue or null if
        *         the Queue is empty
        */
        virtual Object::Holder peekNoWait() = 0;

        /**
        * Waits for and removes the first element from the front of this
        * Queue.
        *
        * If the Queue is empty, this method will block until an element is
        * in the Queue. The unblocking equivalent of this method is
        * "removeNoWait".
        *
        * @return the first element in the front of this Queue
        */
        virtual Object::Holder remove() = 0;

        /**
        * Removes and returns the first element from the front of this Queue.
        *
        * The blocking equivalent of this method is "remove".
        *
        * @return the first element in the front of this Queue or NULL if
        *         the Queue is empty
        */
        virtual Object::Holder removeNoWait() = 0;

        /**
        * Determine the number of elements in this Queue. The size of the
        * Queue may change after the size is returned from this method.
        *
        * @return the number of elements in this Queue
        */
        virtual size32_t size() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_QUEUE_HPP
