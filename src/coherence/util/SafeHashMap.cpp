/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/SafeHashMap.hpp"

#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"

#include "private/coherence/lang/AssociativeArray.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_TYPED_CLASS(SafeHashMap);


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return a singleton empty array used to indicate that the SHM is resizing
    */
    ObjectArray::Handle getNoEntriesArray()
        {
        static FinalHandle<ObjectArray> h(System::common(),
                ObjectArray::create(0));
        return h;
        }
    COH_STATIC_INIT(getNoEntriesArray());

    /**
    * Return a singleton empty array used to indicate that the SHM is no
    * longer valid. This is used by LocalNamedCache::release/destroy.
    *
    * Note this marker array cannot reuse the NoEntriesArray.
    */
    ObjectArray::Handle getInvalidArray()
        {
        // mf 2015.02.26: Work around for aparent VisualStudio bug.
        // Debug x86 VisualStudio 2013 build running functional tests with the "alloc" heap analyzer enabled
        // segfaults here when the "standard" one line form i.e.
        //   static FinalHandle<ObjectArray> h(System::common(), ObjectArray::create(0));
        // is used.  This occurs during static initialization and "h" is left in a zeroed out state and
        // returning "h" naturally triggers a segfault because it doesn't contain a guardian monitor.
        // This appears to be a compiler bug as breaking it up as follows resolves the issue.

        static Object& monitor         = System::common();
        static ObjectArray::Handle hao = ObjectArray::create(0);
        static FinalHandle<ObjectArray> h(monitor, hao);
        return h;
        }
    COH_STATIC_INIT(getInvalidArray());
    }

COH_EXPORT_SPEC_MEMBER(const size32_t SafeHashMap::default_initialsize)
COH_EXPORT_SPEC_MEMBER(const size32_t SafeHashMap::biggest_modulo)

// ----- constructors -------------------------------------------------------

SafeHashMap::SafeHashMap(size32_t cInitialBuckets, float32_t flLoadFactor, float32_t flGrowthRate)
    : f_vResizing(self(), Object::create()), m_cEntries(0), m_haeBucket(*f_vResizing)
    {
    if (flLoadFactor <= 0)
        {
        COH_THROW (IllegalArgumentException::create(
            "SafeHashMap: Load factor must be greater than zero."));
        }
    if (flGrowthRate <= 0)
        {
        COH_THROW (IllegalArgumentException::create(
            "SafeHashMap: Growth rate must be greater than zero."));
        }
    if (cInitialBuckets == 0)
        {
        // Note that 0 is reserved for detecting growth
        cInitialBuckets = 1;
        }

    // initialize the hash map data structure
    m_haeBucket     = ObjectArray::create(cInitialBuckets);
    m_cCapacity    = (size32_t) (cInitialBuckets * flLoadFactor);
    m_flLoadFactor = flLoadFactor;
    m_flGrowthRate = flGrowthRate;
    }

SafeHashMap::SafeHashMap(const SafeHashMap& that)
    : super(), f_vResizing(self(), Object::create()), m_cEntries(0),
      m_haeBucket(*f_vResizing)
    {
    COH_SYNCHRONIZED (&that)
        {
        // only have to deep-clone the bucket array and the map entries
        ObjectArray::View   vaeBucket = that.m_haeBucket;
        size32_t            cBuckets  = vaeBucket->length;
        ObjectArray::Handle haeBucket = ObjectArray::create(cBuckets);
        for (size32_t i = 0; i < cBuckets; ++i)
            {
            Entry::View vEntryThat = cast<Entry::View>(vaeBucket[i]);
            if (vEntryThat != NULL)
                {
                haeBucket[i] = copyEntryList(vEntryThat, true);
                }
            }

        m_haeBucket        = haeBucket;
        m_cCapacity        = that.m_cCapacity;
        m_cEntries         = that.m_cEntries;
        m_flLoadFactor     = that.m_flLoadFactor;
        m_flGrowthRate     = that.m_flGrowthRate;
        }
    }


// ----- Map interface ------------------------------------------------------

