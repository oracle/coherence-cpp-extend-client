/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_STORE_HPP
#define COH_CACHE_STORE_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheLoader.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Map;


/**
* A cache store.
*
* @author jh  2008.03.12
*/
class COH_EXPORT CacheStore
    : public interface_spec<CacheStore,
        implements<CacheLoader> >
    {
    // ----- CacheStore interface -------------------------------------------

    public:
        /**
        * Store the specified value under the specified key in the underlying
        * store. This method is intended to support both key/value creation
        * and value update for a specific key.
        *
        * @param vKey     key to store the value under
        * @param ohValue  value to be stored
        *
        * @throws UnsupportedOperationException if this implementation or the
        *         underlying store is read-only
        */
        virtual void store(Object::View vKey, Object::Holder ohValue) = 0;

        /**
        * Store the specified values under the specified keys in the
        * underlying store. This method is intended to support both key/value
        * creation and value update for the specified keys.
        *
        * @param vMapEntries  a Map of any number of keys and values to store
        *
        * @throws UnsupportedOperationException if this implementation or the
        *         underlying store is read-only
        */
        virtual void storeAll(Map::View vMapEntries) = 0;

        /**
        * Remove the specified key from the underlying store if present.
        *
        * @param vKey key whose mapping is being removed from the cache
        *
        * @throws UnsupportedOperationException if this implementation or the
        *         underlying store is read-only
        */
        virtual void erase(Object::View vKey) = 0;

        /**
        * Remove the specified keys from the underlying store if present.
        *
        * @param vColKeys  keys whose mappings are being removed from the
        *                  cache
        *
        * @throws UnsupportedOperationException if this implementation or the
        *         underlying store is read-only
        */
        virtual void eraseAll(Collection::View vColKeys) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_STORE_HPP
