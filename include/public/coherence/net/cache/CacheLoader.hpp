/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_LOADER_HPP
#define COH_CACHE_LOADER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Map;


/**
* A cache loader.
*
* @author jh  2008.03.12
*/
class COH_EXPORT CacheLoader
    : public interface_spec<CacheLoader>
    {
    // ----- CacheLoader interface ------------------------------------------

    public:
        /**
        * Return the value associated with the specified key, or NULL if the
        * key does not have an associated value in the underlying store.
        *
        * @param vKey  key whose associated value is to be returned
        *
        * @return the value associated with the specified key, or
        *         <tt>NULL</tt> if no value is available for that key
        */
        virtual Object::Holder load(Object::View vKey) = 0;

        /**
        * Return the values associated with each the specified keys in the
        * passed collection. If a key does not have an associated value in
        * the underlying store, then the return map will not have an entry
        * for that key.
        *
        * @param vColKeys  a collection of keys to load
        *
        * @return a Map of keys to associated values for the specified keys
        */
        virtual Map::View loadAll(Collection::View vColKeys) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_LOADER_HPP
