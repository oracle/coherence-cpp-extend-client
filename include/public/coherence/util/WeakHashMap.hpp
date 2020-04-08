/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_HASH_MAP_HPP
#define COH_WEAK_HASH_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/SafeHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* WeakHashMap is a HashMap implementation based on weak keys. That is the
* use an object as a key in the map will not prevent it from being destroyed
* if all other non-weak references to it are released.
*
* The time at which the entry associated with the weak key is cleaned up is
* not guaranteed, it may live in the map for some time. While it is not safe
* to assume that the map will shrink to its absolute minimum at any point, it
* is safe to assume that given constant random key insertions the size of the
* map will initially grow and then stabilize as the keys are reclaimed.
*
* Note: Iterating the entries may yield entries with reclaimed (NULL) keys.
*
* @author mf  2008.05.27
*/
class COH_EXPORT WeakHashMap
    : public cloneable_spec<WeakHashMap,
        extends<SafeHashMap> >
    {
    friend class factory<WeakHashMap>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a thread-safe weak hash map using the specified settings.
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
        WeakHashMap(size32_t cInitialBuckets = 17,
                float32_t flLoadFactor = 1.0F,
                float32_t flGrowthRate = 3.0F);

        /**
        * Copy constructor.
        */
        WeakHashMap(const WeakHashMap& that);


    // ----- SafeHashMap interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(Object::View vKey,
                Object::Holder ohValue, size32_t nHash);

        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(
                SafeHashMap::Entry::View vThat);

        /**
        * {@inheritDoc}
        */
        virtual void grow();


    // ----- WeakHashMap interface ------------------------------------------

    public:
        /**
         * Cleanup any "garbage" entries in the map.
         */
        virtual void compact();

    protected:
        /**
        * Shrink the specified bucket.
        *
        * @param iBucket  the bucket to shrink
        * @param cEntries  the maximum number of entries to shrink by
        *
        * @return  the number of entries removed
        */
        virtual size32_t shrink(size32_t iBucket, size32_t cEntries);


    // ----- inner class: Entry ---------------------------------------------

    protected:
        /**
        * A map entry (key-value pair).  The <tt>Map.entrySet</tt> method
        * returns a collection-view of the map, whose elements are of this
        * class.
        *
        * The stored key is always a WeakReference to the user supplied key.
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<SafeHashMap::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Construct a thread-safe weak hash map using the specified
                * settings.
                *
                * @param cInitialBuckets  the initial number of hash buckets,
                *                         0 &lt; n
                * @param flLoadFactor     the acceptable load factor before
                *                         resizing occurs, 0 &lt; n, such
                *                         that a load factor of 1.0 causes
                *                         resizing when the number of entries
                *                         exceeds the number of buckets
                * @param flGrowthRate     the rate of bucket growth when a
                *                         resize occurs, 0 &lt; n, such that
                *                         a growth rate of 1.0 will double
                *                         the number of buckets:
                *                         bucketcount =
                *                             bucketcount * (1 + growthrate)
                */
                Entry(Object::View vKey, Object::Holder ohValue,
                        size32_t nHash);

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

            // ----- WeakHashMap::Entry interface -----------------------

            public:
                /**
                * Return true the entry references a key which is still
                * valid, i.e. has not been reclaimed.
                *
                * @return true if the key is still valid
                */
                virtual bool isValid() const;

            // ----- SafeHashMap::Entry interface -----------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual bool isKeyEqual(Object::View vKey) const;

            // ----- Map::Entry interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

            // ----- friends --------------------------------------------

            friend class WeakHashMap;
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_HASH_MAP_HPP
