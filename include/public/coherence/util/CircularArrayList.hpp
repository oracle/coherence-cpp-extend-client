/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CIRCULAR_ARRAY_LIST_HPP
#define COH_CIRCULAR_ARRAY_LIST_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/ListIterator.hpp"
#include "coherence/util/AbstractList.hpp"
#include "coherence/util/SubList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Resizable-array implementation of the <tt>List</tt> interface. Implements
* all optional list operations, and permits all elements, including
* <tt>NULL</tt>. This class is optimized operations on the front and back of
* the list to facilitate use as a queue or deque.
*
* The <tt>size</tt>, <tt>get</tt>, <tt>set</tt>, <tt>listIterator</tt>
* operations run in constant time.  The <tt>add</tt> operation runs in
* <i>amortized constant time</i>, that is, adding n elements requires O(n)
* time.  All of the other operations run in linear time (roughly speaking).
* The constant factor is low compared to that for the <tt>LinkedList</tt>
* implementation.
*
* Each <tt>CircularArrayList</tt> instance has a <i>capacity</i>.
* The capacity is the size of the array used to store the elements in the
* list.  It is always at least as large as the list size.  As elements are
* added to an CircularArrayList, its capacity grows automatically.  The
* details of the growth policy are not specified beyond the fact that adding
* an element has constant amortized time cost.
*
* An application can increase the capacity of an <tt>CircularArrayList</tt>
* instance before adding a large number of elements using the
* <tt>ensureCapacity</tt> operation.  This may reduce the amount of
* incremental reallocation.
*
* <strong>Note that this implementation is not synchronized.</strong> If
* multiple threads access a <tt>CircularArrayList</tt> concurrently, and at
* least one of the threads modifies the list structurally, it <i>must</i> be
* synchronized externally.  (A structural modification is any operation that
* adds or deletes one or more elements, or explicitly resizes the backing
* array; merely setting the value of an element is not a structural
* modification.)  This is typically accomplished by synchronizing on some
* object that naturally encapsulates the list.
*
* The iterators returned by this class's <tt>listIterator</tt> methods are
* fail-fast: if list is structurally modified at any time after the iterator
* is created, in any way except through the iterator's own remove or add
* methods, the iterator will throw a ConcurrentModificationException.  Thus,
* in the face of concurrent modification, the iterator fails quickly and
* cleanly, rather than risking arbitrary, non-deterministic behavior at an
* undetermined time in the future.
*
* Note that the fail-fast behavior of an iterator cannot be guaranteed as
* it is, generally speaking, impossible to make any hard guarantees in the
* presence of unsynchronized concurrent modification.  Fail-fast iterators
* throw <tt>ConcurrentModificationException</tt> on a best-effort basis.
* Therefore, it would be wrong to write a program that depended on this
* exception for its correctness: <i>the fail-fast behavior of iterators
* should be used only to detect bugs.</i>
*
*
* @author djl  2009.01.14
*
* @since Coherence 3.5
*/
class COH_EXPORT CircularArrayList
    : public cloneable_spec<CircularArrayList,
        extends<AbstractList> >
    {
    friend class factory<CircularArrayList>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new CircularArrayList with the specified initial capacity.
        *
        * @param cInitialElements  the initial capacity of the list
        *
        * @throws IllegalArgumentException if the specified initial capacity
        *            is negative
        */
        CircularArrayList(size32_t cInitialElements = 16);

        /**
        * Create a new CircularArrayList that has a reference to every
        * element in the supplied collection.
        *
        * @param vc  The collection to base the CircularArrayList on
        *
        * @return a new CircularArrayList
        */
        CircularArrayList(Collection::View vc);

        /**
        * @internal
        */
        CircularArrayList(const CircularArrayList& that);


    // ----- CircularArrayList interface ------------------------------------

    public:
        /**
        * Trim the capacity of this list instance to be as small as
        * possible.
        */
        virtual void trimToSize();

        /**
        * Increase the capacity of this list instance, if necessary, to
        * ensure that it can hold at least the specified number of elements.
        *
        * @param cMin  the minimum allowable capacity
        *
        * @return true if the capacity was increased
        */
        bool ensureCapacity(size32_t cMin);


    // ----- List interface -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool add(size32_t i, Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(size32_t i, Collection::View vc);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(size32_t i) const;

        /**
        * {@inheritDoc}
        */
        using List::get;

        /**
        * {@inheritDoc}
        */
        virtual size32_t indexOf(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t lastIndexOf(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual ListIterator::Handle listIterator(size32_t index = 0) const;

        /**
        * {@inheritDoc}
        */
        virtual ListMuterator::Handle listIterator(size32_t index = 0);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(size32_t index);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder set(size32_t index, Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual List::View subList(size32_t fromIndex, size32_t toIndex) const;

        /**
        * {@inheritDoc}
        */
        virtual List::Handle subList(size32_t fromIndex, size32_t toIndex);


    // ----- Collection interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const;

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator();

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(
                ObjectArray::Handle hao = NULL) const;

        /**
        * {@inheritDoc}
        */
        virtual bool add(Object::Holder oh);

        /**
        * {@inheritDoc}
        */
        virtual bool addAll(Collection::View vc);

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v);

        /**
        * {@inheritDoc}
        */
        virtual bool removeAll(Collection::View vColl);

        /**
        * {@inheritDoc}
        */
        virtual bool retainAll(Collection::View vCol);

        /**
        * {@inheritDoc}
        */
        virtual void clear();


    // ----- nested class: SubCircularArrayList -----------------------------

    protected:
        /**
        * Utility class to implement a SubList of a CircularArrayList.
        * SubCircularArrayList delegates through the the CircularArrayList
        * for all modification operations
        */
        class SubCircularArrayList
            : public class_spec<SubCircularArrayList,
                extends<SubList> >
            {
            friend class factory<SubCircularArrayList>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * create a new SubCircularArrayList.
                *
                * @param fromIndex  the starting point of the sublist in the
                *                   list provided (inclusive).
                * @param toIndex    the end point of the sublist in the list
                *                   provided (exclusive)
                * @param ohList     the list to create a sublist of
                *
                * @return a new SubCircularArrayList
                */
                SubCircularArrayList(size32_t fromIndex, size32_t toIndex,
                        List::Holder ohList);


            // ----- List interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool retainAll(Collection::View vColl);

                /**
                * {@inheritDoc}
                */
                virtual void clear();

                /**
                * {@inheritDoc}
                */
                virtual List::View
                        subList(size32_t fromIndex, size32_t toIndex) const;

                /**
                * {@inheritDoc}
                */
                virtual List::Handle
                        subList(size32_t fromIndex, size32_t toIndex);
            };


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Calculate the effective index taking into account offsets and the
        * circular nature of CircularArrayList.
        *
        * @param index  the index to transform
        *
        * @return the effective index in the physical storage array
        */
        virtual size32_t effectiveIndex(size32_t index) const;

        /**
        * Check if the given index is in range.  If not, throw an appropriate
        * runtime exception.
        *
        * @param index  the index to be checked for being between
        *               size() and 0 inclusive
        *
        * @throws IndexOutOfBoundsException
        */
        virtual void rangeCheck(size32_t index) const;

        /**
        * After range checking Calculate the effective index while taking into
        * account the offsets and the circular nature of the list.
        *
        * @param index  the index to transform
        *
        * @return the effective index in the physical storage array
        * @throws IndexOutOfBoundsException
        */
        virtual size32_t ensureEffectiveIndex(size32_t index) const;

        /**
        * Ensure the representation of this list is appropriatly compact
        * by shrinking if necessary.
        *
        * @return true if an actual compaction happened; false otherwise
        */
        virtual bool ensureCompactness();

        /**
        * Removes from this list all of the elements whose indexes are
        * between fromIndex, inclusive and toIndex, exclusive.  Shifts any
        * succeeding elements to the left (reduces their index).
        * This call shortens the list by (toIndex - fromIndex) elements.
        * (If toIndex==fromIndex, this operation has no effect.)
        *
        * @param fromIndex  the index of first element to be removed
        * @param toIndex    the index after last element to be removed.
        */
        virtual void removeRange(size32_t fromIndex, size32_t toIndex);



    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The array into which the elements of the list are stored.
        * The capacity of the list is the length of this array buffer.
        */
        MemberHandle<ObjectArray> m_haoData;

        /**
        * The offset to the first element
        */
        size32_t m_iFirst;

        /**
        * The offset to one past the last element
        */
        size32_t m_iLast;

        /**
        * The size of the list (the number of elements it contains)
        */
        size32_t m_cElements;

        /**
        * The current mod count which is used to detect concurrent
        * modification
        */
        size32_t m_cMod;


    // ----- friends --------------------------------------------------------

    friend class SubCircularArrayList;
    friend class CircularArrayListIterator;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CIRCULAR_ARRAY_LIST_HPP
