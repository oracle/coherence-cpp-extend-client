/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_CACHE_STORE_HPP
#define COH_ABSTRACT_CACHE_STORE_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/AbstractCacheLoader.hpp"
#include "coherence/net/cache/CacheStore.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Map;


/**
* An abstract base class for CacheStore implementations.
*
* @author jh  2008.03.12
*/
class COH_EXPORT AbstractCacheStore
    : public abstract_spec<AbstractCacheStore,
        extends<AbstractCacheLoader>,
        implements<CacheStore> >
    {
    // ----- CacheStore interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void store(Object::View vKey, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual void storeAll(Map::View vMapEntries);

        /**
        * {@inheritDoc}
        */
        virtual void erase(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void eraseAll(Collection::View vColKeys);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_CACHE_STORE_HPP