size32_t SafeHashMap::size() const
    {
    COH_SYNCHRONIZED_MEMBER_READ
        {
        return m_cEntries;
        }
    }

bool SafeHashMap::isEmpty() const
    {
    return size() == 0;
    }

bool SafeHashMap::containsKey(Object::View vKey) const
    {
    return getEntryInternal(vKey) != NULL;
    }

Object::Holder SafeHashMap::get(Object::View vKey) const
    {
    Entry::View vEntry = getEntryInternal(vKey);
    return NULL == vEntry ? (Object::Holder) NULL : vEntry->getValue();
    }

SafeHashMap::Entry::View SafeHashMap::getEntry(Object::View vKey) const
    {
    return getEntryInternal(vKey);
    }

SafeHashMap::Entry::Handle SafeHashMap::getEntry(Object::View vKey)
    {
    return getEntryInternal(vKey);
    }

Object::Holder SafeHashMap::put(Object::View vKey, Object::Holder ohValue)
    {
    Entry::Handle hEntry = getEntryInternal(vKey);
    nonexistent: if (NULL == hEntry)
        {
        // calculate hash code for the key
        size32_t nHash = getHashCode(vKey);

        // instantiate and configure a new Entry
        hEntry = instantiateEntry(vKey, ohValue, nHash);

        // synchronize the addition of the new Entry
        // note that it is possible that an Entry with the same key
        // has been added by another thread
        COH_SYNCHRONIZED (this)
            {
            // get the array of buckets
            ObjectArray::Handle haeBucket = m_haeBucket;
            size32_t            cBuckets  = haeBucket->length;

            // hash to a particular bucket
            size32_t      nBucket   = getBucketIndex(nHash, cBuckets);
            Entry::Handle hEntryCur = cast<Entry::Handle>(haeBucket[nBucket]);

            // walk the linked list of entries (open hash) in the bucket
            // to verify the Entry has not already been added
            while (hEntryCur != NULL)
                {
                // optimization:  check hash first
                if (nHash == hEntryCur->m_nHash && hEntryCur->isKeyEqual(vKey))
                    {
                    // found the entry; it is no longer non-existent
                    hEntry = hEntryCur;
                    goto nonexistent;
                    }

                hEntryCur = hEntryCur->m_hNext;
                }

            // put the Entry in at the front of the list of entries
            // for that bucket
            hEntry->m_hNext    = cast<Entry::Handle>(haeBucket[nBucket]);
            haeBucket[nBucket] = hEntry;

            size32_t cEntries = m_cEntries;
            m_cEntries = ++cEntries;
            if (cEntries > m_cCapacity)
                {
                grow();
                }
            }

        // note: supports subclasses that implement ObservableMap
        hEntry->onAdd();

        return NULL;
        }

    // note that it is possible that the Entry is being removed
    // by another thread or that the value is being updated by
    // another thread because this is not synchronized
    return hEntry->setValue(ohValue);
    }

