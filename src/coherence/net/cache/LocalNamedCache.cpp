/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/cache/LocalNamedCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

// ----- constructors -------------------------------------------------------

LocalNamedCache::LocalNamedCache(String::View vsName,
        bool                 fEnforceLocking,
        int64_t              cWaitMillis,
        int32_t              cUnits,
        int32_t              cExpiryMillis,
        float64_t            dflPruneLevel,
        size32_t             cInitialBuckets,
        float32_t            flLoadFactory,
        float32_t            flGrowthRate)
    : super(fEnforceLocking, cWaitMillis, cUnits, cExpiryMillis, dflPruneLevel,
            cInitialBuckets, flLoadFactory, flGrowthRate), f_vsName(self(), vsName),
            m_fIsActive(true)
    {
    }


// ----- NamedCache interface -----------------------------------------------

String::View LocalNamedCache::getCacheName() const
    {
    return f_vsName;
    }

void LocalNamedCache::setCacheName(String::View vsName)
    {
    if (NULL == f_vsName)
        {
        initialize(f_vsName, vsName);
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create("LocalCache cannot be renamed"));
        }
    }

CacheService::Handle LocalNamedCache::getCacheService()
    {
    return NULL;
    }

CacheService::View LocalNamedCache::getCacheService() const
    {
    return NULL;
    }

bool LocalNamedCache::isActive() const
    {
    return isValid();
    }

void LocalNamedCache::release()
    {
    inherited::release();
    invalidate();
    }

void LocalNamedCache::destroy()
    {
    clear();
    release();
    }

// ----- Describable interface ----------------------------------------------

String::View LocalNamedCache::getDescription() const
    {
    return COH_TO_STRING("Name=" << getCacheName() << ", " << super::getDescription());
    }

COH_CLOSE_NAMESPACE3
