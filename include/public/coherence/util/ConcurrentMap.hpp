/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONCURRENT_MAP_HPP
#define COH_CONCURRENT_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* Map with additional concurrency features.
*
* @author jh  2008.03.05
*/
class COH_EXPORT ConcurrentMap
    : public interface_spec<ConcurrentMap,
        implements<Map> >
    {
    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- ConcurrentMap interface ----------------------------------------

    public:
        /**
        * Attempt to lock the specified item within the specified period of
        * time.
        *
        * The item doesn't have to exist to be <i>locked</i>. While the item
        * is locked there is known to be a <i>lock holder</i> which has an
        * exclusive right to modify (calling put and remove methods) that
        * item.
        *
        * Lock holder is an abstract concept that depends on the
        * ConcurrentMap implementation. For example, holder could be a
        * process or a thread (or both).
        *
        * Locking strategy may vary for concrete implementations as well.
        * Lock could have an expiration time (this lock is sometimes called a
        * "lease") or be held indefinitely (until the lock holder
        * terminates).
        *
        * Some implementations may allow the entire map to be locked. If the
        * map is locked in such a way, then only a lock holder is allowed to
        * perform any of the "put" or "remove" operations. Pass the special
        * constant {@link #getLockAll()} as the <i>vKey</i> parameter to
        * indicate the map lock.
        *
        * @param vKey  key being locked
        * @param cWait the number of milliseconds to continue trying to
        *              obtain a lock; pass zero to return immediately; pass
        *              -1 to block the calling thread until the lock could be
        *              obtained
        *
        * @return true if the item was successfully locked within the
        *         specified time; false otherwise
        */
        virtual bool lock(Object::View vKey, int64_t cWait) const = 0;

        /**
        * Attempt to lock the specified item and return immediately.
        *
        * This method behaves exactly as if it simply performs the call
        * <tt>lock(vKey, 0)</tt>.
        *
        * @param vKey key being locked
        *
        * @return true if the item was successfully locked; false otherwise
        */
        virtual bool lock(Object::View vKey) const = 0;

        /**
        * Unlock the specified item. The item doesn't have to exist to be
        * <i>unlocked</i>. If the item is currently locked, only the
        * <i>holder</i> of the lock could successfully unlock it.
        *
        * @param vKey key being unlocked
        *
        * @return true if the item was successfully unlocked; false otherwise
        */
        virtual bool unlock(Object::View vKey) const = 0;


    // ----- inner class: LockBlock -----------------------------------------

    public:
        /**
        * The LockBlock allows for easy creation of ConcurrentMap::lock code.
        * It's used on conjunction with the COH_CACHE_LOCK macros.
        */
        class LockBlock
            {
            // ----- constructors ---------------------------------------

            public:
                /**
                * Construct a LockBlock that will lock the supplied map.
                *
                * @param hMap   the map to lock
                * @param vKey   the Key to lock int he map
                * @param cWait  the number of milliseconds to continue trying
                *               to obtain a lock; pass zero to return
                *               immediately; pass -1 to block the calling
                *               thread until the lock could be obtained
                */
                LockBlock(ConcurrentMap::Handle hMap, Object::View vKey,
                        bool fTryLock, int64_t cWait = -1L);

                /**
                * Copy constructor used by the COH_CACHE_LOCK macros
                */
                LockBlock(const LockBlock& that);

                /**
                * Destroy an LockBlock object, exiting the associated map
                */
                ~LockBlock();

            // ----- LockBlock interface ---------------------------------

            public:
                /**
                * Boolean conversion for the COH_CACHE_LOCK macros
                *
                * @return true if it's a time based lock and false if no
                */
                operator bool() const;

            private:
                /**
                * Blocked assignment operator
                */
                const LockBlock& operator=(const LockBlock&);

                /**
                * Blocked dynamic allocation.
                */
                static void* operator new(size_t);

            // ----- data members ---------------------------------------

            protected:
                /**
                * Map to lock and unlock
                */
                mutable ConcurrentMap::Handle m_hMap; // on stack

                /**
                * Key to lock and unlock in the associated Map
                */
                const Object::View m_vKey; // on stack

                /**
                * Flag describing the type of lock being used
                */
                const bool m_fTryLock;
            };


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Special key value indicating an intent to lock the entire map.
        */
        static Object::View getLockAll();
    };


// ----- non-member operators and functions ---------------------------------


/**
* Macro for more readable cache locking blocks. See the ConcurrentMap docs
* for more details.
*/
#define COH_CACHE_LOCK(MAP, KEY) \
    if (coherence::util::ConcurrentMap::LockBlock COH_UNIQUE_IDENTIFIER(_coh_cache_lock_) \
        = coherence::util::ConcurrentMap::LockBlock(MAP, KEY, false, -1L)) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
        else


#define COH_CACHE_TRYLOCK(MAP, KEY, TIME) \
    if (coherence::util::ConcurrentMap::LockBlock COH_UNIQUE_IDENTIFIER(_coh_cache_trylock_) \
            = coherence::util::ConcurrentMap::LockBlock( \
                coherence::util::ConcurrentMap::LockBlock(MAP, KEY, true, TIME)))

COH_CLOSE_NAMESPACE2

#endif // COH_CONCURRENT_MAP_HPP