void SafeHashMap::grow()
    {
    if (!isValid())
        {
        COH_THROW(IllegalStateException::create("No longer valid"));
        }

    COH_SYNCHRONIZED (this)
        {
        // store off the old bucket array
        ObjectArray::Handle haeOld = m_haeBucket;
        size32_t            cOld   = haeOld->length;

        // check if there is no more room to grow
        if (cOld >= biggest_modulo)
            {
            return;
            }

        // use a 0-length array to signify that a resize is taking place
        m_haeBucket = getNoEntriesArray();

        // calculate growth
        size32_t cNew = (size32_t) std::min((int64_t) (cOld * (1.0F + m_flGrowthRate)), (int64_t) biggest_modulo);
        if (cNew <= cOld)
            {
            // very low growth rate or very low initial size (stupid!)
            cNew = cOld + 1;
            }

        // use a prime number at least as big than the new size
        cNew = getNextPrime(cNew); // see AssociativeArray.hpp

        // create a new bucket array; in the case of an OutOfMemoryError
        // be sure to restore the old bucket array
        ObjectArray::Handle haeNew;
        try
            {
            haeNew = ObjectArray::create(cNew);
            if (NULL == haeNew)
                {
                m_haeBucket = haeOld;
                COH_THROW (IllegalStateException::create(
                        "out of memory during SafeHashMap::grow()"));
                }
            }
        catch (...)
            {
            m_haeBucket = haeOld;
            throw;
            }

        // if there are Iterators active, they are iterating over an array of
        // buckets which is about to be completely whacked; to make sure that
        // the Iterators can recover from the bucket array getting whacked,
        // the resize will create a clone of each entry in the old bucket
        // array and put those clones into the old bucket array in the same
        // order that the original entries were found, thus allowing the
        // Iterators to find their place again without missing or repeating
        // any data, other than the potential for missing data added after
        // the iteration began (which is always possible anyways)
        bool fRetain = isActiveIterator();

        // rehash
        for (size32_t i = 0; i < cOld; ++i)
            {
            Entry::Handle hEntry       = cast<Entry::Handle>(haeOld[i]);
            Entry::Handle hEntryRetain = NULL;

            // NSA Store off the old entries so we can put them in place for
            // the iterators. I'm sure this can be more optimized but until
            // I have confidence in the approach and a deeper understanding
            // of this code I want to Keep It Simple Stupid!
            Entry::Handle hEntryCopy   = fRetain ? copyEntryList(hEntry, false) : NULL;

            while (hEntry != NULL)
                {
                // store off the next Entry
                // (it is going to get hammered otherwise)
                Entry::Handle hEntryNext = hEntry->m_hNext;

                // rehash the Entry into the new bucket array
                size32_t nBucket = getBucketIndex(hEntry->m_nHash, cNew);
                hEntry->m_hNext  = cast<Entry::Handle>(haeNew[nBucket]);
                haeNew[nBucket]  = hEntry;

                /*
                // clone each entry if Iterators are active (since they will
                // need the entries in the same order to avoid having to
                // throw a CME in most cases)
                if (fRetain)
                    {
                    Entry::Handle hEntryCopy = cast<Entry::Handle>(hEntry->clone());
                    if (NULL == hEntryRetain)
                        {
                        haeOld[i] = hEntryCopy;
                        }
                    else
                        {
                        hEntryRetain->m_hNext = hEntryCopy;
                        }
                    hEntryRetain = hEntryCopy;
                    }
                    */
                // process next Entry in the old list
                hEntry = hEntryNext;
                }

            // NSA is this an acceptable order modification or are we at risk
            // here for CMEs?
            if (fRetain)
                {
                haeOld[i] = hEntryCopy;
                }
            }

        // store updated bucket array
        m_cCapacity = (size32_t) (cNew * m_flLoadFactor);
        m_haeBucket  = haeNew;

        // notify threads that are waiting for the resize to complete
        COH_SYNCHRONIZED (f_vResizing)
            {
            f_vResizing->notifyAll();
            }
        }
    }

size32_t SafeHashMap::getHashCode(Object::View vKey) const
    {
    return Object::hashCode(vKey);
    }

Object::Holder SafeHashMap::remove(Object::View vKey)
    {
    COH_SYNCHRONIZED (this)
        {
        // get the array of buckets
        ObjectArray::Handle haeBucket = m_haeBucket;
        size32_t            cBuckets  = haeBucket->length;

        // hash to a particular bucket
        size32_t      nHash     = getHashCode(vKey);
        size32_t      nBucket   = getBucketIndex(nHash, cBuckets);
        Entry::Handle hEntryCur = cast<Entry::Handle>(haeBucket[nBucket]);

        // walk the linked list of entries (open hash) in the bucket
        // to verify the Entry has not already been added
        Entry::Handle hEntryPrev = NULL;
        while (hEntryCur != NULL)
            {
            // optimization:  check hash first
            if (nHash == hEntryCur->m_nHash && hEntryCur->isKeyEqual(vKey))
                {
                // remove the current Entry from the list
                if (NULL == hEntryPrev)
                    {
                    haeBucket[nBucket] = (Object::Handle) hEntryCur->m_hNext;
                    }
                else
                    {
                    hEntryPrev->m_hNext = hEntryCur->m_hNext;
                    }
                m_cEntries = m_cEntries - 1;
                return hEntryCur->getValue();
                }

            hEntryPrev = hEntryCur;
            hEntryCur  = hEntryCur->m_hNext;
            }

        return NULL;
        }
    }

