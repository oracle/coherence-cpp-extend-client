/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_HASH_SET_HPP
#define COH_SAFE_HASH_SET_HPP

#include "coherence/lang.ns"

#include "coherence/util/MappedSet.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

/**
 * A thread-safe Set implementation which is backed by a SafeHashMap.
 *
 * @author lh  2012.08.23
 * @since Coherence 12.1.2
 */
class COH_EXPORT SafeHashSet
    : public cloneable_spec<SafeHashSet,
        extends<MappedSet> >
    {
    friend class factory<SafeHashSet>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
         * Default constructor.
         */
        SafeHashSet();

        /**
         * Construct a thread-safe hash set using the specified settings.
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
        SafeHashSet(size32_t cInitialBuckets, float32_t flLoadFactor,
            float32_t flGrowthRate);

        /**
         * Copy constructor.
         */
        SafeHashSet(const SafeHashSet& that);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SAFE_HASH_SET
