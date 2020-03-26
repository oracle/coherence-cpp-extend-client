/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MappedSet.hpp"

#include "coherence/util/ConverterCollections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

MappedSet::MappedSet(Map::Handle hMap)
    : super(hMap)
    {
    }

MappedSet::MappedSet(Map::View vMap)
    : super(vMap)
    {
    }

MappedSet::MappedSet(const MappedSet& that)
    : super(cast<Map::View>(that.getMapDelegate()->clone()))
    {
    }

// ----- Set interface --------------------------------------------------

bool MappedSet::add(Object::Holder oh)
    {
    return NULL == getMapDelegate()->put(oh, oh);
    }

bool MappedSet::addAll(Collection::View vCol)
    {
    Map::Handle hMap     = getMapDelegate();
    bool        fChanged = false;

    for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext(); )
        {
        Object::Holder oh = hIter->next();
        if (NULL == hMap->put(oh, oh))
            {
            fChanged = true;
            }
        }

    return fChanged;
    }

Iterator::Handle MappedSet::iterator() const
    {
    return getMapDelegate()->values()->iterator();
    }

Muterator::Handle MappedSet::iterator()
    {
    return getMapDelegate()->values()->iterator();
    }

ObjectArray::Handle MappedSet::toArray(
        ObjectArray::Handle hoa) const
    {
    return getMapDelegate()->values()->toArray(hoa);
    }

COH_CLOSE_NAMESPACE2
