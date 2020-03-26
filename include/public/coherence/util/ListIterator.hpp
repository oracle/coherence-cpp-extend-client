/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIST_ITERATOR_HPP
#define COH_LIST_ITERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An iterator for lists that allows the programmer to traverse the list in
* either direction, modify the list during iteration, and obtain the
* iterator's current position in the list. A ListIterator has no current
* element; its cursor position always lies between the element that would be
* returned by a call to previous() and the element that would be returned
* by a call to next(). In a list of length n, there are n+1 valid index
* values, from 0 to n, inclusive.
*
* @author nsa 2008.01.28
*/
class COH_EXPORT ListIterator
    : public interface_spec<ListIterator,
        implements<Iterator> >
    {
    // ----- ListIterator interface -----------------------------------------

    public:
        /**
        * Return the index of the next element to be returned from a call to
        * next().
        *
        * @return the index of the next element, or the list size if the
        *         iterator has reached the end of the list
        */
        virtual size32_t nextIndex() const = 0;

        /**
        * Return the index of the element to be returned from a call to
        * previous().
        *
        * @return the index of the element that would be returned from a call
        *         to previous, or List::npos if the iterator is at the start
        *         of the list.
        */
        virtual size32_t previousIndex() const = 0;

        /**
        * Returns true if this list iterator has more elements when
        * traversing the list in the reverse direction.
        *
        * @return true if this list iterator has more elements when
        *         traversing the list in the reverse direction
        */
        virtual bool hasPrevious() const = 0;

        /**
        * Returns the previous element in the list.
        *
        * @return the previous element in the list
        */
        virtual Object::Holder previous() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LIST_ITERATOR_HPP
