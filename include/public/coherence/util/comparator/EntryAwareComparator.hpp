/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENTRY_AWARE_COMPARATOR_HPP
#define COH_ENTRY_AWARE_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)


/**
* EntryAwareComparator is an extension to the Comparator interface that
* allows the EntryComparator to know whether the underlying  comparator
* expects to compare the corresponding Entries' keys or values.
*
* @author djl  2008.05.05
*/
class COH_EXPORT EntryAwareComparator
    : public interface_spec<EntryAwareComparator,
        implements<Comparator> >
    {
    // ----- EntryAwareComparator interface ---------------------------------

    public:
        /**
        * Specifies whether this comparator expects to compare keys or values.
        *
        * @return true if Entry keys are expected; false otherwise
        */
        virtual bool isKeyComparator() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ENTRY_AWARE_COMPARATOR_HPP
