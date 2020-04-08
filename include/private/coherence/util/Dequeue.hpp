/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DEQUEUE_HPP
#define COH_DEQUEUE_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Represents a double-ended queue (dequeue) of objects. A dequeue allows
* items which have been removed from the front of the queue to be put back
* on the front of the queue.
*
* @author tb  2007.12.13
*/
class COH_EXPORT Dequeue
    : public class_spec<Dequeue,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<Dequeue>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Dequeue instance
        */
        Dequeue();

        /**
        * Create a new Dequeue instance from a given Dequeue
        *
        * @param vDeque  the Dequeue to clone from
        */
        Dequeue(Dequeue::View vDeque);


    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext()const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next();

    // ----- Dequeue interface ----------------------------------------------

    public:
        /**
        * Adds an element to the dequeue.  The passed object is placed at the
        * end of the dequeue.
        *
        * @param ohObject object to add to the Dequeue
        */
        virtual void addElement(Object::Holder ohObject);

        /**
        * Returns the most recently returned element to the Dequeue.  After
        * calling this method, a call to nextElement will return the element
        * that was returned by the most recent call to nextElement.  This
        * method can be called multiple times to put back multiple elements.
        *
        * @param ohObject the object to put back to the Dequeue
        *
        * @exception NoSuchElementException if the element that is being put
        *            back is no longer in the Dequeue
        */
        virtual void putBackElement(Object::Holder ohObject);

        /**
        * Removes all items from the dequeue.
        */
        virtual void removeAllElements();

        /**
        * Determine the current number of objects in this dequeue.
        *
        * @return the current number of objects in this dequeue
        */
        virtual size32_t size() const;

        /**
        * Determines if the dequeue is empty.
        *
        * @return true if the dequeue is empty, false otherwise
        */
        virtual bool isEmpty() const;

        /**
        * Determines if the dequeue is full.  This method is mainly of use
        * internally, since the dequeue auto-resizes when additional items
        * are added.
        *
        * @return true if the dequeue is full, false otherwise
        */
        virtual bool isFull() const;

        /**
        * Determine the current capacity of this dequeue.
        *
        * @return the current capacity of this dequeue
        */
        virtual size32_t capacity() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Determines the index following the passed index for storing or
        * accessing an item from the m_ao storage.
        *
        * @param i  the starting index
        *
        * @return the next index
        */
        virtual size32_t next(size32_t i) const;

        /**
        * Determines the index preceding the passed index for storing or
        * accessing an item from the m_ao storage.
        *
        * @param i  the starting index
        *
        * @return the previous index
        */
        virtual size32_t prev(size32_t i) const;

        /**
        * Increase the capacity of the Dequeue.
        */
        virtual void grow();

        /**
        * Copy the contents of the dequeue into a target object array.
        *
        * @param cSrcStart  the starting position in the dequeue
        * @param haTgt      the target array
        * @param cTgtStart  the starting position in the target array
        * @param cLen       the number of elements to copy
        */
        virtual void arrayCopy(size32_t cSrcStart,
                ObjectArray::Handle haTgt, size32_t cTgtStart, size32_t cLen);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The default dequeue size.
        */
        static const size32_t default_max_elements = 32;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The storage for the objects in the dequeue.
        */
        MemberHandle<ObjectArray> m_aoItem;

        /**
        * The front of the queue, which is the next element to return.
        */
        size32_t m_iFront;

        /**
        * The back of the queue, which is the last element that can be
        * returned.
        */
        size32_t m_iBack;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DEQUEUE_HPP
