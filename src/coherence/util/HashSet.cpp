/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/HashSet.hpp"

#include "coherence/util/HashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(HashSet);


// ----- constructors -------------------------------------------------------

HashSet::HashSet(size32_t cInitialBuckets, float32_t flLoadFactor,
        float32_t flGrowthRate)
    : super(cast<Map::Handle>(HashMap::create(cInitialBuckets, flLoadFactor, flGrowthRate)))
    {
    }

HashSet::HashSet(Collection::View vCol)
    : super(cast<Map::Handle>(HashMap::create(vCol->size(), getDefaultLoadFactor(), getDefaultGrowthRate())))
    {
    addAll(vCol);
    }

HashSet::HashSet(const HashSet& that)
    : super(that)
    {
    }

// ----- constants ----------------------------------------------------------

float32_t HashSet::getDefaultLoadFactor()
    {
    static FinalView<Float32> vLoadFactor(System::common(), Float32::create(1.0F));
    return vLoadFactor->getFloat32Value();
    }
COH_STATIC_INIT(HashSet::getDefaultLoadFactor());

float32_t HashSet::getDefaultGrowthRate()
    {
    static FinalView<Float32> vGrowthRate(System::common(), Float32::create(3.0F));
    return vGrowthRate->getFloat32Value();
    }
COH_STATIC_INIT(HashSet::getDefaultGrowthRate());

COH_CLOSE_NAMESPACE2
