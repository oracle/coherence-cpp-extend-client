/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/AbstractCacheLoader.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::HashMap;
using coherence::util::Iterator;


// ----- CacheLoader interface ----------------------------------------------

Map::View AbstractCacheLoader::loadAll(Collection::View vColKeys)
    {
    Map::Handle hMap = HashMap::create();
    for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
        {
        Object::View   vKey    = hIter->next();
        Object::Holder ohValue = load(vKey);
        if (NULL != ohValue)
            {
            hMap->put(vKey, ohValue);
            }
        }
    return hMap;
    }

COH_CLOSE_NAMESPACE3
