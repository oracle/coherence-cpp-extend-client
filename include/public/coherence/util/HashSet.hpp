/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_HASH_SET_HPP
#define COH_HASH_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/MappedSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Set implementation which is backed by a HashMap.
*
* @author mf  2008.02.25
*/
class COH_EXPORT HashSet
    : public cloneable_spec<HashSet,
        extends<MappedSet> >
    {
    friend class factory<HashSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a hash set using the specified settings.
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
        HashSet(size32_t  cInitialBuckets = default_initial_buckets,
                float32_t flLoadFactor    = getDefaultLoadFactor(),
                float32_t flGrowthRate    = getDefaultGrowthRate());

        /**
        * Constructs a new set containing the elements in the specified
        * Collection. The HashSet is created with default load factor (1.0)
        * and an initial capacity sufficient to contain the elements in the
        * specified collection.
        *
        * @param vCol  the collection whose elements are to be placed into this set
        *
        * @since Coherence 12.1.2
        */
        HashSet(Collection::View vCol);

        /**
        * Copy constructor.
        */
        HashSet(const HashSet& that);


    // ----- constants ---------------------------------------------------

    public:
        /**
        * The default value for initial number of hash buckets.
        */
        static const size32_t default_initial_buckets = 17;

        /**
        * The default acceptable load factor before resizing occurs (1.0F).
        */
        static float32_t getDefaultLoadFactor();

        /**
        * The default rate of bucket growth when a resize occurs (3.0F).
        */
        static float32_t getDefaultGrowthRate();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_HASH_SET
