/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractMap.hpp"

#include "coherence/util/AbstractCollection.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/MapKeySet.hpp"
#include "coherence/util/MapValuesCollection.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- Map interface ------------------------------------------------------

size32_t AbstractMap::size() const
    {
    return entrySet()->size();
    }

bool AbstractMap::isEmpty() const
    {
    return size() == 0;
    }

bool AbstractMap::containsKey(Object::View vKey) const
    {
    // This implementation iterates over entrySet() searching for an entry
    // with the specified key.  If such an entry is found, true is
    // returned.  If the iteration terminates without finding such an entry,
    // false is returned.  Note that this implementation requires
    // linear time in the size of the map.

    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        {
        Entry::View vEntry = cast<Entry::View>(i->next());
        if (Object::equals(vEntry->getKey(), vKey))
            {
            return true;
            }
        }
    return false;
    }

bool AbstractMap::containsValue(Object::View vValue) const
    {
    // This implementation iterates over entrySet() searching for an entry
    // with the specified value.  If such an entry is found, true is
    // returned.  If the iteration terminates without finding such an entry,
    // false is returned.  Note that this implementation requires
    // linear time in the size of the map.

    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        {
        Entry::View vEntry = cast<Entry::View>(i->next());
        if (Object::equals(vEntry->getValue(), vValue))
            {
            return true;
            }
        }
    return false;
    }

Object::Holder AbstractMap::get(Object::View vKey) const
    {
    // This implementation iterates over entrySet() searching for an entry
    // with the specified key.  Note that this implementation requires
    // linear time in the size of the map.

    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        {
        Entry::View vEntry = cast<Entry::View>(i->next());
        if (Object::equals(vEntry->getKey(), vKey))
            {
            return vEntry->getValue();
            }
        }

    // not found
    return Object::Holder(); // NULL
    }

Object::Holder AbstractMap::get(Object::View vKey)
    {
    return ((const AbstractMap*) this)->get(vKey);
    }

void AbstractMap::putAll(Map::View vMap)
    {
    for (Iterator::Handle i = vMap->entrySet()->iterator(); i->hasNext(); )
        {
        Entry::View vEntry = cast<Entry::View>(i->next());
        put(vEntry->getKey(), vEntry->getValue());
        }
    }

Set::View AbstractMap::keySet() const
    {
    return MapKeySet::create(Map::View(this));
    }

Set::Handle AbstractMap::keySet()
    {
    return MapKeySet::create(Map::Handle(this));
    }


Collection::View AbstractMap::values() const
    {
    return MapValuesCollection::create(Map::View(this));
    }


Collection::Handle AbstractMap::values()
    {
    return MapValuesCollection::create(Map::Handle(this));
    }


// ----- Object interface ---------------------------------------------------

bool AbstractMap::equals(Object::View v) const
    {
    // This implementation first checks if the specified Object is this map;
    // if so it returns true.  Then, it checks if the specified
    // Object is a map whose size is identical to the size of this map; if
    // not, it returns false.  If so, it iterates over this map's
    // entrySet collection, and checks that the specified map
    // contains each mapping that this map contains.  If the specified map
    // fails to contain such a mapping, false is returned.  If the
    // iteration completes, true is returned.

    Map::View vThat = cast<Map::View>(v, false);

    if (NULL == vThat)
        {
        // not a Map
        return false;
        }

    if (vThat == (const Map*) this)
        {
        return true;
        }

    if (vThat->size() != size())
        {
        return false;
        }

    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        {
        Entry::View  vEntry = cast<Map::Entry::View>(i->next());
        Object::View vKey  = vEntry->getKey();

        if (!Object::equals(vEntry->getValue(), vThat->get(vKey)))
            {
            return false;
            }
        }
    return true;
    }

size32_t AbstractMap::hashCode() const
    {
    // This implementation iterates over entrySet(), calling
    // hashCode on each element (entry) in the Collection, and adding
    // up the results.

    size32_t nHash = 0;
    for (Iterator::Handle i = entrySet()->iterator(); i->hasNext(); )
        nHash += i->next()->hashCode();
    return nHash;
    }

TypedHandle<const String> AbstractMap::toString() const
    {
    return Collections::toString((Map::View) this);
    }

COH_CLOSE_NAMESPACE2