void SafeHashMap::clear()
    {
    COH_SYNCHRONIZED (this)
        {
        m_haeBucket  = ObjectArray::create(default_initialsize);
        m_cEntries   = 0;
        m_cCapacity  = (size32_t) (default_initialsize * m_flLoadFactor);
        }
    }

Set::View SafeHashMap::entrySet() const
    {
    // we can't cache the entrySet as we'd end up with a circular reference
    // and be self sustaining.  Using a WeakReference from the Map to the
    // EntrySet wouldn't be too useful, and doing a WeakReference from the
    // Set to the Map would be plain incorrect
    return instantiateEntrySet();
    }

Set::Handle SafeHashMap::entrySet()
    {
    // we can't cache the entrySet as we'd end up with a circular reference
    // and be self sustaining.  Using a WeakReference from the Map to the
    // EntrySet wouldn't be too useful, and doing a WeakReference from the
    // Set to the Map would be plain incorrect
    return instantiateEntrySet();
    }


// ----- helper methods -----------------------------------------------------

SafeHashMap::Entry::Handle SafeHashMap::cloneEntryList(Entry::View vEntryThat) const
    {
    if (NULL == vEntryThat)
        {
        return NULL;
        }

    // clone the head of the chain
    Entry::Handle hEntryThis = cast<Entry::Handle>(vEntryThat->clone());

    // clone the rest of the chain
    Entry::Handle hEntryPrevThis = hEntryThis;
    Entry::View   vEntryNextThat = vEntryThat->m_hNext;
    while (vEntryNextThat != NULL)
        {
        // clone the entry
        Entry::Handle hEntryNextThis = cast<Entry::Handle>(vEntryNextThat->clone());

        // link it in
        hEntryPrevThis->m_hNext = hEntryNextThis;

        // advance
        hEntryPrevThis = hEntryNextThis;
        vEntryNextThat = vEntryNextThat->m_hNext;
        }

    return hEntryThis;
    }

SafeHashMap::Entry::Handle SafeHashMap::getEntryInternal(Object::View vKey) const
    {
    // calculate hash code for the oKey
    size32_t nHash = getHashCode(vKey);

    while (true)
        {
        // get the bucket array
        ObjectArray::View vaeBucket = getStableBucketArray();
        size32_t          cBuckets  = vaeBucket->length;

        // hash to a particular bucket
        size32_t      nBucket = getBucketIndex(nHash, cBuckets);
        Entry::Handle hEntry  = cast<Entry::Handle>(vaeBucket[nBucket]);

        // walk the linked list of entries (open hash) in the bucket
        while (hEntry != NULL)
            {
            // optimization:  check hash first
            if (nHash == hEntry->m_nHash && hEntry->isKeyEqual(vKey))
                {
                // COH-1542: check for resize before returning Entry
                break;
                }

            hEntry = hEntry->m_hNext;
            }

        // if a resize occurred, the bucket array may have been reshuffled
        // while we were searching it; we know a resize occurred iff the
        // hash bucket array changed
        if (vaeBucket == m_haeBucket)
            {
            // no resize occurred
            return hEntry;
            }
        }
    }

