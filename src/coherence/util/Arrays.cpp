/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Arrays.hpp"

#include "coherence/util/ReadOnlyArrayList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


namespace
    {
    // ----- constants ------------------------------------------------------

    /**
    * Tuning parameter: list size at or below which insertion sort will
    * be used in preference to mergesort or quicksort.
    */
    const size32_t insertionsort_threshold = 7;


    // ----- helper methods -------------------------------------------------

    /**
    * Copies an array from the specified source array, beginning at the
    * specified position, to the specified position of the destination
    * array.
    *
    * @param      haoSrc      the source array
    * @param      iSrcStart   starting position in the source array
    * @param      haoDest     the destination array
    * @param      iDestStart  starting position in the destination data
    * @param      cElements   the number of array elements to be copied
    * @throws IndexOutOfBoundsException  if copying would cause
    *               access of data outside array bounds
    */
    void arrayCopy(ObjectArray::Handle haoSrc, size32_t iSrcStart,
            ObjectArray::Handle haoDest, size32_t iDestStart, size32_t cElements)
        {
        for (size32_t i=0; i < cElements; ++i)
            {
            haoDest[iDestStart + i] =
                    haoSrc[iSrcStart + i];
            }
        }

    /**
    * Sort the source array into the dest array use merge sort.
    *
    * @param haoSrc   the source array that starts at index 0
    * @param haoDest  the (possibly larger) array destination with a
    *                 possible offset
    * @param iLow     the index in dest to start sorting
    * @param iHigh    the end index in dest to end sorting
    * @param iOffset  the offset into haoSrc corresponding to iLow
    *                 in haoDest
    */
    void mergeSort(ObjectArray::Handle haoSrc,
            ObjectArray::Handle haoDest,
            size32_t iLow, size32_t iHigh, int32_t iOffset)
        {
        size32_t iLength = iHigh - iLow;

        // Insertion sort on smallest arrays
        if (iLength < insertionsort_threshold)
            {
            for (size32_t i = iLow; i < iHigh; i++)
                {
                for (size32_t j = i; j > iLow &&
                        (cast<Comparable::Handle>(haoDest[j-1]))->
                            compareTo(haoDest[j])>0;
                        j--)
                    {
                    Arrays::swap(haoDest, j, j-1);
                    }
                }
            return;
            }

        // Recursively sort halves of dest into src
        size32_t iDestLow  = iLow;
        size32_t iDestHigh = iHigh;
        iLow               += iOffset;
        iHigh              += iOffset;
        size32_t iMid      = (iLow + iHigh) >> 1;

        mergeSort(haoDest, haoSrc, iLow, iMid, -iOffset);
        mergeSort(haoDest, haoSrc, iMid, iHigh, -iOffset);

        // If list is already sorted, just copy from src to dest.  This is an
        // optimization that results in faster sorts for nearly ordered
        // lists.
        if ((cast<Comparable::Handle>(haoSrc[iMid-1]))->
                compareTo(haoSrc[iMid]) <= 0)
            {
            arrayCopy(haoSrc, iLow, haoDest, iDestLow, iLength);
            return;
            }

        // Merge sorted halves (now in src) into dest
        for (size32_t i = iDestLow, p = iLow, q = iMid; i < iDestHigh; i++)
            {
            if (q >= iHigh || (p < iMid &&
                    (cast<Comparable::Handle>(haoSrc[p]))->
                    compareTo(haoSrc[q])<=0))
                {
                haoDest[i] = haoSrc[p++];
                }
            else
                {
                haoDest[i] = haoSrc[q++];
                }
            }
        }

    /**
    * Sort the source array into the dest array use merge sort.
    *
    * @param haoSrc        the source array that starts at index 0
    * @param haoDest       the (possibly larger) array destination with a
    *                      possible offset
    * @param iLow          the index in dest to start sorting
    * @param iHigh         the end index in dest to end sorting
    * @param iOffset       the offset into haoSrc corresponding to
    *                      iLow in haoDest
    * @param hComaparator  the comparator to determine the order of the
    *                      array
    */
    void mergeSort(ObjectArray::Handle haoSrc,
            ObjectArray::Handle haoDest,
            size32_t iLow, size32_t iHigh, int32_t iOffset,
            Comparator::Handle hComparator)
        {
        size32_t iLength = iHigh - iLow;

        // Insertion sort on smallest arrays
        if (iLength < insertionsort_threshold)
            {
            for (size32_t i = iLow; i < iHigh; i++)
                for (size32_t j = i; j > iLow && hComparator->
                        compare(haoDest[j-1],
                        haoDest[j])>0; j--)
                    Arrays::swap(haoDest, j, j-1);
            return;
            }

        // Recursively sort halves of dest into src
        size32_t iDestLow  = iLow;
        size32_t iDestHigh = iHigh;
        iLow              += iOffset;
        iHigh             += iOffset;
        size32_t iMid      = (iLow + iHigh) >> 1;

        mergeSort(haoDest, haoSrc, iLow, iMid, -iOffset, hComparator);
        mergeSort(haoDest, haoSrc, iMid, iHigh, -iOffset, hComparator);

        // If list is already sorted, just copy from src to dest.  This is an
        // optimization that results in faster sorts for nearly ordered
        // lists.
        if (hComparator->compare(haoSrc[iMid-1],
                haoSrc[iMid]) <= 0)
            {
            arrayCopy(haoSrc, iLow, haoDest, iDestLow, iLength);
            return;
            }

        // Merge sorted halves (now in src) into dest
        for (size32_t i = iDestLow, p = iLow, q = iMid; i < iDestHigh; i++)
            {
            if (q >= iHigh || (p < iMid && hComparator->
                    compare(haoSrc[p], haoSrc[q]) <= 0))
                {
                haoDest[i] = haoSrc[p++];
                }
            else
                {
                haoDest[i] = haoSrc[q++];
                }
            }
        }
    }  // end anonymous namespace


