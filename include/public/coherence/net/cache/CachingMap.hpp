/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHING_MAP_HPP
#define COH_CACHING_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/internal/net/NamedCacheDeactivationListener.hpp"

#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/SimpleCacheStatistics.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/util/AbstractMapListener.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"
#include "coherence/util/Set.hpp"

#include "coherence/util/filter/CacheEventFilter.hpp"
#include "coherence/util/filter/MapEventFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::internal::net::NamedCacheDeactivationListener;
using coherence::net::NamedCache;
using coherence::util::AbstractMapListener;
using coherence::util::Collection;
using coherence::util::ConcurrentMap;
using coherence::util::Filter;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::MultiplexingMapListener;
using coherence::util::Set;
using coherence::util::filter::CacheEventFilter;
using coherence::util::filter::MapEventFilter;


/**
* Map implementation that wraps two maps - a front map (assumed to be
* "inexpensive" and probably "incomplete") and a back map (assumed to
* be "complete" and "correct", but more "expensive") - using a
* read-through/write-through approach.
*
* If the back map implements ObservableMap interface, the CachingMap provides
* four different strategies of invalidating the front map entries that have
* changed by other processes in the back map:
*
* listen_none strategy instructs the cache not to listen for invalidation
*     events at all. This is the best choice for raw performance and
*     scalability when business requirements permit the use of data which
*     might not be absolutely current.  Freshness of data can be guaranteed
*     by use of a sufficiently brief eviction policy for the front map;
* listen_present strategy instructs the CachingMap to listen to the
*     back map events related <b>only</b> to the items currently present in
*     the front map. This strategy works best when each instance of a front
*     map contains distinct subset of data relative to the other front map
*     instances (e.g. sticky data access patterns);
* listen_all strategy instructs the CachingMap to listen to <b>all</b>
*     back map events. This strategy is optimal for read-heavy tiered access
*     patterns where there is significant overlap between the different
*     instances of front maps;
* listen_auto strategy instructs the CachingMap implementation to switch
*     automatically between listen_present and listen_all strategies based
*     on the cache statistics.
* listen_logical strategy instructs the CachingMap to listen to <b>all</b>
*     back map events that are <b>not synthetic</b>. A synthetic event could
*     be emitted as a result of eviction or expiration. With this
*     invalidation strategy, it is possible for the front map to contain
*     cache entries that have been synthetically removed from the back
*     (though any subsequent re-insertion will cause the corresponding
*     entries in the front map to be invalidated).
*
* The front map implementation is assumed to be thread safe; additionally
* any modifications to the front map are allowed only after the corresponding
* lock is acquired against the ControlMap.
*
* <b>Note:</b> NULL values are not cached in the front map and therefore this
* implementation is not optimized for maps that allow NULL values to be
* stored.
*
* @author tb  2008.06.12
*/
class COH_EXPORT CachingMap
    : public class_spec<CachingMap,
        extends<Object>,
        implements<Map> >
    {
    friend class factory<CachingMap>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- enums ----------------------------------------------------------

    public:
        /**
        * Enum for invalidation strategies
        */
        enum InvalidationStrategy
            {
            /**
            * No invalidation strategy.
            */
            listen_none,

            /**
            * Invalidation strategy that instructs the CachingMap to listen
            * to the back map events related <b>only</b> to the items
            * currently present in the front map; this strategy serves best
            * when the changes to the back map come mostly from the
            * CachingMap itself.
            */
            listen_present,

            /**
            * Invalidation strategy that instructs the CachingMap to listen
            * to <b>all</b> back map events; this strategy is preferred when
            * updates to the back map are frequent and with high probability
            * come from the outside of this CachingMap; for example multiple
            * CachingMap instances using the same back map with a large
            * degree of key set overlap between front maps.
            */
            listen_all,

            /**
            * Invalidation strategy that instructs the CachingMap
            * implementation to switch automatically between listen_present
            * and listen_all strategies based on the cache statistics.
            */
            listen_auto,

            /**
            * Invalidation strategy that instructs the CachingMap to listen
            * to <b>all</b> back map events that are <b>not synthetic</b>. A
            * synthetic event could be emitted as a result of eviction or
            * expiration. With this invalidation strategy, it is possible
            * for the front map to contain cache entries that have been
            * synthetically removed from the back (though any subsequent
            * re-insertion will cause the corresponding entries in the front
            * map to be invalidated).
            */
            listen_logical
            };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a CachingMap using two specified maps:
        * <i>FrontMap</i> (aka "cache", "near" or "shallow") and
        * <i>BackMap</i>  (aka "actual", "real" or "deep").
        *
        * If the BackMap implements the ObservableMap interface a listener
        * will be added to the BackMap to invalidate FrontMap items updated
        * [externally] in the back map using the listen_auto strategy.
        *
        * If no MapControl is specified then a new one is created.
        *
        * @param hMapFront    front map
        * @param hMapBack     back map
        * @param strategy     specifies the strategy used for the front map
        *                     invalidation; valid values are LISTEN_*
        *                     constants
        * @param hMapControl  the ConcurrentMap to keep track of front map
        *                     updates; may be NULL
        */
        CachingMap(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
                InvalidationStrategy strategy = listen_auto,
                ConcurrentMap::Handle hMapControl = NULL);


    // ----- life-cycle -----------------------------------------------------

    public:
        /**
        * Release the CachingMap. If the BackMap implements an ObservableMap
        * calling this method is necessary to remove the BackMap listener.
        * Any access to the CachingMap which has been released will cause
        * IllegalStateException.
        *
        * @throws IllegalStateException if accessing the CachingMap
        *         which has been released
        */
        virtual void release();


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the front map reference.
        *
        * <b>Note:</b> direct modifications of the returned map may cause an
        * unpredictable behavior of the CachingMap.
        *
        * @return the front Map
        */
        virtual CacheMap::Handle getFrontMap() const;

        /**
        * Obtain the back map reference.
        *
        * <b>Note:</b> direct modifications of the returned map may cause an
        * unpredictable behavior of the CachingMap.
        *
        * @return the back Map
        */
        virtual CacheMap::Handle getBackMap() const;

        /**
        * Obtain the invalidation strategy used by this CachingMap.
        *
        * @return one of LISTEN_* values
        */
        virtual InvalidationStrategy getInvalidationStrategy() const;

        /**
        * Obtain the ConcurrentMap that should be used to synchronize
        * the front map modification access.
        *
        * @return a ConcurrentMap controlling the front map modifications
        */
        virtual ConcurrentMap::Handle getControlMap() const;

        /**
        * Obtain the CacheStatistics for this cache.
        *
        * @return a CacheStatistics object
        */
        virtual CacheStatistics::Handle getCacheStatistics() const;

        /**
        * Determine the rough number of front map invalidation hits since
        * the cache statistics were last reset.
        *
        * An invalidation hit is an externally induced map event for an entry
        * that exists in the front map.
        *
        * @return the number of cache invalidation hits
        */
        virtual int64_t getInvalidationHits() const;

        /**
        * Determine the rough number of front map invalidation misses since
        * the cache statistics were last reset.
        *
        * An invalidation miss is an externally induced map event for an
        * entry that does not exists in the front map.
        *
        * @return the number of cache invalidation misses
        */
        virtual int64_t getInvalidationMisses() const;

        /**
        * Determine the total number of registerListener(Object::View vKey)
        * operations since the cache statistics were last reset.
        *
        * @return the total number of listener registrations
        */
        virtual int64_t getTotalRegisterListener() const;

    protected:
        /**
        * Determine if changes to the back map affect the front map so that
        * data in the front map stays in sync.
        *
        * @return true if the front map has a means to stay in sync with the
        *         back map so that it does not contain stale data
        */
        virtual bool isCoherent() const;


    // ----- CachingMap interface -------------------------------------------

    protected:
        /**
        * Invalidate the key from the front.  The caller must have the key
        * locked.
        *
        * @param vKey  the key to invalidate
        */
        virtual void invalidateFront(Object::View vKey) const;

        /**
        * Helper method used by put() and putAll() to perform common
        * maintanence tasks after completing an operation against the back.
        * This includes removing the keys from the control map, and
        * evaluating if it is safe to update the front with the "new" value.
        * The implementation makes use of the following assumption: if
        * listEvents == IGNORE_LIST then oKey does not exist in the
        * front, and there is no key based listener for it.  Any key passed
        * to this method must be locked in the control map by the caller.
        *
        * @param vKey        the key
        * @param ohValue     the new value
        * @param hlistEvents the event list associated with the key
        * @param cMillis     the number of milliseconds until the cache entry
        *                    will expire
        */
        virtual void finalizePut(Object::View vKey, Object::Holder ohValue,
                List::Handle hlistEvents, int64_t cMillis);

    public:
        /**
        * Implementation of put method that optionally skips the return value
        * retrieval and allows to specify an expiry for the cache entry.
        *
        * @param vKey     the key
        * @param ohValue  the value
        * @param fReturn  if true, the return value is required; otherwise
        *                 the return value will be ignored
        * @param cMillis  the number of milliseconds until the cache entry
        *                 will expire
        * @return previous value (if required)
        *
        * @throws UnsupportedOperationException if the requested expiry is a
        *         positive value and either the front map or the back map
        *         implementations do not support the expiration functionality
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue,
                bool fReturn, int64_t cMillis);

        /**
        * Get all the specified keys, if they are in the cache. For each key
        * that is in the cache, that key and its corresponding value will be
        * placed in the map that is returned by this method. The absence of
        * a key in the returned map indicates that it was not in the cache,
        * which may imply (for caches that can load behind the scenes) that
        * the requested data could not be loaded.
        *
        * <b>Note:</b> this implementation does not differentiate between
        * missing keys or NULL values stored in the back map; in both cases
        * the returned map will not contain the corresponding entry.
        *
        * @param vColKeys  a collection of keys that may be in the named
        *                  cache
        *
        * @return a Map of keys to values for the specified keys passed in
        *         <tt>col</tt>
        */
        virtual Map::View getAll(Collection::View vColKeys) const;


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsValue(Object::View vValue) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey);

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
        virtual Set::View keySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle keySet();

        /**
        * {@inheritDoc}
        */
        virtual Collection::View values() const;

        /**
        * {@inheritDoc}
        */
        virtual Collection::Handle values();

        /**
        * {@inheritDoc}
        */
        virtual Set::View entrySet() const;

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle entrySet();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- back map listener support --------------------------------------

    public:
        /**
        * Register the global back map listener.
        */
        virtual void registerListener() const;

        /**
        * Unregister the global back map listener.
        */
        virtual void unregisterListener() const;

        /**
        * Register the back map listener for the specified key.
        *
        * @param vKey  the key
        */
        virtual void registerListener(Object::View vKey) const;

        /**
        * Register the back map listeners for the specified set of keys.
        *
        * @param hSetKeys  the key set
        *
        * @since 12.2.1
        */
        virtual void registerListeners(Set::Handle hSetKeys) const;

        /**
        * Unregister the back map listener for the specified key.
        *
        * @param vKey  the key
        */
        virtual void unregisterListener(Object::View vKey) const;

        /**
        * Unregister the back map listener for the specified keys.
        *
        * Note: all the keys in the passed-in set must be locked and will be
        *       unlocked.
        *
        * @param hSetKeys  Set of keys to unregister (and unlock)
        *
        * @since 12.2.1
        */
        virtual void unregisterListeners(Set::Handle hSetKeys) const;

        /**
        * Register the global front map listener.
        */
        virtual void registerFrontListener() const;

        /**
        * Unregister the global front map listener.
        */
        virtual void unregisterFrontListener() const;

        /**
        * Register back cache deactivation listener.
        */
        virtual void registerDeactivationListener() const;

        /**
        * Unregister back cache deactivation listener.
        */
        virtual void unregisterDeactivationListener() const;

        /**
        * Reset the front map
        */
        virtual void resetFrontMap();

        /**
        * Ensure that a strategy has been choosen and that any appropriate
        * global listeners have been registered.
        *
        * @return the current strategy
        */
        virtual InvalidationStrategy ensureInvalidationStrategy() const;

        /**
        * Reset the "current invalidation strategy" flag.
        *
        * This method should be called <b>only</b> while the access to the
        * front map is fully synchronzied and the front map is empty to
        * prevent stalled data.
        */
        virtual void resetInvalidationStrategy();

    // ----- inner class: PrimingListener -----------------------------------

    protected:
        /**
         * MapListener for back map responsible for keeping the front map
         * coherent with the back map. This listener is registered as a
         * synchronous listener for lite events (carrying only a key) and
         * generates a "priming" event when registered.
         *
         * @since 12.2.1
         */
        class PrimingListener
        : public class_spec<PrimingListener,
        extends<MultiplexingMapListener>,
        implements<MapListenerSupport::PrimingListener> >
        {
            friend class factory<PrimingListener>;

            // ----- constructors ---------------------------------------

        protected:
            /**
             * Construct a PrimingListener
             *
             * @param hMap  the map associated with this listener
             */
            PrimingListener(CachingMap::Handle hMap);

            // ----- MultiplexingMapListener interface ------------------

            /**
             * {@inheritDoc}
             */
            virtual void onMapEvent(MapEvent::View vEvent);

            // ---- data members ----------------------------------------

        protected:
            /**
             * The map associated with this listener.
             */
            WeakHandle<CachingMap> m_whMap;
        };

    // ----- inner class: SimpleListener --------------------------------

    protected:
        /**
        * MapListener for back map responsible for keeping the front map
        * coherent with the back map. This listener is registered as a
        * synchronous listener for lite events (carrying only a key).
        */
        class SimpleListener
           : public class_spec<SimpleListener,
               extends<MultiplexingMapListener>,
               implements<MapListenerSupport::SynchronousListener> >
            {
            friend class factory<SimpleListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a SimpleListener
                *
                * @param hMap  the map associated with this listener
                */
                SimpleListener(CachingMap::Handle hMap);

            // ----- MultiplexingMapListener interface ------------------

                /**
                * {@inheritDoc}
                */
                virtual void onMapEvent(MapEvent::View vEvent);

            // ---- data members ----------------------------------------

            protected:
                /**
                * The map associated with this listener.
                */
                WeakHandle<CachingMap> m_whMap;
            };

    // ----- inner class: DeactivationListener --------------------------

    protected:
        /**
        * DeactivationListener for the back NamedCache.
        * The primary goal of that listener is invalidation of the front map
        * when the back cache is destroyed or all storage nodes are stopped.
        */
        class DeactivationListener
           : public class_spec<DeactivationListener,
               extends<AbstractMapListener>,
               implements<NamedCacheDeactivationListener> >
            {
            friend class factory<DeactivationListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a DeactivationListener
                *
                * @param hMap  the map associated with this listener
                */
                DeactivationListener(CachingMap::Handle hMap);

            // ----- NamedCacheDeactivationListener interface ------------------

                /**
                * {@inheritDoc}
                */
                virtual void entryDeleted(MapEvent::View vEvent);

                /**
                * {@inheritDoc}
                */
                virtual void entryUpdated(MapEvent::View vEvent);

            // ---- data members ----------------------------------------

            protected:
                /**
                * The map associated with this listener.
                */
                WeakHandle<CachingMap> m_whMap;
            };

    // ----- inner class: FrontMapListener ----------------------------------

    protected:
        /**
        * MapListener for back map responsible for keeping the front map
        * coherent with the back map. This listener is registered as a
        * synchronous listener for lite events (carrying only a key).
        */
        class FrontMapListener
            : public class_spec<FrontMapListener,
                extends<AbstractMapListener>,
                implements<MapListenerSupport::SynchronousListener> >
            {
            friend class factory<FrontMapListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a FrontMapListener
                *
                * @param hMap  the map associated with this listener
                */
                FrontMapListener(CachingMap::Handle hMap);

            // ----- MapListener interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void entryDeleted(MapEvent::View vEvent);

            // ---- helper registration methods -------------------------

            public:
                /**
                * Register this listener with the "front" map.
                */
                virtual void registerWithMap();

                /**
                * Unregister this listener with the "front" map.
                */
                virtual void unregisterFromMap();

            // ---- data members ----------------------------------------

            protected:
                /**
                * The filter associated with this listener.
                */
                FinalView<Filter> f_vFilter;

                /**
                * The map associated with this listener.
                */
                WeakHandle<CachingMap> m_whMap;
            };


    // ----- listener support -----------------------------------------------

    protected:
        /**
        * Factory pattern: instantiate back map listener.
        *
        * @param strategy  the strategy to instantiate a back map listener for
        *
        * @return an instance of back map listener responsible for keeping
        *         the front map coherent with the back map
        */
        virtual MapListener::Handle instantiateBackMapListener(InvalidationStrategy strategy);

        /**
        * Factory pattern: instantiate front map listener.
        *
        * @return an instance of front map listener
        */
        virtual FrontMapListener::Handle instantiateFrontMapListener();

        /**
        * Validate the front map entry for the specified back map event.
        *
        * @param vEvent  the MapEvent from the back map
        */
        virtual void validate(MapEvent::View vEvent);

        /**
        * Set up a thread local Set to hold all the keys that might be evicted
        * from the front cache.
        *
        * @return a Set to hold all the keys in the ThreadLocal object or null
        *         if the bulk unregistering is not needed
        *
        * @since 12.2.1
        */
        virtual Set::Handle setKeyHolder() const;

        /**
        * Remove the key holder from the ThreadLocal object.
        *
        * @since 12.2.1
        */
        virtual void removeKeyHolder() const;

        /**
         * Check if the specified event is a "priming" one.
         *
         * @since 12.2.1
         */
        static bool isPriming(MapEvent::View vEvent);

    // ----- constants and data fields  -------------------------------------

    protected :
        /**
        * The "back" map, considered to be "complete" yet "expensive" to
        * access.
        */
        mutable FinalHandle<CacheMap> f_hMapBack;

        /**
        * The "front" map, considered to be "incomplete" yet "inexpensive" to
        * access.
        */
        mutable FinalHandle<CacheMap> f_hMapFront;

        /**
        * The invalidation strategy that this map is to use.
        */
        mutable InvalidationStrategy m_strategyTarget;

        /**
        * The current invalidation strategy, which at times could be different
        * from the target strategy.
        */
        mutable InvalidationStrategy m_strategyCurrent;

        /**
        * An optional listener for the "back" map.
        */
        mutable MemberHandle<MapListener> m_hListener;

        /**
         * An optional Simplelistener for the "back" map.
         * @see note in CachingMap::onInit()
         *
         * @since 12.2.1
         */
        mutable MemberHandle<MapListener> m_hSimpleListener;

        /**
         * An optional DeactivationListener for the "back" map.
         * @see note in CachingMap::onInit()
         *
         * @since 12.1.3
         */
        mutable MemberHandle<DeactivationListener> m_hListenerDeactivation;

        /**
        * An optional listener for the "front" map.
        */
        mutable FinalHandle<FrontMapListener> f_hListenerFront;

        /**
        * A filter that selects events for the back-map listener.
        */
        mutable MemberHandle<Filter> m_hFilterListener;

        /**
        * The ConcurrentMap to keep track of front map updates.
        * Values are list of events received by the listener while the
        * corresponding key was locked.  Use of LOCK_ALL is restricited to
        * non-blocking operations to prevent deadlock with the service thread.
        */
        mutable FinalHandle<ConcurrentMap> f_hMapControl;

        /**
        * The CacheStatistics object maintained by this cache.
        */
        mutable FinalHandle<SimpleCacheStatistics> f_hStats;

        /**
        * The rough (ie unsynchronized) number of times the front map entries
        * that were present in the front map were invalidated by the listener.
        */
        mutable /*volatile stat*/ int64_t m_cInvalidationHits;

        /**
        * The rough (ie unsynchronized) number of times the front map entries
        * that were absent in the front map received invalidation event.
        */
        mutable /*volatile stat*/ int64_t m_cInvalidationMisses;

        /**
        * The total number of registerListener(oKey) operations.
        */
        mutable /*volatile stat*/ int64_t m_cRegisterListener;

        /**
        * True if the cache has been released.
        */
        bool m_fReleased;

        /**
        * The ThreadLocal to hold all the keys that are evicted while the front cache
        * is updated during get or getAll operation.
        *
        * @since 12.2.1
        */
        mutable FinalHandle<ThreadLocalReference> f_htloKeys;

    // ----- constants ------------------------------------------------------

    protected :
        /**
        * A unique Object that serves as a control key for global operations
        * such as clear and release and synchronization point for the current
        * strategy change.
        */
        FinalView<Object> f_vKeyGlobal;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHING_MAP_HPP
