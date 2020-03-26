/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ITERABLE_CACHE_LOADER_HPP
#define COH_ITERABLE_CACHE_LOADER_HPP

#include "coherence/lang.ns"
#include "coherence/net/cache/CacheLoader.hpp"
#include "coherence/util/Iterator.hpp"


COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Iterator;

/**
* A CacheLoader that can iterate its underlying contents.
*
* @author nsa 2008.07.01
*/
class COH_EXPORT IterableCacheLoader
    : public interface_spec<IterableCacheLoader,
        implements<CacheLoader> >
    {
    // ----- IterableCacheLoader interface ----------------------------------

    public:
        /**
        * Iterate all keys in the underlying store.
        *
        * @return a read-only iterator of the keys in the underlying store
        *
        * @throws UnsupportedOperationException  if the underlying store is
        *         not iterable
        */
        virtual Iterator::Handle keys() = 0;
    };


COH_CLOSE_NAMESPACE3


#endif // #ifndef COH_ITERABLE_CACHE_LOADER_HPP
