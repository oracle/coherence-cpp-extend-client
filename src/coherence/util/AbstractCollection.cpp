/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractCollection.hpp"

#include "coherence/util/Collections.hpp"

#include <assert.h>

COH_OPEN_NAMESPACE2(coherence,util)


// ----- Collection interface -----------------------------------------------

bool AbstractCollection::isEmpty() const
    {
    return size() == 0;
    }

bool AbstractCollection::contains(Object::View v) const
    {
    // This implementation iterates over the elements in the collection,
    // checking each element in turn for equality with the specified element.
    for (Iterator::Handle i = iterator(); i->hasNext(); )
        {
        if (Object::equals(i->next(), v))
            {
            return true;
            }
        }
    return false;
    }

ObjectArray::Handle AbstractCollection::toArray(ObjectArray::Handle hao) const
    {
    return Collections::toArray(this, hao);
    }

bool AbstractCollection::containsAll(Collection::View vColl) const
    {
    for (Iterator::Handle i = vColl->iterator(); i->hasNext(); )
        {
        if (!contains(i->next()))
            {
            return false;
            }
        }
    return true;
    }

bool AbstractCollection::add(Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create("Collection::add()"));
    }

bool AbstractCollection::addAll(Collection::View vColl)
    {
    // This implementation iterates over the specified collection, and
    // adds each object returned by the iterator to this collection, in
    // turn.<p>
    bool fChanged = false;
    for (Iterator::Handle i = vColl->iterator(); i->hasNext(); )
        {
        fChanged = add(i->next()) || fChanged;
        }
    return fChanged;
    }

bool AbstractCollection::remove(Object::View /*v*/)
    {
    COH_THROW (UnsupportedOperationException::create("Collection::remove()"));
    }

bool AbstractCollection::removeAll(Collection::View vColl)
    {
    // This implementation iterates over the specified collection, and
    // remove each object returned by the iterator to this collection, in
    // turn.<p>
    Muterator::Handle hMIter = iterator();
    bool fChanged       = false;

    while (hMIter->hasNext())
        {
        if (vColl->contains(hMIter->next()))
            {
            hMIter->remove();
            fChanged = true;
            }
        }

    return fChanged;
    }

bool AbstractCollection::retainAll(Collection::View vColl)
    {
    ObjectArray::Handle haCopy = toArray();

    bool fChanged = false;
    for (size32_t i = 0, c = haCopy->length; i < c; ++i)
        {
        Object::Holder oh = haCopy[i];
        if (!vColl->contains(oh))
            {
            fChanged |= remove(oh);
            }
        }
    return fChanged;
    }

void AbstractCollection::clear()
    {
    COH_THROW (UnsupportedOperationException::create(
            "The collection does not support modification operations"));
    }

TypedHandle<const String> AbstractCollection::toString() const
    {
    return Collections::toString(this);
    }

COH_CLOSE_NAMESPACE2
