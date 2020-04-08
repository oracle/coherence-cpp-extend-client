/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/ObservableHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructors -------------------------------------------------------
ObservableHashMap::ObservableHashMap(size32_t cInitialBuckets,
        float32_t flLoadFactor, float32_t flGrowthRate)
    : super(cInitialBuckets, flLoadFactor, flGrowthRate),
      m_hListenerSupport(self())
    {
    }


// ----- Map interface --------------------------------------------------

Object::Holder ObservableHashMap::remove(Object::View vKey)
    {
    Entry::Handle hEntry;
    COH_SYNCHRONIZED (this)
        {
        hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
        if (NULL == hEntry)
            {
            return NULL;
            }

        removeEntryInternal(hEntry);
        }

    hEntry->onRemove();
    return hEntry->getValue();
    }

void ObservableHashMap::clear()
    {
    clear(false);
    }


// ----- ObservableMap methods ------------------------------------------

void ObservableHashMap::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE_PARAM(hListener);

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (NULL == hSupport)
            {
            hSupport = m_hListenerSupport = MapListenerSupport::create();
            }

        hSupport->addListener(hListener, vKey, fLite);
        }
    }

void ObservableHashMap::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE_PARAM(hListener);

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (hSupport != NULL)
            {
            hSupport->removeListener(hListener, vKey);
            if (hSupport->isEmpty())
                {
                m_hListenerSupport = NULL;
                }
            }
        }
    }

void ObservableHashMap::addMapListener(MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }

void ObservableHashMap::removeMapListener(MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }

void ObservableHashMap::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE_PARAM(hListener);

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (NULL == hSupport)
            {
            hSupport = m_hListenerSupport = MapListenerSupport::create();
            }

        hSupport->addListener(hListener, vFilter, fLite);
        }
    }

void ObservableHashMap::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE_PARAM(hListener);

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (hSupport != NULL)
            {
            hSupport->removeListener(hListener, vFilter);
            if (hSupport->isEmpty())
                {
                m_hListenerSupport = NULL;
                }
            }
        }
    }

// ----- ObservableHashMap methods ------------------------------------------

void ObservableHashMap::truncate()
    {
    clear(true);
    }


// ----- event dispatching ----------------------------------------------

MapListenerSupport::Handle ObservableHashMap::getMapListenerSupport()
    {
    return m_hListenerSupport;
    }

bool ObservableHashMap::hasListeners()
    {
    // m_listenerSupport defaults to NULL, and it is reset to NULL when
    // the last listener unregisters
    return m_hListenerSupport != NULL;
    }

void ObservableHashMap::dispatchEvent(MapEvent::Handle hEvt)
    {
    MapListenerSupport::Handle hListenerSupport = getMapListenerSupport();
    if (hListenerSupport != NULL)
        {
        // the events can only be generated while the current thread
        // holds the monitor on this map
        COH_SYNCHRONIZED (this)
            {
            hListenerSupport->fireEvent(hEvt, false);
            }
        }
    }


// ----- helper methods -------------------------------------------------

void ObservableHashMap::clear(bool fTruncate)
    {
    // COH-6009: map mutations must be synchronized with event dispatch
    // to ensure in-order delivery

    ObjectArray::Handle haeBucket;
    if (fTruncate)
        {
        // clear the map
        COH_SYNCHRONIZED (this)
            {
            // clear the map
            super::clear();
            }
        }
    else
        {
        COH_SYNCHRONIZED(this)
            {
            // grab a copy of all entries
            haeBucket = m_haeBucket;

            // clear the map
            super::clear();
            }

        // walk all buckets
        for (size32_t i = 0, c = haeBucket->length; i < c; ++i)
            {
            // walk all entries in the bucket
            Entry::Handle hEntry = cast<Entry::Handle>(haeBucket[i]);
            while (hEntry != NULL)
                {
                hEntry->onRemove();
                hEntry = cast<Entry::Handle>(hEntry->m_hNext);
                }
            }
        }
    }


// ----- inner class: Entry ---------------------------------------------

SafeHashMap::Entry::Handle ObservableHashMap::instantiateEntry(Object::View vKey,
        Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash, this);
    }

SafeHashMap::Entry::Handle ObservableHashMap::instantiateEntry(
        SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(cast<ObservableHashMap::Entry::View>(vEntry));
    }

ObservableHashMap::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash, ObservableHashMap::Handle hMap)
    : super(vKey, ohValue, nHash),
      m_whMap(self(), hMap, /*fMutable*/ true)
    {
    }

ObservableHashMap::Entry::Entry(const Entry& that)
    : super(that), m_whMap(self(), NULL, /*fMutable*/ true)
    {
    }

ObservableHashMap::Entry::Entry(ObservableHashMap::Entry::View vThat)
    : super(vThat),
      m_whMap(self(), vThat->m_whMap, /*fMutable*/ true)
    {
    }

// ----- Map.Entry interface --------------------------------------------

Object::Holder ObservableHashMap::Entry::setValue(Object::Holder ohValue)
    {
    // perform the entry update
    Object::Holder ohPrev = super::setValue(ohValue);

    // note: previous to Coherence 3.1, there was an optimization
    // that would only raise an event if the reference was changing,
    // i.e. if (oPrev != oValue) {..}

    // issue update notification
    ObservableHashMap::Handle hMap = getMap();
    if (hMap->hasListeners())
        {
        hMap->dispatchEvent(MapEvent::create(hMap, MapEvent::entry_updated,
            getKey(), ohPrev, ohValue));
        }

    return ohPrev;
    }

ObservableHashMap::Handle ObservableHashMap::Entry::getMap()
    {
    return m_whMap;
    }

ObservableHashMap::View ObservableHashMap::Entry::getMap() const
    {
    return m_whMap;
    }

void ObservableHashMap::Entry::onAdd()
    {
    // issue add notification
    ObservableHashMap::Handle hMap = getMap();
    if (hMap->hasListeners())
        {
        hMap->dispatchEvent(MapEvent::create(hMap, MapEvent::entry_inserted,
            getKey(), (Object::View)NULL, getValue()));
        }
    }

void ObservableHashMap::Entry::onRemove()
    {
    // issue remove notification
    ObservableHashMap::Handle hMap = getMap();
    if (hMap->hasListeners())
        {
        hMap->dispatchEvent(MapEvent::create(hMap, MapEvent::entry_deleted,
            getKey(), getValue(), (Object::View)NULL));
        }
    }


COH_CLOSE_NAMESPACE2
