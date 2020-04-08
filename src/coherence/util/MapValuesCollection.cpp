/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapValuesCollection.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructor --------------------------------------------------------

MapValuesCollection::MapValuesCollection(Map::Handle hMapDelegate)
        : super(Collection::Handle(hMapDelegate->entrySet()),
            ValueConverter::getInstance(),
            ConverterCollections::UnsupportedConverter::getInstance()),
          f_thMapDelegate(self(), hMapDelegate)
    {
    }

MapValuesCollection::MapValuesCollection(Map::View vMapDelegate)
        : super(Collection::View(vMapDelegate->entrySet()),
            ValueConverter::getInstance(),
            ConverterCollections::UnsupportedConverter::getInstance()),
          f_thMapDelegate(self(), vMapDelegate)
    {
    }


// ----- Collection interface -----------------------------------------------

bool MapValuesCollection::remove(Object::View v)
    {
    Iterator::Handle hIter    = getDelegate()->iterator();
    bool             fRemoved = false;

    while (hIter->hasNext())
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        if (v->equals(vEntry->getValue()))
            {
            if (NULL != getMapDelegate()->remove(vEntry->getKey()))
                {
                fRemoved = true;
                }

            break;
            }
        }
    return fRemoved;
    }

bool MapValuesCollection::removeAll(Collection::View vc)
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

bool MapValuesCollection::retainAll(Collection::View vc)
    {
    // The shorter of the collections will be used for the
    // the contains check so that we iterate multiple times
    // over the smaller list.
    ObjectArray::Handle haCopy;
    Collection::View vCollection;

    if (vc->size() < size())
        {
        haCopy = toArray();
        vCollection = vc;
        }
    else
        {
        haCopy = vc->toArray();
        vCollection = this;
        }

    bool fChanged = false;
    for (size32_t i = 0, c = haCopy->length; i < c; ++i)
        {
        Object::Holder oh = haCopy[i];
        if (!vCollection->contains(oh))
            {
            fChanged |= remove(oh);
            }
        }

    return fChanged;
    }

bool MapValuesCollection::contains(Object::View v) const
    {
    return getMapDelegate()->containsValue(v);
    }

bool MapValuesCollection::containsAll(Collection::View vc) const
    {
    Map::View vMap = getMapDelegate();
    for (Iterator::Handle hIter = vc->iterator(); hIter->hasNext(); )
        {
        if (!vMap->containsValue(hIter->next()))
            {
            return false;
            }
        }
    return true;
    }

Map::Handle MapValuesCollection::getMapDelegate()
    {
    return cast<Map::Handle>(f_thMapDelegate);
    }

Map::View MapValuesCollection::getMapDelegate() const
    {
    return f_thMapDelegate;
    }


// ----- inner class: ValueConverter ----------------------------------------

// ----- constructor ----------------------------------------------------

MapValuesCollection::ValueConverter::ValueConverter()
    {
    }

// ----- Converter interface --------------------------------------------

Object::Holder MapValuesCollection::ValueConverter::convert(Object::Holder oh) const
    {
    Map::Entry::View vEntry = cast<Map::Entry::View>(oh);
    return vEntry->getValue();
    }

MapValuesCollection::ValueConverter::View
MapValuesCollection::ValueConverter::getInstance()
    {
    static FinalView<ValueConverter> v(System::common(), create());
    ValueConverter::View vLocal = v;
    return vLocal ==NULL
            ? (ValueConverter::View) ValueConverter::create() // avoid NULL during shutdown
            : vLocal;
    }
COH_STATIC_INIT(MapValuesCollection::ValueConverter::getInstance());

COH_CLOSE_NAMESPACE2
