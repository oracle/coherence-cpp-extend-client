/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SORTED_SET_HPP
#define COH_SORTED_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* A Set that further provides a <i>total ordering</i> on its elements.
* The elements are ordered using their Comparable natural
* ordering, or by a Comparator typically provided at sorted
* set creation time.  The set's iterator will traverse the set in
* ascending element order. Several additional operations are provided
* to take advantage of the ordering.  (This interface is the set
* analogue of SortedMap.)
*
* @see Set
* @see TreeSet
* @see TreeMap
*
* @author hr  2011.09.07
*
* @since Coherence 12.1.2
*/
class COH_EXPORT SortedSet
    : public interface_spec<SortedSet,
        implements<Set> >
    {

    // ----- SortedSet interface --------------------------------------------

    public:
        /**
        * Returns the comparator used in sorting this set, or NULL if it is
        * the keys' natural ordering.
        *
        * @return the sorting comparator
        */
        virtual Comparator::View comparator() const = 0;

        /**
        * Returns a handle of the portion of the set greater than or equal to
        * vFromElement, and strictly less than vToElement. The handle is
        * backed by this set, so changes in one show up in the other.
        *
        * @param vFromElement  the inclusive lower range of the sub-set
        * @param vToElement    the exclusive upper range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vFromElement or vToElement is not
        *         comparable to the set's contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vFromElement or or vToElement is out of range
        * @throws NullPointerException if vFromElement or vToElement is NULL
        *         but the map does not allow NULL elements
        */
        virtual SortedSet::Handle subSet(Object::View vFromElement,
                Object::View vToElement) = 0;

        /**
        * Returns a view of the portion of the set greater than or equal to
        * vFromElement, and strictly less than vToElement.
        *
        * @param vFromElement  the inclusive lower range of the sub-set
        * @param vToElement    the exclusive upper range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vFromElement or vToElement is not
        *         comparable to the set's contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vFromElement or vToElement is out of range
        * @throws NullPointerException if vFromElement or vToElement is NULL
        *         but the set does not allow NULL elements
        */
        virtual SortedSet::View subSet(Object::View vFromElement,
                Object::View vToElement) const = 0;

        /**
        * Returns a handle of the portion of the set strictly less than
        * vToElement. The handle is backed by this set, so changes in one show
        * up in the other. The sub-set supports all optional operations of
        * the original.
        *
        * @param vToElement  the exclusive upper range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vToElement is not comparable to the
        *         set contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vToElement is out of range
        * @throws NullPointerException if vToElement is NULL but the set does
        *         not allow NULL elements
        */
        virtual SortedSet::Handle headSet(Object::View vToElement) = 0;

        /**
        * Returns a view of the portion of the set strictly less than
        * vToElement.
        *
        * @param vToElement  the exclusive upper range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vToElement is not comparable to the
        *         set contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vToElement is out of range
        * @throws NullPointerException if vToElement is NULL but the set does
        *         not allow NULL elements
        */
        virtual SortedSet::View headSet(Object::View vToElement) const = 0;

        /**
        * Returns a handle of the portion of the set greater than or equal to
        * vFromElement. The handle is backed by this set, so changes in one
        * show up in the other. The sub-set supports all optional operations
        * of the original.
        *
        * @param vFromElement  the inclusive lower range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vFromElement is not comparable to the
        *         set contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vFromElement is out of range
        * @throws NullPointerException if vFromElement is NULL but the set
        *         does not allow NULL elements
        */
        virtual SortedSet::Handle tailSet(Object::View vFromElement) = 0;

        /**
        * Returns a view of the portion of the set greater than or equal to
        * vFromElement.
        *
        * @param vFromElement  the inclusive lower range of the sub-set
        *
        * @return the sub-set
        *
        * @throws ClassCastException if vFromElement is not comparable to the
        *         set contents
        * @throws IllegalArgumentException if this is a sub-set, and
        *         vFromElement is out of range
        * @throws NullPointerException if vFromElement is NULL but the set
        *         does not allow NULL elements
        */
        virtual SortedSet::View tailSet(Object::View vFromElement) const = 0;

        /**
        * Returns the first (lowest sorted) element in the set.
        *
        * @return the first element
        *
        * @throws NoSuchElementException if this set is empty.
        */
        virtual Object::View first() const = 0;

        /**
        * Returns the last (highest sorted) element in the set.
        *
        * @return the last element
        *
        * @throws NoSuchElementException if this set is empty.
        */
        virtual Object::View last() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SORTED_SET_HPP
