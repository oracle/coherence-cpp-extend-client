/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LONG_ARRAY_HPP
#define COH_LONG_ARRAY_HPP

#include "coherence/lang.ns"

#include "coherence/util/LongArrayIterator.hpp"

#include "coherence/io/pof/PofIntrinsic.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An interface, similar in its methods to List, and similar in its purpose
* to an array, designed for sparse storage and indexed by long values.
*
* Unlike the List interface, the LongArray interface assumes that every
* valid index (i.e. greater than or equal to zero) can be accessed and has
* storage available.
*
* @author js 2008.04.03
*/
class COH_EXPORT LongArray
    : public interface_spec<LongArray,
          implements<coherence::io::pof::PofIntrinsic> >
    {
    // ----- LongArray interface --------------------------------------------

    public:
        /**
        * Return the value stored at the specified index.
        *
        * @param lIndex  a long index value
        *
        * @return the object stored at the specified index, or NULL
        */
        virtual Object::Holder get(int64_t lIndex) const = 0;

        /**
        * Add the passed item to the LongArray at the specified index.
        *
        * If the index is already used, the passed value will replace the
        * current value stored with the key, and the replaced value will be
        * returned.
        *
        * It is expected that LongArray implementations will "grow" as
        * necessary to support the specified index.
        *
        * @param lIndex   a long index value
        * @param ohValue  the object to store at the specified index
        *
        * @return the object that was stored at the specified index, or NULL
        */
        virtual Object::Holder set(int64_t lIndex, Object::Holder ohValue) = 0;

        /**
        * Add the passed element value to the LongArray and return the index
        * at which the element value was stored.
        *
        * @param ohValue  the object to add to the LongArray
        *
        * @return  the long index value at which the element value was stored
        */
        virtual int64_t add(Object::Holder ohValue) = 0;

        /**
        * Determine if the specified index is in use.
        *
        * @param lIndex  a long index value
        *
        * @return true if a value (including NULL) is stored at the specified
        *         index, otherwise false
        */
        virtual bool exists(int64_t lIndex) const = 0;

        /**
        * Remove the specified index from the LongArray, returning its
        * associated value.
        *
        * @param lIndex  the index into the LongArray
        *
        * @return the associated value (which can be NULL) or NULL if the
        *         specified index is not in the LongArray
        */
        virtual Object::Holder remove(int64_t lIndex) = 0;

        /**
        * Determine if the LongArray contains the specified element.
        *
        * More formally, returns <tt>true</tt> if and only if this LongArray
        * contains at least one element <tt>e</tt> such that
        * <tt>(o==NULL&nbsp;?&nbsp;e==NULL&nbsp;:&nbsp;o.equals(e))</tt>.
        *
        * @param vElement  element whose presence in this list is to be
        *                  tested
        *
        * @return <tt>true</tt> if this list contains the specified element
        */
        virtual bool contains(Object::View vElement) const = 0;

        /**
        * Remove all nodes from the LongArray.
        */
        virtual void clear() = 0;

        /**
        * Test for empty LongArray.
        *
        * @return true if LongArray has no nodes
        */
        virtual bool isEmpty() const = 0;

        /**
        * Determine the size of the LongArray.
        *
        * @return the number of nodes in the LongArray
        */
        virtual size32_t getSize() const = 0;

        /**
        * Obtain a LongArray.Iterator of the contents of the LongArray.
        *
        * @return an instance of LongArray.Iterator
        */
        virtual LongArrayIterator::Handle iterator() = 0;

        /**
        * Obtain a "read-only" LongArray.Iterator of the contents of the
        * LongArray.  Any attempt to modify the backing LongArray through
        * this iterator will result in an exception.
        *
        * @return an instance of LongArray.Iterator
        */
        virtual LongArrayIterator::Handle iterator() const = 0;

        /**
        * Obtain a LongArray.Iterator of the contents of the LongArray,
        * starting at a particular index such that the first call to
        * <tt>next</tt> will set the location of the iterator at the first
        * existent index that is greater than or equal to the specified
        * index, or will throw a NoSuchElementException if there is no such
        * existent index.
        *
        * @param lIndex  the LongArray index to iterate from
        *
        * @return an instance of LongArray.Iterator
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex) = 0;

        /**
        * Obtain a "read-only" LongArray.Iterator of the contents of the
        * LongArray, starting at a particular index such that the first call
        * to <tt>next</tt> will set the location of the iterator at the first
        * existent index that is greater than or equal to the specified
        * index, or will throw a NoSuchElementException if there is no such
        * existent index. Any attempt to modify the backing LongArray through
        * this iterator will result in an exception.
        *
        * @param lIndex  the LongArray index to iterate from
        *
        * @return an instance of LongArray.Iterator
        */
        virtual LongArrayIterator::Handle iterator(int64_t lIndex) const = 0;

        /**
        * Determine the first index that exists in the LongArray.
        *
        * @return the lowest long value, 0 <= n <= Long.max_value, that
        *         exists in this LongArray, or -1 if the LongArray is empty
        */
        virtual int64_t getFirstIndex() const = 0;

        /**
        * Determine the last index that exists in the LongArray.
        *
        * @return the highest long value, 0 <= n <= Long.max_value, that
        *         exists in this LongArray, or -1 if the LongArray is empty
        */
        virtual int64_t getLastIndex() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LONG_ARRAY_HPP