void SafeHashMap::removeEntryInternal(Entry::Handle hEntry)
    {
    COH_SYNCHRONIZED (this)
        {
        if (NULL == hEntry)
            {
            COH_THROW (IllegalArgumentException::create("entry is NULL"));
            }

        // get the array of buckets
        ObjectArray::Handle haeBucket = m_haeBucket;
        size32_t            cBuckets  = haeBucket->length;

        // hash to a particular bucket
        size32_t  nHash    = hEntry->m_nHash;
        size32_t  nBucket  = getBucketIndex(nHash, cBuckets);

        // check the head
        Entry::Handle hEntryHead = cast<Entry::Handle>(haeBucket[nBucket]);
        if (hEntry == hEntryHead)
            {
            haeBucket[nBucket] = hEntry->m_hNext;
            }
        else
            {
            // walk the linked list of entries (open hash) in the bucket
            Entry::Handle hEntryPrev = hEntryHead;
            while (true)
                {
                if (NULL == hEntryPrev)
                    {
                    // another thread has already removed the entry
                    return;
                    }

                Entry::Handle hEntryCur = hEntryPrev->m_hNext;
                if (hEntry == hEntryCur)
                    {
                    hEntryPrev->m_hNext = hEntryCur->m_hNext;
                    break;
                    }

                hEntryPrev = hEntryCur;
                }
            }

        m_cEntries = m_cEntries - 1;
        }
    }

size32_t SafeHashMap::getBucketIndex(size32_t nHash, size32_t cBuckets) const
    {
    return nHash % cBuckets;
    }

ObjectArray::View SafeHashMap::getStableBucketArray() const
    {
    // get the bucket array
    ObjectArray::View vaeBucket = m_haeBucket;

    // wait for any ongoing resize to complete
    while (vaeBucket->length == 0)
        {
        COH_SYNCHRONIZED (f_vResizing)
            {
            // now that we have the lock, verify that it is
            // still necessary to wait
            if (m_haeBucket->length == 0)
                {
                if (isValid())
                    {
                    // limit the wait, so grow() can fail w/out notifying
                    f_vResizing->wait(1000);
                    }
                else
                    {
                    COH_THROW(IllegalStateException::create("No longer valid"));
                    }
                }
            }

        vaeBucket = m_haeBucket;
        }

    return vaeBucket;
    }

void SafeHashMap::iteratorActivated() const
    {
    m_cIterators.adjust(1);
    }

void SafeHashMap::iteratorDeactivated() const
    {
    m_cIterators.adjust(-1);
    }

bool SafeHashMap::isActiveIterator() const
    {
    return m_cIterators.get() != 0;
    }

void SafeHashMap::invalidate()
    {
    COH_SYNCHRONIZED (this)
        {
        m_haeBucket  = getInvalidArray();
        m_cEntries   = 0;
        m_cCapacity  = 0;
        }
    }

bool SafeHashMap::isValid() const
    {
    return m_haeBucket != getInvalidArray();
    }

SafeHashMap::Entry::Handle SafeHashMap::copyEntryList(
        SafeHashMap::Entry::View vEntry, bool fDeep)
    {
    if (vEntry == NULL)
        {
        return NULL;
        }


    // Clone the head of the chain
    Entry::Handle hResult = copyEntry(vEntry, fDeep);

    SafeHashMap::Entry::Handle hEntry = hResult;
    while (vEntry->m_hNext != NULL)
        {
        // grab the next entry to copy
        vEntry = vEntry->m_hNext;
        // copy the next entry
        hEntry->m_hNext = copyEntry(vEntry, fDeep);
        // move the chains
        hEntry = hEntry->m_hNext;
        }

    return hResult;
    }

SafeHashMap::Entry::Handle SafeHashMap::copyEntry(Entry::View vEntry, bool fDeep)
    {
    // Clone the head of the chain
    Entry::Handle hResult;
    if (fDeep)
        {
        Object::View vKey = vEntry->getKey()->clone();
        hResult = instantiateEntry(vKey,
                vEntry->getValue()->clone(), getHashCode(vKey));
        }
    else
        {
        hResult = instantiateEntry(vEntry);
        }

    return hResult;
    }


// ----- inner class: Entry -------------------------------------------------

SafeHashMap::Entry::Handle SafeHashMap::instantiateEntry(Object::View vKey,
        Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash);
    }

SafeHashMap::Entry::Handle SafeHashMap::instantiateEntry(SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(vEntry);
    }

