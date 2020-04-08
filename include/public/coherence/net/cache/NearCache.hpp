/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NEAR_CACHE_HPP
#define COH_NEAR_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/CachingMap.hpp"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::CacheService;
using coherence::net::MemberListener;
using coherence::net::NamedCache;
using coherence::util::Comparator;
using coherence::util::ValueExtractor;


/**
* A "near cache" is a CachingMap whose front map is a size-limited and/or
* auto-expiring local cache, and whose back map is a distributed cache.
*
* (A CachingMap is a map that has a "front" map and a "back" map; the front
* map is assumed to be low latency but incomplete, and the back map is
* assumed to be complete but high latency.)
*
* @author tb  2008.07.02
*/
class COH_EXPORT NearCache
    : public class_spec<NearCache,
        extends<CachingMap>,
        implements<NamedCache> >
    {
    friend class factory<NearCache>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a NearCache, using a <i>mapBack</i> NamedCache as the
        * complete (back) storage and <i>mapFront</i> Map as a near (front)
        * storage using the listen_auto invalidation strategy.
        *
        * @param hMapFront    map to put in front of the back cache
        * @param hMapBack     NamedCache to put behind the front cache
        * @param strategy     specifies the strategy used for the front map
        *                     invalidation
        * @param hMapControl  map to keep track of front map updates
        */
        NearCache(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
                CachingMap::InvalidationStrategy strategy = listen_auto,
                ConcurrentMap::Handle hMapControl = NULL);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the NamedCache object that sits behind this NearCache.
        *
        * @return the NamedCache object, which is the back map of this
        *         NearCache
        *
        * @throws IllegalStateException if this NamedCache has been released
        */
        virtual NamedCache::Handle getBackCache();

        /**
        * Obtain the NamedCache object that sits behind this NearCache.
        *
        * @return the NamedCache object, which is the back map of this
        *         NearCache
        *
        * @throws IllegalStateException if this NamedCache has been released
        */
        virtual NamedCache::View getBackCache() const;


    // ----- CachingMap interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        using CachingMap::put;

        /**
        * {@inheritDoc}
        */
        using CachingMap::keySet;

        /**
        * {@inheritDoc}
        */
        using CachingMap::entrySet;


    // ----- NamedCache interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getCacheName() const;

        /**
        * {@inheritDoc}
        */
        virtual NamedCache::CacheServiceHandle getCacheService();

        /**
        * {@inheritDoc}
        */
        virtual NamedCache::CacheServiceView getCacheService() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isActive() const;

        /**
        * {@inheritDoc}
        */
        virtual void release();

        /**
        * {@inheritDoc}
        */
        virtual void destroy();

        /**
        * {@inheritDoc}
        */
        virtual void truncate();


    // ----- ObservableMap interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void addKeyListener(MapListener::Handle hListener,
                Object::View vKey, bool fLite);

        /**
        * {@inheritDoc}
        */
        virtual void removeKeyListener(MapListener::Handle hListener,
                Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void addMapListener(MapListener::Handle hListener);
        
        /**
        * {@inheritDoc}
        */
        virtual void removeMapListener(MapListener::Handle hListener);
        
        /**
        * {@inheritDoc}
        */
        virtual void addFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL, bool fLite = false);

        /**
        * {@inheritDoc}
        */
        virtual void removeFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL);


    // ----- CacheMap interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View getAll(Collection::View vColKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey,
                Object::Holder ohValue, int64_t cMillis);


    // ----- ConcurrentMap interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool lock(Object::View vKey, int64_t cWait) const;

        /**
        * {@inheritDoc}
        */
        virtual bool lock(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual bool unlock(Object::View vKey) const;


    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Set::View keySet(Filter::View vFilter) const;

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet(Filter::View vFilter) const;

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet(Filter::View vFilter,
                Comparator::View vComparator) const;

        /**
        * {@inheritDoc}
        */
        virtual void addIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator);

        /**
        * {@inheritDoc}
        */
        virtual void removeIndex(ValueExtractor::View vExtractor);


    // ----- InvocableMap interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder invoke(Object::View vKey,
                EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Collection::View vCollKeys,
                EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Filter::View vFilter,
                EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Collection::View vCollKeys,
                EntryAggregator::Handle hAgent) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Filter::View vFilter,
                EntryAggregator::Handle hAgent) const;


    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- internal helpers -----------------------------------------------

    protected:
        /**
        * Release this cache, optionally destroying it.
        *
        * @param fDestroy  true to destroy the cache as well
        */
        virtual void release(bool fDestroy);

        /**
        * Instantiate and register a MemberListener with the back cache's
        * service.
        *
        * The primary goal of that listener is invalidation of the front map
        * in case of the service [automatic] restart.
        *
        * @return the instantiated and registered MemberListener object
        */
        virtual MemberListener::Handle registerBackServiceListener();

        /**
        * Unregister back caches's service member listener.
        */
        virtual void unregisterBackServiceListener();


    // ----- data fields ----------------------------------------------------

    protected:
        /**
        * Cache name
        */
        FinalView<String> f_vsName;

        /**
        * The back cache service.
        */
        FinalHandle<CacheService> f_hService;

        /**
        * The back cache service MemberListener.
        */
        FinalHandle<MemberListener> f_hListenerBackService;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_NEAR_CACHE_HPP
