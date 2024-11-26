/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTINUOUS_QUERY_CACHE_HPP
#define COH_CONTINUOUS_QUERY_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/util/AbstractMap.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/Supplier.hpp"
#include "coherence/util/ValueExtractor.hpp"

#include "coherence/util/filter/MapEventFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::CacheService;
using coherence::net::NamedCache;
using coherence::util::AbstractMap;
using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::MapEvent;
using coherence::util::MapIndex;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::MultiplexingMapListener;
using coherence::util::Set;
using coherence::util::Supplier;
using coherence::util::ValueExtractor;
using coherence::util::filter::MapEventFilter;

/**
* Create a materialized view of a NamedCache using the Coherence
* <i>Continuous Query</i> capability.
*
* @author tb, js  2008.07.23
*/
class COH_EXPORT ContinuousQueryCache
    : public class_spec<ContinuousQueryCache,
        extends<AbstractMap>,
        implements<NamedCache> >
    {
    friend class factory<ContinuousQueryCache>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a materialized view of a NamedCache using a Filter. A
        * materialized view is an implementation of <i>Continuous Query</i>
        * exposed through the standard NamedCache API. This constructor
        * allows a client to receive all events, including those that result
        * from the initial population of the ContinuousQueryCache. In other
        * words, all contents of the ContinuousQueryCache will be delivered
        * to the listener as a sequence of events, including those items that
        * already exist in the underlying (unfiltered) cache.
        *
        * @param hCache        the NamedCache to create a view of
        * @param vFilter       the Filter that defines the view
        * @param fCacheValues  pass true to cache both the keys and values of
        *                      the materialized view locally, or false to
        *                      only cache the keys. Override of false described
        *                      in isCacheValues()
        * @param hListener     an initial MapListener that will receive all
        *                      the events from the ContinuousQueryCache,
        *                      including those corresponding to its initial
        *                      population
        * @param vTransformer  the transformer that should be used to convert
        *                      values from the underlying cache before storing
        *                      them locally
        * Note: When parameter fCacheValues is false, it is inferred that provided parameter
        * listener is a lite listener as described by fLite parameter of
        * addMapListener(MapListener, Filter, boolean).
        */
        ContinuousQueryCache(NamedCache::Handle hCache,
                Filter::View vFilter, bool fCacheValues = false,
                MapListener::Handle  hListener    = NULL,
                ValueExtractor::View vTransformer = NULL);

        /**
        * Create a materialized view of a NamedCache using a Filter. A
        * materialized view is an implementation of <i>Continuous Query</i>
        * exposed through the standard NamedCache API. This constructor
        * allows a client to receive all events, including those that result
        * from the initial population of the ContinuousQueryCache. In other
        * words, all contents of the ContinuousQueryCache will be delivered
        * to the listener as a sequence of events, including those items that
        * already exist in the underlying (unfiltered) cache.
        * 
        * @param vCacheSupplier  the Supplier returning the NamedCache to create
        *                        a view of
        * @param vFilter         the Filter that defines the view
        * @param fCacheValues    pass true to cache both the keys and values of
        *                        the materialized view locally, or false to
        *                        only cache the keys. Override of false described
        *                        in isCacheValues()
        * @param hListener       an initial MapListener that will receive all
        *                        the events from the ContinuousQueryCache,
        *                        including those corresponding to its initial
        *                        population
        * @param vTransformer    the transformer that should be used to convert
        *                        values from the underlying cache before storing
        *                        them locally
        * Note: When parameter fCacheValues is false, it is inferred that provided parameter
        * listener is a lite listener as described by fLite parameter of
        * addMapListener(MapListener, Filter, boolean).
        */
        ContinuousQueryCache(Supplier::View vCacheSupplier,
                Filter::View vFilter, bool fCacheValues,
                MapListener::Handle  hListener,
                ValueExtractor::View vTransformer);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the NamedCache that this ContinuousQueryCache is based on.
        *
        * @return the underlying NamedCache
        */
        virtual NamedCache::View getCache() const;

        /**
        * Obtain the NamedCache that this ContinuousQueryCache is based on.
        *
        * @return the underlying NamedCache
        */
        virtual NamedCache::Handle getCache();

        /**
        * Obtain the Filter that this ContinuousQueryCache is using to query
        * the underlying NamedCache.
        *
        * @return the Filter that this cache uses to select its contents from
        *         the underlying NamedCache
        */
        virtual Filter::View getFilter() const;

        /**
        * Obtain the transformer that this ContinuousQueryCache is using to transform
        * the results from the underlying cache.
        *
        * @return the ValueExtractor that this cache uses to transform entries from
        *         the underlying cache
        *
        * @since 12.2.1.4
        */
        virtual ValueExtractor::View getTransformer() const;

       /**
        * Obtain the MapListener provided at construction time for
        * this ContinuousQueryCache.
        *
        * @return the MapListener provided at construction time for
        *         this ContinuousQueryCache
        *
        * @since 12.2.1.4
        */
        virtual MapListener::View getMapListener() const;

        /**
        * Determine if this ContinuousQueryCache caches values locally.
        *
        * @return true if this object caches values locally, and false if it
        *        relies on the underlying NamedCache
        * 
        * Note: if addMapListener(MapListener, Filter, boolean) adds a standard 
        * (non-lite) listener or a filter to this ObservableMap, cache values are 
        * always maintained locally. The locally cached values are used to filter events
        * and to supply the old and new values for the events that it raises.
        * Additionally, a non-null transformer infers caches values being stored locally.
        */
        virtual bool isCacheValues() const;

        /**
        * Modify the local-caching option for the ContinuousQueryCache. By
        * changing this value from <tt>false</tt> to <tt>true</tt>, the
        * ContinuousQueryCache will fully realize its contents locally and
        * maintain them coherently in a manner analogous to the Coherence
        * NearCache. By changing this value from <tt>true</tt> to
        * <tt>false</tt>, the ContinuousQueryCache will discard its locally
        * cached data and rely on the underlying NamedCache.
        *
        * @param fCacheValues pass true to enable local caching, or false to
        *                     disable it.  Override of false described 
        *                     in isCacheValues()
        */
        virtual void setCacheValues(bool fCacheValues);

        /**
        * Determine if this ContinuousQueryCache disallows data modification
        * operations.
        *
        * @return true if this ContinuousQueryCache has been configured as
        *         read-only
        */
        virtual bool isReadOnly() const;

        /**
        * Modify the read-only option for the ContinuousQueryCache. Note that
        * the cache can be made read-only, but the opposite (making it
        * mutable) is explicitly disallowed.
        *
        * @param fReadOnly  pass true to prohibit clients from making
        *                   modifications to this cache
        */
        virtual void setReadOnly(bool fReadOnly);

        /**
        * Obtain the state of the ContinuousQueryCache.
        *
        * @return  one of the STATE_ enums
        */
        virtual int32_t getState() const;

        /**
        * Return the reconnection interval (in milliseconds). This value indicates the period
        * in which re-synchronization with the underlying cache will be delayed in the case the
        * connection is severed.  During this time period, local content can be accessed without
        * triggering re-synchronization of the local content.
        *
        * @return a reconnection interval (in milliseconds)
        */
        virtual int64_t getReconnectInterval() const;

        /**
        * Specify the reconnection interval (in milliseconds). This value indicates the period
        * in which re-synchronization with the underlying cache will be delayed in the case the
        * connection is severed.  During this time period, local content can be accessed without
        * triggering re-synchronization of the local content.
        *
        * @param cReconnectMillis  reconnection interval (in milliseconds). A value of zero
        *                          or less means that the ContinuousQueryCache cannot
        *                          be used when not connected.
        */
        virtual void setReconnectInterval(int64_t cReconnectMillis);

        /**
        * Return the Supplier used to provide the name of this ContinuousQueryCache.
        *
        * @return the Supplier used to provide the name of this ContinuousQueryCache
        *
        * @since 12.2.1.4
        */
        virtual Supplier::View getCacheNameSupplier() const;

        /**
        * Provide the Supplier used to provide the name of this ContinuousQueryCache.
        *
        * @param vCacheNameSupplier the Supplier used to provide the name of this ContinuousQueryCache
        *
        * @since 12.2.1.4
        */
        virtual void setCacheNameSupplier(Supplier::View vCacheNameSupplier);

    protected:
        /**
        * Create the internal cache used by the ContinuousQueryCache.
        *
        * @return a new ObservableMap that will represent the materialized
        *         view of the ContinuousQueryCache
        */
        virtual ObservableMap::Handle instantiateInternalCache() const;

        /**
        * Obtain a reference to the internal cache. The internal cache
        * maintains all of the keys in the ContinuousQueryCache, and if
        * isCacheValues()is true, it also maintains the up-to-date values
        * corresponding to those keys.
        *
        * @return the internal cache that represents the materialized view of
        *         the ContinuousQueryCache
        */
        virtual ObservableMap::View getInternalCache() const;

        /**
        * Obtain a reference to the internal cache. The internal cache
        * maintains all of the keys in the ContinuousQueryCache, and if
        * isCacheValues()is true, it also maintains the up-to-date values
        * corresponding to those keys.
        *
        * @return the internal cache that represents the materialized view of
        *         the ContinuousQueryCache
        */
        virtual ObservableMap::Handle getInternalCache();

        /**
        * Determine if the ContinuousQueryCache has any listeners that cannot
        * be served by this Map listening to lite events.
        *
        * @return true iff there is at least one listener
        */
        virtual bool isObserved() const;

        /**
        * Specify whether the ContinuousQueryCache has any listeners that
        * cannot be served by this Map listening to lite events.
        *
        * @param fObserved  true iff there is at least one listener
        */
        virtual void setObserved(bool fObserved);

        /**
        * Change the state of the ContinuousQueryCache.
        *
        * @param nState  one of the STATE_ enums
        */
        virtual void changeState(int32_t nState) const;


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        using Map::get;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
        * {@inheritDoc}
        */
        virtual void putAll(Map::View vMap);

        /**
        * {@inheritDoc}
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Set::View keySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle keySet();

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet();

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;


    // ----- CacheMap interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View getAll(Collection::View vKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey,
                Object::Holder ohValue, int64_t cMillis);


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
        * If isCacheValues is true, the index will be created locally as well as
        * on the NamedCache this ContinuousQueryCache wraps, otherwise, the index will be
        * created on the wrapped NamedCache only.
        *
        * See QueryMap::addIndex(ValueExtractor, bool, Comparator)
        */
        virtual void addIndex(ValueExtractor::View vExtractor,
                bool fOrdered, Comparator::View vComparator);

        /**
        * If isCacheValues() is true, the index will be removed locally, however, this call
        * will not cause the index on the NamedCache this ContinuousQueryCache wraps.
        * Developers must remove the index on the wrapped cache manually.
        *
        * @see QueryMap::removeIndex(ValueExtractor)
        */
        virtual void removeIndex(ValueExtractor::View vExtractor);


    // ----- InvocableMap interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder invoke(Object::View vKey,
                InvocableMap::EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Collection::View vCollKeys,
                InvocableMap::EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Map::View invokeAll(Filter::View vFilter,
                InvocableMap::EntryProcessor::Handle hAgent);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Collection::View vCollKeys,
                InvocableMap::EntryAggregator::Handle hAgent) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Filter::View vFilter,
                InvocableMap::EntryAggregator::Handle hAgent) const;


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


    // ----- NamedCache interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getCacheName() const;

        /**
        * {@inheritDoc}
        */
        virtual CacheService::Handle getCacheService();

        /**
        * {@inheritDoc}
        */
        virtual CacheService::View getCacheService() const;

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


    // ----- inner class: AsynchronousEvent ---------------------------------

    public:
        /**
        * Encapsulates an event and a listener that are to be dispatched
        * asynchronously.
        */
        class AsynchronousEvent
                : public class_spec<AsynchronousEvent>
            {
            friend class factory<AsynchronousEvent>;

            // ----- constructors/destructor ----------------------------

            protected:
                /**
                * @internal
                */
                AsynchronousEvent(MapEvent::Handle hEvent,
                        MapListener::Handle hListener);

            // ----- AsynchronousEvent interface -------------------------

            public:
                /**
                * Dispatch the event.
                */
                virtual void dispatch();

            // ----- data members ---------------------------------------

            protected:
                /**
                * The event to dispatch.
                */
                FinalHandle<MapEvent> f_hEvent;

                /**
                * The listener to receive the event.
                */
                FinalHandle<MapListener> f_hListener;
            };


    // ----- inner interface: EventQueueDaemon ------------------------------

    public:
        /**
        * Daemon thread used to dispatch messages asynchronously.
        */
        class COH_EXPORT TaskDaemon
                : public interface_spec<TaskDaemon>
            {
            public:
                /**
                * Schedule an event for asynchronous processing
                *
                * @param hEvent  the event to dispatch
                */
                virtual void scheduleDispatch(
                        AsynchronousEvent::Handle hEvent) = 0;

                /**
                * Start the daemon thread.
                */
                virtual void start() = 0;

                /**
                * Stop the daemon thread.
                */
                virtual void stop() = 0;
            };


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * Return a filter which merges the ContinousQueueCache's filter with
        * the supplied filter.
        *
        * @param vFilter  the filter to merge with this cache's filter
        *
        * @return the merged filter
        */
        virtual Filter::View mergeFilter(Filter::View vFilter) const;

        /**
        * Check the read-only setting to verify that the cache is NOT
        * read-only.
        *
        * @throws IllegalStateException if the ContinuousQueryCache is
        *         read-only
        */
        virtual void checkReadOnly() const;

        /**
        * Check the passed value to verify that it does belong in this
        * ContinuousQueryCache.
        *
        * @param vEntry  a key value pair to check.
        *
        * @throws IllegalArgumentException if the entry does not belong in
        *         this ContinuousQueryCache (based on the cache's filter)
        */
        virtual void checkEntry(Map::Entry::View vEntry);

        /**
        * Check the passed value to verify that it does belong in this
        * ContinuousQueryCache.
        *
        * @param vKey     the key for the entry
        * @param ohValue  the value for the entry
        *
        * @throws IllegalArgumentException if the entry does not belong in
        *         this ContinuousQueryCache (based on the cache's filter)
        */
        virtual void checkEntry(Object::View vKey, Object::Holder ohValue);

        /**
        * Return a String description of the provided STATE_* variables.
        *
        * @param nState  the state for which a description will be returned
        *
        * @return the state description
        *
        * @throws IllegalStateException if an unknown state is provided
        *
        * @since 12.2.1.4.5
        */
        virtual String::View getStateString(int32_t nState) const;

        /**
        * Set up the listeners that keep the ContinuousQueryCache up-to-date.
        *
        * @param fReload  pass true to force a data reload
        */
        virtual void configureSynchronization(bool fReload) const;

        /**
        * Simple helper to create an exception for communicating invalid state transitions.
        *
        * @param nExpectedState  expected state
        * @param nActualState    actual state
        *
        * @return a new {@link RuntimeException} with a description of the invalid state transition
        *
        * @since 12.2.1.4.5
        */
        virtual RuntimeException::View createUnexpectedStateException(int32_t nExpectedState, int32_t nActualState) const;

        /**
        * Wrap specified MapEventFilter with a MapEventTransformerFilter that
        * will either transform cache value using transformer defined for this
        * ContinuousQueryCache, or remove the old value from the event using
        * SemiLiteEventTransformer, if no transformer is defined for this CQC.
        *
        * @param filterAdd  add filter to wrap
        *
        * @return MapEventTransformerFilter that wraps specified add filter
        *
        * @since Coherence 12.1.2
        */
        virtual Filter::View createTransformerFilter(
            MapEventFilter::View vFilterAdd) const;

        /**
        * Ensure that the ContinousQueryCache listeners have been registered
        * and its content synchronized with the underlying NamedCache.
        *
        * @param fReload  the value to pass to the #configureSynchronization
        *                 method if the ContinousQueryCache needs to be
        *                 configured and synchronized
        */
        virtual void ensureSynchronized(bool fReload) const;

        /**
        * Called when an event has occurred. Allows the key to be logged as
        * requiring deferred synchronization if the event occurs during the
        * configuration or population of the ContinuousQueryCache.
        *
        * @param vKey  the key that the event is related to
        *
        * @return true if the event processing has been deferred
        */
        virtual bool isEventDeferred(Object::View vKey) const;

        /**
        * Removes the mapping for this key from this map if present. This
        * method exists to allow sub-classes to optmiize remove
        * functionalitly for situations in which the original value is not
        * required.
        *
        * @param vKey key whose mapping is to be removed from the map
        *
        * @return true iff the Map changed as the result of this operation
        */
        virtual bool removeBlind(Object::View vKey);

        /**
        * Obtain a set of keys that are represented by this Map.
        *
        * @return an internal Set of keys that are contained by this Map
        */
        virtual Set::View getInternalKeySet() const;

        /**
        * Return the underlying cache.
        *
        * @return the underlying cache
        */
        virtual NamedCache::Handle getCacheInternal() const;

        /**
        * Factory Method: Instantiate a MapListener for adding items to the
        * query, and (if there are listeners on the ContinuousQueryCache) for
        * dispatching inserts and updates.
        *
        * @return a new MapListener that will add items to and update items
        *         in the ContinuousQueryCache
        */
        virtual MapListener::Handle instantiateAddListener() const;

        /**
        * Factory Method: Instantiate a MapListener for evicting items from
        * the query.
        *
        * @return a new MapListener that will listen to all events that will
        *         remove items from the ContinuousQueryCache
        */
        virtual MapListener::Handle instantiateRemoveListener() const;

        /**
        * Instantiate and register a MemberListener with the underlying
        * caches's service.
        * 
        * The primary goal of that listener is invalidation of the front map
        * in case of the service [automatic] restart.
        */
        virtual void registerServiceListener() const;

        /**
        * Unregister underlying caches's service member listener.
        */
        virtual void unregisterServiceListener() const;

        /**
        * Create and initialize this ContinuousQueryCache's (if not already present) internal cache.
        * This method is called by configureSynchronization, as such, it shouldn't be called
        * directly.  Use getInternalCache.
        *
        * @return the ObservableMap functioning as this ContinuousQueryCache's
        *        internal cache
        *
        * @since 12.2.1.4
        */
        virtual ObservableMap::Handle ensureInternalCache() const;

        /**
        * Instantiate and register a NamedCacheDeactivationListener with the underlying cache
        * service.
        * 
        * The primary goal of that listener is invalidation of the named cache
        * in case the named caches is destroyed / truncated.
        *
        * @since 12.2.1.4
        */
        virtual void registerDeactivationListener() const;

        /**
        * Unregister underlying cache service member listener.
        *
        * @since 12.2.1.4
        */
        virtual void unregisterDeactivationListener() const;

        /**
        * Release the listeners.
        *
        * @since 12.2.1.4
        */
        virtual void releaseListeners() const;

        /**
        * Factory Method: Instantiate a listener on the internal map that
        * will direct events to the passed listener, either synchronously or
        * asynchronously as appropriate.
        *
        * @param hListener  the listener to route to
        * @param fLite      true to indicate that the MapEvent objects do
        *                   not have to include the OldValue and NewValue
        *                   property values in order to allow optimizations
        *
        * @return a new EventRouter specific to the passed listener
        */
        virtual MultiplexingMapListener::Handle instantiateEventRouter(
                MapListener::Handle hListener, bool fLite) const;

        /**
        * Create a self-processing event queue.
        *
        * @return a QueueProcessor onto which events can be placed in order
        *         to be dispatched asynchronously
        */
        virtual TaskDaemon::Handle instantiateEventQueue() const;

        /**
        * Obtain this ContinuousQueryCache's event queue.
        *
        * @return the event queue that this ContinuousQueryCache uses to
        *         dispatch its events to its non-synchronous listeners
        */
        virtual TaskDaemon::Handle getEventQueue() const;

        /**
        * Obtain the existing event queue or create one if none exists.
        *
        * @return the event queue that this ContinuousQueryCache uses to
        *         dispatch its events to its non-synchronous listeners
        */
        virtual TaskDaemon::Handle ensureEventQueue() const;

        /**
        * Shut down running event queue.
        */
        virtual void shutdownEventQueue();

        /**
        * Create a new key set.
        *
        * @return the new key set
        */
        virtual Set::Handle instantiateKeySet();

        /**
        * Create a new EntrySet
        *
        * @return the new empty set
        */
        virtual Set::Handle instantiateEntrySet();

        /**
        * Ensure that the map of indexes maintained by this cache exists.
        *
        * @return the map of indexes.
        */
        virtual Map::Handle ensureIndexMap();

        /**
        * Get the map of indexes maintained by this cache.
        *
        * @return the map of indexes.
        */
        virtual Map::View getIndexMap() const;

        /**
        * Get the map of indexes maintained by this cache.
        *
        * @return the map of indexes.
        */
        virtual Map::Handle getIndexMap();

        /**
        * Release the the entire index map.
        */
        virtual void releaseIndexMap();

    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * State: Disconnected state. The content of the ContinousQueryCache is
        * not fully synchronized with the underlying [clustered] cache. If the
        * value of the ReconnectInterval property is zero, it must be
        * configured (synchronized) before it can be used.
        */
        static const int32_t state_disconnected  = 0;

        /**
        * State: The ContinuousQueryCache is configuring or re-configuring its
        * listeners and content.
        */
        static const int32_t state_configuring  = 1;

        /**
        * State: The ContinousQueryCache has been configured.
        */
        static const int32_t state_configured   = 2;

        /**
        * State: The ContinousQueryCache has been configured and fully
        * synchronized.
        */
        static const int32_t state_synchronized = 3;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying NamedCache object.
        */
        mutable MemberHandle<NamedCache> m_hCache;

        /**
        * The name of the underlying NamedCache. A copy is kept here because
        * the reference to the underlying NamedCache is discarded when this
        * cache is released.
        */
        FinalView<String> f_vsName;

        /**
        * The filter that represents the subset of information from the
        * underlying NamedCache that this ContinuousQueryCache represents.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The option of whether or not to locally cache values.
        */
        bool m_fCacheValues;

        /**
        * The option to disallow modifications through this
        * ContinuousQueryCache interface.
        */
        bool m_fReadOnly;

        /**
        * The interval (in milliseconds) that indicates how often the
        * ContinuousQueryCache should attempt to synchronize its content with
        * the underlying cache in case the connection is severed.
        */
        int64_t m_cReconnectMillis;

        /**
        * The timestamp when the synchronization was last attempted.
        */
        mutable Volatile<int64_t> m_ldtConnectionTimestamp;

        /**
        * The keys that are in this ContinuousQueryCache, and (if
        * m_fCacheValues is true) the corresponding values as well.
        */
        mutable MemberHandle<ObservableMap> m_hMapLocal;

        /**
        * State of the ContinousQueryCache. One of the STATE_* enums.
        */
        mutable Volatile<int32_t> m_nState;

        /**
        * While the ContinuousQueryCache is configuring or re-configuring its
        * listeners and content, any events that are received must be logged
        * to ensure that the corresponding content is in sync.
        */
        mutable MemberHandle<Map> m_hMapSyncReq;

        /**
        * The event queue for this ContinuousQueryCache.
        */
        mutable MemberHandle<TaskDaemon> m_hTaskQueue;

        /**
        * Keeps track of whether the ContinuousQueryCache has listeners that
        * require this cache to cache values.
        */
        bool m_fListeners;

        /**
        * The MapEventFilter that uses the ContinuousQueryCache's filter to
        * select events that would add elements to this cache's contents.
        */
        mutable MemberView<MapEventFilter> m_vFilterAdd;

        /**
        * The MapEventFilter that uses the ContinuousQueryCache's filter to
        * select events that would remove elements from this cache's contents.
        */
        mutable MemberView<MapEventFilter> m_vFilterRemove;

        /**
        * The listener that gets information about what should be in this
        * cache.
        */
        mutable WeakHandle<MapListener> m_whListenerAdd;

        /**
        * The listener that gets information about what should be thrown out
        * of this cache.
        */
        mutable WeakHandle<MapListener> m_whListenerRemove;

        /**
        * The cache service MemberListener for the underlying NamedCache.
        */
        mutable WeakHandle<MemberListener> m_whListenerService;

        /*
        * The initial MapListener that will receive all the events from the
        * ContinuousQueryCache, including those corresponding to its initial
        * population.
        */
        mutable MemberHandle<MapListener> m_hListener;

        /**
        * The transformer that should be used to convert values from the
        * underlying cache.
        */
        FinalView<ValueExtractor> f_vTransformer;

        /**
        * The map of indexes maintained by this cache. The keys of the Map are
        * ValueExtractor objects, and for each key, the corresponding value
        * stored in the Map is a MapIndex object.
        */
        FinalHandle<Map> f_hMapIndex;

        /**
        * KeySet backed by this cache
        */
        FinalHandle<Set> f_hSetKeys;

        /**
        * EntrySet backed by this cache
        */
        FinalHandle<Set> f_hSetEntries;

        /**
        * The Supplier of the NamedCache to create a view of. The Supplier must return
        * a new instance every time the supplier's get() method is called.
        *
        * @since 12.2.1.4
        */
        FinalView<Supplier> f_vCacheSupplier;

        /**
        * The optional supplier to obtain this cache's name.
        *
        * @since 12.2.1.4
        */
        MemberView<Supplier> m_vCacheNameSupplier;

        /**
        * The NamedCache deactivation listener.
        *
        * @since 12.2.1.4
        */
        mutable MemberHandle<MapListener> m_hListenerDeactivation;


    // ----- friends --------------------------------------------------------

    friend class AddListener;
    friend class RemoveListener;
    friend class ServiceListener;
    friend class EventRouter;
    friend class KeySet;
    friend class EntrySet;
    friend class KeySetMuterator;
    friend class EntrySetIterator;
    friend class DeactivationListener;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONTINUOUS_QUERY_CACHE_HPP

