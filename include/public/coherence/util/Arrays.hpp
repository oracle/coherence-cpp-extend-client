/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ARRAYS_HPP
#define COH_ARRAYS_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* This class contains various methods for manipulating arrays (such as
* sorting and searching).
*
* @author tb  2008.04.04
*/
class COH_EXPORT Arrays
    {
    // ----- static methods -------------------------------------------------

    public:
        /**
        * Copies an array from the specified source array, beginning at the
        * specified position, to the specified position of the destination
        * array.
        *
        * @param haoSrc      the source array
        * @param iSrcStart   starting position in the source array
        * @param haoDest     the destination array
        * @param iDestStart  starting position in the destination data
        * @param cElements   the number of array elements to be copied
        * @throws IndexOutOfBoundsException  if copying would cause
        *               access of data outside array bounds
        */
        static void copy(ObjectArray::Handle haoSrc,
                size32_t iSrcStart, ObjectArray::Handle haoDest,
                size32_t iDestStart, size32_t cElements);

        /**
         * Reverses the order of the elements in the specified array.
         *
         * This method runs in linear time.
         *
         * @param hao  the array whose elements are to be reversed.
         *
         * since 12.2.1.3
         */
        static void reverse(ObjectArray::Handle hao);

        /**
        * Sorts the specified array of objects according to the order induced
        * by the specified comparator.  All elements in the array must be
        * mutually comparable by the specified comparator.
        *
        * This sort is guaranteed to be stable:  equal elements will not be
        * reordered as a result of the sort.<p>
        *
        * @param hao          the array to be sorted
        * @param hComparator  the comparator to determine the order of the
        *                     array.  A null value indicates that the
        *                     elements' natural ordering should be used
        * @throws ClassCastException  if the array contains elements that are
        *      not mutually comparable
        */
        static void sort(ObjectArray::Handle hao,
                Comparator::Handle hComparator = NULL);

        /**
         * Swaps the elements at the specified positions in the specified array.
         * (If the specified positions are equal, invoking this method leaves
         * the array unchanged.)
         *
         * @param hao  the array in which to swap elements.
         * @param i    the index of one element to be swapped.
         * @param j    the index of the other element to be swapped.
         *
         * @throws IndexOutOfBoundsException if either <tt>i</tt> or <tt>j</tt>
         *         is out of range (i &lt; 0 || i &gt;= hList->length
         *         || j &lt; 0 || j &gt;= hList->length).
         *
         * @since 12.2.1.3
         */
        static void swap(ObjectArray::Handle hao, size32_t i, size32_t j);

        /**
        * Performs a binary search for the specified element in the
        * specified sorted array using the Comparator to compare elements.
        *
        * @param vaoSorted    the sorted Object array to search
        * @param vo           the value element to find
        * @param vComparator  the Comparator (optional)
        *
        * @return the non-negative index of the element, or a negative index
        *         which is the -index - 1 where the element would be inserted
        */
        static int32_t binarySearch(ObjectArray::View vaoSorted,
                Object::View vo, Comparator::View vComparator = NULL);

        /**
        * Performs a binary search for the specified element in a part of the
        * specified sorted array using the Comparator to compare elements.
        *
        * @param vaoSorted    the sorted Object array to search
        * @param iStart       the inclusive start index
        * @param iEnd         the exclusive end index
        * @param vo           the value element to find
        * @param vComparator  the Comparator (optional)
        *
        * @return the non-negative index of the element, or a negative index
        *         which is the -index - 1 where the element would be inserted
        */
        static int32_t binarySearch(ObjectArray::View vaoSorted,
                size32_t iStart, size32_t iEnd, Object::View vo,
                Comparator::View vComparator = NULL);

        /**
        * Returns a fixed-size list backed by the specified array. (Changes
        * to the returned list "write through" to the array.) This method
        * acts as bridge between array-based and collection-based APIs.
        * This method also provides a convenient way to create a fixed-size
        * list initialized to contain several elements:
        *
        * @param vao  the array by which the list will be backed
        *
        * @return a list view of the specified array
        *
        * @since Coherence 12.1.2
        */
        static List::View asList(ObjectArray::View vao);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ARRAYS_HPP
