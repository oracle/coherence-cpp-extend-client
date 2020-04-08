/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_CACHE_HPP
#define COH_LOCAL_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheLoader.hpp"
#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/CacheStore.hpp"
#include "coherence/net/cache/OldCache.hpp"

#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::MapListener;


/**
* A LocalCache implementation that supports CacheLoader and CacheStore
* objects.
*
* @author nsa  2008.08.01
*/
class COH_EXPORT LocalCache
    : public class_spec<LocalCache,
        extends<OldCache>,
        implements<CacheMap> >
    {
    friend class factory<LocalCache>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constants ------------------------------------------------------
    public:
        /**
        * Max number of units this cache can hold
        */
        static const size32_t max_units     = 0x7FFFFFFF;

        /**
        * By default, the cache size (in units) is infinite.
        */
        static const size32_t default_units = max_units;

        /**
        * Never expire entries from the cache
        */
        static const int32_t never_expire  = 0;

        /**
        * By default, the cache entries never expire.
        */
        static const int32_t default_expire = never_expire;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an instance of LocalCache
        *
        * @param cUnits           the number of units that the cache manager
        *                         will cache before pruning the cache
        * @param cExpiryMillis    the number of milliseconds that each cache
        *                         entry lives before being automatically
        *                         expired
        * @param dflPruneLevel    the percentage of the total number of units
        *                         that will remain after the cache manager
        *                         prunes the cache (i.e. this is the
        *                         "low water mark" value); this value is in
        *                         the range 0.0 to 1.0
        * @param cInitialBuckets  the initial number of hash buckets,
        *                         0 &lt; n
        * @param flLoadFactor     the acceptable load factor before resizing
        *                         occurs, 0 &lt; n, such that a load factor
        *                         of 1.0 causes resizing when the number of
        *                         entries exceeds the number of buckets
        * @param flGrowthRate     the rate of bucket growth when a resize
        *                         occurs, 0 &lt; n, such that a growth rate
        *                         of 1.0 will double the number of buckets:
        *                         bucketcount = bucketcount * (1 + growthrate)
        */
        LocalCache(int32_t      cUnits          = default_units,
                int32_t         cExpiryMillis   = default_expire,
                float64_t       dflPruneLevel   = 0.75F,
                size32_t        cInitialBuckets = 17,
                float32_t       flLoadFactory   = 1.0F,
                float32_t       flGrowthRate    = 3.0F);


    // ----- inner class: InternalListener ----------------------------------

    protected:
        /**
        * Factory pattern: Instantiate an internal MapListener to listen to
        * this cache and report changes to the CacheStore.
        *
        * @return  a new MapListener instance
        */
        MapListener::Handle instantiateInternalListener();

        /**
        * An internal MapListener that listens to this cache and reports
        * changes to the CacheStore.
        */
        class InternalListener
            : public class_spec<InternalListener,
                extends<Object>,
                implements<MapListener> >
            {
            friend class factory<InternalListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a new InternalListener
                */
                InternalListener(LocalCache::Handle hCache);

            // ----- MapListener interface ------------------------------

            public:
                /**
                * Invoked when a map entry has been inserted.
                *
                * @param vEvt  the MapEvent carrying the insert information
                */
                virtual void entryInserted(MapEvent::View vEvt);

                /**
                * Invoked when a map entry has been updated.
                *
                * @param vEvt  the MapEvent carrying the update information
                */
                virtual void entryUpdated(MapEvent::View vEvt);

                /**
                * Invoked when a map entry has been removed.
                *
                * @param vEvt  the MapEvent carrying the delete information
                */
                virtual void entryDeleted(MapEvent::View vEvt);

            // ----- helper methods -------------------------------------

            protected:
                /**
                * A value modification event (insert or update) has occurred.
                *
                * @param vEvt  the MapEvent object
                */
                virtual void onModify(MapEvent::View vEvt);

            // ----- Data members ---------------------------------------

            protected:
                /**
                * Reference back to the containing cache
                */
                WeakHandle<LocalCache> m_hCache;
            };


    // ----- inner class: KeyMask -------------------------------------------

    public:
        /**
        * A class that masks certain changes so that they are not reported back
        * to the CacheStore.
        */
        class COH_EXPORT KeyMask
            : public class_spec<KeyMask>
            {
            friend class factory<KeyMask>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a new KeyMask instance
                */
                KeyMask();

            // ----- KeyMask interface ----------------------------------

            public:
                /**
                * Check if a key should be ignored.
                *
                * @param vKey  the key that a change event has occurred for
                *
                * @return true if change events for the key should be ignored
                */
                virtual bool isIgnored(Object::View vKey);

                /**
                * Check whether or not the currently performed operation is
                * internally initiated.
                *
                * @return true iff the the current operation is internal
                */
                virtual bool isSynthetic();

                /**
                * Specify whether or not the currently performed operation is
                * internally initiated.
                *
                * @param fSynthetic  true iff the the current operation is
                *                    internal
                */
                virtual void setSynthetic(bool fSynthetic);

                /**
                * Ensure that the synthetic operation flag is set.
                *
                * @return the previous value of the flag
                */
                virtual bool ensureSynthetic();
            };


    // ----- inner class: Entry ---------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(Object::View vKey,
                Object::Holder ohValue, size32_t nHash);

        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(
                SafeHashMap::Entry::View vEntry);

    public:
        /**
        * A holder for a cached value.
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<OldCache::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructors ---------------------------------------
            protected:
                /**
                * {@internal}
                */
                Entry(Object::View vKey, Object::Holder ohValue,
                        size32_t nHash, LocalCache::Handle hCache);

                /**
                * {@internal}
                */
                Entry(const Entry& that);

                /**
                * {@internal}
                */
                Entry(Entry::View vThat);
            };


    // ----- Map interface --------------------------------------------------

    public:

        /**
        * Removes the mapping for this key from this map if present.
        *
        * @param oKey  key whose mapping is to be removed from the map
        *
        * @return previous value associated with specified key, or <tt>null</tt>
        *         if there was no mapping for key.  A <tt>null</tt> return can
        *         also indicate that the map previously associated <tt>null</tt>
        *         with the specified key, if the implementation supports
        *         <tt>null</tt> values
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;

        /**
         * Removes all mappings from this map.
         */
        virtual void clear();


    // ----- LocalCache interface -------------------------------------------

    public:
        /**
        * Determine the loader used by this LocalCache, if any.
        *
        * @return the loader used by this LocalCache, or null if none
        */
        virtual CacheLoader::View getCacheLoader() const;

        /**
        * Determine the loader used by this LocalCache, if any.
        *
        * @return the loader used by this LocalCache, or null if none
        */
        virtual CacheLoader::Handle getCacheLoader();

        /**
        * Specify the loader used by this LocalCache.
        *
        * @param loader  loader to use, or null
        */
        virtual void setCacheLoader(CacheLoader::Handle hLoader);

        /**
        * If the specified item is in the cache, return it. Otherwise,
        * load the value for the specified key and return it.
        *
        * @param vKey  the key to the desired cached item
        *
        * @return the value corresponding to the specified key, otherwise null
        */
        virtual Object::Holder get(Object::View vKey) const;
        using Map::get;

        /**
        * Locate an Entry in the hash map based on its key. If the Entry is not in
        * the cache, load the Entry for the specified key and return it.
        *
        * @param oKey  the key to the desired cached Entry
        *
        * @return the Entry corresponding to the specified key, otherwise null
        */
        SafeHashMap::Entry::View getEntry(Object::View vKey) const;

        /**
        * Locate an Entry in the hash map based on its key. If the Entry is not in
        * the cache, load the Entry for the specified key and return it.
        *
        * @param oKey  the key to the desired cached Entry
        *
        * @return the Entry corresponding to the specified key, otherwise null
        */
        SafeHashMap::Entry::Handle getEntry(Object::View vKey);

        /**
        * Get all the specified keys, if they are in the cache. For each key
        * that is in the cache, that key and its corresponding value will be
        * placed in the map that is returned by this method. The absence of
        * a key in the returned map indicates that it was not in the cache,
        * which may imply (for caches that can load behind the scenes) that
        * the requested data could not be loaded.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation, without regards to threading issues:
        *
        * <pre><tt>
        * Map map = new AnyMap(); // could be hash map or ...
        * for (Iterator iter = col.iterator(); iter.hasNext(); )
        *     {
        *     Object oKey = iter.next();
        *     Object oVal = get(oKey);
        *     if (oVal != null || containsKey(oKey))
        *         {
        *         map.put(oKey, oVal);
        *         }
        *     }
        * return map;
        * </tt></pre>
        *
        * @param colKeys  a collection of keys that may be in the named cache
        *
        * @return a Map of keys to values for the specified keys passed in
        *         <tt>col</tt>
        */
        virtual Map::View getAll(Collection::View vColKeys) const;

        /**
        * Indicates to the cache that the specified key should be loaded into the
        * cache, if it is not already in the cache. This provides a means to
        * "pre-load" a single entry into the cache using the cache's loader.
        *
        * If a valid entry with the specified key already exists in the cache,
        * or if the cache does not have a loader, then this method has no effect.
        *
        * An implementation may perform the load operation asynchronously.
        *
        * @param oKey  the key to request to be loaded
        */
        virtual void load(Object::View vKey) const;

        /**
        * Indicates to the cache that the specified key should be loaded into the
        * cache, if it is not already in the cache. This provides a means to
        * "pre-load" a single entry into the cache using the cache's loader.
        *
        * If a valid entry with the specified key already exists in the cache,
        * or if the cache does not have a loader, then this method has no effect.
        *
        * An implementation may perform the load operation asynchronously.
        *
        * @param oKey  the key to request to be loaded
        */
        virtual void load(Object::View vKey);

        /**
        * Indicates to the cache that it should load data from its loader to
        * fill the cache; this is sometimes referred to as "pre-loading" or
        * "warming" a cache.
        *
        * The specific set of data that will be loaded is unspecified. The
        * implementation may choose to load all data, some specific subset
        * of the data, or no data. An implementation may require that the
        * loader implement the IterableCacheLoader interface in order for
        * this method to load any data.
        *
        * An implementation may perform the load operation asynchronously.
        */
        virtual void loadAll();

        /**
        * Indicates to the cache that the specified keys should be loaded into
        * the cache, if they are not already in the cache. This provides a means
        * to "pre-load" entries into the cache using the cache's loader.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation:
        *
        * <pre><tt>
        * CacheLoader loader = getCacheLoader();
        * if (loader != null && !colKeys.isEmpty())
        *     {
        *     Set setRequest = new HashSet(colKeys);
        *     setRequest.removeAll(peekAll(colKeys).keySet());
        *     if (!setRequest.isEmpty())
        *         {
        *         Map map = loader.loadAll(colKeys);
        *         if (!map.isEmpty())
        *             {
        *             putAll(map);
        *             }
        *         }
        *     }
        * </tt></pre>
        *
        * @param colKeys  a collection of keys to request to be loaded
        */
        virtual void loadAll(Collection::View vColKeys);

        /**
        * Indicates to the cache that the specified keys should be loaded into
        * the cache, if they are not already in the cache. This provides a means
        * to "pre-load" entries into the cache using the cache's loader.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation:
        *
        * <pre><tt>
        * CacheLoader loader = getCacheLoader();
        * if (loader != null && !colKeys.isEmpty())
        *     {
        *     Set setRequest = new HashSet(colKeys);
        *     setRequest.removeAll(peekAll(colKeys).keySet());
        *     if (!setRequest.isEmpty())
        *         {
        *         Map map = loader.loadAll(colKeys);
        *         if (!map.isEmpty())
        *             {
        *             putAll(map);
        *             }
        *         }
        *     }
        * </tt></pre>
        *
        * @param colKeys  a collection of keys to request to be loaded
        */
        virtual void loadAll(Collection::View vColKeys) const;

        /**
        * Checks for a valid entry corresponding to the specified key in the
        * cache, and returns the corresponding value if it is. If it is not in
        * the cache, returns null, and does not attempt to load the value using
        * its cache loader.
        *
        * @param oKey  the key to "peek" into the cache for
        *
        * @return the value corresponding to the specified key
        */
        virtual Object::Holder peek(Object::View vKey) const;

        /**
        * Checks for a valid entry corresponding to each specified key in the
        * cache, and places the corresponding value in the returned map if it is.
        * For each key that is not in the cache, no entry is placed into the
        * returned map. The cache does not attempt to load any values using
        * its cache loader.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation, without regards to threading issues:
        *
        * <pre><tt>
        * Map::Handle hMap = HashMap::create();
        * for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
        *     {
        *     Object::View   vKey    = hIter->next();
        *     Object::Holder ohValue = peek(ohKey);
        *     if (NULL != ohValue || containsKey(vKey))
        *         {
        *         hMap->put(vKey, ohValue);
        *         }
        *     }
        * return hMap;
        * </tt></pre>
        *
        * @param vColKeys  a collection of keys to "peek" into the cache for
        *
        * @return a Map of keys that were found in the cache and their values
        */
        virtual Map::Handle peekAll(Collection::View vColKeys) const;

        /**
        * Return the default KeyMask for a LocalCache
        *
        * @return the default KeyMask for a LocalCache
        */
        static KeyMask::Handle getDefaultKeyMask();


    // ----- internal -------------------------------------------------------

    protected:
        /**
        * Determine the store used by this LocalCache, if any.
        *
        * @return the CacheStore used by this LocalCache, or null if none
        */
        virtual CacheStore::View getCacheStore() const;

        /**
        * Determine the store used by this LocalCache, if any.
        *
        * @return the CacheStore used by this LocalCache, or null if none
        */
        virtual CacheStore::Handle getCacheStore();

        /**
        * ThreadLocal: Get the current key mask for the current thread.
        *
        * @return the current key mask
        */
        virtual KeyMask::View getKeyMask() const;

        /**
        * ThreadLocal: Get the current key mask for the current thread.
        *
        * @return the current key mask
        */
        virtual KeyMask::Handle getKeyMask();

        /**
        * ThreadLocal: Set the key mask for the current thread.
        *
        * @param mask  the new key mask, or null to clear the mask
        */
        virtual void setKeyMask(KeyMask::Handle hMask);

        /**
        * Remove an entry because it has expired.
        *
        * @param hEntry           the expired cache entry
        * @param fRemoveInternal  true if the cache entry still needs to be
        *                         removed from the cache
        */
        virtual void removeExpired(OldCache::Entry::Handle hEntry,
                bool fRemoveInternal);

        /**
        * {@inheritDoc}
        */
        using OldCache::removeExpired;

        /**
        * Factory pattern: instantiate a new CacheEvent corresponding
        * to the specified parameters.
        *
        * @return a new instance of the CacheEvent class (or a subclass thereof)
        */
        virtual MapEvent::Handle instantiateMapEvent(int32_t nId,
                Object::View vKey, Object::Holder ohValueOld,
                Object::Holder ohValueNew);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The loader used by this cache for misses.
        */
        MemberHandle<CacheLoader> m_hLoader;

        /**
        * The store used by this cache for modifications. If this value is
        * non-null, then it is the same reference as the loader.
        */
        MemberHandle<CacheStore> m_hStore;

        /**
        * The map listener used by this cache to listen to itself in order to
        * pass events to the CacheStore. Only used when there is a CacheStore.
        */
        MemberHandle<MapListener> m_hListener;

        /**
        * The thread-local object to check for keys that the current thread
        * is supposed to ignore if those keys change. Contains KeyMask objects.
        */
        FinalHandle<ThreadLocalReference> f_tloIgnore;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LOCAL_CACHE_HPP
