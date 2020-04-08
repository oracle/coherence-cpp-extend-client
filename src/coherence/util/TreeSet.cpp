/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/TreeSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructors -------------------------------------------------------

TreeSet::TreeSet()
    : super(cast<Map::Handle>(TreeMap::create()))
    {
    }

TreeSet::TreeSet(Comparator::View vComparator)
    : super(cast<Map::Handle>(TreeMap::create(vComparator)))
    {
    }

TreeSet::TreeSet(SortedMap::Handle hMap)
    : super(cast<Map::Handle>(hMap))
    {
    }

TreeSet::TreeSet(SortedMap::View vMap)
    : super(cast<Map::View>(vMap))
    {
    }

TreeSet::TreeSet(const TreeSet& that)
    : super(that)
    {
    }

// ----- SortedSet interface ------------------------------------------------

/**
* {@inheritDoc}
*/
Comparator::View TreeSet::comparator() const
    {
    return getSortedMap()->comparator();
    }

/**
* {@inheritDoc}
*/
SortedSet::Handle TreeSet::subSet(Object::View vFromElement,
                Object::View vToElement)
    {
    ensureMap();
    return TreeSet::create(getSortedMap()->subMap(vFromElement, vToElement));
    }

/**
* {@inheritDoc}
*/
SortedSet::View TreeSet::subSet(Object::View vFromElement,
                Object::View vToElement) const
    {
    return TreeSet::create(getSortedMap()->subMap(vFromElement, vToElement));
    }

/**
* {@inheritDoc}
*/
SortedSet::Handle TreeSet::headSet(Object::View vToElement)
    {
    ensureMap();
    return TreeSet::create(getSortedMap()->headMap(vToElement));
    }

/**
* {@inheritDoc}
*/
SortedSet::View TreeSet::headSet(Object::View vToElement) const
    {
    return TreeSet::create(getSortedMap()->headMap(vToElement));
    }

/**
* {@inheritDoc}
*/
SortedSet::Handle TreeSet::tailSet(Object::View vFromElement)
    {
    ensureMap();
    return TreeSet::create(getSortedMap()->tailMap(vFromElement));
    }

/**
* {@inheritDoc}
*/
SortedSet::View TreeSet::tailSet(Object::View vFromElement) const
    {
    return TreeSet::create(getSortedMap()->tailMap(vFromElement));
    }

/**
* {@inheritDoc}
*/
Object::View TreeSet::first() const
    {
    return getSortedMap()->firstKey();
    }

/**
* {@inheritDoc}
*/
Object::View TreeSet::last() const
    {
    return getSortedMap()->lastKey();
    }

// ----- helpers ------------------------------------------------------------

void TreeSet::ensureMap()
    {
    if (NULL == getMapDelegate())
        {
        COH_THROW_STREAM(IllegalStateException,
                   "The internal structure of TreeSet is invalid, "
                << "which could be due to non-const access subsequent to a "
                << "const function call");
        }
    }

SortedMap::Handle TreeSet::getSortedMap()
    {
    return cast<SortedMap::Handle>(f_thMapDelegate);
    }

SortedMap::View TreeSet::getSortedMap() const
    {
    return cast<SortedMap::View>(f_thMapDelegate);
    }

COH_CLOSE_NAMESPACE2
