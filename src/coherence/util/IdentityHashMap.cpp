/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/IdentityHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

IdentityHashMap::IdentityHashMap(size32_t cInitialBuckets, float32_t flLoadFactor,
        float32_t flGrowthRate)
    : super(cInitialBuckets, flLoadFactor, flGrowthRate)
    {
    }

IdentityHashMap::IdentityHashMap(const IdentityHashMap& that)
    : super(that)
    {
    }

// ----- SafeHashMap inerface -----------------------------------------------

SafeHashMap::Entry::Handle IdentityHashMap::instantiateEntry(Object::View vKey,
        Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash);
    }

SafeHashMap::Entry::Handle IdentityHashMap::instantiateEntry(
        SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(cast<IdentityHashMap::Entry::View>(vEntry));
    }

size32_t IdentityHashMap::getHashCode(Object::View vKey) const
    {
    return System::identityHashCode(vKey);
    }

// ----- inner class: Entry -------------------------------------------------

// ----- constructor ----------------------------------------------------

IdentityHashMap::Entry::Entry(const IdentityHashMap::Entry& that)
    : super(that)
    {
    }

IdentityHashMap::Entry::Entry(Entry::View vThat)
    : super(vThat)
    {
    }

IdentityHashMap::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash)
        : super(vKey, ohValue, nHash)
    {
    }

// ----- SafeHashMap::Entry interface ----------------------------------

bool IdentityHashMap::Entry::isKeyEqual(Object::View vKey) const
    {
    return f_vKey == vKey;
    }

COH_CLOSE_NAMESPACE2
