/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DUAL_QUEUE_HPP
#define COH_DUAL_QUEUE_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractConcurrentQueue.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The DualQueue is optimized for the producer consumer use case.
*
* Producers work on the tail of the queue, consumers operate on the head of
* the queue.  The two portions of the queue are maintained as separate lists,
* and protected by separate locks.
*
* When a consumer looks at the head of the queue, if it is empty, the head
* and tail will be swapped.
*
* @author nsa 2008.02.13
*/
class COH_EXPORT DualQueue
    : public class_spec<DualQueue,
        extends<AbstractConcurrentQueue> >
    {
    friend class factory<DualQueue>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new DualQueue.
        *
        * @return a new DualQueue
        */
        DualQueue();


    // ----- DualQueue interface --------------------------------------------

    protected:
        /**
        * Swap the head and the tail, but only if the head is empty and the
        * tail is not. The calling thread must already hold a the head lock.
        *
        * @return true iff the head and tail were swapped
        */
        virtual bool swapNoWait();

        /**
        * Return the head element list.
        *
        * @return the head element list
        */
        virtual List::Handle getHeadElementList();

        /**
        * Set the head element list.
        *
        * @param hList  the new head list to set
        */
        virtual void setHeadElementList(List::Handle hList);

        /**
        * Return the element list (tail).
        *
        * @return the element list
        */
        virtual List::Handle getElementList();

        /**
        * Set the element list (tail)
        *
        * @param hList  the new list to set
        */
        virtual void setElementList(List::Handle hList);

        /**
        * Return the head lock.
        *
        * @return the head lock
        */
        virtual Object::Handle getHeadLock();


    // ----- Queue interface ------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool addHead(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder peekNoWait();

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder removeNoWait();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Lock protecting operations on the head of the queue, and tail
        * swapping. We cannot simply lock on the head element list as it
        * gets swapped with the tail.
        *
        * To avoid deadlock issues the Queue lock should never be obtained
        * while holding the head lock.
        *
        * For example:
        * <pre>
        * COH_SYNCHRONIZED(getHeadLock())
        *     {
        *     COH_SYNCHRONIZED(this)
        *         {
        *         // this is NOT ok
        *         }
        *     }
        * COH_SYNCHRONIZED(this)
        *     {
        *     COH_SYNCHRONIZED(getHeadLock())
        *         {
        *         // this is ok
        *         }
        *     }
        * </pre>
        *
        * The latter approach was chosen as it allows users of the DualQueue
        * to perform external synchronization without risking deadlock.
        */
        FinalHandle<Object> f_hHeadLock;

        /**
        * The List that backs the queue. For a dual queue the ElementList is
        * the tail.
        */
        MemberHandle<List> m_hElementList;

        /**
        * The storage for the head of the queue.
        */
        MemberHandle<List> m_hHeadElementList;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DUAL_QUEUE_HPP
