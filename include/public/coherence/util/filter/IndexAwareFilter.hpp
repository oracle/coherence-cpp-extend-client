/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INDEX_AWARE_FILTER_HPP
#define COH_INDEX_AWARE_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/filter/EntryFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* IndexAwareFilter is an extension to the EntryFilter interface that allows
* a filter to use a Map index to fully or partially evaluate itself.
*
* @author tb  2009.02.09
*/
class COH_EXPORT IndexAwareFilter
    : public interface_spec<IndexAwareFilter,
        implements<EntryFilter> >
    {
    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * Given a Map of available indexes, determine if this IndexAwareFilter
        * can use any of the indexes to assist in its processing, and if so,
        * determine how effective the use of that index would be.
        * The returned value is an effectiveness estimate of how well this
        * filter can use the specified indexes to filter the specified keys.
        * An operation that requires no more than a single access to the index
        * content (i.e. Equals, NotEquals) has an effectiveness of <b>one</b>.
        * Evaluation of a single entry is assumed to have an effectiveness
        * that depends on the index implementation and is usually measured as
        * a constant number of the single operations.  This number is referred
        * to as <i>evaluation cost</i>.
        *
        * If the effectiveness of a filter evaluates to a number larger than
        * the <tt>keySet.size() * &lt;evaluation cost&gt;</tt> then a user
        * could avoid using the index and iterate through the keySet calling
        * <tt>evaluate</tt> rather then <tt>applyIndex</tt>.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by
        *                     the related ValueExtractor; read-only
        * @param vSetKeys     the set of keys that will be filtered; read-only
        *
        * @return an effectiveness estimate of how well this filter can use
        *         the specified indexes to filter the specified keys
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const = 0;

        /**
        * Filter remaining keys using a Map of available indexes.
        * The filter is responsible for removing all keys from the passed set
        * of keys that the applicable indexes can prove should be filtered. If
        * the filter does not fully evaluate the remaining keys using just the
        * index information, it must return a filter (which may be an
        * EntryFilter) that can complete the task using an iterating
        * implementation. If, on the other hand, the filter does fully
        * evaluate the remaining keys using just the index information, then
        * it should return NULL to indicate that no further filtering is
        * necessary.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by
        *                     the related ValueExtractor; read-only
        * @param hSetKeys     the mutable set of keys that remain to be
        *                     filtered
        *
        * @return a Filter object (which may be an EntryFilter) that can be
        *         used to process the remaining keys, or NULL if no additional
        *         filter processing is necessary
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_INDEX_AWARE_FILTER_HPP
