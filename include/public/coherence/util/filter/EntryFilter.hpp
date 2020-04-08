/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENTRY_FILTER_HPP
#define COH_ENTRY_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* EntryFilter provides an extension to Filter for those cases in which both
* a key and a value may be necessary to evaluate the conditional inclusion
* of a particular object.
*
* @author jh  2008.02.28
*/
class COH_EXPORT EntryFilter
    : public interface_spec<EntryFilter,
        implements<Filter> >
    {
    // ----- EntryFilter interface ------------------------------------------

    public:
        /**
        * Apply the test to a Map::Entry.
        *
        * @param vEntry  the Map::Entry to evaluate; never NULL
        *
        * @return true if the test passes, false otherwise
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ENTRY_FILTER_HPP
