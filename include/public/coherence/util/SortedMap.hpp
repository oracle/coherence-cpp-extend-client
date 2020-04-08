/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SORTED_MAP_HPP
#define COH_SORTED_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An interface for a map which guarantees its key's iteration order.
*
* @see Map
* @see TreeMap
*
* @author tb  2009.02.13
*/
class COH_EXPORT SortedMap
    : public interface_spec<SortedMap,
        implements<Map> >
    {

    // ----- Sorted Map interface -------------------------------------------

    public:
        /**
        * Returns the comparator used in sorting this map, or NULL if it is
        * the keys' natural ordering.
        *
        * @return the sorting comparator
        */
        virtual Comparator::View comparator() const = 0;

        /**
        * Returns the first (lowest sorted) key in the map.
        *
        * @return the first key
        * @throws NoSuchElementException if this map is empty.
        */
        virtual Object::View firstKey() const = 0;

        /**
        * Returns the last (highest sorted) key in the map.
        *
        * @return the last key
        *
        * @throws NoSuchElementException if this map is empty.
        */
        virtual Object::View lastKey() const = 0;

        /**
        * Returns a handle of the portion of the map strictly less than vToKey.
        * The handle is backed by this map, so changes in one show up in the
        * other. The sub-map supports all optional operations of the original.
        *
        * @param vToKey the exclusive upper range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vToKey is not comparable to the map
        *         contents
        * @throws IllegalArgumentException if this is a sub-map, and vToKey is
        *         out of range
        * @throws NullPointerException if vToKey is NULL but the map does not
        *         allow NULL keys
        */
        virtual SortedMap::Handle headMap(Object::View vToKey) = 0;

        /**
        * Returns a view of the portion of the map strictly less than vToKey.
        *
        * @param vToKey the exclusive upper range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vToKey is not comparable to the map
        *         contents
        * @throws IllegalArgumentException if this is a sub-map, and vToKey is
        *         out of range
        * @throws NullPointerException if vToKey is NULL but the map does not
        *         allow NULL keys
        */
        virtual SortedMap::View headMap(Object::View vToKey) const = 0;

        /**
        * Returns a handle of the portion of the map greater than or equal to
        * vFromKey, and strictly less than vToKey. The handle is backed by this
        * map, so changes in one show up in the other. The sub-map supports all
        * optional operations of the original.
        *
        * @param vFromKey the inclusive lower range of the sub-map
        * @param vToKey   the exclusive upper range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vFromKey or vToKey is not comparable to
        *         the map contents
        * @throws IllegalArgumentException if this is a sub-map, and vFromKey or
        *         vToKey is out of range
        * @throws NullPointerException if vFromKey or vToKey is NULL but the map
        *         does not allow NULL keys
        */
        virtual SortedMap::Handle subMap(Object::View vFromKey,
                Object::View vToKey) = 0;

        /**
        * Returns a view of the portion of the map greater than or equal to
        * vFromKey, and strictly less than vToKey.
        *
        * @param vFromKey the inclusive lower range of the sub-map
        * @param vToKey   the exclusive upper range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vFromKey or vToKey is not comparable to
        *         the map contents
        * @throws IllegalArgumentException if this is a sub-map, and vFromKey or
        *         vToKey is out of range
        * @throws NullPointerException if vFromKey or vToKey is NULL but the map
        *         does not allow NULL keys
        */
        virtual SortedMap::View subMap(Object::View vFromKey,
                Object::View vToKey) const = 0;

        /**
        * Returns a handle of the portion of the map greater than or equal to
        * vFromKey. The handle is backed by this map, so changes in one show up
        * in the other. The sub-map supports all optional operations of the
        * original.
        *
        * @param vFromKey the inclusive lower range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vFromKey is not comparable to the map
        *         contents
        * @throws IllegalArgumentException if this is a sub-map, and vFromKey is
        *         out of range
        * @throws NullPointerException if vFromKey is NULL but the map does not
        *         allow NULL keys
        */
        virtual SortedMap::Handle tailMap(Object::View vFromKey) = 0;

        /**
        * Returns a view of the portion of the map greater than or equal to
        * vFromKey.
        *
        * @param vFromKey the inclusive lower range of the sub-map
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vFromKey is not comparable to the map
        *         contents
        * @throws IllegalArgumentException if this is a sub-map, and vFromKey is
        *         out of range
        * @throws NullPointerException if vFromKey is NULL but the map does not
        *         allow NULL keys
        */
        virtual SortedMap::View tailMap(Object::View vFromKey) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SORTED_MAP_HPP
