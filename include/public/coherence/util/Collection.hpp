/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COLLECTION_HPP
#define COH_COLLECTION_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/Muterator.hpp"

#include "coherence/io/pof/PofIntrinsic.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The base interface of all <i>collections</i> managed by Coherence. A
* collection is a group of objects also known as elements. The following
* behaviors of a collection are implementation dependent:
* <ul>
*   <li>ordered iteration</li>
*   <li>support for duplicate elements</li>
*   <li>element equivalency by state equality (Object::equals())</li>
*   <li>accessibility of elements only as views</li>
*   <li>retention of elements by copy</li>
*   <li>thread-safe</li>
* </ul>
*
* Unless otherwise stated, collections are assumed to not support any of the
* above.
*
* @see Iterator
* @see List
* @see Map
* @see Set
*
* @author jh/mf/nsa  2008.01.28
*/
class COH_EXPORT Collection
    : public interface_spec<Collection,
          implements<coherence::io::pof::PofIntrinsic> >
    {
    // ----- Collection interface -------------------------------------------

    public:
        /**
        * Return the number of elements in this collection.
        *
        * @return the number of elements in this collection
        */
        virtual size32_t size() const = 0;

        /**
        * Determine whether this collection contains any elements.
        *
        * @return true if this collection has no elements
        */
        virtual bool isEmpty() const = 0;

        /**
        * Determine if this collection contains the specified element.
        *
        * @param v  the element to test for containment
        *
        * @return true iff this collection contains the given element
        */
        virtual bool contains(Object::View v) const = 0;

        /**
        * Determine if this collection contains all elements from the
        * supplied collection.
        *
        * @param vCol  the collection of elements to test for containment
        *
        * @return true iff this collection contains all elements from the
        *         supplied collection
        *
        * @throws coherence::lang::NullPointerException if the specified
        *         collection is NULL
        *
        * @see contains()
        */
        virtual bool containsAll(Collection::View vCol) const = 0;

        /**
        * Return an Iterator over this collection.
        *
        * @return an Iterator over this collection
        */
        virtual Iterator::Handle iterator() const = 0;

        /**
        * Return an Iterator over this collection.
        *
        * @return an Iterator over this collection
        */
        virtual Muterator::Handle iterator() = 0;

        /**
        * Return the contents of this collection as an ObjectArray.
        * If the collection fits in the specified array, it is returned,
        * otherwise, a new array is allocated that is the size of this
        * collection.
        *
        * If this collection fits in the array with additional room
        * then the element in the array immediately following the end of the
        * collection is set to NULL.  This can be useful in determining the
        * length of this collection if the caller knows that the collection
        * does not contain any NULL elements.
        *
        * @param hao  an array in which to store the collection's contents
        *
        * @return a ObjectArray containing all the elements of the collection
        *         in the same order as returned by the collection's Iterator
        *
        * @see Iterator
        */
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hao = NULL) const = 0;

        /**
        * Add the given element to this collection.
        *
        * @param oh  the element to add
        *
        * @return true iff this collection was modified as a result of this
        *         operation
        */
        virtual bool add(Object::Holder oh) = 0;

        /**
        * Add all elements from the supplied collection to this collection.
        *
        * @param vCol  the collection of elements to add
        *
        * @return true iff this collection was modified as a result of this
        *         operation
        *
        * @throws coherence::lang::NullPointerException if the specified
        *         collection is NULL
        *
        * @see add()
        */
        virtual bool addAll(Collection::View vCol) = 0;

        /**
        * Remove the supplied element from this collection.
        *
        * @param v     the element to remove
        *
        * @return true iff this collection was modified as a result of this
        *         operation
        */
        virtual bool remove(Object::View v) = 0;

        /**
        * Remove all instances of the elements in the supplied collection
        * from this collection. Upon completion, contains() on this
        * collection will return false for all elements in the supplied
        * collection.
        *
        * @param vCol  the collection of elements to remove
        *
        * @return true iff this collection was modified as a result of this
        *         operation
        *
        * @throws coherence::lang::NullPointerException if the specified
        *         collection is NULL
        *
        * @see remove()
        * @see contains()
        */
        virtual bool removeAll(Collection::View vCol) = 0;

        /**
        * Remove all elements from this collection that are not present in
        * the supplied collection.
        *
        * @param vCol  the collection of elements to retain
        *
        * @return true iff this collection was modified as a result of this
        *         operation.
        *
        * @throws coherence::lang::NullPointerException if the specified
        *         collection is NULL
        *
        * @see remove()
        * @see contains()
        */
        virtual bool retainAll(Collection::View vCol) = 0;

        /**
        * Remove all elements from this collection.
        */
        virtual void clear() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_COLLECTION_HPP
