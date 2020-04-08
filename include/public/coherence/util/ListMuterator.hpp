/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIST_MUTERATOR_HPP
#define COH_LIST_MUTERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/ListIterator.hpp"
#include "coherence/util/Muterator.hpp"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* Muterator is mutating iterator, that is it is capable of changing the
* list it iterates.
*
* @author mf  2008.05.05
*/
class COH_EXPORT ListMuterator
    : public interface_spec<ListMuterator,
        implements<ListIterator, Muterator> >
    {
    // ----- ListMuterator interface ----------------------------------------

    public:
        /**
        * Insert the specified element immediately before the element to be
        * returned from the next call to the next() method. A subsequent call
        * to next() will not return the added element, while a call to
        * previous() would return the added element.
        *
        * @param oh  the element to add
        *
        * @throws UnsupportedOperationException if addition is not supported
        */
        virtual void add(Object::Holder oh) = 0;

        /**
        * Replace the last element returned by next() or previous() with
        * the specified element.
        *
        * @param oh  the element to store
        *
        * @throws UnsupportedOperationException if set is not supported
        */
        virtual void set(Object::Holder oh) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_LIST_MUTERATOR_HPP
