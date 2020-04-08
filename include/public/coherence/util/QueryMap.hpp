/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_QUERY_MAP_HPP
#define COH_QUERY_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Map with additional query features.
*
* @author jh  2008.02.27
*/
class COH_EXPORT QueryMap
    : public interface_spec<QueryMap,
        implements<Map> >
    {
    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * Return a set of the keys contained in this map for entries that
        * satisfy the criteria expressed by the filter.
        *
        * Unlike the {@link Map#keySet()} method, the set returned by this
        * method may not be backed by the map, so changes to the set may not
        * reflected in the map, and vice-versa.
        *
        * @param vFilter  the Filter object representing the criteria that
        *                 the entries of this map should satisfy
        *
        * @return a set of keys for entries that satisfy the specified
        *         criteria
        */
        virtual Set::View keySet(Filter::View vFilter) const = 0;

        /**
        * Return a set of the entries contained in this map that satisfy the
        * criteria expressed by the filter. Each element in the returned set
        * is a Map::Entry.
        *
        * Unlike the Map#entrySet() method, the set returned by this
        * method may not be backed by the map, so changes to the set may not
        * be reflected in the map, and vice-versa.
        *
        * @param vFilter  the Filter object representing the criteria that
        *                the entries of this map should satisfy
        *
        * @return a set of entries that satisfy the specified criteria
        */
        virtual Set::View entrySet(Filter::View vFilter) const = 0;

        /**
        * Return a set of the entries contained in this map that satisfy the
        * criteria expressed by the filter. Each element in the returned set
        * is a Map::Entry. It is further guaranteed that its iterator will
        * traverse the set in such a way that the entry values come up
        * in ascending order, sorted by the specified Comparator or
        * according to the <i>natural ordering</i> (see Comparable).
        *
        * Unlike the Map#entrySet() method, the set returned by this
        * method may not be backed by the map, so changes to the set may not
        * be reflected in the map, and vice-versa.
        *
        * @param vFilter      the Filter object representing the criteria
        *                     that the entries of this map should satisfy
        * @param vComparator  the Comparator object which imposes an ordering
        *                     on entries in the resulting set; or
        *                     <tt>NULL</tt> if the entries' values natural
        *                     ordering should be used
        *
        * @return a set of entries that satisfy the specified criteria
        */
        virtual Set::View entrySet(Filter::View vFilter,
                Comparator::View vComparator) const = 0;

        /**
        * Add an index to this QueryMap. This allows to correlate values
        * stored in this <i>indexed Map</i> (or attributes of those values)
        * to the corresponding keys in the indexed Map and increase the
        * performance of <tt>keySet</tt> and <tt>entrySet</tt> methods.
        *
        * This method is only intended as a hint to the cache implementation,
        * and as such it may be ignored by the cache if indexes are not
        * supported or if the desired index (or a similar index) already
        * exists. It is expected that an application will call this method to
        * suggest an index even if the index may already exist, just so that
        * the application is certain that index has been suggested. For
        * example in a distributed environment, each server will likely
        * suggest the same set of indexes when it starts, and there is no
        * downside to the application blindly requesting those indexes
        * regardless of whether another server has already requested the same
        * indexes.
        *
        * @param vExtractor   the ValueExtractor object that is used to
        *                     extract an indexable Object from a value stored
        *                     in the indexed Map.  Must not be <tt>NULL</tt>.
        * @param fOrdered     true iff the contents of the indexed
        *                     information should be ordered; false otherwise
        * @param vComparator  the Comparator object which imposes an ordering
        *                     on entries in the indexed map; or <tt>NULL</tt>
        *                     if the entries' values natural ordering should
        *                     be used
        */
        virtual void addIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator) = 0;

        /**
        * Remove an index from this QueryMap.
        *
        * @param vExtractor  the ValueExtractor object that is used to
        *                    extract an indexable Object from a value stored
        *                    in the Map
        */
        virtual void removeIndex(ValueExtractor::View vExtractor) = 0;


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        using Map::keySet;

        /**
        * {@inheritDoc}
        */
        using Map::entrySet;


    // ----- inner interface: Entry -----------------------------------------

    public:
        /**
        * A QueryMap::Entry exposes additional index-related operation that
        * the basic Map::Entry does not.
        */
        class COH_EXPORT Entry
            : public interface_spec<Entry,
                implements<Map::Entry> >
            {
            // ----- QueryMap::Entry interface --------------------------

            public:
                /**
                * Extract a value out of the Entry's value. Calling this
                * method is semantically equivalent to
                * <tt>vExtractor->extract(getValue())</tt>, but this method
                * may be significantly less expensive. For example, the
                * resultant value may be obtained from a forward index,
                * avoiding a potential object de-serialization.
                *
                * @param vExtractor  a ValueExtractor to apply to the Entry's
                *                    value
                *
                * @return the extracted value
                */
                virtual Object::Holder extract(
                        ValueExtractor::View vExtractor) const = 0;
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_QUERY_MAP_HPP
