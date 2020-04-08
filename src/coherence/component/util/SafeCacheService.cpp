/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/SafeCacheService.hpp"


#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::internal::ScopedReferenceStore;
using coherence::security::SecurityHelper;


// ----- constructors/destructor  -------------------------------------------

SafeCacheService::SafeCacheService()
    : f_hStoreNamedCache(self())
    {
    }


// ----- CacheService interface ---------------------------------------------

NamedCache::Handle SafeCacheService::ensureCache(String::View vsName)
    {
    if (NULL == vsName || vsName->length() == 0)
        {
        vsName = String::create("Default");
        }

    ScopedReferenceStore::Handle hNamedCacheStore = f_hStoreNamedCache;
    SafeNamedCache::Handle       hSafeCache       = cast<SafeNamedCache::Handle>(
            hNamedCacheStore->getCache(vsName));

    if (NULL == hSafeCache)
        {
        COH_SYNCHRONIZED(hNamedCacheStore)
            {
            NamedCache::Handle hNamedCache = ensureRunningCacheService(
                    true)->ensureCache(vsName);

            hSafeCache = SafeNamedCache::create();

            hSafeCache->setSafeCacheService(this);
            hSafeCache->setCacheName(vsName);
            hSafeCache->setNamedCache(hNamedCache);
            hSafeCache->setSubject(SecurityHelper::getCurrentSubject());

            hNamedCacheStore->putCache(hSafeCache);
            }
        }

    return hSafeCache;
    }

Collection::View SafeCacheService::getCacheNames() const
    {
    return ensureRunningCacheServiceInternal(true)->getCacheNames();
    }

void SafeCacheService::releaseCache(NamedCache::Handle hNamedCache)
    {
    SafeNamedCache::Handle hSafeCache =
            cast<SafeNamedCache::Handle>(hNamedCache);

    removeCacheReference(hSafeCache);

    CacheService::Handle hCacheService =
            cast<CacheService::Handle>(m_hService);
    try
        {
        NamedCache::Handle hWrappedCache = hSafeCache->getNamedCache();
        if (NULL != hWrappedCache)
            {
            hCacheService->releaseCache(hWrappedCache);
            }
        }
    catch (...)
        {
        if (NULL != hCacheService && hCacheService->isRunning())
            {
            throw;
            }
        }
    }

void SafeCacheService::destroyCache(NamedCache::Handle hNamedCache)
    {
    SafeNamedCache::Handle hSafeCache =
            cast<SafeNamedCache::Handle>(hNamedCache);

    removeCacheReference(hSafeCache);

    CacheService::Handle hCacheService =
            cast<CacheService::Handle>(m_hService);
    try
        {
        NamedCache::Handle hWrappedCache = hSafeCache->getNamedCache();
        if (NULL == hWrappedCache)
            {
            COH_THROW (IllegalStateException::create(
                    "Cache is already released"));
            }
        else
            {
            hCacheService->destroyCache(hWrappedCache);
            }
        }
    catch (...)
        {
        if (NULL != hCacheService && hCacheService->isRunning())
            {
            throw;
            }
        }
    }


// ----- SafeService interface ----------------------------------------------

void SafeCacheService::cleanup()
    {
    SafeService::cleanup();
    f_hStoreNamedCache->clear();
    }

void SafeCacheService::setOperationalContext(OperationalContext::View vContext)
    {
    SafeService::setOperationalContext(vContext);
    initialize(f_hStoreNamedCache, ScopedReferenceStore::create(vContext));
    }


// ----- SafeCacheService interface -----------------------------------------

CacheService::Handle SafeCacheService::ensureRunningCacheService(bool fDrain)
    {
    return ensureRunningCacheServiceInternal(fDrain);
    }

CacheService::View SafeCacheService::ensureRunningCacheService(
        bool fDrain) const
    {
    return ensureRunningCacheServiceInternal(fDrain);
    }


// ----- helper methods -----------------------------------------------------

void SafeCacheService::removeCacheReference(NamedCache::Handle hNamedCache)
    {
    SafeNamedCache::Handle hSafeCache =
            cast<SafeNamedCache::Handle>(hNamedCache);

    hSafeCache->setReleased(true);

    String::View                 vsName           = hSafeCache->getCacheName();
    ScopedReferenceStore::Handle hNamedCacheStore = f_hStoreNamedCache;

    if (NULL != hNamedCacheStore)
        {
        COH_SYNCHRONIZED(hNamedCacheStore)
            {
            hNamedCacheStore->remove(vsName);
            }
        }
    }

CacheService::Handle SafeCacheService::ensureRunningCacheServiceInternal(
        bool fDrain) const
    {
    return cast<CacheService::Handle>(ensureRunningServiceInternal(fDrain));
    }


// ----- property getters/settters ------------------------------------------

CacheService::View SafeCacheService::getCacheService() const
    {
    return cast<CacheService::View>(getService());
    }

void SafeCacheService::setNamedCacheStore(ScopedReferenceStore::Handle
        hNamedCacheStore)
    {
    initialize(f_hStoreNamedCache, hNamedCacheStore);
    }

ScopedReferenceStore::View SafeCacheService::getNamedCacheStore()
    {
    return f_hStoreNamedCache;
    }

COH_CLOSE_NAMESPACE3
