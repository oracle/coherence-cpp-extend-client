/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_INDEX_HPP
#define COH_MAP_INDEX_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* MapIndex is used to correlate values stored in an <i>indexed Map</i> (or
* attributes of those values) to the corresponding keys in the indexed Map.
*
* @author tb  2009.02.09
*/
class COH_EXPORT MapIndex
    : public interface_spec<MapIndex>
    {
    // ----- MapIndex interface ---------------------------------------------

    public:
        /**
        * Obtain the ValueExtractor object that the MapIndex uses to extract
        * an indexable Object from a value stored in the indexed Map. This
        * property is never NULL.
        *
        * @return a ValueExtractor object, never NULL
        */
        virtual ValueExtractor::View getValueExtractor() const = 0;

        /**
        * Determine if the MapIndex orders the contents of the indexed
        * information. To determine in which way the contents are ordered,
        * get the Comparator from the <i>index contents</i> SortedMap object.
        *
        * @return true if the index contents are ordered, false otherwise
        */
        virtual bool isOrdered() const = 0;

        /**
        * Determine if indexed information for any entry in the indexed Map has
        * been excluded from this index.  This information is used for
        * IndexAwareFilter implementations to determine the most optimal
        * way to apply the index.
        *
        * @return true if any entry of the indexed Map has been excluded from
        *         the index, false otherwise
        * @since Coherence 3.6
        */
        virtual bool isPartial() const = 0;

        /**
        * Get the Map that contains the <i>index contents</i>.
        * <p>
        * The keys of the Map are the return values from the ValueExtractor
        * operating against the indexed Map's values, and for each key, the
        * corresponding value stored in the Map is a Set of keys to the
        * indexed Map.
        * <p>
        * If the MapIndex is known to be ordered, then the returned Map object
        * will be an instance of SortedMap. The SortedMap may or may
        * not have a Comparator object associated with it.
        * <p>
        * A client should assume that the returned Map object is read-only and
        * must not attempt to modify it.
        *
        * @return a Map (or a SortedMap) of the index contents
        */
        virtual Map::View getIndexContents() const = 0;

        /**
        * Using the index information if possible, get the value associated
        * with the specified key. This is expected to be more efficient than
        * using the ValueExtractor against an object containing the value,
        * because the index should already have the necessary information at
        * hand.
        *
        * @param vKey  the key that specifies the object to extract the value
        *              from
        *
        * @return the value that would be extracted by this MapIndex's
        *         ValueExtractor from the object specified by the passed key;
        *         getNoValue() if the index does not have the necessary
        *         information
        */
        virtual Object::Holder get(Object::View vKey) const = 0;

        /**
        * Using the index information if possible, get the value associated
        * with the specified key. This is expected to be more efficient than
        * using the ValueExtractor against an object containing the value,
        * because the index should already have the necessary information at
        * hand.
        *
        * @param oKey  the key that specifies the object to extract the value
        *              from
        *
        * @return the value that would be extracted by this MapIndex's
        *         ValueExtractor from the object specified by the passed key
        */
        virtual Object::Holder get(Object::View vKey) = 0;

        /**
        * Get the Comparator used to sort the index.
        *
        * @return the comparator
        *
        * @since Coherence 3.5
        */
        virtual Comparator::View getComparator() const = 0;

        /**
        * Update the index in response to a insert operation on a cache.
        *
        * @param vEntry  the entry representing the object being inserted
        *
        * @since Coherence 3.5
        */
        virtual void insert(Map::Entry::View vEntry) = 0;

        /**
        * Update the index in response to an update operation on a cache.
        *
        * @param vEntry the entry representing the object being updated
        *
        * @since Coherence 3.5
        */
        virtual void update(Map::Entry::View vEntry) = 0;

        /**
        * Update the index in response to a remove operation on a cache.
        *
        * @param vEntry the entry representing the object being deleted
        *
        * @since Coherence 3.5
        */
        virtual void remove(Map::Entry::View vEntry) = 0;


    // ---- constants -------------------------------------------------------

    public:
        /**
        * Constant used to indicate that the index does not contain requested
        * value.
        *
        * @return  a constant used to indicate that the index does not
        *          contain requested value.
        *
        * @since Coherence 3.6.1
        */
        static Object::View getNoValue();
};

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_INDEX_HPP
