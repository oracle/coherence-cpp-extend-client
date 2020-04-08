/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/WeakHashMap.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

WeakHashMap::WeakHashMap(size32_t cInitialBuckets, float32_t flLoadFactor,
        float32_t flGrowthRate)
        : super(cInitialBuckets, flLoadFactor, flGrowthRate)
    {
    }

WeakHashMap::WeakHashMap(const WeakHashMap& that)
    : super(that)
    {
    }


// ----- SafeHashMap inerface -----------------------------------------------

SafeHashMap::Entry::Handle WeakHashMap::instantiateEntry(Object::View vKey,
        Object::Holder ohValue, size32_t nHash)
    {
    // scan the map and clean a few reclaimable entries from the bucket
    // associated with the new entry
    shrink(getBucketIndex(nHash, m_haeBucket->length), 2);

    return Entry::create(vKey, ohValue, nHash);
    }

SafeHashMap::Entry::Handle WeakHashMap::instantiateEntry(
        SafeHashMap::Entry::View vThat)
    {
    // scan the map and clean a few reclaimable entries from the bucket
    // associated with the new entry
    shrink(getBucketIndex(vThat->hashCode(), m_haeBucket->length), 2);

    return Entry::create(cast<WeakHashMap::Entry::View>(vThat));
    }

void WeakHashMap::grow()
    {
    // before growing see if we can shrink enough to avoid the growth
    COH_SYNCHRONIZED (this)
        {
        // before growing see if we can shrink enough to avoid the growth
        compact();

        if (m_cEntries > m_cCapacity)
            {
            // we must grow
            super::grow();
            }
        }
    }


// ----- WeakHashMap interface ----------------------------------------------

void WeakHashMap::compact()
    {
    COH_SYNCHRONIZED (this)
        {
        ObjectArray::Handle haeBucket = m_haeBucket;
        for (size32_t i = 0, c = haeBucket->length; i < c; ++i)
            {
            shrink(i, size32_t(-1));
            }
        }
    }

size32_t WeakHashMap::shrink(size32_t iBucket, size32_t cEntries)
    {
    COH_SYNCHRONIZED (this)
        {
        ObjectArray::Handle haeBucket = m_haeBucket;
        size32_t            cRemoved  = 0;
        Entry::Handle       hEntry    = cast<Entry::Handle>(haeBucket[iBucket]);
        Entry::Handle       hEntryPrev;

        for (; cRemoved < cEntries && hEntry != NULL; )
            {
            if (hEntry->isValid())
                {
                hEntryPrev = hEntry;
                hEntry     = cast<Entry::Handle>(hEntry->m_hNext);
                }
            else
                {
                if (NULL == hEntryPrev)
                    {
                    haeBucket[iBucket] = hEntry = cast<Entry::Handle>(
                            hEntry->m_hNext);
                    }
                else
                    {
                    hEntryPrev->m_hNext = hEntry = cast<Entry::Handle>(
                            hEntry->m_hNext);
                    }
                ++cRemoved;
                }
            }
        m_cEntries = m_cEntries - cRemoved;
        return cRemoved;
        }
    }

// ----- inner class: Entry -------------------------------------------------

// ----- constructor ----------------------------------------------------

WeakHashMap::Entry::Entry(const WeakHashMap::Entry& that)
    : super(that)
    {
    }

WeakHashMap::Entry::Entry(WeakHashMap::Entry::View vThat)
    : super(vThat)
    {
    }

WeakHashMap::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash)
    : super(NULL == vKey ? vKey : Object::View(vKey->_attachWeak()),
            ohValue, nHash)
    {
    }

// ----- WeakHashMap::Entry interface ----------------------------------

bool WeakHashMap::Entry::isValid() const
    {
    WeakReference::View vKeyWeak = cast<WeakReference::View>(
            SafeHashMap::Entry::getKey());
    return NULL == vKeyWeak || NULL != vKeyWeak->get();
    }

// ----- SafeHashMap::Entry interface ----------------------------------

bool WeakHashMap::Entry::isKeyEqual(Object::View vKey) const
    {
    WeakReference::View vKeyWeak = cast<WeakReference::View>(
            SafeHashMap::Entry::getKey());
    return NULL == vKey ? NULL == vKeyWeak
                        : Object::equals(vKeyWeak->get(), vKey);
    }

// ----- Map::Entry interface ------------------------------------------

Object::View WeakHashMap::Entry::getKey() const
    {
    WeakReference::View vKeyWeak = cast<WeakReference::View>(
            SafeHashMap::Entry::getKey());
    return NULL == vKeyWeak ? Object::Holder(NULL) : vKeyWeak->get();
    }

COH_CLOSE_NAMESPACE2
