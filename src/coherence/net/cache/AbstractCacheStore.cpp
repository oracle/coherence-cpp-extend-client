/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/AbstractCacheStore.hpp"

#include "coherence/util/Iterator.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::Iterator;


// ----- CacheStore interface -----------------------------------------------

void AbstractCacheStore::store(Object::View /*vKey*/, Object::Holder /*ohValue*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void AbstractCacheStore::storeAll(Map::View vMapEntries)
    {
    for (Iterator::Handle hIter = vMapEntries->entrySet()->iterator();
         hIter->hasNext(); )
        {
        Map::Entry::View vEntry  = cast<Map::Entry::View>(hIter->next());
        Object::View     vKey    = vEntry->getKey();
        Object::Holder   ohValue = vEntry->getValue();
        store(vKey, ohValue);
        }
    }

void AbstractCacheStore::erase(Object::View /*vKey*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void AbstractCacheStore::eraseAll(Collection::View vColKeys)
    {
    for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
        {
        erase(hIter->next());
        }
    }

COH_CLOSE_NAMESPACE3
