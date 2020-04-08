/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SafeHashSet.hpp"

#include "coherence/util/SafeHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(SafeHashSet);

// ----- constructors -------------------------------------------------------

SafeHashSet::SafeHashSet()
    : super(cast<Map::Handle>(SafeHashMap::create()))
    {
    }

SafeHashSet::SafeHashSet(size32_t cInitialBuckets, float32_t flLoadFactor,
    float32_t flGrowthRate)
    : super(cast<Map::Handle>(SafeHashMap::create(cInitialBuckets, flLoadFactor, flGrowthRate)))
    {
    }

SafeHashSet::SafeHashSet(const SafeHashSet& that)
    : super(that)
    {
    }

COH_CLOSE_NAMESPACE2
