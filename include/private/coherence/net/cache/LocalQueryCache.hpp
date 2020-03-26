/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_QUERY_CACHE_HPP
#define COH_LOCAL_QUERY_CACHE_HPP

#include "coherence/lang.ns"

#include "private/coherence/net/cache/LocalConcurrentCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)


/**
* LocalQueryCache implements the QueryMap interface based on LocalCache.
*
* @author nsa 2008.08.01
*/
class COH_EXPORT LocalQueryCache
    : public class_spec<LocalQueryCache,
        extends<LocalConcurrentCache>,
        implements<QueryMap> >
    {
    friend class factory<LocalQueryCache>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct an instance of LocalQueryCache
        *
        * @param fEnforceLocking  if true the locking is enforced for put,
        *                         remove, and clear operations; otherwise a
        *                         client is responsible for calling lock and
        *                         unlock explicitly
        * @param cWaitMillis      if locking enforcement is required then
        *                         this parameter specifies the number of
        *                         milliseconds to continue trying to obtain
        *                         a lock; pass -1 to block the calling thread
        *                         until the lock could be obtained
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
        LocalQueryCache(bool         fEnforceLocking = true,
                int64_t              cWaitMillis     = -1L,
                int32_t              cUnits          = default_units,
                int32_t              cExpiryMillis   = default_expire,
                float64_t            dflPruneLevel   = 0.75F,
                size32_t             cInitialBuckets = 17,
                float32_t            flLoadFactory   = 1.0F,
                float32_t            flGrowthRate    = 3.0F);

    private:
        /**
        * Blocked copy constructor.
        */
        LocalQueryCache(const LocalQueryCache&);


    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * @{inheritDoc}
        */
        virtual Set::View keySet(Filter::View vFilter) const;

        /**
        * {@inheritDoc}
        */
        using Map::keySet;

        /**
        * @{inheritDoc}
        */
        virtual Set::View entrySet(Filter::View vFilter) const;

        /**
        * @{inheritDoc}
        */
        virtual Set::View entrySet(Filter::View vFilter,
                Comparator::View vComparator) const;

        /**
        * {@inheritDoc}
        */
        using Map::entrySet;

        /**
        * @{inheritDoc}
        */
        virtual void addIndex(ValueExtractor::View vExtractor, bool fOrdered,
                Comparator::View vComparator);
        /**
        * @{inheritDoc}
        */
        virtual void removeIndex(ValueExtractor::View vExtractor);


    // ----- helper methods -------------------------------------------------

    protected:
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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The map of indexes maintained by this cache. The keys of the Map are
        * ValueExtractor objects, and for each key, the corresponding value
        * stored in the Map is a MapIndex object.
        */
        FinalHandle<Map> f_hMapIndex;
    };

COH_CLOSE_NAMESPACE3

#endif // #ifndef COH_LOCAL_QUERY_CACHE_HPP
