/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PAGED_MUTERATOR_HPP
#define COH_PAGED_MUTERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Muterator.hpp"

#include "private/coherence/util/PagedIterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* PagedMuterator is an Muterator implementation based on a concept of a 
* <i>page Advancer</i> - a pluggable component that knows how to supply a 
* next page of objects to iterate through. As common to iterators, this 
* implementation is not thread safe.
* 
* @author tb  2008.08.11
*/
class COH_EXPORT PagedMuterator
    : public class_spec<PagedMuterator,
        extends<PagedIterator>,
        implements<Muterator> >
    {
    friend class factory<PagedMuterator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PagedMuterator instance
        * 
        * @param hAdvancer  the underlying Advancer
        */
        PagedMuterator(Advancer::Handle hAdvancer);

    private:
        /**
        * Blocked copy constructor.
        */
        PagedMuterator(const PagedMuterator&);


    // ----- Muterator interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_PAGED_MUTERATOR_HPP
