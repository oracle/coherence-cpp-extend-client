/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OLD_CACHE_HPP
#define COH_OLD_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheMap.hpp"
#include "coherence/net/cache/EvictionPolicy.hpp"
#include "coherence/net/cache/SimpleCacheStatistics.hpp"
#include "coherence/net/cache/UnitCalculator.hpp"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapKeySet.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/MapValuesCollection.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/ObservableMap.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"



COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::InvocableMap;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::MapKeySet;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::MapValuesCollection;
using coherence::util::Muterator;
using coherence::util::ObservableMap;
using coherence::util::SafeHashMap;
using coherence::util::Set;
using coherence::util::ValueExtractor;


/**
* A generic cache manager.
*
* The implementation is thread safe and uses a combination of
* Most Recently Used (MRU) and Most Frequently Used (MFU) caching
* strategies.
*
* The cache is size-limited, which means that once it reaches its maximum
* size ("high-water mark") it prunes itself (to its "low-water mark"). The
* cache high- and low-water-marks are measured in terms of "units", and each
* cached item by default uses one unit. All of the cache constructors, except
* for the default constructor, require the maximum number of units to be
* passed in. To change the number of units that each cache entry uses, either
* set the Units property of the cache entry, or extend the Cache
* implementation so that the inner Entry class calculates its own unit size.
* To determine the current, high-water and low-water sizes of the cache, use
* the cache object's Units, HighUnits and LowUnits properties. The HighUnits
* and LowUnits properties can be changed, even after the cache is in use.
* To specify the LowUnits value as a percentage when constructing the cache,
* use the extended constructor taking the percentage-prune-level.
*
* Each cached entry expires after one hour by default. To alter this
* behavior, use a constructor that takes the expiry-millis; for example, an
* expiry-millis value of 10000 will expire entries after 10 seconds. The
* ExpiryDelay property can also be set once the cache is in use, but it
* will not affect the expiry of previously cached items.
*
* The cache can optionally be flushed on a periodic basis by setting
* the FlushDelay property or scheduling a specific flush time by setting
* the FlushTime property.
*
* Cache hit statistics can be obtained from the CacheHits, CacheMisses,
* HitProbability, KeyHitProbability and CompositeHitProbability read-only
* properties. The statistics can be reset by invoking resetHitStatistics.
* The statistics are automatically reset when the cache is cleared (the clear
* method).
*
* The OldCache implements the ObservableMap interface, meaning it provides
* event notifications to any interested listener for each insert, update and
* delete, including those that occur when the cache is pruned or entries
* are automatically expired.
*
* This implementation is designed to support extension through inheritence.
* When overriding the inner Entry class, the OldCache.instantiateEntry factory
* method must be overridden to instantiate the correct Entry sub-class. To
* override the one-unit-per-entry default behavior, extend the inner Entry
* class and override the calculateUnits method.
*
* The C++ version of OldCache requires a call release(), in order for it to
* be collected.  Not including a call to release will result in the OldCache
* being leaked.
*
* @author nsa 2008.06.23
*/
class COH_EXPORT OldCache
    : public class_spec<OldCache,
         extends<SafeHashMap>,
         implements<ObservableMap, CacheMap> >
    {
    friend class factory<OldCache>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constants ------------------------------------------------------
    public:
        /**
        * By default, the cache size (in units).
        */
        static const size32_t default_units = 1000;

        /**
        * By default, the cache entries expire after one hour.
        */
        static const int32_t default_expire  = 3600000;

        /**
        * By default, expired cache entries are flushed on a minute interval.
        */
        static const int32_t default_flush   = 60000;

        /**
        * Unit calculator configuration enum
        */
        typedef enum
            {
            /**
            * Specifies the default unit calculator that weighs all entries
            * equally as 1.
            */
            unit_calculator_fixed    = 0,
            /**
            * Specifies an external (custom) unit calculator implementation.
            */
            unit_calculator_external = 2
            } UnitCalculatorType;

        /**
        * By default, when the cache prunes, it reduces its entries by 25%,
        * meaning it retains 75% (.75) of its entries.
        */
        static float64_t getDefaultPrune();


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an instanceo of OldCache
        *
        * @param vsName           the Name of the cache to construct
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
        OldCache(size32_t cUnits = default_units,
                int32_t cExpiryMillis = default_expire,
                float64_t dflPruneLevel = 0.75F,
                size32_t cInitialBuckets = 17,
                float32_t flLoadFactor = 1.0F,
                float32_t flGrowthRate = 3.0F);


    // ----- forward declarations -------------------------------------------

    protected:
        class KeySet;
        class ValuesCollection;


    // ----- inner class: Entry ---------------------------------------------

    public:
        /**
        * Entry for the local cache extends SafeHashMap::Entry adding
        * entry statistics used for the various eviction policies
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<SafeHashMap::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new OldCache::Entry
                *
                * @param vKey     the associated key
                * @param ohValue  the associated value
                * @param nHash    the associated hash code
                * @param hCache   reference to the cache containing this
                *                 entry
                */
                Entry(Object::View vKey, Object::Holder ohValue,
                        size32_t nHash, OldCache::Handle hCache);

                /**
                * Copy constructor
                */
                Entry(const Entry& that);

                /**
                * Copy an Entry.
                *
                * @param vThat   the entry to copy
                */
                Entry(Entry::View vThat);

            // ----- LocalCache::Entry interface ------------------------

            public:
                /**
                * Determine if the cache entry has expired.
                *
                * @return true if the cache entry was subject to automatic
                *         expiry and the current time is greater than the
                *         entry's expiry time
                */
                virtual bool isExpired() const;

                /**
                * Calculate a cache priority.
                *
                * @return a value between 0 and 10, 0 being the highest
                *         priority
                */
                virtual int32_t getPriority() const;

            protected:
                /**
                * Called to inform the Entry that it is no longer used.
                */
                virtual void discard();

                /**
                * Determine if this entry has already been discarded from the
                * cache.
                *
                * @return true if this entry has been discarded
                */
                virtual bool isDiscarded();

                /**
                * Reschedule the cache entry expiration.
                */
                virtual void scheduleExpiry();

                /**
                * Called each time the entry is accessed or modified.
                */
                virtual void touch();

                /**
                * Reset the number of times that the cache entry has been
                * touched. The touch count does not get reset to zero, but
                * rather to a fraction of its former self; this prevents long
                * lived items from gaining an unassailable advantage in the
                * eviction process.
                *
                * @since Coherence 3.5
                */
                virtual void resetTouchCount();

            // ----- SafeHashMap::Entry interface -----------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void onAdd();

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

            // ----- helper methods -------------------------------------

            protected:
                /**
                * Calculate a cache cost for the specified object.
                *
                * The default implementation uses the unit calculator type
                * of the containing cache.
                *
                * @param ohValue  the cache value to evaluate for unit cost
                *
                * @return an integer value 0 or greater, with a larger value
                *         signifying a higher cost
                */
                virtual size32_t calculateUnits(Object::Holder ohValue);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;


            // ----- accessors ------------------------------------------

            public:
                /**
                * Determine when the cache entry was created.
                *
                * @return the date/time value, in millis, when the entry was
                * created
                */
                virtual int64_t getCreatedMillis() const;

                /**
                * Determine when the cache entry will expire, if ever.
                *
                * @return the date/time value, in millis, when the entry will
                *         (or did) expire; zero indicates no expiry
                */
                virtual int64_t getExpiryMillis() const;

                /**
                * Determine when the cache entry was last touched.
                *
                * @return the date/time value, in millis, when the entry was
                *         most recently touched
                */
                virtual int64_t getLastTouchMillis() const;

                /**
                * Determine the number of times that the cache entry has been
                * touched.
                *
                * @return the number of times that the cache entry has been
                *         touched
                */
                virtual int32_t getTouchCount() const;

                /**
                * Determine the number of cache units used by this Entry.
                *
                * @return an integer value 0 or greater, with a larger value
                *         signifying a higher cost; -1 implies that the Entry
                *         has been discarded
                */
                virtual size32_t getUnits() const;

                /**
                * Specify when the cache entry will expire, or disable
                * expiry. Note that if the cache is configured for automatic
                * expiry, each subsequent update to this cache entry will
                * reschedule the expiry time.
                *
                * @param lMillis  pass the date/time value, in millis, for
                *                 when the entry will expire, or pass zero to
                *                 disable automatic expiry
                */
                virtual void setExpiryMillis(int64_t lMillis);

                /**
                * Specify the number of cache units used by this Entry.
                *
                * @param cUnits  an integer value 0 or greater, with a larger
                *                value
                */
                virtual void setUnits(int32_t cUnits);

            private:
                /**
                * Package Private: Obtain the next cache entry in the chain of
                * cache entries for a given hash bucket.
                *
                * @return the next cache entry in the hash bucket
                */
                Entry::Handle getNext();

                /**
                * Package Private: Obtain the next cache entry in the chain of
                * cache entries for a given hash bucket.
                *
                * @return the next cache entry in the hash bucket
                */
                Entry::View getNext() const;

                /**
                * Specify the next cache entry in the chain of cache entries
                * for a given hash bucket.
                *
                * @param hEntry  the next cache entry
                */
                void setNext(SafeHashMap::Entry::Handle hEntry);

                /**
                * Determine the most significant bit of the passed integral
                * value.
                *
                * @param n  an int
                *
                * @return -1 if no bits are set; otherwise, the bit position
                *         <tt>p</tt> of the most significant bit such that
                *         <tt>1 &lt;&lt; p</tt> is the most significant bit
                *         of <tt>n</tt>
                */
                int32_t indexOfMSB(size32_t n) const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The time at which this entry was created
                */
                Volatile<int64_t> m_dtCreated;

                /**
                * The time at which this Entry was last accessed.
                */
                Volatile<int64_t> m_dtLastUse;

                /**
                * The time at which this Entry will (or did) expire.
                */
                Volatile<int64_t> m_dtExpiry;

                /**
                * The number of times that this Entry has been accessed.
                */
                int32_t m_cUses;

                /**
                * The number of units for the Entry.
                */
                size32_t m_cUnits;

                /**
                * Reference back to the original cache.
                */
                mutable FinalHandle<OldCache> f_hCache;

            friend class OldCache::KeySet;
            friend class OldCache;
            friend class OldCache::ValuesCollection;
            };

        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(
                Object::View vKey, Object::Holder ohValue,
                size32_t nHash);

        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle instantiateEntry(
                SafeHashMap::Entry::View vThat);


    // ----- inner class: IteratorFilter ------------------------------------

    protected:
        /**
        * This iterator will filter out expired entries from the result set
        * and expire them.
        */
        class IteratorFilter
            : public class_spec<IteratorFilter,
                extends<Object>,
                implements<Filter> >
            {
            friend class factory<IteratorFilter>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * @internal
                */
                IteratorFilter(OldCache::Holder thCache);

            // ----- Filter interface -----------------------------------

            public:
                /**
                * Apply the test to the object.
                *
                * @param v  the object to test
                *
                * @return true if the test passes, false otherwise
                */
                virtual bool evaluate(Object::View v) const;

            // ----- data members ---------------------------------------

            protected:
                FinalHolder<OldCache> f_thCache;
            };


    // ----- inner class: EntrySet -----------------------------------------

    protected:
        /**
        * A set of entries backed by this map.
        */
        class EntrySet
            : public class_spec<EntrySet,
                extends<SafeHashMap::EntrySet> >
            {
            friend class factory<EntrySet>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * @internal
                */
                EntrySet(SafeHashMap::Handle hMap);

                /**
                * @internal
                */
                EntrySet(SafeHashMap::View vMap);

            private:
                /**
                * @internal
                */
                EntrySet(const Entry& that);

            // ----- Set interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Iterator::Handle iterator() const;

                /**
                * {@inheritDoc}
                */
                virtual Muterator::Handle iterator();

                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hao = NULL) const;
            };

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle instantiateEntrySet();

        /**
        * {@inheritDoc}
        */
        virtual Set::View instantiateEntrySet() const;


    // ----- inner class: KeySet --------------------------------------------

    protected:
        /**
        * A set of entries backed by this map.
        */
        class KeySet
            : public class_spec<KeySet,
                extends<MapKeySet> >
            {
            friend class factory<KeySet>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * @internal
                */
                KeySet(SafeHashMap::Handle hMap);

                /**
                * @internal
                */
                KeySet(SafeHashMap::View vMap);


            // ----- Set interface --------------------------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hoa = NULL) const;
            };

        /**
        * {@inheritDoc}
        */
        virtual Set::Handle instantiateKeySet();

        /**
        * {@inheritDoc}
        */
        virtual Set::View instantiateKeySet() const;


    // ----- inner class: ValuesCollection ----------------------------------

    protected:
        /**
        * A set of entries backed by this map.
        */
        class ValuesCollection
            : public class_spec<ValuesCollection,
                extends<MapValuesCollection> >
            {
            friend class factory<ValuesCollection>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * @internal
                */
                ValuesCollection(SafeHashMap::Handle hMap);

                /**
                * @internal
                */
                ValuesCollection(SafeHashMap::View vMap);

            // ----- Collection interface -------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual ObjectArray::Handle toArray(
                        ObjectArray::Handle hoa = NULL) const;
            };

        /**
        * {@inheritDoc}
        */
        virtual Collection::Handle instantiateValuesCollection();

        /**
        * {@inheritDoc}
        */
        virtual Collection::View instantiateValuesCollection() const;


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


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const;

        /**
        * {@inheritDoc}
        */
        virtual bool containsKey(Object::View vKey) const;

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
        virtual void clear();


    // ----- CacheMap interface ---------------------------------------------
    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue,
                int64_t cMillis);

        /**
        * {@inheritDoc}
        */
        virtual Map::View getAll(Collection::View vColKeys) const;


    // ----- Cache management methods ---------------------------------------

    public:
        /**
        * Release local resources associated with the Cache.
        *
        * Releasing a cache makes it no longer usable
        */
        virtual void release();

        /**
        * Evict a specified key from the cache, as if it had expired from the
        * cache. If the key is not in the cache, then the method has no effect.
        *
        * @param oKey  the key to evict from the cache
        */
        virtual void evict(Object::View vKey);

        /**
        * Evict the specified keys from the cache, as if they had each expired
        * from the cache.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation:
        *
        * <tt>
        * for (Iterator iter = colKeys.iterator(); iter.hasNext(); )
        *     {
        *     Object oKey = iter.next();
        *     evict(oKey);
        *     }
        * </tt>
        *
        * @param colKeys  a collection of keys to evict from the cache
        */
        virtual void evictAll(Collection::View vColKeys);

        /**
        * Evict all entries from the cache that are no longer valid, and
        * potentially prune the cache size if the cache is size-limited
        * and its size is above the caching low water mark.
        */
        virtual void evict();

        /**
        * Returns the CacheStatistics for this cache.
        *
        * @return a CacheStatistics object
        */
        virtual CacheStatistics::View getCacheStatistics() const;

        /**
        * Returns the CacheStatistics for this cache.
        *
        * @return a CacheStatistics object
        */
        virtual CacheStatistics::Handle getCacheStatistics();

        /**
        * Determine the number of units that the cache currently stores.
        *
        * @return the current size of the cache in units
        */
        virtual size32_t getUnits() const;

        /**
        * Determine the limit of the cache size in units. The cache will prune
        * itself automatically once it reaches its maximum unit level. This is
        * often referred to as the "high water mark" of the cache.
        *
        * @return the limit of the cache size in units
        */
        virtual size32_t getHighUnits() const;

        /**
        * Update the maximum size of the cache in units. This is often referred
        * to as the "high water mark" of the cache.
        *
        * @param cMax  the new maximum size of the cache, in units
        */
        virtual void setHighUnits(size32_t cMax);

        /**
        * Determine the point to which the cache will shrink when it prunes.
        * This is often referred to as a "low water mark" of the cache.
        *
        * @return the number of units that the cache prunes to
        */
        virtual size32_t getLowUnits() const;

        /**
        * Specify the point to which the cache will shrink when it prunes.
        * This is often referred to as a "low water mark" of the cache.
        *
        * @param cUnits  the number of units that the cache prunes to
        */
        virtual void setLowUnits(size32_t cUnits);

        /**
        * Determine the current eviction type.
        *
        * @return one of the EVICTION_POLICY_* enumerated values
        */
        virtual EvictionPolicy::EvictionPolicyType getEvictionType() const;

        /**
        * Specify the eviction type for the cache. The type can only be
        * set to an external policy if an EvictionPolicy object has been
        * provided.
        *
        * @param nType  one of the EVICTION_POLICY_* enumerated values
        */
        virtual void setEvictionType(EvictionPolicy::EvictionPolicyType nType);

        /**
        * Determine the current external eviction policy, if any.
        *
        * @return the external eviction policy, if one has been
        *         provided
        */
        virtual EvictionPolicy::View getEvictionPolicy() const;

        /**
        * Determine the current external eviction policy, if any.
        *
        * @return the external eviction policy, if one has been provided
        */
        virtual EvictionPolicy::Handle getEvictionPolicy();

        /**
        * Set the external eviction policy, and change the eviction type to
        * eviction_policy_external. If null is passed, clear the external
        * eviction policy, and use the default internal policy.
        *
        * @param policy  an external eviction policy, or null to use the default
        *                policy
        */
        virtual void setEvictionPolicy(EvictionPolicy::Handle hPolicy);

        /**
        * Determine the current unit calculator type.
        *
        * @return one of the UNIT_CALCULATOR_* enumerated values
        */
        virtual UnitCalculatorType getUnitCalculatorType() const;

        /**
        * Specify the unit calculator type for the cache. The type can only be
        * set to an external unit calculator if a UnitCalculator object has been
        * provided.
        *
        * @param nType  one of the UNIT_CALCULATOR_* enumerated values
        */
        virtual void setUnitCalculatorType(UnitCalculatorType nType);

        /**
        * Determine the current external unit calculator, if any.
        *
        * @return the external unit calculator, if one has been provided
        */
        virtual UnitCalculator::View getUnitCalculator() const;

        /**
        * Set the external unit calculator, and change the unit calculator type to
        * unit_calculator_external. If null is passed, clear the external
        * unit calculator, and use the default unit calculator.
        *
        * @param calculator  an external unit calculator, or null to use the default
        *                    unit calculator
        */
        virtual void setUnitCalculator(UnitCalculator::Handle hCalculator);

        /**
        * Determine the "time to live" for each individual cache entry.
        *
        * @return the number of milliseconds that a cache entry value will live,
        *         or zero if cache entries are never automatically expired
        */
        virtual int32_t getExpiryDelay() const;

        /**
        * Specify the "time to live" for cache entries. This does not affect
        * the already-scheduled expiry of existing entries.
        *
        * @param cMillis  the number of milliseconds that cache entries will
        *                 live, or zero to disable automatic expiry
        */
        virtual void setExpiryDelay(int cMillis);

        /**
        * Determine the delay between cache flushes.
        *
        * @return the number of milliseconds between cache flushes, or zero which
        *         signifies that the cache never flushes
        */
        virtual int32_t getFlushDelay() const;

        /**
        * Specify the delay between cache flushes.
        *
        * @param cMillis  the number of milliseconds between cache flushes, or
        *                 zero to never flush
        */
        virtual void setFlushDelay(int32_t cMillis);

        /**
        * Determine the date/time at which the next cache flush is scheduled.
        * Note that the date/time may be Long.max_value, which implies that a
        * flush will never occur. Also note that the cache may internally adjust
        * the flush time to prevent a flush from occurring during certain
        * processing as a means to raise concurrency.
        *
        * @return the date/time value, in milliseconds, when the cache will next
        *         automatically flush
        */
        virtual int64_t getFlushTime() const;

        /**
        * Specify the date/time at which the next cache flush is to occur.
        * Note that the date/time may be Long.max_value, which implies that a
        * flush will never occur. A time in the past or at the present will
        * cause an immediate flush.
        *
        * @param lMillis  the date/time value, in milliseconds, when the cache
        *                 should next automatically flush
        */
        virtual void setFlushTime(int64_t lMillis);

        /**
        * Specify if the cache is allowed to hold mutable values.
        *
        * If false the cache will ensure immutability before caching any value.
        *
        * @param fAllow  true if the cache is allowed to hold mutable values
        */
        virtual void setAllowMutableValues(bool fAllow);

        /**
        * Return whether the cache is allowed to hold mutable values.
        *
        * If false the cache will ensure immutability before caching any value.
        *
        * @return  true if the cache is allowed to hold mutable values
        */
        virtual bool isAllowMutableValues() const;

    // ----- statistics -----------------------------------------------------

    public:
        /**
        * Determine the rough number of cache hits since the cache statistics
        * were last reset.
        *
        * @return the number of {@link #get} calls that have been served by
        *         existing cache entries
        */
        virtual int64_t getCacheHits() const;

        /**
        * Determine the rough number of cache misses since the cache statistics
        * were last reset.
        *
        * @return the number of {@link #get} calls that failed to find an
        *         existing cache entry because the requested key was not in the
        *         cache
        */
        virtual int64_t getCacheMisses() const;

        /**
        * Determine the rough probability (0 <= p <= 1) that any particular
        * {@link #get} invocation will be satisfied by an existing entry in
        * the cache, based on the statistics collected since the last reset
        * of the cache statistics.
        *
        * @return the cache hit probability (0 <= p <= 1)
        */
        virtual float64_t getHitProbability() const;

        /**
        * Reset the cache statistics.
        */
        virtual void resetHitStatistics();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::View getEntry(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual SafeHashMap::Entry::Handle getEntry(Object::View vKey);

    protected:
        /**
        * Configure the eviction type and policy.
        *
        * @param nType   one of the EVICTION_POLICY_* enumerated values
        * @param hPolicy  an external eviction policy, or null
        */
        virtual void configureEviction(EvictionPolicy::EvictionPolicyType nType,
                EvictionPolicy::Handle hPolicy);

        /**
        * Configure the unit calculator type and implementation.
        *
        * @param nType       one of the UNIT_CALCULATOR_* enumerated values
        * @param hCalculator  an external unit calculator, or null
        */
        virtual void configureUnitCalculator(UnitCalculatorType nType,
                UnitCalculator::Handle hCalculator);

        /**
        * Locate an Entry in the hash map based on its key. If the Entry has
        * expired, it is removed from the hash map.
        *
        * Unlike the {@link #getEntry} method, this method does not flush the cache
        * (if necessary) or update cache statistics.
        *
        * @param oKey the key object to search for
        *
        * @return the Entry or null if the entry is not found in the hash map or
        *         has expired
        */
        virtual SafeHashMap::Entry::Handle getEntryInternal(Object::View vKey) const;

        /**
        * Check if the cache is timed out, and clear if it is.
        */
        virtual void checkFlush() const;

        /**
        * Check if the cache is timed out, and clear if it is.
        */
        virtual void checkFlush();

        /**
        * Remove an entry because it has expired.
        *
        * @param vEntry           the expired cache entry
        * @param fRemoveInternal  true if the cache entry still needs to be
        *                         removed from the cache
        */
        virtual void removeExpired(Entry::View vEntry, bool fRemoveInternal) const;

        /**
        * Remove an entry because it has expired.
        *
        * @param hEntry           the expired cache entry
        * @param fRemoveInternal  true if the cache entry still needs to be
        *                         removed from the cache
        */
        virtual void removeExpired(Entry::Handle hEntry, bool fRemoveInternal);

        /**
        * Increment the current size
        */
        virtual void incrementUnits(size32_t cDelta);

        /**
        * Decrement the current size
        */
        virtual void decrementUnits(size32_t cDelta);

        /**
        * Check if the cache is too big, and if it is prune it by discarding the
        * lowest priority cache entries.
        */
        virtual void checkSize();

        /**
        * Prune the cache by discarding the lowest priority cache entries.
        */
        virtual void prune();

        /**
        * Defer the next flush by scheduling it for infinity and beyond.
        */
        virtual void deferFlush();

        /**
        * Schedule the next flush.
        */
        virtual void scheduleFlush();

        /**
        * Factory pattern: instantiate a new MapEvent corresponding
        * to the specified parameters.
        *
        * @return a new instance of the MapEvent class (or a subclass thereof)
        */
        virtual MapEvent::Handle instantiateMapEvent(
                int32_t nId, Object::View vKey, Object::Holder ohValueOld, Object::Holder ohValueNew);


    // ----- event dispatching ----------------------------------------------

    protected:
        /**
        * Accessor for the MapListenerSupport for sub-classes.
        *
        * @return the MapListenerSupport, or null if there are no listeners
        */
        virtual MapListenerSupport::Handle getMapListenerSupport();

        /**
        * Determine if the OverflowMap has any listeners at all.
        *
        * @return true iff this OverflowMap has at least one MapListener
        */
        virtual bool hasListeners();

        /**
        * Dispatch the passed event.
        *
        * @param hEvt  a CacheEvent object
        */
        virtual void dispatchEvent(MapEvent::Handle hEvt);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The current number of units in the cache. A unit is an undefined means
        * of measuring cached values, and must be 0 or positive. The particular
        * Entry implementation being used defines the meaning of unit.
        */
        size32_t  m_cCurUnits;

        /**
        * The number of units to allow the cache to grow to before pruning.
        */
        size32_t  m_cMaxUnits;

        /**
        * The percentage of the total number of units that will remain after
        * the cache manager prunes the cache (i.e. this is the "low water
        * mark" value); this value is in the range 0.0 to 1.0.
        */
        float64_t  m_dflPruneLevel;

        /**
        * The number of units to prune the cache down to.
        */
        size32_t  m_cPruneUnits;

        /**
        * The number of milliseconds that a value will live in the cache.
        * Zero indicates no timeout.
        */
        size32_t m_cExpiryDelay;

        /**
        * The interval between full cache flushes, in milliseconds.
        */
        int32_t  m_cFlushDelay;

        /**
        * The time (ie System.currentTimeMillis) at which the next full cache
        * flush should occur.
        */
        Volatile<int64_t> m_lNextFlush;

        /**
        * The CacheStatistics object maintained by this cache.
        */
        mutable FinalHandle<SimpleCacheStatistics> f_hStats;

        /**
        * The MapListenerSupport object.
        */
        MemberHandle<MapListenerSupport> m_hListenerSupport;

        /**
        * The type of eviction policy employed by the cache; one of the
        * EVICTION_POLICY_* enumerated values.
        */
        EvictionPolicy::EvictionPolicyType m_nEvictionType;

        /**
        * The eviction policy; for eviction type eviction_policy_external.
        */
        MemberHandle<EvictionPolicy> m_hPolicy;

        /**
        * The type of unit calculator employed by the cache; one of the
        * UNIT_CALCULATOR_* enumerated values.
        */
        UnitCalculatorType m_CalculatorType;

        /**
        * The external unit calculator.
        */
        MemberView<UnitCalculator> m_vCalculator;

        /**
        * The last time that a prune was run. This value is used by the
        * hybrid eviction policy.
        * @since Coherence 3.5
        */
        int64_t m_lLastPrune;

        /**
        * For a prune cycle, this value is the average number of touches that
        * an entry should have. This value is used by the hybrid eviction
        * policy.
        * @since Coherence 3.5
        */
        size32_t m_cAvgTouch;

        /**
        * Allow mutable values.
        */
        bool m_fAllowMutableValues;

    // ----- friends --------------------------------------------------------

    friend class IteratorFilter;
    friend class KeySet;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LOCAL_CACHE_HPP
