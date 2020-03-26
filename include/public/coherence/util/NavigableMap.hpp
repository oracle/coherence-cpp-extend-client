/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAVIGABLE_MAP_HPP
#define COH_NAVIGABLE_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SortedMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
* A SortedMap extended with navigation methods returning the
* closest matches for given search targets. Methods lowerKey,
* floorKey, ceilingKey, and higherKey return only the associated
* keys. All of these methods are designed for locating, not
* traversing entries.
*
* Methods subMap, headMap, tailMap differ from the like-named
* SortedMap methods in accepting additional arguments describing
* whether lower and upper bounds are inclusive versus exclusive.
* Sub-maps of any NavigableMap must implement the NavigableMap
* interface.
*
* This interface additionally defines methods pollFirstEntry
* pollLastEntry that return and remove the least and greatest mappings,
* if any exist, else returning NULL.
*
* Implementations of entry-returning methods are expected to
* return Map.Entry pairs representing snapshots of mappings
* at the time they were produced, and thus generally do <em>not</em>
* support the optional Entry.setValue method. Note however
* that it is possible to change mappings in the associated map using
* method put.
*
* Methods
* #subMap(Object, Object) subMap(K, K),
* #headMap(Object) headMap(K), and
* #tailMap(Object) tailMap(K)
* are specified to return SortedMap to allow existing implementations
* of SortedMap to be compatibly retrofitted to implement NavigableMap,
* but extensions and implementations of this interface are encouraged
* to override these methods to return NavigableMap.
*
* @see Map
* @see TreeMap
*
* @author Wei Lin 2013.08.30
* @since Coherence 12.1.3
*/
class COH_EXPORT NavigableMap
    : public interface_spec<NavigableMap,
        implements<SortedMap> >
    {
    public:
        using SortedMap::subMap;
        using SortedMap::headMap;
        using SortedMap::tailMap;

    // ----- NavigableMap interface -----------------------------------------

    public:
        /**
        * Returns the least key greater than or equal to the given key,
        * or NULL if there is no such key.
        *
        * @param vKey  the key
        *
        * @return the least key greater than or equal to the given key,
        *         or NULL if there is no such key.
        *
        * @throws ClassCastException if the specified key cannot be compared
        *         with the keys currently in the map
        * @throws NullPointerException if the specified key is NULL
        *         and this map does not permit NULL keys
        */
        virtual Object::View ceilingKey(Object::View vKey) const = 0;

        /**
        * Returns the greatest key less than or equal to the given key,
        * or NULL if there is no such key.
        *
        * @param vKey  the key
        *
        * @return the greatest key less than or equal to the given key,
        *         or NULL if there is no such key.
        *
        * @throws ClassCastException if the specified key cannot be compared
        *         with the keys currently in the map
        * @throws NullPointerException if the specified key is NULL
        *         and this map does not permit NULL keys
        */
        virtual Object::View floorKey(Object::View vKey) const = 0;

        /**
        * Returns the least key strictly greater than the given key, or
        * NULL if there is no such key.
        *
        * @param vKey  the key
        * @return the least key greater than the given key,
        *         or NULL if there is no such key
        *
        * @throws ClassCastException if the specified key cannot be compared
        *         with the keys currently in the map
        * @throws NullPointerException if the specified key is NULL
        *         and this map does not permit NULL keys
        */
        virtual Object::View higherKey(Object::View vKey) const = 0;

       /**
       * Returns the greatest key strictly less than the given key, or
       * NULL if there is no such key.
       *
       * @param vKey  the key
       * @return the greatest key less than the given key,
       *         or NULL if there is no such key
       *
       * @throws ClassCastException if the specified key cannot be compared
       *         with the keys currently in the map
       * @throws NullPointerException if the specified key is NULL
       *         and this map does not permit NULL keys
       */
       virtual Object::View lowerKey(Object::View vKey) const = 0;

       /**
        * Removes and returns a key-value mapping associated with
        * the least key in this map, or NULL if the map is empty.
        *
        * @return the removed first entry of this map,
        *         or NULL if this map is empty
        */
        virtual Map::Entry::Holder pollFirstEntry() = 0;

        /**
        * Removes and returns a key-value mapping associated with
        * the greatest key in this map, or NULL if the map is empty.
        *
        * @return the removed last entry of this map,
        *         or NULL if this map is empty
        */
        virtual Map::Entry::Holder pollLastEntry() = 0;

        /**
        * Returns a handle of the portion of the map whose keys are less than (or
        * equal to, if toInclusive is true) vToKey.
        * The handle is backed by this map, so changes in one show up in the
        * other. The sub-map supports all optional operations of the original.
        *
        * @param vToKey       the exclusive upper range of the sub-map
        * @param toInclusive  true if the high endpoint is to be included
        *                     in the returned view
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
        virtual NavigableMap::Handle headMap(Object::View vToKey, bool toInclusive) = 0;

        /**
        * Returns a view of the portion of the map whose keys are less than (or
        * equal to, if toInclusive is true) vToKey.
        *
        * @param vToKey       the exclusive upper range of the sub-map
        * @param toInclusive  true if the high endpoint is to be included
        *                     in the returned view
        *
        * @return the sub-map
        *
        * @throws ClassCastException if vToKey is not comparable to the map
        *         contents
        * @throws IllegalArgumentException if this is a sub-map, and toKey is
        *         out of range
        * @throws NullPointerException if vToKey is NULL but the map does not
        *         allow NULL keys
        */
        virtual NavigableMap::View headMap(Object::View vToKey, bool toInclusive) const = 0;

        /**
        * Returns a handle of the portion of this map whose keys range from
        * vFromKey to vToKey. If vFromKey and vToKey are equal, the returned
        * map is empty unless fromInclusive and toInclusive are both true.
        * The handle is backed by this map, so changes in one show up in the other.
        * The sub-map supports all optional operations of the original.
        *
        * @param vFromKey       the inclusive lower range of the sub-map
        * @param fromInclusive  true if the low endpoint is to be included
        *                       in the returned view
        * @param vToKey         the exclusive upper range of the sub-map
        * @param toInclusive    true if the high endpoint is to be included
        *                       in the returned view
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
        virtual NavigableMap::Handle subMap(Object::View vFromKey, bool fromInclusive,
                Object::View vToKey, bool toInclusive) = 0;

        /**
        * Returns a view of the portion of this map whose keys range from
        * vFromKey to vToKey. If vFromKey and vToKey are equal, the returned
        * map is empty unless fromInclusive and toInclusive are both true.
        *
        * @param vFromKey       the inclusive lower range of the sub-map
        * @param fromInclusive  true if the low endpoint is to be included
        *                       in the returned view
        * @param vToKey         the exclusive upper range of the sub-map
        * @param toInclusive    true if the high endpoint is to be included
        *                       in the returned view
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
        virtual NavigableMap::View subMap(Object::View vFromKey, bool fromInclusive,
                Object::View vToKey, bool toInclusive) const = 0;

        /**
        * Returns a handle of the portion of the map whose keys are greater than (or
        * equal to, if fromInclusive} is true) vFromKey.
        * The handle is backed by this map, so changes in one show up in the
        * other. The sub-map supports all optional operations of the original.
        *
        * @param vFromKey       the inclusive lower range of the sub-map
        * @param fromInclusive  true if the low endpoint is to be included
        *                       in the returned view
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
        virtual NavigableMap::Handle tailMap(Object::View vFromKey, bool fromInclusive) = 0;

        /**
        * Returns a view of the portion of the map whose keys are greater than (or
        * equal to, if fromInclusive} is true) vFromKey.
        *
        * @param vFromKey       the inclusive lower range of the sub-map
        * @param fromInclusive  true if the low endpoint is to be included
        *                       in the returned view
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
        virtual NavigableMap::View tailMap(Object::View vFromKey, bool fromInclusive) const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NAVIGABLE_MAP_HPP

