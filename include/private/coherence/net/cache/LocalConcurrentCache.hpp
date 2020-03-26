/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_CONCURRENT_CACHE_HPP
#define COH_LOCAL_CONCURRENT_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/LocalCache.hpp"

#include "coherence/util/ConcurrentMap.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/ThreadGate.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::ConcurrentMap;
using coherence::util::SafeHashMap;
using coherence::util::ThreadGate;


/**
* LocalConcurrentcache implements the ConcurrentMap interface based on
* LocalCache.
*
* @author nsa 2008.07.31
*/
class COH_EXPORT LocalConcurrentCache
    : public class_spec<LocalConcurrentCache,
        extends<LocalCache>,
        implements<ConcurrentMap> >
    {
    friend class factory<LocalConcurrentCache>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct an instance of LocalConcurrentCache
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
        LocalConcurrentCache(bool         fEnforceLocking = false,
                int64_t                   cWaitMillis     = -1L,
                int32_t                   cUnits          = default_units,
                int32_t                   cExpiryMillis   = default_expire,
                float64_t                 dflPruneLevel   = 0.75F,
                size32_t                  cInitialBuckets = 17,
                float32_t                 flLoadFactory   = 1.0F,
                float32_t                 flGrowthRate    = 3.0F);


    // ----- inner class: Lock ----------------------------------------------

    protected:
        /**
        * A lock object.
        */
        class COH_EXPORT Lock
            : public class_spec<Lock>
            {
            friend class factory<Lock>;

            // ----- constructors ---------------------------------------
            protected:
                /**
                * Construct a new Lock object.
                */
                Lock();

            // ----- Lock interface -------------------------------------

            protected:
                /**
                * Assign the ownership of this Lock to the calling thread.
                * <p>
                * Note: caller of this method is expected to hold a synchronization
                * monitor for the Lock object while making this call.
                *
                * @param cWait the number of milliseconds to continue trying to obtain
                *              a lock; pass zero to return immediately; pass -1 to block
                *              the calling thread until the lock could be obtained
                * @return true if lock was successful; false otherwise
                */
                virtual bool assign(int64_t cWait);

                /**
                * Wait for a Lock release notification.
                * <p>
                * Note: caller of this method is expected to hold a synchronization
                * monitor for the Lock object while making this call.
                *
                * @param cWait the number of milliseconds to continue waiting;
                *              pass -1 to block the calling thread indefinitely
                *
                * @return updated wait time.
                */
                virtual int64_t waitForNotify(int64_t cWait);

                /**
                * Release this Lock.
                * <p>
                * Note: caller of this method is expected to hold a synchronization
                * monitor for the Lock object while making this call.
                *
                * @return true if unlock is successful; false if the entry remained locked
                */
                virtual bool release();

                /**
                * Checks whether or not this Lock object is held by another thread.
                * <p>
                * Note: caller of this method is expected to hold a synchronization
                * monitor for the Lock object while making this call.
                *
                * @return true if the Lock is held by another thread; false otherwise
                */
                virtual bool isDirty();

                /**
                * Checks whether or not this Lock object is held by the calling thread.
                * <p>
                * Note: unlike other methods of this class, the caller of this method
                * is <i>not</i> required to hold a synchronization monitor for the Lock
                * object while making this call.
                *
                * @return true if the Lock is held by the calling thread; false
                *         otherwise
                */
                virtual bool isOwnedByCaller();

                /**
                * Checks whether or not this Lock object is discardable.
                * <p>
                * Note: caller of this method is expected to hold a synchronization
                * monitor for the Lock object while making this call.
                *
                * @return true if the Lock is discardable; false otherwise
                */
                virtual bool isDiscardable();

                /**
                * Return the Thread object holding this Lock.
                *
                * @return the Thread object holding this Lock.
                */
                virtual Thread::Handle getLockThread();

                /**
                * Return the lock count.
                *
                * @return the lock count
                */
                virtual int32_t getLockCount();

                /**
                * Return the blocked threads count.
                *
                * @return the blocked threads count
                */
                virtual int32_t getBlockCount();

                /**
                * Return a human readable decription of the Lock type
                *
                * @return a human readable decription of the Lock type
                */
                virtual String::View getLockTypeDescription() const;

            // ----- Object interface -----------------------------------

            public:
                /**
                * Return a human readable decription of the Lock
                *
                * @return a human readable decription of the Lock
                */
                virtual TypedHandle<const String> toString() const;

            // ----- Data members ---------------------------------------

            private:
                /**
                * The Thread object holding a lock for this entry.
                */
                MemberHandle<Thread> m_hThread;

                /**
                * The lock count (number of times the "assign" was called by
                * the locking thread).
                */
                int16_t m_cLock;

                /**
                * The number of threads waiting on this Lock to be released.
                */
                int16_t m_cBlock;

            friend class LocalConcurrentCache;
            };


            /**
            * Factory pattern.
            *
            * @return a new instance of the Lock class (or a subclass thereof)
            */
            Lock::Handle instantiateLock(Object::View vKey) const;


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


    // ----- Map interface --------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        * <p>
        * If lock enforcement is required an attempt will be made to lock the
        * entire map using the {@link ConcurrentMap#LOCK_ALL} object.
        * <p>
        * <b>Note:</b> if this operation fails due to a
        * ConcurrentModificationException, then any subset of the current
        * mappings could still remain in the map.
        *
        * @throws ConcurrentModificationException if any entry is locked
        *         by another thread
        */
        virtual void clear();

        /**
        * {@inheritDoc}
        *
        * @throws ConcurrentModificationException if the entry is locked
        *         by another thread
        */
        virtual Object::Holder put(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        using CacheMap::put;

        /**
        * {@inheritDoc}
        *
        * @throws ConcurrentModificationException if the entry is locked
        *         by another thread
        */
        virtual void putAll(Map::View vMap);

        /**
        * {@inheritDoc}
        *
        * @throws ConcurrentModificationException if the entry is locked
        *         by another thread
        */
        virtual Object::Holder remove(Object::View vKey);
        using Map::remove;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the flag indicatinig whether or not the locking is enforced for
        * put, remove and clear operations.
        *
        * @return true if locking is enforced; false otherwise
        */
        virtual bool isLockingEnforced() const;

        /**
        * Set the flag indicatinig whether or not the locking is enforced for
        * put, remove and clear operations.
        *
        * @param fEnforce  pass true to enforce locking; false otherwise
        */
        virtual void setLockingEnforced(bool fEnforce);

        /**
        * Return the number of milliseconds to continue trying to obtain a lock
        * in case when the locking is enforced.
        *
        * @return the wait time in milliseconds
        */
        virtual int64_t getWaitMillis() const;

        /**
        * Specifiy the number of milliseconds to continue trying to obtain a lock
        * in case when the locking is enforced.
        *
        * @param cWaitMillis  the wait time in milliseconds
        */
        virtual void setWaitMillis(int64_t cWaitMillis);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- helpers --------------------------------------------------------


    public:
        /**
        * Make a human-readable description of the information kept about the
        * passed key.
        *
        * @param oKey  the key
        *
        * @return the information known about the passed key
        */
        virtual String::Handle getLockDescription(Object::View vKey);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Flag indicating whether or not the locking is enforced for put, remove
        * and clear operations.
        */
        bool m_fEnforceLocking;

        /**
        * The number of milliseconds to continue trying to obtain a lock
        * in case when the locking is enforced.
        */
        int64_t m_cWaitMillis;

        /**
        * The map containing all the locks.
        */
        mutable FinalHandle<SafeHashMap> f_hMapLock;

        /**
        * The ThreadGate object for the entire map.
        */
        mutable FinalHandle<ThreadGate> f_hGateMap;
    };


COH_CLOSE_NAMESPACE3


#endif // #define COH_LOCAL_CONCURRENT_CACHE_HPP