SafeHashMap::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash)
        : f_vKey(self(), vKey), m_ohValue(self(), ohValue), m_nHash(nHash),
          m_hNext(self())
    {
    }

SafeHashMap::Entry::Entry(const SafeHashMap::Entry& that)
    : super(that), f_vKey(self(), Object::clone(that.f_vKey)),
      m_ohValue(self(), Object::clone(that.m_ohValue)),
      m_nHash(that.m_nHash), m_hNext(self())
    {
    }

SafeHashMap::Entry::Entry(Entry::View vThat)
        : f_vKey(self(), vThat->f_vKey),
          m_ohValue(self(), vThat->m_ohValue),
          m_nHash(vThat->m_nHash), m_hNext(self())
    {
    }

bool SafeHashMap::Entry::isKeyEqual(Object::View vKey) const
    {
    return Object::equals(f_vKey, vKey);
    }

Object::View SafeHashMap::Entry::getKey() const
    {
    return f_vKey;
    }

Object::Holder SafeHashMap::Entry::getValue() const
    {
    return m_ohValue;
    }

Object::Holder SafeHashMap::Entry::getValue()
    {
    return ((const Entry*) this)->getValue();
    }

Object::Holder SafeHashMap::Entry::setValue(Object::Holder ohValue)
    {
    COH_SYNCHRONIZED_MEMBER_WRITE
        {
        return coh_synchronized_member_write.exchangeMember(m_ohValue, ohValue);
        }
    }

bool SafeHashMap::Entry::equals(Object::View that) const
    {
    Map::Entry::View vThat = cast<Map::Entry::View>(that, false);

    if (vThat != NULL)
        {
        if (this == vThat)
            {
            return true;
            }

        Object::View vThisValue = this->m_ohValue;
        Object::View vThatValue = vThat->getValue();

        return isKeyEqual(vThat->getKey()) &&
            (NULL == vThisValue ? NULL == vThatValue
                                : vThisValue->equals(vThatValue));
        }

    return false;
    }

size32_t SafeHashMap::Entry::hashCode() const
    {
    return Object::hashCode(getKey());
    }

TypedHandle<const String> SafeHashMap::Entry::toString() const
    {
    return Collections::toString(this);
    }

void SafeHashMap::Entry::onAdd()
    {
    }

// ----- inner class: EntrySet ----------------------------------------------

Set::Handle SafeHashMap::instantiateEntrySet()
    {
    return EntrySet::create(Handle(this));
    }

Set::View SafeHashMap::instantiateEntrySet() const
    {
    return EntrySet::create(View(this));
    }

SafeHashMap::EntrySet::EntrySet(SafeHashMap::Holder ohMap)
        : f_ohMap(self(), ohMap)
    {
    }

// ----- Set interface --------------------------------------------------

Iterator::Handle SafeHashMap::EntrySet::iterator() const
    {
    return instantiateIterator();
    }

Muterator::Handle SafeHashMap::EntrySet::iterator()
    {
    return instantiateIterator();
    }

size32_t SafeHashMap::EntrySet::size() const
    {
    return getDelegate()->size();
    }

bool SafeHashMap::EntrySet::contains(Object::View v) const
    {
    Map::Entry::View vEntry = cast<Map::Entry::View>(v);
    if (vEntry != NULL)
        {
        Map::Entry::View vThisEntry = getDelegate()->getEntryInternal(vEntry->getKey());
        return vThisEntry != NULL && vThisEntry->equals(vEntry);
        }

    return false;
    }

bool SafeHashMap::EntrySet::remove(Object::View v)
    {
    SafeHashMap::Handle hMap = getDelegate();
    COH_SYNCHRONIZED (hMap)
        {
        if (contains(v))
            {
            hMap->remove((cast<Map::Entry::View>(v))->getKey());
            return true;
            }
        else
            {
            return false;
            }
        }
    }

void SafeHashMap::EntrySet::clear()
    {
    getDelegate()->clear();
    }

