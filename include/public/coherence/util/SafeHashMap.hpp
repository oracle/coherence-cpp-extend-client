/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_HASH_MAP_HPP
#define COH_SAFE_HASH_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/AbstractStableIterator.hpp"
#include "coherence/util/AtomicCounter.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"


COH_OPEN_NAMESPACE2(coherence,util)


/**
* An implementation of coherence::util::Map that is synchronized, but
* minimally so. This class is for use in situation where high concurrency is
* required, but so is data integrity.
*
* All additions and removals are synchronized on the map, so to temporarily
* prevent changes to the map contents, synchronize on the map object.
*
* @author mf  2008.02.25
*/
class COH_EXPORT SafeHashMap
    : public cloneable_spec<SafeHashMap,
        extends<AbstractMap> >
    {
    friend class factory<SafeHashMap>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a thread-safe hash map using the specified settings.
        *
        * @param cInitialBuckets  the initial number of hash buckets,
        *                         0 &lt; n
        * @param flLoadFactor     the acceptable load factor before resizing
        *                         occurs, 0 &lt; n, such that a load factor
        *                         of 1.0 causes resizing when the number of
        *                         entries exceeds the number of buckets
        * @param flGrowthRate     the rate of bucket growth when a resize
        *                         occurs, 0 &lt; n, such that a growth rate
        *                         of 1.0 will double the number of buckets:
        *                         bucketcount = bucketcount * (1 + growthrate)
        */
        SafeHashMap(size32_t cInitialBuckets = 17,
                float32_t flLoadFactor = 1.0F, float32_t flGrowthRate = 3.0F);

        /**
        * Copy constructor.
        */
        SafeHashMap(const SafeHashMap& that);


    // ----- forward declarations -------------------------------------------

    class Entry;
    class EntrySet;
    class EntrySetIterator;


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        using Map::get;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet();


    // ----- inner class: Entry ---------------------------------------------

    protected:
        /**
        * A map entry (key-value pair).  The <tt>Map.entrySet</tt> method
        * returns a collection-view of the map, whose elements are of this
        * class.
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<Object>,
                implements<Map::Entry> >
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
                *
                * @return a new Map::Entry
                */
                Entry(Object::View vKey, Object::Holder ohValue,
                        size32_t nHash);

                /**
                * Copy Constructor
                */
                Entry(const Entry& that);

                /**
                * Instantiate an Entry shallow copying the key and value from
                * the Entry provided.
                *
                * @param vThat  the entry to copy from
                */
                Entry(Entry::View vThat);

            // ----- SafeHashMap::Entry interface -----------------------

            public:
                /**
                * Return true if the supplied key is equal to the entries
                * key.
                */
                virtual bool isKeyEqual(Object::View vKey) const;

            // ----- Map::Entry interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue();

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

            // ----- class methods --------------------------------------

                /**
                * This method is invoked when the containing Map has actually
                * added this Entry to itself.
                */
                virtual void onAdd();

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View vThat) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The key.
                */
                FinalView<Object> f_vKey;

                /**
                * The value. This object reference can change within the life
                * of the Entry.
                */
                MemberHolder<Object> m_ohValue;

                /**
                * The key's hash code.  This value will not change for the
                * life of the Entry.
                */
                const size32_t m_nHash;

                /**
                * The next entry in the linked list (an open hashing
                * implementation).
                */
                MemberHandle<Entry> m_hNext;

            // ----- friends --------------------------------------------

            friend class SafeHashMap;
            friend class EntrySet;
            friend class EntrySetIterator;
            };

        /**
        * Factory pattern, initialized with the specified valued
        *
        * @param vKey     the associated key
        * @param ohValue  the assocaited value
        * @param nHash    the associated hash code
        *
        * @return a new instance of the Entry class (or a subclass thereof)
        */
        virtual Entry::Handle instantiateEntry(Object::View vKey,
                Object::Holder ohValue, size32_t nHash);

        /**
        * Factory pattern, instantiate an Entry that is either a deep or
        * shalow copy.
        *
        * @param vEntry  the Entry to copy
        * @param fDeep   whether to make a deep copy of the Entry or not
        */
        virtual Entry::Handle instantiateEntry(Entry::View vEntry);

    // ----- inner class: EntrySet ------------------------------------------

    protected:
        /**
        * A set of entries backed by this map.
        */
        class COH_EXPORT EntrySet
            : public class_spec<EntrySet,
                extends<AbstractCollection>,
                implements<Set> >
            {
            friend class factory<EntrySet>;

            // ----- constructors --------------------------------------

            protected:
                /**
                * Return a new EntrySet.
                */
                EntrySet(SafeHashMap::Holder ohMap);


            // ----- Set interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                using Collection::add;

                /**
                * {@inheritDoc}
                */
                virtual Iterator::Handle iterator() const;

                /**
                * {@inheritDoc}
                */
                virtual Muterator::Handle iterator();

                /**
                * {@inheritDoc}
                */
                virtual size32_t size() const;

                /**
                * {@inheritDoc}
                */
                virtual bool contains(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual bool remove(Object::View v);

                /**
                * {@inheritDoc}
                */
                virtual void clear();

                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hao = NULL) const;

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View that) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

            // ----- helper methods --------------------------------------

            protected:
                /**
                * Factory pattern.
                *
                * @return a new instance of an Iterator over the EntrySet
                */
                virtual Iterator::Handle instantiateIterator() const;

                /**
                * Factory pattern.
                *
                * @return a new instance of an Iterator over the EntrySet
                */
                virtual Muterator::Handle instantiateIterator();

                /**
                * Return a handle to the assocaited Map.
                */
                virtual SafeHashMap::Handle getDelegate();

                /**
                * Return a view to the assocaited Map.
                */
                virtual SafeHashMap::View getDelegate() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The SafeHashMap associated with the EntrySet.
                */
                FinalHolder<SafeHashMap> f_ohMap;

            // ----- friends --------------------------------------------

            friend class SafeHashMap;
            friend class EntrySetIterator;
            };


    // ----- inner class: EntrySetIterator ------------------------------

    protected:
        /**
        * An Iterator over the EntrySet that is backed by the
        * SafeHashMap.
        */
        class COH_EXPORT EntrySetIterator
            : public class_spec<EntrySetIterator,
                extends<AbstractStableIterator> >
            {
            friend class factory<EntrySetIterator>;

            protected:
                /**
                * Construct an Iterator over the Entries in the
                * SafeHashMap. Special care must be taken to handle
                * the condition in which the SafeHashMap is currently
                * resizing.
                *
                * @param ohMap  the set to iterate
                */
                EntrySetIterator(SafeHashMap::Holder ohMap);

                /**
                * @internal
                */
                virtual ~EntrySetIterator();


            // ----- AbstractStableIterator interface -------------------

            public:
                /**
                * {@inheritDoc}
                */
                using Muterator::remove;

            protected:
                /**
                * Advance to the next object.
                */
                virtual void advance();

                /**
                * Shut down the Iterator. This is done on exhaustion of the
                * contents of the Iterator, or on destruction of the Iterator.
                */
                virtual void deactivate() const;

                /**
                * {@inheritDoc}
                */
                virtual void remove(Object::Holder oh);

            // ----- data members ---------------------------------------

            private:
                /**
                * The EntrySet's Map being iterated.
                */
                FinalHolder<SafeHashMap> f_ohMap;

                /**
                * Array of buckets in the hash map. This is a
                * purposeful copy of the hash map's reference to its
                * buckets in order to detect that a resize has
                * occurred.
                */
                MemberView<ObjectArray> m_vaeBucket;

                /**
                * Current bucket being iterated.
                */
                size32_t m_iBucket;

                /**
                * The most recent Entry object internally iterated.
                * This is not necessarily the same Entry object that
                * was reported to the stable iterator (via the
                * setNext() method), since when a resize occurs, the
                * entries that are being iterated over internally are
                * the "old" Entry objects (pre-resize) while the
                * entries being returned from the Iterator are the
                * "new" Entry objects (post-resize).
                */
                MemberHandle<Entry> m_hEntryPrev;

                /**
                * Set to true when a resize has been detected.
                */
                bool m_fResized;

                /**
                * Set to true when the Iterator is complete.
                */
                mutable bool m_fDeactivated;

                /**
                * True if the iterator will only return views.
                */
                const bool m_fViewer;
            };


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Locate an Entry in the hash map based on its key.
        *
        * @param vKey  the key object to search for
        *
        * @return the Entry or NULL
        */
        virtual Entry::View getEntry(Object::View vKey) const;

        /**
        * Locate an Entry in the hash map based on its key.
        *
        * @param vKey  the key object to search for
        *
        * @return the Entry or NULL
        */
        virtual Entry::Handle getEntry(Object::View vKey);

    protected:
        /**
        * Factory pattern.
        *
        * @return a new instance of the EntrySet class (or a subclass
        * thereof)
        */
        virtual Set::Handle instantiateEntrySet();

        /**
        * Factory pattern.
        *
        * @return a new instance of the EntrySet class (or a subclass
        * thereof)
        */
        virtual Set::View instantiateEntrySet() const;

        /**
        * Resize the bucket array, rehashing all Entries.
        */
        virtual void grow();

        /**
        * Return the hash code to use for the specified key.
        *
        * @param vKey  the key to hash
        *
        * @return the hash code to use for the key
        */
        virtual size32_t getHashCode(Object::View vKey) const;

        /**
        * Clone an entire linked list of entries.
        *
        * This method must be called on the map that will contain the clones,
        * to allow the map to be the parent of the entries if the entries are
        * not static inner classes.
        *
        * @param vEntryThat  the entry that is the head of the list to clone
        *
        * @return NULL if the entry is NULL, otherwise an entry that is a
        *         clone of the passed entry, and a linked list of clones for
        *         each entry in the linked list that was passed
        */
        virtual Entry::Handle cloneEntryList(Entry::View vEntryThat) const;

        /**
        * Locate an Entry in the hash map based on its key.
        *
        * Unlike the #getEntry method, there must be no side-effects
        * of calling this method.
        *
        * @param vKey  the key object to search for
        *
        * @return the Entry or null
        */
        virtual Entry::Handle getEntryInternal(Object::View vKey) const;

        /**
        * Removes the passed Entry from the map.
        *
        * <b>Note:</b> Unlike calling the "remove" method, which is overriden
        * at subclasses, calling this method directly does not generate any
        * events.
        *
        * @param hEntry  the entry to remove from this map
        */
        virtual void removeEntryInternal(Entry::Handle hEntry);

        /**
        * Calculate the bucket number for a particular hash code.
        *
        * @param nHash     the hash code
        * @param cBuckets  the number of buckets
        *
        * @return the bucket index for the specified hash code
        */
        virtual size32_t getBucketIndex(size32_t nHash, size32_t cBuckets) const;

        /**
        * Get the bucket array, or if a resize is occurring, wait for the
        * resize to complete and return the new bucket array.
        *
        * @return the latest bucket array
        */
        virtual ObjectArray::View getStableBucketArray() const;

        /**
        * Register the activation of an Iterator.
        */
        virtual void iteratorActivated() const;

        /**
        * Unregister the (formerly active) Iterator.
        */
        virtual void iteratorDeactivated() const;

        /**
        * Determine if there are any active Iterators, which may mean that
        * they are in the middle of iterating over the Map.
        *
        * @return true iff there is at least one active Iterator
        */
        virtual bool isActiveIterator() const;

        /**
        * Invaldiate the Map so it's no longer useable
        */
        virtual void invalidate();

        /**
         * Retrun wither the Map is still valid or not
         *
         * @return whether the Map is still valid or not.
         */
        virtual bool isValid() const;

    private:
        /**
        * Copy a list of Entries.
        *
        * @param vEntry  the Entry to copy
        * @param fDeep   whether to deep copy each Entry in the list or not
        *
        * @return a copy of the list of Entries passed in
        */
        virtual Entry::Handle copyEntryList(Entry::View vEntry, bool fDeep);

        /**
        * Copy an individual Entry.
        *
        * @param vEntry  the Entry to copy
        * @param fDeep   whether to deep copy the Entry or not
        *
        * @return a copy of the Entry passed in
        */
        virtual Entry::Handle copyEntry(Entry::View vEntry, bool fDeep);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Default initial size provides a prime modulo and is large enough
        * that resize is not immediate.  (A hash map probably uses less than
        * 128 bytes initially.)
        */
        static const size32_t default_initialsize = 17;

        /**
        * Biggest possible modulo.
        */
        static const size32_t biggest_modulo = 2147483647;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * When resizing completes, a notification is issued against this
        * object.
        */
        FinalView<Object> f_vResizing;

        /**
        * The number of entries stored in the hash map, 0 <= n.
        */
        size32_t m_cEntries;

        /**
        * The array of hash buckets.  This field is declared volatile in
        * order to reduce synchronization.
        */
        MemberHandle<ObjectArray> m_haeBucket;

        /**
        * The capacity of the hash map (the point at which it must resize),
        *  1 <= n.
        */
        size32_t m_cCapacity;

        /**
        * The determining factor for the hash map capacity given a certain
        * number of buckets, such that capactiy = bucketcount * loadfactor.
        */
        float32_t m_flLoadFactor;

        /**
        * The rate of growth as a fraction of the current number of buckets,
        * 0 < n, such that the hash map grows to bucketcount * (1 +
        * growthrate)
        */
        float32_t m_flGrowthRate;

        /**
        * A count of the number of active Iterators on this map.
        */
        mutable NativeAtomic32 m_cIterators;


    // ----- friends --------------------------------------------------------

    friend class Entry;
    friend class EntrySet;
    friend class EntrySetIterator;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SAFE_HASH_MAP_HPP
