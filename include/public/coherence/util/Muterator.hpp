/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MUTERATOR_HPP
#define COH_MUTERATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Muterator is mutating iterator, that is it is capable of changing the
* collection it iterates.
*
* @author mf  2008.05.05
*/
class COH_EXPORT Muterator
    : public interface_spec<Muterator,
        implements<Iterator> >
    {
    // ----- Muterator interface --------------------------------------------

    public:
        /**
        * Remove from the collection associated with this iterator, the last
        * element returned from the iterator.
        *
        * @throws UnsupportedOperationException if removal is not supported
        */
        virtual void remove() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MUTERATOR_HPP
