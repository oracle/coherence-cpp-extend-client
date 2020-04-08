/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_CACHE_LOADER_HPP
#define COH_ABSTRACT_CACHE_LOADER_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheLoader.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Collection;
using coherence::util::Map;


/**
* An abstract base class for CacheLoader implementations.
*
* @author jh  2008.03.12
*/
class COH_EXPORT AbstractCacheLoader
    : public abstract_spec<AbstractCacheLoader,
        extends<Object>,
        implements<CacheLoader> >
    {
    // ----- CacheLoader interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Map::View loadAll(Collection::View vColKeys);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_CACHE_LOADER_HPP
