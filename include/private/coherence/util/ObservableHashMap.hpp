/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBSERVABLE_HASHMAP_HPP
#define COH_OBSERVABLE_HASHMAP_HPP

#include "coherence/lang.ns"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "coherence/util/MapListenerSupport.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* An ObservableMap implementation that extends the SafeHashMap.
* 
* This Map implements the ObservableMap interface, meaning it provides
* event notifications to any interested listener for each insert, update and
* delete.
*
* @author nsa  2008.06.28
*/

class COH_EXPORT ObservableHashMap
    : public class_spec<ObservableHashMap,
        extends<SafeHashMap>,
        implements<ObservableMap> >
    {
    friend class factory<ObservableHashMap>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an ObservableHashMap using the specified settings.
        *
        * @param cInitialBuckets  the initial number of hash buckets, 0 < n
        * @param flLoadFactor     the acceptable load factor before resizing
        *                         occurs, 0 < n, such that a load factor of
        *                         1.0 causes resizing when the number of
        *                         entries exceeds the number of buckets
        * @param flGrowthRate     the rate of bucket growth when a resize
        *                         occurs, 0 < n, such that a growth rate of
        *                         1.0 will double the number of buckets:
        *                         bucketcount =
        *                             bucketcount * (1 + growthrate)
        */
        ObservableHashMap(size32_t cInitialBuckets = 17,
                float32_t flLoadFactor = 1.0F, float32_t flGrowthRate = 3.0F);


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * Remove an entry from the cache.
        *
        * @param vKey the key of a cached value
        *
        * @return the value that was cached associated with that key, or null
        *         if no value was cached associated with that key
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
        * Remove everything from the cache.
        */
        virtual void clear();

    // ----- ObservableMap methods ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void addKeyListener(MapListener::Handle hListener,
                Object::View vKey, bool fLite);

        /**
        * {@inheritDoc}
        */
        virtual void removeKeyListener(MapListener::Handle hListener,
                Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void addMapListener(MapListener::Handle hListener);
        
        /**
        * {@inheritDoc}
        */
        virtual void removeMapListener(MapListener::Handle hListener);
        
        /**
        * {@inheritDoc}
        */
        virtual void addFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL, bool fLite = false);

        /**
        * {@inheritDoc}
        */
        virtual void removeFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL);


    // ----- ObservableHashMap methods --------------------------------------

    public:
        /**
        * Removes all mappings from this map.
        *
        * Note: the removal of entries caused by this truncate operation will
        * not be observable.
        *
        * @since 12.2.1.4
        */
        virtual void truncate();
            

    // ----- event dispatching ----------------------------------------------

    protected:
        /**
        * Accessor for the MapListenerSupport for sub-classes.
        *
        * @return the MapListenerSupport, or null if there are no listeners
        */
        virtual MapListenerSupport::Handle getMapListenerSupport();

        /**
        * Determine if the OverflowMap has any listeners at all.
        *
        * @return true iff this OverflowMap has at least one MapListener
        */
        virtual bool hasListeners();

        /**
        * Dispatch the passed event.
        *
        * @param hEvt   a CacheEvent object
        */
        virtual void dispatchEvent(MapEvent::Handle hEvt);


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Clears the cache with the flag fTruncate controlling
        * whether or not events are generated as entries are removed.
        * 
        * @param fTruncate  if true, events will not be
        *                   generated when the map is cleared
        */  
        void clear(bool fTruncate);


    // ----- inner class: Entry ---------------------------------------------

    protected:
        /**
        * Factory method.  This method exists to allow the Cache class to be
        * easily inherited from by allowing the Entry class to be easily
        * sub-classed.
        *
        * @return an instance of Entry that holds the passed cache value
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(Object::View vKey,
                Object::Holder ohValue, size32_t nHash);

        /**
        * Factory pattern, instantiate an Entry that is either a deep or
        * shalow copy.
        *
        * @param vEntry  the Entry to copy
        * @param fDeep   whether to make a deep copy of the Entry or not
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(
                SafeHashMap::Entry::View vEntry);

        /**
        * A holder for a cached value.
        *
        * @author nsa 2008.06.28
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<SafeHashMap::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new Map::Entry.
                *
                * @param vKey     the assocaited key
                * @param ohValue  the associated value
                * @param hHash    the associated hash code
                * @param hMap     the Map that conatins this entry
                *
                * @return a new Map::Entry
                */
                Entry(Object::View vKey, Object::Holder ohValue,
                        size32_t nHash, ObservableHashMap::Handle hMap);

                /**
                * Copy constructor
                */
                Entry(const Entry& that);

                /**
                * Copy an Entry.
                *
                * @param vThat  the entry to copy
                */
                Entry(Entry::View vThat);

            // ----- Map.Entry interface --------------------------------

            public:
                /**
                * Update the cached value.
                *
                * @param oValue  the new value to cache
                *
                * @return the old cache value
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

            protected:
                /**
                * This method is invoked when the containing Map has actually
                * added this Entry to itself.
                */
                virtual void onAdd();

                /**
                * Called to inform the Entry that it has been removed.
                */
                virtual void onRemove();

                /**
                * Return the Map associated with this Entry.
                *
                * @return the Map associated with this Entry
                */
                virtual ObservableHashMap::Handle getMap();

                /**
                * Return the Map associated with this Entry.
                *
                * @return the Map associated with this Entry
                */
                virtual ObservableHashMap::View getMap() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * Reference to the map containing this entry
                */
                mutable WeakHandle<ObservableHashMap> m_whMap;

            friend class ObservableHashMap;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The MapListenerSupport object.
        */
        MemberHandle<MapListenerSupport> m_hListenerSupport;
    };


COH_CLOSE_NAMESPACE2


#endif // #ifndef COH_OBSERVABLE_HASHMAP