// ----- Arrays interface ---------------------------------------------------

void Arrays::copy(ObjectArray::Handle haoSrc, size32_t iSrcStart,
        ObjectArray::Handle haoDest, size32_t iDestStart, size32_t cElements)
    {
    arrayCopy(haoSrc, iSrcStart, haoDest, iDestStart, cElements);
    }

void Arrays::reverse(ObjectArray::Handle hao)
    {
    size32_t cSize = hao->length;
    for (size32_t i = 0, mid = cSize >> 1, j = cSize - 1; i < mid; i++, j--)
        {
        swap(hao, i, j);
        }
    }

void Arrays::sort(ObjectArray::Handle haoSorted, Comparator::Handle hComparator)
    {
    ObjectArray::Handle aux = ObjectArray::create(haoSorted->length);
    arrayCopy(haoSorted, 0, aux, 0, haoSorted->length);

    if (is_null(hComparator))
        {
        mergeSort(aux, haoSorted, 0, haoSorted->length, 0);
        }
    else
        {
        mergeSort(aux, haoSorted, 0, haoSorted->length, 0, hComparator);
        }
    }

void Arrays::swap(ObjectArray::Handle hao, size32_t i1, size32_t i2)
    {
    Object::Holder t = hao[i1];
    hao[i1] = hao[i2];
    hao[i2] = t;
    }

int32_t Arrays::binarySearch(ObjectArray::View vaoSorted, Object::View vo,
        Comparator::View vComparator)
    {
    return Arrays::binarySearch(vaoSorted, 0,vaoSorted->length, vo,
            vComparator);
    }

int32_t Arrays::binarySearch(ObjectArray::View vaoSorted, size32_t iStart,
        size32_t iEnd, Object::View vo, Comparator::View vComparator)
    {
    // no need to check if iStart < 0 as iStart is unsigned
    if (iStart > iEnd || vaoSorted->length < iEnd)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "invalid start or end index");
        }

    size32_t iLow    = iStart;
    size32_t iMid    = 0;
    size32_t iHigh   = iEnd - 1;
    int32_t  nResult = 0;

    while (iLow <= iHigh)
        {
        iMid    = uint32_t(iLow + iHigh) >> 1;
        nResult = NULL == vComparator ?
                (cast<Comparable::Handle>(vaoSorted[iMid]))->compareTo(vo) :
                vComparator->compare(vaoSorted[iMid], vo);

        if (nResult < 0)
            {
            iLow = iMid + 1;
            }
        else if (nResult == 0)
            {
            return iMid;
            }
        else
            {
            iHigh = iMid - 1;
            }
        }
    int32_t nMid = iMid;
    return -nMid - (nResult >= 0 ? 1 : 2);
    }

List::View Arrays::asList(ObjectArray::View vao)
    {
    return ReadOnlyArrayList::create(vao);
    }

COH_CLOSE_NAMESPACE2