ObjectArray::Handle SafeHashMap::EntrySet::toArray(
        ObjectArray::Handle ha) const
    {
    // synchronizing prevents add/remove, keeping size() constant
    SafeHashMap::View vMap = getDelegate();
    COH_SYNCHRONIZED (vMap)
        {
        // verify ha size or create the array to store the map contents
        size32_t c = vMap->size();
        if (ha == NULL || ha->length < c)
            {
            ha = ObjectArray::create(c);
            }
        else if (ha->length > c)
            {
             ha[c] = NULL;
            }

        // walk all buckets
        ObjectArray::View vaeBucket = vMap->m_haeBucket;
        size32_t          cBuckets  = vaeBucket->length;
        for (size32_t iBucket = 0, i = 0; iBucket < cBuckets; ++iBucket)
            {
            // walk all entries in the bucket
            Entry::Holder ohEntry = cast<Entry::Holder>(vaeBucket[iBucket]);
            while (ohEntry != NULL)
                {
                ha[i++] = ohEntry;
                if (instanceof<Entry::Handle>(ohEntry))
                    {
                    ohEntry = cast<Entry::Handle>(ohEntry)->m_hNext;
                    }
                else
                    {
                    ohEntry = ohEntry->m_hNext;
                    }
                }
            }
        }

    return ha;
    }

// ----- Object interface -----------------------------------------------

bool SafeHashMap::EntrySet::equals(Object::View that) const
    {
    if (that == this)
        {
        return true;
        }

    Set::View vThat = cast<Set::View>(that, false);
    if (NULL == vThat)
        {
        return false;
        }

    return vThat->size() == size() && containsAll(vThat);
    }

size32_t SafeHashMap::EntrySet::hashCode() const
    {
    size32_t nHash = 0;
    for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
        {
        nHash += Object::hashCode(hIter->next());
        }
    return nHash;
    }

SafeHashMap::View SafeHashMap::EntrySet::getDelegate() const
    {
    return f_ohMap;
    }

SafeHashMap::Handle SafeHashMap::EntrySet::getDelegate()
    {
    return cast<SafeHashMap::Handle>(f_ohMap);
    }


// ----- inner class: EntrySet Iterator -------------------------------------

Iterator::Handle SafeHashMap::EntrySet::instantiateIterator() const
    {
    return EntrySetIterator::create(getDelegate());
    }

Muterator::Handle SafeHashMap::EntrySet::instantiateIterator()
    {
    return EntrySetIterator::create(getDelegate());
    }

SafeHashMap::EntrySetIterator::EntrySetIterator(SafeHashMap::Holder ohMap)
        : f_ohMap(self(), ohMap), m_vaeBucket(self()), m_iBucket(size32_t(-1)),
          m_hEntryPrev(self(), Entry::Handle(NULL), true),
          m_fResized(false), m_fDeactivated(false),
          m_fViewer(!instanceof<Map::Handle>(ohMap))
    {
    }

SafeHashMap::EntrySetIterator::~EntrySetIterator()
    {
    try
        {
        deactivate();
        }
    catch (Exception::View e)
        {
        COH_LOG("Unexpected exception in destructor: " << e, 1);
        // eat it
        }
    }


// ----- internal -------------------------------------------------------

