/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_NAMED_CACHE_HPP
#define COH_LOCAL_NAMED_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/CacheService.hpp"
#include "coherence/util/Describable.hpp"

#include "private/coherence/net/cache/LocalInvocableCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::net::CacheService;
using coherence::util::Describable;


/**
* LocalNamedCache implements the NamedCache interface based on LocalCache.
*
* @author nsa 2008.08.01
*/
class COH_EXPORT LocalNamedCache
    : public class_spec<LocalNamedCache,
        extends<LocalInvocableCache>,
        implements<NamedCache> >
    {
    friend class factory<LocalNamedCache>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an instance of LocalNamedCache
        *
        * @param vsName           the Name of the cache to construct
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
        *
        * @return a new LocalNamedCache
        */
        LocalNamedCache(String::View vsName          = String::null_string,
                bool                 fEnforceLocking = false,
                int64_t              cWaitMillis     = -1L,
                int32_t              cUnits          = default_units,
                int32_t              cExpiryMillis   = default_expire,
                float64_t            dflPruneLevel   = 0.75F,
                size32_t             cInitialBuckets = 17,
                float32_t            flLoadFactory   = 1.0F,
                float32_t            flGrowthRate    = 3.0F);


    // ----- NamedCache interface -------------------------------------------

    public:
        /**
        * Return the cache name.
        *
        * @return the cache name
        */
        virtual String::View getCacheName() const;

        /**
        * Set the cache name.
        *
        * @param vsName  the cache name
        */
        virtual void setCacheName(String::View vsName);

        /**
        * Return the CacheService that this NamedCache is a part of.
        *
        * @return the CacheService
        */
        virtual CacheService::Handle getCacheService();

        /**
        * Return the CacheService that this NamedCache is a part of.
        *
        * @return the CacheService
        */
        virtual CacheServiceView getCacheService() const;

        /**
        * Specifies whether or not the NamedCache is active.
        *
        * @return true if the NamedCache is active; false otherwise
        */
        virtual bool isActive() const;

        /**
        * Release local resources associated with this instance of
        * NamedCache.
        *
        * Releasing a cache makes it no longer usable, but does not affect
        * the cache itself. In other words, all other references to the cache
        * will still be valid, and the cache data is not affected by
        * releasing the reference. Any attempt to use this reference
        * afterwards will result in an exception.
        *
        * @see CacheService#releaseCache
        */
        virtual void release();

        /**
        * Release and destroy this instance of NamedCache.
        *
        * <b>Warning:</b> This method is used to completely destroy the
        * specified cache across the cluster. All references in the entire
        * cluster to this cache will be invalidated, the cached data will be
        * cleared, and all resources will be released.
        *
        * @see CacheService#destroyCache
        */
        virtual void destroy();


    // ----- Describable interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data members  --------------------------------------------------

    protected:
        /**
        * The name of the cache.
        */
        FinalView<String> f_vsName;

        /**
        * Flag signaling whether this cache is active or not
        */
        bool m_fIsActive;
    };


COH_CLOSE_NAMESPACE3

#endif // #define COH_LOCAL_NAMED_CACHE_HPP
