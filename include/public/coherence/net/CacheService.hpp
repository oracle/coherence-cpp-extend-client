/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_SERVICE_HPP
#define COH_CACHE_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::util::Collection;


/**
* A CacheService is a clustered service providing a collection of named Maps
* that hold resources shared among members of a cluster. These resources are
* expected to be managed in memory, and are typically composed of data that
* are also stored persistently in a database, or data that have been
* assembled or calculated at some significant cost, thus these resources are
* referred to as <i>cached</i>.
*
* @author jh  2007.12.21
*/
class COH_EXPORT CacheService
    : public interface_spec<CacheService,
        implements<Service> >
    {
    // ----- CacheService interface -----------------------------------------

    public:
        /**
        * Obtain a NamedCache interface that provides a view of resources
        * shared among members of a cluster. The view is identified by name
        * within this CacheService. Typically, repeated calls to this method
        * with the same view name and class loader instance will result in
        * the same view reference being returned.
        *
        * @param vsName  the name, within this CacheService, that uniquely
        *                identifies a view; null is legal, and may imply a
        *                default name
        *
        * @return a NamedCache interface which can be used to access the
        *         resources of the specified view
        *
        * @throws IllegalStateException thrown if the service is not
        *                                  running
        */
        virtual NamedCache::Handle ensureCache(String::View vsName) = 0;

        /**
        * Returns a Collection of String objects, one for each cache name
        * that has been previously registered with this CacheService.
        *
        * @return a Collection of cache names
        *
        * @throws IllegalStateException thrown if the CacheService
        *            is not running or has stopped
        */
        virtual Collection::View getCacheNames() const = 0;

        /**
        * Release local resources associated with the specified instance of
        * the cache. This invalidates a reference obtained by using the
        * {@link #ensureCache(String::View)} method.
        *
        * Releasing a Map reference to a cache makes the Map reference no
        * longer usable, but does not affect the cache itself. In other
        * words, all other references to the cache will still be valid, and
        * the cache data is not affected by releasing the reference.
        *
        * The reference that is released using this method can no longer be
        * used; any attempt to use the reference will result in an exception.
        *
        * The purpose for releasing a cache reference is to allow the cache
        * implementation to release resources.
        *
        * @param hMap  the cache object to be released
        *
        * @see NamedCache#release()
        */
        virtual void releaseCache(NamedCache::Handle hMap) = 0;

        /**
        * Release and destroy the specified cache.
        *
        * <b>Warning:</b> This method is used to completely destroy the
        * specified cache across the cluster. All references in the entire
        * cluster to this cache will be invalidated, the cached data will be
        * cleared, and all resources will be released.
        *
        * @param hMap  the cache object to be released
        *
        * @see NamedCache#destroy()
        */
        virtual void destroyCache(NamedCache::Handle hMap) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CACHE_SERVICE_HPP