void SafeHashMap::EntrySetIterator::advance()
    {
    if (m_fDeactivated)
        {
        // the Iterator has already reached the end on a previous
        // call to advance()
        return;
        }

    SafeHashMap::View vMap      = f_ohMap;
    ObjectArray::View vaeBucket = m_vaeBucket;

    if (NULL == vaeBucket)
        {
        vMap->iteratorActivated();
        vaeBucket = m_vaeBucket = vMap->getStableBucketArray();
        }

    Entry::Handle hEntry   = m_hEntryPrev;
    size32_t      iBucket  = size32_t(-1);  // -1 indicates no change
    bool          fResized = m_fResized;  // resize previously detected
    while (true)
        {
        if (hEntry != NULL)
            {
            // advance within the currrent bucket
            hEntry = hEntry->m_hNext;
            }

        // check if the current bucket has been exhausted, and if
        // it has, then advance to the first non-empty bucket
        if (NULL == hEntry)
            {
            iBucket = m_iBucket;
            size32_t cBuckets = vaeBucket->length;
            do
                {
                if (++iBucket >= cBuckets)
                    {
                    // a resize could have occurred to cause this
                    if (!fResized && vaeBucket != vMap->m_haeBucket)
                        {
                        // at this point, a resize has just been
                        // detected; the handling for the resize
                        // is below
                        break;
                        }

                    deactivate();
                    return;
                    }

                hEntry = cast<Entry::Handle>(vaeBucket[iBucket]);
                }
            while (NULL == hEntry);
            }

        // check for a resize having occurred since the iterator
        // was created
        if (!fResized && vaeBucket != vMap->m_haeBucket)
            {
            m_fResized = true;

            // if there is a previously-iterated entry, the
            // Iterator has to back up and find that same entry
            // in the cloned list of entries in the bucket; that
            // cloned list is used to maintain a nearly CME-free
            // view of the Map contents after the resize has
            // occurred.
            if (m_hEntryPrev != NULL)
                {
                // wait for the resize to complete (so that the
                // necessary clones of each entry will have been
                // created)
                vMap->getStableBucketArray();

                // find the same entry
                Object::View vKey = m_hEntryPrev->getKey();
                hEntry = cast<Entry::Handle>(vaeBucket[m_iBucket]);
                while (hEntry != NULL && hEntry->getKey() != vKey)
                    {
                    hEntry = hEntry->m_hNext;
                    }
                if (NULL == hEntry)
                    {
                    // previous has been removed, thus we don't
                    // know where to pick up the iteration and
                    // have to revert to a CME
                    deactivate();
                    COH_THROW (ConcurrentModificationException::create());
                    }
                m_hEntryPrev = hEntry;
                }

            // since a resize has occurred, the Iterator has to
            // start again from the last-iterated entry to find
            // the next entry, because the entry that this
            // Iterator had previously is a "real" entry which
            // is in the new "current" bucket array for the Map,
            // while the entries being iterated (post-resize) are
            // just copies of the entries created to maintain the
            // pre-resize iteration order
            advance();
            return;
            }

        // after a resize occurs, the entries being iterated
        // over are no longer the "real" entries; they are simply
        // place-holders for purpose of maintaining the order of
        // the iterator; if this has occurred, find the real
        // entry and make it visible from the Iterator
        Entry::Handle hEntryVisible = fResized
                           ? vMap->getEntryInternal(hEntry->getKey())
                           : hEntry;

        // update the current bucket index if the iterator
        // advanced to a new bucket
        if (iBucket != (size32_t) -1)
            {
            m_iBucket = iBucket;
            }


        // after a resize, the entry could have been removed, and
        // that would not have shown up in the pre-resize entries
        // that this iterator is going over so check for a remove
        if (hEntryVisible != NULL)
            {
            // remember the entry being iterated next; if a
            // resize has occurred, this is a copy of the
            // actual entry, maintained by the Iterator for
            // purposes of providing a nearly CME-free
            // iteration
            m_hEntryPrev = hEntry;

            // report back the actual entry that exists in the
            // Map that is being iterated next
            if (m_fViewer)
                {
                // Iterator over Set::View, don't return Handles to Entries
                setNext((Object::View) hEntryVisible);
                }
            else
                {
                setNext(hEntryVisible);
                }
            return;
            }
        }
    }

void SafeHashMap::EntrySetIterator::deactivate() const
    {
    if (!m_fDeactivated)
        {
        // no more entries to iterate; notify the
        // containing Map that this Iterator is no
        // longer active against a particular version
        // of the bucket array

        f_ohMap->iteratorDeactivated();
        m_fDeactivated = true;
        }
    }

void SafeHashMap::EntrySetIterator::remove(Object::Holder oh)
    {
    if (m_fViewer)
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    SafeHashMap::Handle hMap = cast<SafeHashMap::Handle>(f_ohMap);
    hMap->remove(cast<Map::Entry::Handle>(oh)->getKey());
    }

COH_CLOSE_NAMESPACE2
