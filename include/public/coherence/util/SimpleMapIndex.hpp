/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_MAP_INDEX_HPP
#define COH_SIMPLE_MAP_INDEX_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* SimpleMapIndex is a MapIndex implementation used to correlate property values
* extracted from resource map entries with corresponding keys using what is
* commonly known as an <i>Inverted Index algorithm.</i>.
*
* @author tb  2009.02.09
* @since Coherence 3.5
*/
class COH_EXPORT SimpleMapIndex
    : public class_spec<SimpleMapIndex,
        extends<Object>,
        implements<MapIndex> >
    {
    friend class factory<SimpleMapIndex>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an index from the given map.
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
        * @param fInit        initialize the index if true, default is true
        */
        SimpleMapIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator, bool fInit = true);


    // ----- MapIndex interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ValueExtractor::View getValueExtractor() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isOrdered() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isPartial() const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View getIndexContents() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual Comparator::View getComparator() const;

        /**
        * {@inheritDoc}
        */
        virtual void insert(Map::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual void update(Map::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual void remove(Map::Entry::View vEntry);

    protected:
        /**
        * Check the entry against the set of entries not included in the index and
        * update the set if necessary.
        *
        * @param vEntry    the entry to be checked
        * @param fExclude  true if the insert or update of the entry into the index
        *                  caused an exception
        */
        void updateExcludedKeys(Map::Entry::View vEntry, bool fExclude);

        /**
        * Check if the entry with the given key is excluded from the index.
        *
        * @param vKey  the key to test
        *
        * @return true if the key is in the list of keys currently excluded
        *         from the index, false if the entry with the key is in the index
        */
        bool isKeyExcluded(Object::View vKey) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Initialize the index's data structures.
        *
        * @param fForwardIndex  specifies whether or not a forward index
        *                       map is supported, default is true
        */
        virtual void init(bool fForwardIndex = true);

        /**
        * Get the forward index entry associated with the specified key.
        *
        * @param vKey  the key
        *
        * @return the entry associated with the given key.
        */
        virtual Map::Entry::View getForwardEntry(Object::View vKey) const;

        /**
        * Get the forward index entry associated with the specified key.
        *
        * @param vKey  the key
        *
        * @return the entry associated with the given key.
        */
        virtual Map::Entry::Handle getForwardEntry(Object::View vKey);

        /**
        * Remove the forward index entry for the specified key.
        *
        * @param vKey  the key to remove the forward index entry for
        */
        void removeForwardEntry(Object::View vKey);

        /**
        * Extract the "new" value from the specified entry.
        *
        * @param vEntry  the entry to extract the "new" value from
        *
        * @return the extracted "new" value, or no_value if the extraction failed
        */
        Object::Holder extractNewValue(Map::Entry::View vEntry);

        /**
        * Extract the "old" value from the specified entry.
        *
        * @param vEntry  the entry to extract the "old" value from
        *
        * @return the extracted "old" value, or no_value if the extraction failed
        */
        Object::Holder extractOldValue(MapTrigger::Entry::View vEntry);

       /**
        * Return a Collection representation of the specified value, which could be
        * a Collection, ObjectArray, scalar, or no_value.
        *
        * @param vValue  the value
        *
        * @return a Collection representation of the specified value, or an empty
        *         Collection if no_value
        */
        Collection::View ensureCollection(Object::View vValue);

        /**
        * Instantiate and initialize the forward index.
        *
        * Note: To optimize the memory footprint of the forward index, any
        * subclasses of the SimpleMapIndex that override this method must also
        * implement the #getForwardEntry(Object) method accordingly.
        *
        * @return the forward index.
        */
        Map::Handle instantiateForwardIndex() const;

        /**
        * Instantiate and initialize the inverse index.
        *
        * @param fOrdered     true iff the contents of the indexed information
        *                     should be ordered; false otherwise
        * @param vComparator  the Comparator object which imposes an ordering
        *                     on entries in the index map; or <tt>NULL</tt>
        *                     if the entries' values natural ordering should be
        *                     used
        *
        * @return the inverse index.
        */
        virtual Map::Handle instantiateInverseIndex(bool fOrdered,
                Comparator::View vComparator) const;

        /**
        * Add a new mapping from the given indexed value to the given key in the
        * inverse index.
        *
        * @param ohIxValue  the indexed value (serves as a key in the inverse index)
        * @param vKey       the key to insert into the inverse index
        *
        * @return an already existing reference that is "equal" to the specified
        *         value (if available)
        */
        virtual Object::Holder addInverseMapping(Object::Holder ohIxValue,
                Object::View vKey);

        /**
        * Add a new mapping from the given indexed value to the given key in
        * the supplied index.
        *
        * @param hMapIndex  the index to which to add the mapping
        * @param ohIxValue  the indexed value
        * @param vKey       the key
        *
        * @return an already existing reference that is "equal" to the specified
        *         value (if available)
        */
        virtual Object::Holder addInverseMapping(Map::Handle hMapIndex,
                Object::Holder ohIxValue, Object::View vKey);

        /**
        * Add new mappings from the elements of the given value to the given key
        * in the supplied index.  The given value is expected to be either a
        * Collection or an Object array.
        *
        * @param hMapIndex  the index to which to add the mapping
        * @param ohIxValue  the indexed Collection value (each element serves
        *                   as a key in the inverse index)
        * @param vKey       the key to insert into the inverse index
        *
        * @return an already existing reference that is "equal" to the specified
        *         value (if available)
        */
        virtual Object::Holder addInverseCollectionMapping(Map::Handle hMapIndex,
                Object::Holder ohIxValue, Object::View vKey);

        /**
        * Remove the mapping from the given indexed value to the given key from
        * the inverse index.
        *
        * @param ohIxValue   the indexed value
        * @param vKey        the key
        * @param vColIgnore  the Collection of values to ignore (exclude from removal), or NULL
        */
        virtual void removeInverseMapping(Object::Holder ohIxValue, Object::View vKey,
                Collection::View vColIgnore);

        /**
        * Remove the mapping from the given indexed value to the given key from
        * the inverse index.
        *
        * @param ohIxValue  the indexed value
        * @param vKey       the key
        */
        virtual void removeInverseMapping(Object::Holder ohIxValue, Object::View vKey);

        /**
        * Remove the mapping from the given indexed value to the given key
        * from the supplied index.
        *
        * @param hMapIndex  the index from which to remove the mapping
        * @param ohIxValue  the indexed value
        * @param vKey       the key
        */
        virtual void removeInverseMapping(Map::Handle hMapIndex,
                Object::Holder ohIxValue,
                Object::View vKey);

        /**
        * Update this index in response to a insert operation on a cache.
        *
        * @param vEntry  the entry representing the object being inserted
        */
        virtual void insertInternal(Map::Entry::View vEntry);

        /**
        * Update this index in response to an update operation on a cache.
        *
        * @param vEntry  the entry representing the object being updated
        */
        virtual void updateInternal(Map::Entry::View vEntry);

        /**
        * Update this index in response to a remove operation on a cache.
        *
        * @param vEntry  the entry representing the object being removed
        */
        virtual void removeInternal(Map::Entry::View vEntry);

        /**
        * Log messages for missing inverse index.
        *
        * @param vIxValue  the indexed value
        * @param vKey      the key
        */
        virtual void logMissingIdx(Object::View vIxValue, Object::View vKey);

        /**
        * Given that the old value is known to be a Collection or an array,
        * collect all the enclosed elements that are not part of the new value.
        *
        * @param vIxValueOld  the old value (must be a collection or an array)
        * @param vIxValueNew  the new value
        *
        * @return the set of values that are contained in the old collection or array,
        *         but not part of the new value
        */
        virtual Set::Handle collectRemoved(Object::View vIxValueOld, Object::View vIxValueNew);

        /**
        * Factory method used to create a new set containing the keys associated
        * with a single value.
        *
        * @return a Set
        */
        virtual Set::Handle instantiateSet() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * ValueExtractor object that this MapIndex uses to extract an
        * indexable Object from a [converted] value stored in the Storage.
        */
        FinalView<ValueExtractor> f_vValueExtractor;

        /**
        * Specifies whether or not this MapIndex orders the contents of the
        * indexed information.
        */
        const bool m_fOrdered;

        /**
        * Comparator used to sort the index. Used iff the Ordered is  true.
        * Could be null, which implicates a natural order.
        */
        FinalView<Comparator> f_vComparator;

        /**
        * Map that contains the index values (forward index). The keys of the
        * Map are the keys to the ResourceMap and the values are the extracted
        * values. This map is used by the IndexAwareComparators to avoid a
        * conversion and value extraction steps.
        */
        FinalHandle<Map> f_hMapForward;

        /**
        * Map that contains the index contents (inverse index). The keys of
        * the Map are the return values from the ValueExtractor operating
        * against the values of the resource map, and for each key, the
        * corresponding value stored in the Map is a Set of keys to the
        * resource map.
        */
        FinalHandle<Map> f_hMapInverse;

        /**
        * If a value extracted by the ValueExtractor is a Collection, this
        * property specifies whether or not it should be treated as a
        * collection of contained attributes or indexed as a single composite
        * attribute.
        */
        const bool m_fSplitCollection;

        /**
        * The time at which the most recent logging of "missing inverse index"
        * messages started.
        */
        int64_t m_ldtLogMissingIdx;

        /**
        * The number of "missing inverse index" messages that have been logged.
        */
        int m_cLogMissingIdx;

        /**
        * A set of keys for the entries, which could not be included in the index.
        */
        FinalHandle<Set> f_hSetKeyExcluded;

        /**
        * Specifies whether or not index updates are permitted(immutable values).
        *
        * @since Coherence 12.2.1
        */
        const bool m_fImmutableValues;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_MAP_INDEX_HPP
