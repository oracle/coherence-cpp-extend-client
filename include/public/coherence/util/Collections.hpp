/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COLLECTIONS_HPP
#define COH_COLLECTIONS_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/SortedMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* This class consists exclusively of static methods that operate on or return
* collections.
*
* @author tb  2008.04.04
*/
class COH_EXPORT Collections
    {
    // ----- static methods -------------------------------------------------

    public:
       /**
         * Reverses the order of the elements in the specified list.
         *
         * This method runs in linear time.
         *
         * @param hList  the list whose elements are to be reversed.
         *
         * @throws UnsupportedOperationException if the specified list or
         *         its list-iterator does not support the <tt>set</tt> operation.
         *
         * since 12.2.1.3
         */
        static void reverse(List::Handle hList);

        /**
        * Sorts the specified list into ascending order, according to the
        * natural ordering of its elements.  All elements in the list must
        * implement the Comparable interface.  Furthermore, all elements
        * in the list must be mutually comparable.
        *
        * This sort is guaranteed to be stable:  equal elements will not be
        * reordered as a result of the sort.
        *
        * The specified list must be modifiable, but need not be resizable.
        *
        * @param hList  the list to be sorted
        * @throws ClassCastException  if the list contains elements that are
        *                             not mutually comparable
        * @throws UnsupportedOperationException  if the specified list's
        *         list-iterator does not support the set operation
        */
        static void sort(List::Handle hList);

        /**
        * Sorts the specified list according to the order induced by the
        * specified comparator.  All elements in the list must be mutually
        * comparable using the specified comparator
        *
        * This sort is guaranteed to be stable:  equal elements will not be
        * reordered as a result of the sort.
        *
        * The specified list must be modifiable, but need not be resizable.
        *
        * @param  hList        the list to be sorted.
        * @param  hComparator  the comparator to determine the order of the
        *                      list.  A null value indicates that the
        *                      elements' natural ordering should be used.
        * @throws ClassCastException  if the list contains elements that are
        *       not mutually comparable using the specified comparator.
        * @throws UnsupportedOperationException  if the specified list's
        *       list-iterator does not support the set operation.
        */
        static void sort(List::Handle hList, Comparator::Handle hComparator);

        /**
         * Swaps the elements at the specified positions in the specified list.
         * (If the specified positions are equal, invoking this method leaves
         * the list unchanged.)
         *
         * @param hList  the list in which to swap elements.
         * @param i      the index of one element to be swapped.
         * @param j      the index of the other element to be swapped.
         *
         * @throws IndexOutOfBoundsException if either <tt>i</tt> or <tt>j</tt>
         *         is out of range (i &lt; 0 || i &gt;= hList->size()
         *         || j &lt; 0 || j &gt;= hList->size()).
         *
         * @since 12.2.1.3
         */
        static void swap(List::Handle hList, size32_t i, size32_t j);

        /**
        * Returns an immutable set containing only the specified object.
        *
        * @param ohElement  the sole object to be stored in the returned set
        *
        * @return an immutable set containing only the specified object
        */
        static Set::View singleton(Object::Holder ohElement);

        /**
        * Returns an immutable list containing only the specified object.
        *
        * @param ohElement  the sole object to be stored in the returned list
        *
        * @return an immutable list containing only the specified object
        */
        static List::View singletonList(Object::Holder ohElement);

        /**
        * Returns an immutable map, mapping only the specified key to the
        * specified value.
        *
        * @param vKey     the sole key to be stored in the returned map
        * @param ohValue  the value to which the returned map maps key
        *
        * @return an immutable map containing only the specified key-value
        *         mapping
        */
        static Map::View singletonMap(Object::View vKey, Object::Holder ohValue);

        /**
        * Returns a synchronized (thread-safe) map backed by the specified
        * map.  In order to guarantee serial access, it is critical that
        * <strong>all</strong> access to the backing map is accomplished
        * through the returned map.
        * <p>
        * It is imperative that the user manually synchronize on the returned
        * map when iterating over any of its collection views:
        * <pre>
        *  Map::Handle m = Collections::synchronizedMap(HashMap::create());
        *      ...
        *  Set::View s = m->keySet();     // Needn't be in synchronized block
        *      ...
        *  synchronized(m) {              // Synchronizing on m, not s!
        *      // Must be in synchronized block
        *      Iterator::Handle i = s->iterator();
        *      while (i->hasNext())
        *          foo(i->next());
        *  }
        * </pre>
        * Failure to follow this advice may result in non-deterministic behavior.
        *
        * @param hMap  the map to be synchronized
        *
        * @return an synchronized wrapper for a map
        */
        static Map::Handle synchronizedMap(Map::Handle hMap);

        /**
        * Returns a synchronized (thread-safe) map backed by the specified
        * map.
        *
        * @param vMap  the map to be synchronized
        *
        * @return an synchronized wrapper for a map
        */
        static Map::View synchronizedMap(Map::View vMap);

        /**
        * Returns a synchronized (thread-safe) sorted map backed by the
        * specified sorted map.  In order to guarantee serial access, it
        * is critical that <strong>all</strong> access to the backing sorted
        * map is accomplished through the returned sorted map (or its views).
        * <p>
        * It is imperative that the user manually synchronize on the returned
        * sorted map when iterating over any of its collection views, or the
        * collections views of any of its <tt>subMap</tt>, <tt>headMap</tt>
        * or <tt>tailMap</tt> views.
        * <pre>
        *  SortedMap::Handle m = Collections::synchronizedSortedMap(
        *          TreeMap::create());
        *      ...
        *  Set::View s = m->keySet();  // Needn't be in synchronized block
        *      ...
        *  synchronized(m) {           // Synchronizing on m, not s!
        *      // Must be in synchronized block
        *      Iterator::Handle i = s->iterator();
        *      while (i->hasNext())
        *          foo(i->next());
        *  }
        * </pre>
        * or:
        * <pre>
        *  SortedMap::Handle m = Collections::synchronizedSortedMap(
        *          TreeMap::create());
        *  SortedMap::Handle m2 = m->subMap(foo, bar);
        *      ...
        *  Set::View s2 = m2->keySet();  // Needn't be in synchronized block
        *      ...
        *  synchronized(m) {             // Synchronizing on m, not m2 or s2!
        *       // Must be in synchronized block
        *      Iterator::Handle i = s->iterator();
        *      while (i->hasNext())
        *          foo(i->next());
        *  }
        * </pre>
        * Failure to follow this advice may result in non-deterministic behavior.
        *
        * @param hSortedMap  the sorted map to be synchronized
        *
        * @return an synchronized wrapper for a sorted map
        */
        static SortedMap::Handle synchronizedSortedMap(SortedMap::Handle hSortedMap);

        /**
        * Returns a synchronized (thread-safe) map backed by the specified
        * sorted map.
        *
        * @param vSortedMap  the sorted map to be synchronized
        *
        * @return an synchronized wrapper for a sorted map
        */
        static SortedMap::View synchronizedSortedMap(SortedMap::View vSortedMap);

        /**
        * Returns an unmodifiable view of the specified collection.  This
        * method allows modules to provide users with "read-only" access to
        * internal collections.  Query operations on the returned collection
        * "read through" to the specified collection, and attempts to modify
        * the returned collection, whether direct or via its iterator, result
        * in an UnsupportedOperationException.
        *
        * The returned collection does <i>not</i> pass the hashCode and
        * equals operations through to the backing collection, but relies on
        * <tt>Object</tt>'s <tt>equals</tt> and <tt>hashCode</tt> methods.
        * This is necessary to preserve the contracts of these operations in
        * the case that the backing collection is a set or a list.
        *
        * @param  vCollection  the collection for which an unmodifiable view
        *         is to be returned.
        * @return an unmodifiable view of the specified collection.
        */
        static Collection::Handle unmodifiableCollection(Collection::View
                vCollection);

        /**
        * Returns an unmodifiable view of the specified set.  This method
        * allows modules to provide users with "read-only" access to internal
        * sets. Query operations on the returned set "read through" to the
        * specified set, and attempts to modify the returned set, whether
        * direct or via its iterator, result in an
        * UnsupportedOperationException.
        *
        * @param  vSet the set for which an unmodifiable view is to be returned.
        * @return an unmodifiable view of the specified set.
        */
        static Set::Handle unmodifiableSet(Set::View vSet);

        /**
        * Returns an unmodifiable view of an empty set.
        *
        * @return an unmodifiable view of an empty set.
        */
        static Set::View emptySet();

        /**
        * Returns an unmodifiable view of an empty map.
        *
        * @return an unmodifiable view of an empty map.
        */
        static Map::View emptyMap();

        /**
        * Write out the contents of a Iterator.
        *
        * @param hIter  the Iterator to write
        *
        * @return the string representation
        */
        static String::View toString(Iterator::Handle hIter);

        /**
        * Write out the contents of a Collection.
        *
        * @param vCol  the Collection to write
        *
        * @return the string representation
        */
        static String::View toString(Collection::View vCol);

        /**
        * Write out the contents of a Map.
        *
        * @param vCol  the Map to write
        *
        * @return the string representation
        */
        static String::View toString(Map::View vMap);

        /**
        * Write out the contents of a Entry.
        *
        * @param vEntry  the Entry to write
        *
        * @return the string representation
        */
        static String::View toString(Map::Entry::View vEntry);

        /**
        * Return the contents of the collection as an ObjectArray.
        * If the collection fits in the specified array, it is returned,
        * otherwise, a new array is allocated that is the size of this
        * collection.
        *
        * If the collection fits in the array with aditional room
        * then the element in the array immediately following the end of the
        * collection is set to NULL.  This can be useful in determining the
        * length of this collection if the caller knows that the collection
        * does not contain any NULL elements.
        *
        * @param vCol the Collection to turn into an array
        * @param hao  an array in which to store the collection's contents
        *
        * @return a ObjectArray containing all the elements of the collection
        *         in the same order as returned by the collection's Iterator
        *
        * @see Iterator
        */
        static ObjectArray::Handle toArray(Collection::View vCol,
                ObjectArray::Handle hao = NULL);
   };

COH_CLOSE_NAMESPACE2

#endif // COH_COLLECTIONS_HPP
