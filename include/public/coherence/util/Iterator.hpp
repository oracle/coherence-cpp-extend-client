/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ITERATOR_HPP
#define COH_ITERATOR_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An object that implements the Iterator interface generates series of
* Object::Holders, one at a time. Successive calls to next() return
* successive elements of the series.
*
* For example, to print all elements of a collection <i>hCol</i>:
* <pre>
* for (Iterator::Handle hIter = hCol->iterator(); hIter->hasNext(); )
*     {
*     std::cout << hIter->next() << std::endl;
*     }
* </pre>
*
* @see Collection
*
* @author jh/mf/nsa  2008.01.28
*/
class COH_EXPORT Iterator
    : public interface_spec<Iterator>
    {
    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * Determine if the iteration has another element.
        *
        * @return true if the iterator has another element
        */
        virtual bool hasNext() const = 0;

        /**
        * Return the next element in the iterated series.
        *
        * @return the next element in the iterated series
        *
        * @throws coherence::lang::NoSuchElementException if iteration has no
        *         more elements
        */
        virtual Object::Holder next() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ITERATOR_HPP
