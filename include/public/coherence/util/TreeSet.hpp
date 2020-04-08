/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TREE_SET_HPP
#define COH_TREE_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/MappedSet.hpp"
#include "coherence/util/SortedSet.hpp"
#include "coherence/util/TreeMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* A TreeSet implementation based on a TreeMap. The elements are ordered using
* their natural ordering, or by a Comparator provided at set creation time,
* depending on which constructor is used.
*
* @author hr  2011.09.07
*
* @see SortedSet
* @see TreeMap
*
* @since Coherence 12.1.2
*/
class COH_EXPORT TreeSet
    : public cloneable_spec<TreeSet,
          extends<MappedSet>,
          implements<SortedSet> >
    {
    friend class factory<TreeSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Return a new TreeSet that will sort it's elements using their
        * natural ordering.
        */
        TreeSet();

        /**
        * Return a new TreeSet that will sort it's elements using the
        * supplied Comparator.
        *
        * @param vComparator  the Comparator used to sort elements
        */
        TreeSet(Comparator::View vComparator);

        /**
        * Return a TreeSet backed by the given SortedMap.
        *
        * @param hMap  the delegate SortedMap
        */
        TreeSet(SortedMap::Handle hMap);

        /**
        * Return a TreeSet backed by the given SortedMap.
        *
        * @param vMap  the delegate SortedMap
        */
        TreeSet(SortedMap::View vMap);

        /**
        * Copy constructor.
        */
        TreeSet(const TreeSet& that);

    // ----- SortedSet interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Comparator::View comparator() const;

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::Handle subSet(Object::View vFromElement,
                        Object::View vToElement);

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::View subSet(Object::View vFromElement,
                        Object::View vToElement) const;

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::Handle headSet(Object::View vToElement);

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::View headSet(Object::View vToElement) const;

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::Handle tailSet(Object::View vFromElement);

        /**
        * {@inheritDoc}
        */
        virtual SortedSet::View tailSet(Object::View vFromElement) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View first() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::View last() const;

    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Throws an exception iff the internal map is inaccessible.
        *
        * @throws IllegalStateException iff the internal map is inaccessible
        */
        virtual void ensureMap();

        /**
        * Return the delegate SortedMap.
        *
        * @return the delegate SortedMap
        */
        virtual SortedMap::Handle getSortedMap();

        /**
        * Return the delegate SortedMap.
        *
        * @return the delegate SortedMap
        */
        virtual SortedMap::View getSortedMap() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_TREE_SET_HPP
