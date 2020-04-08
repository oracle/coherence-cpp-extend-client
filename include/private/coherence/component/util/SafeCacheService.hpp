/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_CACHE_SERVICE_HPP
#define COH_SAFE_CACHE_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/util/SafeService.hpp"
#include "private/coherence/net/internal/ScopedReferenceStore.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::net::OperationalContext;
using coherence::net::internal::ScopedReferenceStore;
using coherence::util::Collection;
using coherence::util::Map;


/**
* Safe wrapper for RemoteCacheService.
*
* @author js  2008.02.25
*/
class COH_EXPORT SafeCacheService
    : public class_spec<SafeCacheService,
        extends<SafeService>,
        implements<CacheService> >
    {
    friend class factory<SafeCacheService>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a SafeCacheService.
        */
        SafeCacheService();


    // ----- CacheService interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual NamedCache::Handle ensureCache(String::View vsName);

        /**
        * {@inheritDoc}
        */
        virtual Collection::View getCacheNames() const;

        /**
        * {@inheritDoc}
        */
        virtual void releaseCache(NamedCache::Handle hNamedCache);

        /**
        * {@inheritDoc}
        */
        virtual void destroyCache(NamedCache::Handle hNamedCache);


    // ----- SafeService interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void cleanup();

        /**
        * {@inheritDoc}
        */
        virtual void setOperationalContext(
                OperationalContext::View vContext);


    // ----- SafeCacheService interface -------------------------------------

    public:
        /**
        * Ensures that the returned CacheService is running before returning
        * it. If the CacheService is not running and has not been explicitly
        * stopped, theCacheService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped CacheService
        */
        virtual CacheService::Handle ensureRunningCacheService(
                bool fDrain);

        /**
        * Ensures that the returned CacheService is running before returning
        * it. If the CacheService is not running and has not been explicitly
        * stopped, theCacheService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped CacheService
        */
        virtual CacheService::View ensureRunningCacheService(
                bool fDrain) const;


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Removes a NamedCache from the named cache map.
        *
        * @param hNamedCache  the NamedCache to remove
        */
        virtual void removeCacheReference(NamedCache::Handle hNamedCache);

        /**
        * Ensures that the returned CacheService is running before returning
        * it. If the CacheService is not running and has not been explicitly
        * stopped, theCacheService is restarted.
        *
        * @param fDrain  if true, the calling thread will be blocked until
        *                the wrapped CacheService event dispatcher queue is
        *                empty and all outstanding tasks have been executed
        *
        * @return the running wrapped CacheService
        */
        virtual CacheService::Handle ensureRunningCacheServiceInternal(
                bool fDrain) const;


    // ----- property getters/setters ---------------------------------------

    public:
        /**
        * Getter for the cacheService property.
        *
        * @return the wrapped CacheService implementation
        */
        virtual CacheService::View getCacheService() const;

        /**
        * Setter for the namedCacheStore property.
        *
        * @param hNamedCacheStore  store that holds cache references by name
        *                          and optionally, if configured, Subject
        */
        virtual void setNamedCacheStore(ScopedReferenceStore::Handle
                hNamedCacheStore);

        /**
        * Getter for the namedCacheStore property.
        *
        * @return the store that holds cache references by name and
        * optionally, if configured, Subject.
       */
        virtual ScopedReferenceStore::View getNamedCacheStore();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Store that holds cache references by name and optionally,
        * if configured, Subject.
        */
        FinalHandle<ScopedReferenceStore> f_hStoreNamedCache;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SAFE_CACHE_SERVICE_HPP
