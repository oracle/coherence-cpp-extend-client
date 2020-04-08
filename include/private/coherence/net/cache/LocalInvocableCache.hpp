/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LOCAL_INVOCABLE_CACHE_HPP
#define COH_LOCAL_INVOCABLE_CACHE_HPP

#include "coherence/lang.ns"

#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/ValueUpdater.hpp"

#include "private/coherence/net/cache/LocalQueryCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::InvocableMap;
using coherence::util::List;
using coherence::util::ValueUpdater;


/**
* LocalInvocableCache implements the InvocableMap interface based on
* LocalCache.
*
* @author nsa 2008.08.08
*/
class COH_EXPORT LocalInvocableCache
    : public class_spec<LocalInvocableCache,
        extends<LocalQueryCache>,
        implements<InvocableMap> >
    {
    friend class factory<LocalInvocableCache>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- inner class: Entry ---------------------------------------------

    public:
        /**
        * Entry that implements the InvocableMap::Entry interface
        */
        class COH_EXPORT Entry
            : public cloneable_spec<Entry,
                extends<LocalCache::Entry>,
                implements<InvocableMap::Entry> >
            {
            friend class factory<Entry>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct a new Entry
                *
                * @param vKey        the key
                * @param ohValue     the value
                * @param nHash       the key's hashcode
                * @param hCache      the associated cache
                * @param fSurrogate  true if this entry is not part of the cache
                */
                Entry(Object::View vKey, Object::Holder ohValue, size32_t nHash,
                        LocalInvocableCache::Handle hCache, bool fSurrogate = false);

                /**
                * Copy constructor
                */
                Entry(const Entry& that);

                /**
                * {@internal}
                */
                Entry(Entry::View vEntry);


            // ----- InvocableMap::Entry interface ----------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue,
                        bool fSynthetic);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder setValue(Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder getValue();

                /**
                * {@inheritDoc}
                */
                virtual void update(ValueUpdater::View vUpdater,
                        Object::Holder ohValue);

                /**
                * {@inheritDoc}
                */
                virtual bool isPresent() const;

                /**
                * {@inheritDoc}
                */
                virtual void remove(bool fSynthetic);

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder extract(ValueExtractor::View vExtractor) const;

                /**
                * Return true if this is a surrogate entry.
                */
                virtual bool isSurrogate() const;

                /**
                * Specify if the entry is a surrogate.
                *
                * @param fSurrogate  true if the entry is to act as a surrogate
                */
                virtual void setSurrogate(bool fSurrogate);


            // ----- data members ---------------------------------------

            protected:
                /**
                * Indicates if this Entry is part of the associated cache.
                * A value of true indicates that it is a surrogate Entry
                * which simply delegates operations to the cache.
                */
                bool m_fSurrogate;
            };

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

        /**
        * Instantiate an Entry which will not be linked into this cache, and
        * instead operations on the entry must delegate to the cache.
        *
        * @param  vKey  the key
        *
        * @return a surrogate entry
        */
        virtual Entry::Handle instantiateSurrogateEntry(Object::View vKey);

        /**
        * Return an entry for the specified key.  If the key does not exist
        * in the cache then a surrogate entry is returned for it.
        *
        * @param vKey  the key
        *
        * @return an entry
        */
        virtual Entry::Handle ensureEntry(Object::View vKey);

        /**
        * Return an entry for the specified key.  If the key does not exist
        * in the cache then a surrogate entry is returned for it.
        *
        * @param vKey  the key
        *
        * @return an entry
        */
        virtual Entry::View ensureEntry(Object::View vKey) const;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct an instance of LocalInvocableCache
        *
        * @param fEnforceLocking  if true the locking is enforced for put,
        *                         remove, and clear operations; otherwise a
        *                         client is responsible for calling lock and
        *                         unlock explicitly
        * @param cWaitMillis      if locking enforcement is required then
        *                         this parameter speicifes the number of
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
        * @return a new LocalInvocableCache
        */
        LocalInvocableCache(bool     fEnforceLocking = false,
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
            LocalInvocableCache(const LocalInvocableCache&);


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


    // ----- helper methods -------------------------------------------------

    private:
        /**
        * Attempt to lock all the specified keys within a specified period of
        * time.
        *
        * @param vCollKeys  a collection of keys to lock
        * @param cWait      the number of milliseconds to continue trying to
        *                   obtain locks; pass zero to return immediately;
        *                   pass -1 to block the calling thread until the
        *                   lock could be obtained
        *
        * @return a List containing all the locked keys in the order opposite
        *          to the locking order (LIFO); null if timeout has occurred
        */
        List::Handle lockAll(Collection::View vCollKeys, int64_t cWait) const;

        /**
        * Create an entry set for the specified collection of keys
        *
        * @param vCollKeys  The set of keys to create an entry set for
        *
        * @return the set of Entries that correspond to vCollKeys.
        */
        Set::Handle makeEntrySet(Collection::View vCollKeys);

        /**
        * Create an entry set for the specified collection of keys
        *
        * @param vCollKeys  The set of keys to create an entry set for
        *
        * @return the set of Entries that correspond to vCollKeys.
        */
        Set::Handle makeEntrySet(Collection::View vCollKeys) const;

    // ----- data members ---------------------------------------------------

    public:
        /**
        * An "unknown value" tag.
        */
        static Object::Handle getUnknown();
    };



COH_CLOSE_NAMESPACE3

#endif // #ifndef COH_LOCAL_INVOCABLE_CACHE_HPP
