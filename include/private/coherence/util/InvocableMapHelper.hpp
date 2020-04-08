/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCABLE_MAP_HELPER_HPP
#define COH_INVOCABLE_MAP_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Helper methods for InvocableMap implementations and Filter related
* evaluation.
*
* @author djl  2008.05.01
*/
class COH_EXPORT InvocableMapHelper
    : public abstract_spec<InvocableMapHelper>
    {
    // ----- static helper methods ------------------------------------------

    public:
        /**
        * Check if the entry passes the filter evaluation.
        *
        * @param vFilter  the filter to evaluate against
        * @param vEntry   a key value pair to filter
        *
        * @return true if the entry passes the filter, false otherwise
        */
        static bool evaluateEntry(Filter::View vFilter,
                Map::Entry::View vEntry);

        /**
        * Check if an entry, expressed as a key and value, passes the filter
        * evaluation.
        *
        * @param vFilter  the filter to evaluate against
        * @param vKey    the key for the entry
        * @param vValue  the value for the entry
        *
        * @return true if the entry passes the filter, false otherwise
        */
        static bool evaluateEntry(Filter::View vFilter,
                Object::View vKey, Object::View vValue);

        /**
        * Check if the entry, in its "original" form, passes the filter evaluation.
        *
        * @param vFilter  the filter to evaluate against
        * @param vEntry   the entry whose "original" value to evaluate
        *
        * @return true iff the entry has an original value and passes the filter
        *
        * @since Coherence 3.7.1
        */
        static bool evaluateOriginalEntry(Filter::View vFilter,
                MapTrigger::Entry::View vEntry);

        /**
        * Extract a value from the specified entry using the specified
        * extractor.
        *
        * @param vExtractor  the extractor to use
        * @param vEntry      the entry to extract from
        *
        * @return the extracted value
        */
        static Object::Holder extractFromEntry(ValueExtractor::View vExtractor,
                Map::Entry::View vEntry);

        /**
        * Extract a value from the "original value" of the specified entry using
        * the specified extractor.
        *
        * @param vExtractor  the extractor to use
        * @param vEntry      the entry to extract from
        *
        * @return the extracted original value
        */
        static Object::Holder extractOriginalFromEntry(
                ValueExtractor::View vExtractor,
                MapTrigger::Entry::View vEntry);

        /**
        * Update the specified entry using the specified updater and value.
        *
        * @param vUpdater  the updater to use
        * @param hEntry    the entry to update
        * @param ohValue   the new value
        */
        static void updateEntry(ValueUpdater::View vUpdater,
                Map::Entry::Handle hEntry, Object::Holder ohValue);


        // ----- QueryMap support ----------------------------------------------

        /**
        * Generic implementation of the QueryMap API.
        *
        * @param vMap         the underlying Map
        * @param vMapIndexes  the available MapIndex objects keyed by
        *                     the related ValueExtractor; read-only
        * @param vFilter      the Filter
        * @param fEntries     if true, return an entry-set; otherwise a key-set
        * @param fSort        if true, sort the entry-set before returning
        * @param vComparator  the Comparator to use for sorting (optional)
        *
        * @return the query result set
        */
        static Set::View query(Map::View vMap, Map::View vMapIndexes,
                Filter::View vFilter, bool fEntries, bool fSort,
                Comparator::View vComparator);

        /**
        * Add an index to the given map of indexes, keyed by the given
        * extractor. Also add the index as a listener to the given
        * ObservableMap.
        *
        * @param vExtractor   the ValueExtractor object that is used to
        *                     extract an indexable Object from a value stored
        *                     in the indexed Map.  Must not be NULL.
        * @param fOrdered     true iff the contents of the indexed information
        *                     should be ordered; false otherwise
        * @param vComparator  the Comparator object which imposes an ordering
        *                     on entries in the indexed map; or <tt>NULL</tt>
        *                     if the entries' values natural ordering should
        *                     be used
        * @param hMap         the resource map that the newly created MapIndex
        *                     will use for initialization and listen to for
        *                     changes
        * @param hMapIndex    the map of indexes that the newly created
        *                     MapIndex will be added to
        */
        static void addIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator, ObservableMap::Handle hMap,
                Map::Handle hMapIndex);

        /**
        * Remove the index keyed by the given extractor from the given map of
        * indexes.  Also, remove the index as a listener from the given
        * ObservableMap.
        *
        * @param vExtractor  the ValueExtractor object that is used to extract
        *                    an indexable Object from a value stored in the
        *                    Map.
        * @param hMap        the resource map to remove the index from
        * @param hMapIndex   the map of indexes to remove the MapIndex from
        */
        static void removeIndex(ValueExtractor::View vExtractor,
                ObservableMap::Handle hMap, Map::Handle hMapIndex);


    // ----- helpers -------------------------------------------------------

    protected:
        /**
        * Ensure a MapListener for the given index.  The listener will route the
        * map events into the corresponding MapIndex calls.
        *
        * @param hIndex  the index
        *
        * @return a listener for given index
        */
        static MapListener::Handle ensureListener(MapIndex::Handle hIndex);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INVOCABLE_MAP_HELPER_HPP
