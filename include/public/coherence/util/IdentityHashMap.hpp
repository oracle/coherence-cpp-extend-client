/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_IDENTITY_HASH_MAP_HPP
#define COH_IDENTITY_HASH_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/SafeHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* IdentityHashMap is a HashMap implementation where key equality is based
* on reference rather then value equality.
*
* @author mf  2008.05.27
*/
class COH_EXPORT IdentityHashMap
    : public cloneable_spec<IdentityHashMap,
        extends<SafeHashMap> >
    {
    friend class factory<IdentityHashMap>;

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
        IdentityHashMap(size32_t cInitialBuckets = 17,
                float32_t flLoadFactor = 1.0F,
                float32_t flGrowthRate = 3.0F);

        /**
        * Copy Constructor.
        */
        IdentityHashMap(const IdentityHashMap& that);


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
                SafeHashMap::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual size32_t getHashCode(Object::View vKey) const;

    // ----- inner class: Entry ---------------------------------------------

    protected:
        /**
        * A Map::Entry where key equality is based on reference equality.
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<SafeHashMap::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructors -------------------------------------

            protected:
                /**
                * Create a new Map::Entry.
                *
                * @param vKey     the associated key
                * @param ohValue  the associated value
                * @param hHash    the associated hash code
                *
                * @return a new Map::Entry
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

            // ----- SafeHashMap::Entry interface -----------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual bool isKeyEqual(Object::View vKey) const;
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_IDENTITY_HASH_MAP_HPP
