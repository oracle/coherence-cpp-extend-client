/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapKeySet.hpp"

#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

MapKeySet::MapKeySet(Map::Handle hMapDelegate)
    : super(hMapDelegate->entrySet(),
        KeyConverter::getInstance(),
        ConverterCollections::UnsupportedConverter::getInstance()),
      f_thMapDelegate(self(), hMapDelegate)
    {
    }

MapKeySet::MapKeySet(Map::View vMapDelegate)
    : super(vMapDelegate->entrySet(),
        KeyConverter::getInstance(),
        ConverterCollections::UnsupportedConverter::getInstance()),
      f_thMapDelegate(self(), vMapDelegate)
    {
    }


// ----- Set interface ------------------------------------------------------

bool MapKeySet::remove(Object::View v)
    {
    // For an underlying map, remove all instances of a key
    //  make no sense
    Object::Holder hResult = getMapDelegate()->remove(v);
    return NULL == hResult ? false : true;
    }

bool MapKeySet::removeAll(Collection::View vc)
    {
    Muterator::Handle hMIter   = iterator();
    bool              fRemoved = false;

    while (hMIter->hasNext())
        {
        if (vc->contains(hMIter->next()))
            {
            hMIter->remove();
            fRemoved = true;
            }
        }
    return fRemoved;
    }

bool MapKeySet::retainAll(Collection::View vc)
    {
    bool fChanged = false;

    for (Muterator::Handle hIter = iterator(); hIter->hasNext();)
        {
        if (!vc->contains(hIter->next()))
            {
            hIter->remove();
            fChanged = true;
            }
        }
    return fChanged;
    }

bool MapKeySet::contains(Object::View v) const
    {
    return getMapDelegate()->containsKey(v);
    }

bool MapKeySet::containsAll(Collection::View vc) const
    {
    Map::View vMap = getMapDelegate();
    for (Iterator::Handle hIter = vc->iterator(); hIter->hasNext(); )
        {
        if (!vMap->containsKey(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

Map::Handle MapKeySet::getMapDelegate()
    {
    return cast<Map::Handle>(f_thMapDelegate);
    }

Map::View MapKeySet::getMapDelegate() const
    {
    return f_thMapDelegate;
    }


// ----- inner class: KeyConverter ------------------------------------------

// ----- constructor ----------------------------------------------------

MapKeySet::KeyConverter::KeyConverter()
    {
    }

// ----- Converter interface --------------------------------------------

Object::Holder MapKeySet::KeyConverter::convert(Object::Holder oh) const
    {
    return cast<Map::Entry::View>(oh)->getKey();
    }

MapKeySet::KeyConverter::View
MapKeySet::KeyConverter::getInstance()
    {
    static FinalView<KeyConverter> v(System::common(), create());
    KeyConverter::View vLocal = v;
    return vLocal == NULL
            ? (KeyConverter::View) KeyConverter::create() // avoid NULL during shutdown
            : vLocal;
    }
COH_STATIC_INIT(MapKeySet::KeyConverter::getInstance());

COH_CLOSE_NAMESPACE2
