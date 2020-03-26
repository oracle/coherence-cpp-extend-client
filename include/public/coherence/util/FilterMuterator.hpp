/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_FILTER_MUTERATOR_HPP
#define COH_FILTER_MUTERATOR_HPP

#include "coherence/lang.ns"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Muterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Provides a generic implementation of an iterator which can iterate items
* based on an inclusion test
*
* @authore nsa 2008.06.23
*/

class COH_EXPORT FilterMuterator
    : public class_spec<FilterMuterator,
        extends<Object>,
        implements<Muterator> >
    {
    friend class factory<FilterMuterator>;

    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Construct a hash map using the specified settings.
        *
        * @param hMuterator  the muterator to a list that should be filtered
        * @param vTest       the test used to filter results
        */
        FilterMuterator(Iterator::Handle hIter, Filter::View vTest);


    // ----- Iterator interface ---------------------------------------------

    public:
        /**
        * Determine if the iteration has another element.
        *
        * @return true if the iterator has another element
        */
        virtual bool hasNext() const;

        /**
        * Return the next element in the iterated series.
        *
        * @return the next element in the iterated series
        *
        * @throws coherence::lang::NoSuchElementException if iteration has no
        *         more elements
        */
        virtual Object::Holder next();


    // ----- Muterator interface --------------------------------------------

    public:
        /**
        * Remove from the collection associated with this iterator, the last
        * element retruned from the iterator.
        *
        * @throws UnsupportedOperationException if removal is not supported
        */
        virtual void remove();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Objects to filter
        */
        mutable FinalHandle<Iterator> f_hIter;

        /**
        * Test to perform on each item
        */
        FinalView<Filter> f_vTest;

        /**
        * Is there a next item which passed the test?
        */
        mutable bool m_fNext;

        /**
        * Is there a previous item which passed the test and can be removed?
        */
        mutable bool m_fPrev;

        /**
        * The next item which passed the test.
        */
        mutable MemberHolder<Object> m_ohNext;
    };

COH_CLOSE_NAMESPACE2

#endif //COH_FILTER_MUTERATOR_HPP
