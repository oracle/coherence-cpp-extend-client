/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/cache/LocalQueryCache.hpp"

#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::InvocableMapHelper;
using coherence::util::SafeHashMap;


// ----- constructors -------------------------------------------------------

LocalQueryCache::LocalQueryCache(
        bool                 fEnforceLocking,
        int64_t              cWaitMillis,
        int32_t              cUnits,
        int32_t              cExpiryMillis,
        float64_t            dflPruneLevel,
        size32_t             cInitialBuckets,
        float32_t            flLoadFactory,
        float32_t            flGrowthRate)
        : super(fEnforceLocking, cWaitMillis, cUnits, cExpiryMillis,
            dflPruneLevel, cInitialBuckets, flLoadFactory, flGrowthRate),
          f_hMapIndex(self(), SafeHashMap::create())
    {
    }


// ----- QueryMap interface -------------------------------------------------


Set::View LocalQueryCache::keySet(Filter::View vFilter) const
    {
    return InvocableMapHelper::query(this, getIndexMap(), vFilter, false,
            false, NULL);
    }

Set::View LocalQueryCache::entrySet(Filter::View vFilter) const
    {
    return InvocableMapHelper::query(this, getIndexMap(), vFilter, true,
            false, NULL);
    }

Set::View LocalQueryCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return InvocableMapHelper::query(this, getIndexMap(), vFilter, true,
            true, vComparator);
    }

void LocalQueryCache::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    InvocableMapHelper::addIndex(vExtractor, fOrdered, vComparator, this,
            ensureIndexMap());
    }

void LocalQueryCache::removeIndex(ValueExtractor::View vExtractor)
    {
    InvocableMapHelper::removeIndex(vExtractor, this, getIndexMap());
    }


// ----- helper methods -----------------------------------------------------

Map::Handle LocalQueryCache::ensureIndexMap()
    {
    return getIndexMap();
    }

Map::View LocalQueryCache::getIndexMap() const
    {
    return f_hMapIndex;
    }

Map::Handle LocalQueryCache::getIndexMap()
    {
    return f_hMapIndex;
    }

COH_CLOSE_NAMESPACE3
