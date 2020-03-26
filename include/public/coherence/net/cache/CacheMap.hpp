/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_MAP_HPP
#define COH_CACHE_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Map;


/**
* A CacheMap is a coherence::util::Map that supports caching.
*
* CacheMaps maintain immutable copies of the cached entries. Thus any
* non-immutable item supplied to the cache will be cloned.  All results from
* the cache will be accessable only via Views.
*
* @author mf  2007.11.07
*/
class COH_EXPORT CacheMap
    : public interface_spec<CacheMap,
        implements<Map> >
    {
    // ----- CacheMap interface ---------------------------------------------

    public:
        /**
        * Get all the specified keys, if they are in the %cache. For each key
        * that is in the %cache, that key and its corresponding value will be
        * placed in the map that is returned by this method. The absence of a
        * key in the returned map indicates that it was not in the %cache,
        * which may imply (for caches that can load behind the scenes) that
        * the requested data could not be loaded.
        *
        * The result of this method is defined to be semantically the same as
        * the following implementation, without regards to threading issues:
        * @code
        * // could be a HashMap (but does not have to)
        * Map::Handle hMap = SomeMap::create();
        *
        * for (Iterator::Handle iter = colKeys->iterator(); iter->hasNext();)
        *     {
        *     Object::View hKey = iter->next();
        *     Object::View hVal = get(hKey);
        *     if (hVal || containsKey(hKey))
        *         {
        *         hMap->put(hKey, hVal);
        *         }
        *     }
        * return hMap;
        * @endcode
        *
        * @param vKeys a collection of keys that may be in the named %cache
        *
        * @return a coherence::util::Map of keys to values for the specified
        * keys passed in @a vKeys
        */
        virtual Map::View getAll(Collection::View vKeys) const = 0;

        /**
        * Associates the specified value with the specified key in this
        * cache. If the cache previously contained a mapping for this key,
        * the old value is replaced. This variation of the
        * put(Object::View, Object::View) method allows the caller to specify
        * an expiry (or "time to live") for the %cache entry.
        *
        * @param vKey    key with which the specified value is to be
        *                 associated
        * @param ohValue  value to be associated with the specified key
        * @param cMillis  the number of milliseconds until the %cache entry
        *                 will expire, also referred to as the entry's "time
        *                 to live"; pass #expiry_default to use the %cache's
        *                 default time-to-live setting; pass #expiry_never to
        *                 indicate that the %cache entry should never expire;
        *                 this milliseconds value is <b>not</b> a date/time
        *                 value
        *
        * @return previous value associated with specified key, or empty
        *         handle if there was no mapping for key. An empty handle
        *         return can also indicate that the map previously associated
        *         NULL with the specified key, if the implementation supports
        *         NULL values
        *
        * @throws coherence::lang::UnsupportedOperationException
        *         if the requested expiry is a positive value and the
        *         implementation does not support expiry of %cache entries
        */
        virtual Object::Holder put
            (Object::View vKey, Object::Holder ohValue, int64_t cMillis) = 0;

        /**
        * Associates the specified value with the specified key in this
        * cache. If the cache previously contained a mapping for this key,
        * the old value is replaced.
        *
        * Invoking this method is equivalent to the following call:
        * @code
        * put(vKey, vValue, expiry_default);
        * @endcode
        *
        * @param vKey    key with which the specified value is to be
        *                 associated
        * @param ohValue  value to be associated with the specified key
        *
        * @return previous value associated with specified key, or empty
        *         handle if there was no mapping for key. An empty handle
        *         return can also indicate that the map previously associated
        *         NULL with the specified key, if the implementation supports
        *         NULL values
        */
        using Map::put;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * A special time-to-live value that can be passed to the extended
        * put(Object#Handle, Object#Handle, int64_t) method to indicate that
        * the %cache's default expiry should be used.
        */
        static const int64_t expiry_default = 0;

        /**
        * A special time-to-live value that can be passed to the extended
        * put(Object#Handle, Object#Handle, int64_t) method to indicate that the
        * %cache entry should never expire.
        */
        static const int64_t expiry_never   = -1;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_MAP_HPP
