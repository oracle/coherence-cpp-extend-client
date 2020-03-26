/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMED_CACHE_HPP
#define COH_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/ViewBuilder.hpp"

#include "coherence/net/cache/CacheMap.hpp"

#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/QueryMap.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::net::cache::CacheMap;
using coherence::util::ConcurrentMap;
using coherence::util::InvocableMap;
using coherence::util::ObservableMap;
using coherence::util::QueryMap;

class CacheService;

/**
* A NamedCache is a Map that holds resources shared among members of a
* cluster. These resources are expected to be managed in memory, and are
* typically composed of data that are also stored persistently in a database,
* or data that have been assembled or calculated at some significant cost,
* thus these resources are referred to as <i>cached</i>.
*
* @see coherence::net::cache::CacheMap
* @see coherence::util::ConcurrentMap
* @see coherence::util::InvocableMap
* @see coherence::util::ObservableMap
* @see coherence::util::QueryMap
*
* @author jh  2008.03.05
*/
class COH_EXPORT NamedCache
    : public interface_spec<NamedCache,
        implements<CacheMap, ConcurrentMap, InvocableMap, QueryMap,
                   ObservableMap> >
    {
    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * CacheService Handle definition.
        */
        typedef TypedHandle<CacheService> CacheServiceHandle;

        /**
        * CacheService View definition.
        */
        typedef TypedHandle<const CacheService> CacheServiceView;


    // ----- NamedCache interface -------------------------------------------

    public:
        /**
        * Return the cache name.
        *
        * @return the cache name
        */
        virtual String::View getCacheName() const = 0;

        /**
        * Return the CacheService that this NamedCache is a part of.
        *
        * @return the CacheService
        */
        virtual CacheServiceHandle getCacheService() = 0;

        /**
        * Return the CacheService that this NamedCache is a part of.
        *
        * @return the CacheService
        */
        virtual CacheServiceView getCacheService() const = 0;

        /**
        * Specifies whether or not the NamedCache is active.
        *
        * @return true if the NamedCache is active; false otherwise
        */
        virtual bool isActive() const = 0;

        /**
        * Release local resources associated with this instance of
        * NamedCache.
        *
        * Releasing a cache makes it no longer usable, but does not affect
        * the cache itself. In other words, all other references to the cache
        * will still be valid, and the cache data is not affected by
        * releasing the reference. Any attempt to use this reference
        * afterward will result in an exception.
        *
        * Caches should be released by the same mechanism in which they were
        * obtained. For example:
        * <ul>
        *  <li> Cache::create() - cache->release()</li>
        *  <li> CacheFactory::getCache() - CacheFactory::releaseCache()</li>
        *  <li> ConfigurableCacheFactory::ensureCache() - ConfigurableCacheFactory::releaseCache()</li>
        * </ul>
        * Except for the case where the application code explicitly allocated
        * the cache, this method should not be called by application code.
        *
        * @see CacheFactory#releaseCache
        * @see ConfigurableCacheFactory#releaseCache
        */
        virtual void release() = 0;

        /**
        * Release and destroy this instance of NamedCache.
        *
        * <b>Warning:</b> This method is used to completely destroy the
        * specified cache across the cluster. All references in the entire
        * cluster to this cache will be invalidated, the cached data will be
        * cleared, and all resources will be released.
        *
        * Caches should be destroyed by the same mechanism in which they were
        * obtained. For example:
        * <ul>
        *  <li> Cache::create() - cache->destroy()</li>
        *  <li> CacheFactory::getCache() - CacheFactory::destroyCache()</li>
        *  <li> ConfigurableCacheFactory::ensureCache() - ConfigurableCacheFactory::destroyCache()</li>
        * </ul>
        * Except for the case where the application code explicitly allocated
        * the cache, this method should not be called by application code.
        *
        * @see CacheService#destroyCache
        */
        virtual void destroy() = 0;

        /**
        * Removes all mappings from this map.
        * <p>
        * Note: the removal of entries caused by this truncate operation will
        * not be observable. This includes any registered {@link MapListener
        * listeners}, {@link MapTrigger triggers}, or interceptors. However, a
        * CacheLifecycleEvent is raised to notify subscribers of the execution
        * of this operation.
        *
        * @throws UnsupportedOperationException if the server does not support the truncate operation
        */
        virtual void truncate()
            {
            COH_THROW (UnsupportedOperationException::create());
            }

        /**
        * Construct a view of this NamedCache.
        *
        * @return a local view for this NamedCache
        *
        * @see ViewBuilder
        *
        * @since 12.2.1.4
        */
        virtual ViewBuilder::Handle view()
            {
            return ViewBuilder::create(this);
            }
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NAMED_CACHE_HPP
