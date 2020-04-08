/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/OldCache.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/FilterMuterator.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/SparseArray.hpp"

#include "private/coherence/util/Date.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::ArrayList;
using coherence::util::Date;
using coherence::util::Collections;
using coherence::util::ConcurrentModificationException;
using coherence::util::FilterMuterator;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::SparseArray;


// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const size32_t OldCache::default_units)
COH_EXPORT_SPEC_MEMBER(const int32_t  OldCache::default_expire)
COH_EXPORT_SPEC_MEMBER(const int32_t  OldCache::default_flush)


// ----- static initialization ----------------------------------------------

float64_t OldCache::getDefaultPrune()
    {
    float64_t dflPrune = 0.75F;
    return dflPrune;
    }
COH_STATIC_INIT(OldCache::getDefaultPrune());

// ----- constructors -------------------------------------------------------

OldCache::OldCache(size32_t  cUnits,
        int32_t   cExpiryMillis,
        float64_t dflPruneLevel,
        size32_t  cInitialBuckets,
        float32_t flLoadFactor,
        float32_t flGrowthRate)
        : super(cInitialBuckets, flLoadFactor, flGrowthRate),
          m_cCurUnits(0),
          m_cMaxUnits(cUnits),
          m_dflPruneLevel(std::min(std::max(dflPruneLevel, 0.0), 0.99)),
          m_cPruneUnits((size32_t)(std::min(std::max(dflPruneLevel, 0.0), 0.99) *cUnits)),
          m_cExpiryDelay(cExpiryMillis),
          m_cFlushDelay(0),
          m_lNextFlush(self(), 0),
          f_hStats(self(), SimpleCacheStatistics::create(), /*fMutable*/ true),
          m_hListenerSupport(self()),
          m_nEvictionType(EvictionPolicy::eviction_policy_hybrid),
          m_hPolicy(self()),
          m_CalculatorType(unit_calculator_fixed),
          m_vCalculator(self()),
          m_lLastPrune(System::safeTimeMillis()),
          m_cAvgTouch(0),
          m_fAllowMutableValues(false)
    {
    scheduleFlush();
    }


// ----- ObservableMap interface --------------------------------------------


void OldCache::addKeyListener(MapListener::Handle hListener,
        Object::View voKey, bool fLite)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE(hListener != NULL)

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (NULL == hSupport)
            {
            hSupport = m_hListenerSupport = MapListenerSupport::create();
            }

        hSupport->addListener(hListener, voKey, fLite);
        }
    }

void OldCache::removeKeyListener(MapListener::Handle vListener,
        Object::View voKey)
    {
    COH_SYNCHRONIZED(this)
        {
        COH_ENSURE(vListener != NULL);

        MapListenerSupport::Handle hSupport = m_hListenerSupport;
        if (hSupport != NULL)
            {
            hSupport->removeListener(vListener, voKey);
            if (hSupport->isEmpty())
                {
                m_hListenerSupport = NULL;
                }
            }
        }
    }

void OldCache::addMapListener(MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }

void OldCache::removeMapListener(MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }

void OldCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    COH_ENSURE(hListener != NULL);

    MapListenerSupport::Handle hSupport = m_hListenerSupport;
    if (hSupport == NULL)
        {
        hSupport = m_hListenerSupport = MapListenerSupport::create();
        }

    hSupport->addListener(hListener, vFilter, fLite);
    }

void OldCache::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    COH_ENSURE(hListener != NULL);

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


// ----- Map interface ------------------------------------------------------

bool OldCache::isEmpty() const
    {
    // this will call checkFlush()
    return size() == 0;
    }

size32_t OldCache::size() const
    {
    // check if the cache needs flushing
    checkFlush();

    return super::size();
    }

bool OldCache::containsKey(Object::View vKey) const
    {
    // check if the cache needs flushing
    checkFlush();

    return super::containsKey(vKey);
    }

Object::Holder OldCache::get(Object::View vKey) const
    {
    SafeHashMap::Entry::View vEntry = getEntry(vKey);
    return (NULL == vEntry ? NULL : vEntry->getValue());
    }

Object::Holder OldCache::put(Object::View vKey, Object::Holder ohValue)
    {
    return put(vKey, ohValue, 0L);
    }

Object::Holder OldCache::remove(Object::View vKey)
    {
    // check if the cache needs flushing
    checkFlush();

    COH_SYNCHRONIZED (this)
        {
        // determine if the key is in the cache
        Entry::Handle hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
        if (NULL == hEntry)
            {
            return NULL;
            }
        else
            {
            hEntry->discard();
            removeEntryInternal(hEntry);
            return hEntry->getValue();
            }
        }
    }

void OldCache::clear()
    {
    // push next flush out to avoid attempts at multiple simultaneous
    // flushes
    deferFlush();

    COH_SYNCHRONIZED (this)
        {
        while (true)
            {
            try
                {
                // notify cache entries of their impending removal
                for (Iterator::Handle hIter = entrySet()->iterator(); hIter->hasNext(); )
                    {
                    cast<Entry::Handle>(hIter->next())->discard();
                    }

                // verify that the cache maintains its data correctly
               if (m_cCurUnits != 0)
                    {
                    // soft assertion
                    COH_LOG("Invalid unit count after clear: " << m_cCurUnits, 3);
                    m_cCurUnits = 0;
                    }
                break;
                }
            catch (ConcurrentModificationException::View)
                {
                }
            }

        // reset the cache storage
        super::clear();

        // reset hit/miss stats
        resetHitStatistics();

        // schedule next flush
        scheduleFlush();
        }
    }


// ----- CacheMap interface -------------------------------------------------

Object::Holder OldCache::put(Object::View vKey, Object::Holder ohValue,
        int64_t cMillis)
    {
    // check if the cache needs flushing
    checkFlush();

    Entry::Handle  hEntry;
    Object::Holder ohOrig;

    COH_SYNCHRONIZED (this)
        {
        hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
        if (NULL == hEntry)
            {
            // new cache entry
            ohOrig = inherited::put(vKey, ohValue);
            }
        else
            {
            // cache entry already exists
            hEntry->touch();

            ohOrig = hEntry->setValue(ohValue);
            }

        if (cMillis != 0L)
            {
            if (NULL == hEntry)
                {
                hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
                }
            if (hEntry != NULL)
                {
                hEntry->setExpiryMillis(cMillis > 0L ?
                    System::safeTimeMillis() + cMillis : 0L);

                if (cMillis > 0 && getFlushDelay() == 0)
                    {
                    // the cache does not have a flush delay; we need to
                    // ensure that this entry would be eventually flushed
                    setFlushDelay(default_flush);
                    }
                }
            }

        // check the cache size (COH-467, COH-480)
        if (m_cCurUnits > m_cMaxUnits)
            {
            prune();

            // could have evicted the item we just inserted/updated
            if (NULL == getEntryInternal(vKey))
                {
                ohOrig = NULL;
                }
            }
        }

    f_hStats->registerPut(0L);
    return ohOrig;
    }

Map::View OldCache::getAll(Collection::View /*vColKeys*/) const
    {
    COH_THROW(UnsupportedOperationException::create());
    }

// ----- Cache management methods -------------------------------------------

void OldCache::release()
    {
    // clear the underlying map; this is needed as the entries contain hard
    // references back to the OldCache which would otherwise keep it from
    // being collected.  WeakReferences are not used as the associated cost
    // is too high for the performance sensitive caching.
    super::clear();
    }

void OldCache::evict(Object::View vKey)
    {
    Entry::Handle hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
    if (hEntry != NULL)
        {
        removeExpired(hEntry, true);
        }
    }

void OldCache::evictAll(Collection::View vColKeys)
    {
    for (Iterator::Handle hIter = vColKeys->iterator(); hIter->hasNext(); )
        {
        Object::View  vKey   = hIter->next();
        Entry::Handle hEntry = cast<Entry::Handle>(getEntryInternal(vKey));
        if (hEntry != NULL)
            {
            removeExpired(hEntry, true);
            }
        }
    }

void OldCache::evict()
    {
    COH_SYNCHRONIZED(this)
        {
        // push next flush out to avoid attempts at multiple simultaneous
        // flushes
        deferFlush();

        // walk all buckets
        ObjectArray::Handle haeBucket = m_haeBucket;
        size32_t            cBuckets  = haeBucket->length;
        for (size32_t iBucket = 0; iBucket < cBuckets; ++iBucket)
            {
            // walk all entries in the bucket
            Entry::Handle hEntry = cast<Entry::Handle>(haeBucket[iBucket]);
            while (hEntry != NULL)
                {
                if (hEntry->isExpired())
                    {
                    removeExpired(hEntry, true);
                    }
                hEntry = hEntry->getNext();
                }
            }

        // schedule next flush
        scheduleFlush();
        }
    }

CacheStatistics::View OldCache::getCacheStatistics() const
    {
    return (SimpleCacheStatistics::View) f_hStats;
    }

CacheStatistics::Handle OldCache::getCacheStatistics()
    {
    return (SimpleCacheStatistics::Handle) f_hStats;
    }

size32_t OldCache::getUnits() const
    {
    return m_cCurUnits;
    }

size32_t OldCache::getHighUnits() const
    {
    return m_cMaxUnits;
    }

void OldCache::setHighUnits(size32_t cMax)
    {
    COH_SYNCHRONIZED(this)
        {
        bool fShrink = cMax < m_cMaxUnits;

        m_cMaxUnits = cMax;

        // ensure that low units are in range
        setLowUnits((int32_t) (m_dflPruneLevel * cMax));

        if (fShrink)
            {
            checkSize();
            }
        }
    }

size32_t OldCache::getLowUnits() const
    {
    return m_cPruneUnits;
    }

void OldCache::setLowUnits(size32_t cUnits)
    {
    COH_SYNCHRONIZED(this)
        {
        if (cUnits == UnitCalculator::npos)
            {
            COH_THROW(IllegalArgumentException::create("low units out of bounds"));
            }

        if (cUnits >= m_cMaxUnits)
            {
            cUnits = (int32_t) (m_dflPruneLevel * m_cMaxUnits);
            }

        m_cPruneUnits = cUnits;
        }
    }

EvictionPolicy::EvictionPolicyType OldCache::getEvictionType() const
    {
    return m_nEvictionType;
    }

void OldCache::setEvictionType(EvictionPolicy::EvictionPolicyType nType)
    {
    COH_SYNCHRONIZED(this)
        {
        configureEviction(nType, NULL);
        }
    }

EvictionPolicy::View OldCache::getEvictionPolicy() const
    {
    return m_hPolicy;
    }

EvictionPolicy::Handle OldCache::getEvictionPolicy()
    {
    return m_hPolicy;
    }

void OldCache::setEvictionPolicy(EvictionPolicy::Handle hPolicy)
    {
    COH_SYNCHRONIZED(this)
        {
        EvictionPolicy::EvictionPolicyType nType =
                (NULL == hPolicy ? EvictionPolicy::eviction_policy_hybrid
                                 : EvictionPolicy::eviction_policy_external);
        configureEviction(nType, hPolicy);
        }
    }

OldCache::UnitCalculatorType OldCache::getUnitCalculatorType() const
    {
    return m_CalculatorType;
    }

void OldCache::setUnitCalculatorType(UnitCalculatorType nType)
    {
    configureUnitCalculator(nType, NULL);
    }

UnitCalculator::View OldCache::getUnitCalculator() const
    {
    return m_vCalculator;
    }

void OldCache::setUnitCalculator(UnitCalculator::Handle hCalculator)
    {
    UnitCalculatorType nType =
            (NULL == hCalculator ? unit_calculator_fixed
                                 : unit_calculator_external);
    configureUnitCalculator(nType, hCalculator);
    }

int32_t OldCache::getExpiryDelay() const
    {
    return m_cExpiryDelay;
    }

void OldCache::setExpiryDelay(int32_t cMillis)
    {
    m_cExpiryDelay = std::max(cMillis, 0);
    }

int32_t OldCache::getFlushDelay() const
    {
    return m_cFlushDelay;
    }

void OldCache::setFlushDelay(int32_t cMillis)
    {
    m_cFlushDelay = std::max(cMillis, 0);
    scheduleFlush();
    }

int64_t OldCache::getFlushTime() const
    {
    return m_lNextFlush;
    }

void OldCache::setFlushTime(int64_t lMillis)
    {
    m_lNextFlush = lMillis;
    checkFlush();
    }

void OldCache::setAllowMutableValues(bool fAllow)
    {
    COH_ENSURE(size() == 0);
    m_fAllowMutableValues = fAllow;
    }

bool OldCache::isAllowMutableValues() const
    {
    return m_fAllowMutableValues;
    }


// ----- statistics ---------------------------------------------------------

int64_t OldCache::getCacheHits() const
    {
    return f_hStats->getCacheHits();
    }

int64_t OldCache::getCacheMisses() const
    {
    return f_hStats->getCacheMisses();
    }

float64_t OldCache::getHitProbability() const
    {
    return f_hStats->getHitProbability();
    }

void OldCache::resetHitStatistics()
    {
    f_hStats->resetHitStatistics();
    }

TypedHandle<const String> OldCache::toString() const
    {
    return Collections::toString((Collection::View) entrySet());
    }


// ----- helper methods -----------------------------------------------------

SafeHashMap::Entry::Handle OldCache::instantiateEntry(Object::View vKey,
        Object::Holder ohValue, size32_t nHash)
    {
    return Entry::create(vKey, ohValue, nHash, this);
    }

SafeHashMap::Entry::Handle OldCache::instantiateEntry(
        SafeHashMap::Entry::View vEntry)
    {
    return Entry::create(cast<OldCache::Entry::View>(vEntry));
    }

Set::View OldCache::instantiateEntrySet() const
    {
    return OldCache::EntrySet::create(this);
    }

Set::Handle OldCache::instantiateEntrySet()
    {
    return EntrySet::create((SafeHashMap::Handle) this);
    }

Set::Handle OldCache::instantiateKeySet()
    {
    return KeySet::create((SafeHashMap::Handle) this);
    }

Set::View OldCache::instantiateKeySet() const
    {
    return KeySet::create(this);
    }

Collection::Handle OldCache::instantiateValuesCollection()
    {
    return ValuesCollection::create((SafeHashMap::Handle) this);
    }

Collection::View OldCache::instantiateValuesCollection() const
    {
    return ValuesCollection::create(this);
    }

SafeHashMap::Entry::View OldCache::getEntry(Object::View vKey) const
    {
    // check if the cache needs flushing
    checkFlush();

    OldCache::Entry::Handle hEntry = cast<OldCache::Entry::Handle>(getEntryInternal(vKey));
    if (NULL == hEntry)
        {
        f_hStats->registerMiss();
        }
    else
        {
        f_hStats->registerHit();
        hEntry->touch();
        }

    return hEntry;
    }

SafeHashMap::Entry::Handle OldCache::getEntry(Object::View vKey)
    {
    // check if the cache needs flushing
    checkFlush();

    OldCache::Entry::Handle hEntry = cast<OldCache::Entry::Handle>(getEntryInternal(vKey));
    if (NULL == hEntry)
        {
        f_hStats->registerMiss();
        }
    else
        {
        f_hStats->registerHit();
        hEntry->touch();
        }

    return hEntry;
    }

void OldCache::configureEviction(EvictionPolicy::EvictionPolicyType nType,
        EvictionPolicy::Handle hPolicy)
    {
    COH_SYNCHRONIZED(this)
        {
        switch (nType)
            {
            case EvictionPolicy::eviction_policy_hybrid:
            case EvictionPolicy::eviction_policy_lru:
            case EvictionPolicy::eviction_policy_lfu:
                break;

            case EvictionPolicy::eviction_policy_external:
                if (NULL == hPolicy)
                    {
                    COH_THROW_STREAM(IllegalStateException,
                            "an attempt was made to"
                            << " set eviction type to eviction_policy_external"
                            << " without providing an external EvictionPolicy");
                    }
                break;

            default:
                COH_THROW_STREAM (IllegalArgumentException,
                        "unknown eviction type: " << nType);
            }

        EvictionPolicy::Handle hPolicyPrev = m_hPolicy;
        if (instanceof<MapListener::Handle>(hPolicyPrev))
            {
            removeFilterListener(cast<MapListener::Handle>(hPolicyPrev));
            }

        m_nEvictionType = nType;
        m_hPolicy       = hPolicy;

        if (instanceof<MapListener::Handle>(hPolicy))
            {
            addFilterListener(cast<MapListener::Handle>(hPolicy));
            }
        }
    }

void OldCache::configureUnitCalculator(UnitCalculatorType nType,
        UnitCalculator::Handle hCalculator)
    {
    switch (nType)
        {
        case unit_calculator_fixed:
            if (nType == m_CalculatorType)
                {
                // nothing to do
                return;
                }
            break;

        case unit_calculator_external:
            if (NULL == hCalculator)
                {
                COH_THROW_STREAM(IllegalStateException, "an attempt was made to"
                    << " set the unit calculator type to"
                    << " unit_calculator_external"
                    << " without providing an external UnitCalculator");
                }
            else if (unit_calculator_external == m_CalculatorType &&
                    hCalculator->equals(m_vCalculator))
                {
                // nothing to do
                return;
                }
            break;

        default:
            COH_THROW_STREAM(IllegalArgumentException,
                    "unknown unit calculator type: " << nType);
        }

    m_CalculatorType  = nType;
    m_vCalculator     = hCalculator;

    // recalculate unit costs
    for (Iterator::Handle hIter = entrySet()->iterator(); hIter->hasNext(); )
        {
        Entry::Handle hEntry = cast<Entry::Handle>(hIter->next());
        size32_t      cUnits = hEntry->calculateUnits(hEntry->getValue());

        // update both the entry unit count and total unit count
        hEntry->setUnits(cUnits);
        }
    }

SafeHashMap::Entry::Handle OldCache::getEntryInternal(Object::View vKey) const
    {
    Entry::Handle hEntry = cast<Entry::Handle>(super::getEntryInternal(vKey));

    if (hEntry != NULL && hEntry->isExpired())
        {
        removeExpired(hEntry, true);
        hEntry = NULL;
        }

    return hEntry;
    }

void OldCache::checkFlush() const
    {
    /*
    * We're going to cast away constness in removeExpiredEntry so that we
    * can support expiry from const methods. In particular the Iterators for
    * LocalCache expire entries based on a filter, and the get method is
    * also const. Casting away constness in this case will allow us to avoid
    * cyclical graphs present in other solutions, and requires the least
    * amount of extra work. After a long discussion with Mark on this we
    * decided it was the best course of action.
    */
    OldCache::Handle hThis = const_cast<OldCache*>(this);
    COH_ENSURE(hThis != NULL);

    hThis->checkFlush();
    }

void OldCache::checkFlush()
    {
    // check if flushing is required
    if (System::safeTimeMillis() > m_lNextFlush)
        {
        COH_SYNCHRONIZED(this)
            {
            // recheck so that only one thread flushes
            if (System::safeTimeMillis() > m_lNextFlush)
                {
                evict();
                }
            }
        }
    }

void OldCache::removeExpired(Entry::View vEntry, bool fRemoveInternal) const
    {
    /*
    * We're going to cast away constness in removeExpiredEntry so that we
    * can support expiry from const methods. In particular the Iterators for
    * LocalCache expire entries based on a filter, and the get method is
    * also const. Casting away constness in this case will allow us to avoid
    * cyclical graphs present in other solutions, and requires the least
    * amount of extra work. After a long discussion with Mark on this we
    * decided it was the best course of action.
    */
    OldCache::Handle hThis = const_cast<OldCache*>(this);
    COH_ENSURE(hThis != NULL);

    Entry::Handle hEntry = cast<OldCache::Entry::Handle>(
            hThis->super::getEntryInternal(vEntry->getKey()));
    if (hEntry != NULL)
        {
        hThis->removeExpired(hEntry, fRemoveInternal);
        }
    }

void OldCache::removeExpired(Entry::Handle hEntry,
        bool fRemoveInternal)
    {
    COH_SYNCHRONIZED(this)
        {
        hEntry->discard();
        if (fRemoveInternal)
            {
            super::removeEntryInternal(hEntry);
            }
        }
    }

void OldCache::incrementUnits(size32_t cDelta)
    {
    m_cCurUnits += cDelta;
    }

void OldCache::decrementUnits(size32_t cDelta)
    {
    m_cCurUnits -= cDelta;
    }

void OldCache::checkSize()
    {
    // check if pruning is required
    if (m_cCurUnits > m_cMaxUnits)
        {
        COH_SYNCHRONIZED (this)
            {
            // recheck so that only one thread prunes
            if (m_cCurUnits > m_cMaxUnits)
                {
                prune();
                }
            }
        }
    }

void OldCache::prune()
    {
    COH_SYNCHRONIZED(this)
        {
        size32_t cCur = getUnits();
        size32_t cMin = getLowUnits();
		size32_t cMax = getHighUnits();
        if (cCur < cMax)
            {
            return;
            }

        // COH-764: try just throwing away expired stuff
        evict();
		cCur = getUnits();
        if (cCur < cMax)
            {
            return;
            }

        int64_t dtStart = System::safeTimeMillis();

        EvictionPolicy::EvictionPolicyType nType = getEvictionType();
        switch (nType)
            {
            default:
            case EvictionPolicy::eviction_policy_hybrid:
                {
                // calculate a rough average number of touches that each
                // entry should expect to have
                CacheStatistics::View vStats = getCacheStatistics();
                m_cAvgTouch = (size32_t) ((vStats->getTotalPuts() + vStats->getTotalGets())
                            / (((int64_t) super::size() + 1) * ((int64_t) vStats->getCachePrunes() + 1)));

                // sum the entries' units per priority
                Array<size32_t>::Handle hacUnits;
                while (true)
                    {
                    try
                        {
                        hacUnits = Array<size32_t>::create(11);
                        for (Iterator::Handle hIter = entrySet()->iterator(); hIter->hasNext(); )
                            {
                            Entry::Handle hEntry = cast<Entry::Handle>(hIter->next());
                            size32_t      cUnits = hEntry->getUnits();
                            try
                                {
                                hacUnits[hEntry->getPriority()] += cUnits;
                                }
                            catch (IndexOutOfBoundsException::View e)
                                {
                                hacUnits[std::max(0, std::min(hEntry->getPriority(), 10))] += cUnits;
                                }
                            }
                        break;
                        }
                    catch (ConcurrentModificationException::View)
                        {
                        }
                    }

                size32_t cTotal        = 0;
                size32_t nPrunePriority = 0;
                while (nPrunePriority <= 10)
                    {
                    cTotal += hacUnits[nPrunePriority];
                    if (cTotal > cMin)
                        {
                        break;
                        }
                    ++nPrunePriority;
                    }

                // build a list of entries to discard
                Entry::Handle hEntryDiscardHead = NULL;
                Entry::Handle hEntryDiscardTail = NULL;

                // determine the number at the cut-off priority that must be pruned
                size32_t cAdditional = std::max(size32_t(0), cTotal - cMin);

                while (cCur > cMin)
                    {
                    try
                        {
                        for (Iterator::Handle hIter = entrySet()->iterator(); hIter->hasNext() && cCur > cMin; )
                            {
                            Entry::Handle hEntry    = cast<Entry::Handle>(hIter->next());
                            size32_t      nPriority = hEntry->getPriority();
                            if (nPriority >= nPrunePriority)
                                {
                                size32_t cUnits = hEntry->getUnits();
                                if (nPriority == nPrunePriority)
                                    {
                                    if (cAdditional <= 0)
                                        {
                                        continue;
                                        }
                                    cAdditional -= cUnits;
                                    }
                                cCur -= cUnits;

                                // remove the entry from the map
                                super::removeEntryInternal(hEntry);

                                // link the entry into the list of deferred
                                // removals, but without changing its "next"
                                // reference because the iterator that we are
                                // using here is counting on that "next" ref
                                if (NULL == hEntryDiscardHead)
                                    {
                                    hEntryDiscardHead = hEntry;
                                    }
                                else
                                    {
                                    hEntryDiscardTail->setNext(hEntry);
                                    }
                                hEntryDiscardTail = hEntry;
                                }
                            }

                        // seal the end of the linked list of entries to discard
                        if (hEntryDiscardTail != NULL)
                            {
                            hEntryDiscardTail->setNext(NULL);
                            }
                        break;
                        }
                    catch (ConcurrentModificationException::View)
                        {
                        }
                    }

                // process the list of deferred removals
                for (Entry::Handle hEntryDiscard = hEntryDiscardHead; NULL != hEntryDiscard; )
                    {
                    // unlink it altogether
                    Entry::Handle hEntryNext = hEntryDiscard->getNext();
                    hEntryDiscard->setNext(NULL);

                    // discard it
                    removeExpired(hEntryDiscard, false);

                    hEntryDiscard = hEntryNext;
                    }
                }
                break;

            case EvictionPolicy::eviction_policy_lru:
            case EvictionPolicy::eviction_policy_lfu:
                {
                bool fLRU  = (nType == EvictionPolicy::eviction_policy_lru);

                SparseArray::Handle hArray;
                while (true)
                    {
                    try
                        {
                        hArray = SparseArray::create();
                        for (Iterator::Handle hIter = entrySet()->iterator(); hIter->hasNext(); )
                            {
                            Entry::Handle  hEntry = cast<Entry::Handle>(hIter->next());
                            int64_t        lOrder = fLRU ? hEntry->getLastTouchMillis()
                                                 : hEntry->getTouchCount();
                            Object::Holder ohPrev  = hArray->set(lOrder, hEntry);
                            if (ohPrev != NULL)
                                {
                                // oops, more than one entry with the same order;
                                // make a list of entries
                                List::Handle hList;
                                if (instanceof<List::Handle>(ohPrev))
                                    {
                                    hList = cast<List::Handle>(ohPrev);
                                    }
                                else
                                    {
                                    hList = ArrayList::create();
                                    hList->add(cast<Entry::Handle>(ohPrev));
                                    }
                                hList->add(hEntry);
                                hArray->set(lOrder, hList);
                                }
                            }
                        break;
                        }
                    catch (ConcurrentModificationException::View)
                        {
                        }
                    }

                for (Iterator::Handle hIter = hArray->iterator();
                     getUnits() > cMin && hIter->hasNext(); )
                    {
                    Object::Holder oh = hIter->next();
                    if (instanceof<Entry::Handle>(oh))
                        {
                        Entry::Handle hEntry = cast<Entry::Handle>(oh);
                        removeExpired(hEntry, true);
                        }
                    else
                        {
                        List::Handle hList = cast<List::Handle>(oh);
                        for (Iterator::Handle hIterList = hList->iterator();
                             getUnits() > cMin && hIterList->hasNext();)
                            {
                            Entry::Handle hEntry = cast<Entry::Handle>(hIterList->next());
                            removeExpired(hEntry, true);
                            }
                        }
                    }
                }
                break;

            case EvictionPolicy::eviction_policy_external:
                getEvictionPolicy()->requestEviction(cMin);
                break;
            }

        switch (nType)
            {
            default:
            case EvictionPolicy::eviction_policy_hybrid:
            case EvictionPolicy::eviction_policy_lfu:
                {
                // reset touch counts
                ObjectArray::Handle haeBucket = m_haeBucket;
                size32_t            cBuckets  = haeBucket->length;
                for (size32_t iBucket = 0; iBucket < cBuckets; ++iBucket)
                    {
                    // walk all entries in the bucket
                    Entry::Handle hEntry = cast<Entry::Handle>(haeBucket[iBucket]);
                    while (hEntry != NULL)
                        {
                        hEntry->resetTouchCount();
                        hEntry = hEntry->getNext();
                        }
                    }
                }
            }

        f_hStats->registerCachePrune(dtStart);
        m_lLastPrune = System::safeTimeMillis();
        }
    }

void OldCache::deferFlush()
    {
    // push next flush out to avoid attempts at multiple simultaneous
    // flushes
    m_lNextFlush = Integer64::max_value;
    }

void OldCache::scheduleFlush()
    {
    int32_t cDelayMillis = m_cFlushDelay;
    m_lNextFlush = cDelayMillis == 0 ?
            Integer64::max_value :
            System::safeTimeMillis() + cDelayMillis;
    }

MapEvent::Handle OldCache::instantiateMapEvent(
        int32_t nId, Object::View vKey, Object::Holder ohValueOld,
        Object::Holder ohValueNew)
    {
    return MapEvent::create(this, nId, vKey, ohValueOld, ohValueNew);
    }


// ----- event dispatching ----------------------------------------------

MapListenerSupport::Handle OldCache::getMapListenerSupport()
    {
    return m_hListenerSupport;
    }

bool OldCache::hasListeners()
    {
    return NULL != m_hListenerSupport;
    }

void OldCache::dispatchEvent(MapEvent::Handle hEvt)
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

// ----- inner class: Entry -------------------------------------------------

OldCache::Entry::Entry(Object::View vKey, Object::Holder ohValue,
        size32_t nHash, OldCache::Handle hCache)
        : super(vKey, hCache->isAllowMutableValues()
                ? ohValue
                : (Object::Holder) immutable_view<Object>(ohValue),
            nHash),
          m_dtCreated(self()), m_dtLastUse(self()), m_dtExpiry(self(), 0),
          m_cUses(0), m_cUnits(0),
          f_hCache(self(), hCache, /*fMutable*/ true)
    {
    m_dtLastUse = m_dtCreated = System::safeTimeMillis();
    }

OldCache::Entry::Entry(const Entry& that)
    : super(that), m_dtCreated(that.m_dtCreated),
      m_dtLastUse(that.m_dtLastUse), m_dtExpiry(that.m_dtExpiry),
      m_cUses(that.m_cUses), m_cUnits(that.m_cUnits),
      f_hCache(self(), NULL, /*fMutable*/ true)
    {
    }

OldCache::Entry::Entry(Entry::View vThat)
        : super(vThat), m_dtCreated(vThat->m_dtCreated),
        m_dtLastUse(vThat->m_dtLastUse), m_dtExpiry(vThat->m_dtExpiry),
        m_cUses(vThat->m_cUses), m_cUnits(vThat->m_cUnits),
        f_hCache(self(), vThat->f_hCache, /*fMutable*/ true)
    {
    }

bool OldCache::Entry::isExpired() const
    {
    int64_t dtExpiry = m_dtExpiry;
    return dtExpiry != 0 && dtExpiry < System::safeTimeMillis();
    }

int32_t OldCache::Entry::indexOfMSB(size32_t n) const
    {
    // @see http://aggregate.org/MAGIC/
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    n -= ((n >> 1) & 0x55555555);
    n = (((n >> 2) & 0x33333333) + (n & 0x33333333));
    n = (((n >> 4) + n) & 0x0f0f0f0f);
    n += (n >> 8);
    n += (n >> 16);
    n = (n & 0x0000003f) - 1;
    return n;
    }

int32_t OldCache::Entry::getPriority() const
    {
    // calculate an LRU score - how recently was the entry used?
    int64_t        dtPrune   = f_hCache->m_lLastPrune;
    int64_t        dtTouch   = m_dtLastUse;
    int            nScoreLRU = 0;
    if (dtTouch > dtPrune)
        {
        // measure recentness against the window of time since the
        // last prune
        int64_t   dtCurrent      = System::safeTimeMillis();
        int64_t   cMillisDormant = dtCurrent - dtTouch;
        int64_t   cMillisWindow  = dtCurrent - dtPrune;
        float64_t dflPct         = (cMillisWindow - cMillisDormant)
                                 / (1.0 + cMillisWindow);
        nScoreLRU = 1 + indexOfMSB((uint32_t) ((dflPct * dflPct * 64)));
        }

    // calculate "frequency" - how often has the entry been used?
    int32_t cUses     = m_cUses;
    int32_t nScoreLFU = 0;
    if (cUses > 0)
        {
        nScoreLFU = 1;

        int32_t cAvg = f_hCache->m_cAvgTouch;
        if (cUses > cAvg)
            {
            ++nScoreLFU;
            }

        int32_t cAdj = (cUses << 1) - cAvg;
        if (cAdj > 0)
            {
            nScoreLFU += 1 + std::min(4,
                indexOfMSB((uint32_t) ((cAdj << 3) / (1.0 + cAvg))));
            }
        }

    // use comparison to another entry as a bonus score
    Entry::View vEntryNext = getNext();
    if (vEntryNext != NULL)
        {
        if (dtTouch > vEntryNext->m_dtLastUse)
            {
            ++nScoreLRU;
            }
        if (cUses > vEntryNext->m_cUses)
            {
            ++nScoreLFU;
            }
        }

    return std::max(0, 10 - nScoreLRU - nScoreLFU);
    }

void OldCache::Entry::discard()
    {
    if (!isDiscarded())
       {
       FinalHandle<OldCache>& hMap = f_hCache;
       COH_SYNCHRONIZED (hMap)
           {
           size32_t cUnits = m_cUnits;
           if (cUnits == UnitCalculator::npos)
               {
               // entry is discarded; avoid repetitive events
               return;
               }

           if (cUnits > 0)
               {
               hMap->decrementUnits(cUnits);
               }

           m_cUnits = UnitCalculator::npos;
           }

       // issue remove notification
       if (hMap->hasListeners())
           {
           hMap->dispatchEvent(hMap->instantiateMapEvent(
               MapEvent::entry_deleted, getKey(), getValue(), NULL));
           }
       }
    }

bool OldCache::Entry::isDiscarded()
    {
    return m_cUnits == UnitCalculator::npos;
    }

void OldCache::Entry::scheduleExpiry()
    {
    int64_t dtExpiry = 0L;
    int32_t cDelay   = f_hCache->m_cExpiryDelay;
    if (cDelay > 0)
        {
        dtExpiry = System::safeTimeMillis() + cDelay;
        }
    setExpiryMillis(dtExpiry);
    }

void OldCache::Entry::touch()
    {
    ++m_cUses;
    m_dtLastUse = System::safeTimeMillis();

    EvictionPolicy::Handle hPolicy = f_hCache->getEvictionPolicy();
    if (hPolicy != NULL)
        {
        hPolicy->entryTouched(this);
        }
    }

void OldCache::Entry::resetTouchCount()
    {
    int32_t cUses = m_cUses;
    if (cUses > 0)
        {
        m_cUses = std::max(1, cUses >> 4);
        }
    }

void OldCache::Entry::onAdd()
    {
    scheduleExpiry();

    // update units
    size32_t               cNewUnits = calculateUnits(getValue());
    FinalHandle<OldCache>& hMap      = f_hCache;

    COH_SYNCHRONIZED (hMap)
        {
        size32_t cOldUnits = m_cUnits;
        if (cOldUnits == UnitCalculator::npos)
            {
            // entry is discarded; avoid exception
            return;
            }

        if (cNewUnits > cOldUnits)
            {
            hMap->incrementUnits(cNewUnits - cOldUnits);
            m_cUnits = cNewUnits;
            }
        else if (cNewUnits < cOldUnits)
            {
            hMap->decrementUnits(cOldUnits - cNewUnits);
            m_cUnits = cNewUnits;
            }
        }

    // issue add notification
    if (hMap->hasListeners())
        {
        hMap->dispatchEvent(hMap->instantiateMapEvent(
            MapEvent::entry_inserted, getKey(), NULL, getValue()));
        }
    }

Object::Holder OldCache::Entry::setValue(Object::Holder ohValue)
    {
    FinalHandle<OldCache>& hMap = f_hCache;

    if (!hMap->isAllowMutableValues())
        {
        ohValue = immutable_view<Object>(ohValue);
        }

    // optimization - verify that the entry is still valid
    if (m_cUnits == UnitCalculator::npos)
        {
        // entry is discarded; avoid exception
        super::setValue(ohValue);
        return NULL;
        }

    // perform the entry update
    Object::Holder ohPrev;
    size32_t       cNewUnits = calculateUnits(ohValue);
    COH_SYNCHRONIZED (hMap)
        {
        size32_t cOldUnits = m_cUnits;
        if (cOldUnits == UnitCalculator::npos)
            {
            // entry is discarded; avoid repetitive events
            super::setValue(ohValue);
            return NULL;
            }

        if (cNewUnits > cOldUnits)
            {
            hMap->incrementUnits(cOldUnits - cNewUnits);
            m_cUnits = cNewUnits;
            }
        else if (cNewUnits < cOldUnits)
            {
            hMap->decrementUnits(cNewUnits - cOldUnits);
            m_cUnits = cNewUnits;
            }

        ohPrev = super::setValue(ohValue);
        }

    scheduleExpiry();

    // issue update notification
    if (hMap->hasListeners())
        {
        hMap->dispatchEvent(hMap->instantiateMapEvent(
            MapEvent::entry_updated, getKey(), ohPrev, ohValue));
        }

    return ohPrev;
    }

size32_t OldCache::Entry::calculateUnits(Object::Holder ohValue)
    {
    switch (f_hCache->getUnitCalculatorType())
        {
        case unit_calculator_external:
            return f_hCache->getUnitCalculator()->calculateUnits(getKey(), ohValue);

        case unit_calculator_fixed:
        default:
            return 1;
        }
    }

TypedHandle<const String> OldCache::Entry::toString() const
    {
    int64_t dtExpiry = getExpiryMillis();

    String::View vs = COH_TO_STRING(super::toString() << ", priority=" << getPriority()
       << ", created=" << getCreatedMillis()
       << ", last-use=" << getLastTouchMillis()
       << ", expiry=");

       if (dtExpiry == 0)
           {
           vs = COH_TO_STRING(vs << "none");
           }
       else
           {
           vs = COH_TO_STRING(vs << Date::create(dtExpiry));
           }

    return COH_TO_STRING(vs << (isExpired() ? " (expired)" : "")
       << ", use-count=" << getTouchCount()
       << ", units=" << getUnits());
    }

int64_t OldCache::Entry::getCreatedMillis() const
    {
    return m_dtCreated;
    }

int64_t OldCache::Entry::getExpiryMillis() const
    {
    return m_dtExpiry;
    }

int64_t OldCache::Entry::getLastTouchMillis() const
    {
    return m_dtLastUse;
    }

int32_t OldCache::Entry::getTouchCount() const
    {
    return m_cUses;
    }

size32_t OldCache::Entry::getUnits() const
    {
    return m_cUnits;
    }

void OldCache::Entry::setExpiryMillis(int64_t lMillis)
    {
    m_dtExpiry = lMillis;
    }

void OldCache::Entry::setUnits(int32_t cUnits)
    {
    m_cUnits = cUnits;
    }

OldCache::Entry::Handle OldCache::Entry::getNext()
    {
    return cast<OldCache::Entry::Handle>(m_hNext);
    }

OldCache::Entry::View OldCache::Entry::getNext() const
    {
    return cast<OldCache::Entry::View>(m_hNext);
    }

void OldCache::Entry::setNext(SafeHashMap::Entry::Handle hEntry)
    {
    m_hNext = hEntry;
    }


// ----- inner class IteratorFilter -----------------------------------------

OldCache::IteratorFilter::IteratorFilter(OldCache::Holder thCache)
        : f_thCache(self(), thCache)
    {
    }

bool OldCache::IteratorFilter::evaluate(Object::View v) const
    {
    OldCache::Entry::View vEntry   = cast<OldCache::Entry::View>(v);
    bool                  fExpired = vEntry->isExpired();
    if (fExpired)
        {
        f_thCache->removeExpired(vEntry, true);
        }
    return !fExpired;
    }

// ----- inner class: EntrySet ----------------------------------------------

OldCache::EntrySet::EntrySet(SafeHashMap::Handle hMap)
        : super(hMap)
    {
    }

OldCache::EntrySet::EntrySet(SafeHashMap::View vMap)
        : super(vMap)
    {

    }

Iterator::Handle OldCache::EntrySet::iterator() const
    {
    // complete entry set iterator
    Iterator::Handle hIter = instantiateIterator();

    return FilterMuterator::create(hIter, IteratorFilter::create(
            cast<OldCache::Holder >(f_ohMap)));
    }

/**
* {@inheritDoc}
*/
Muterator::Handle OldCache::EntrySet::iterator()
    {
    // complete entry set iterator
    Muterator::Handle hMiter = instantiateIterator();
    return FilterMuterator::create(hMiter, IteratorFilter::create(
            cast<OldCache::Holder >(f_ohMap)));
    }

ObjectArray::Handle OldCache::EntrySet::toArray(ObjectArray::Handle hao) const
    {
    ObjectArray::Handle haoAll = super::toArray(hao);
    size32_t            cAll   = haoAll->length;

    size32_t ofSrc  = 0;
    size32_t ofDest = 0;
    while (ofSrc < cAll)
        {
        Entry::Handle hEntry = cast<Entry::Handle>(haoAll[ofSrc]);
        if (hEntry == NULL)
            {
            break;
            }
        else if (hEntry->isExpired())
            {
            cast<OldCache::Holder>(f_ohMap)->removeExpired(hEntry, true);
            }
        else
            {
            if (ofSrc > ofDest)
                {
                haoAll[ofDest] = haoAll[ofSrc];
                }
            ++ofDest;
            }
        ++ofSrc;
        }

    if (ofSrc == cAll && ofSrc == ofDest)
        {
        // no entries expired; return the original array
        return haoAll;
        }

    return ObjectArray::copy(haoAll, 0, ObjectArray::create(ofDest));
    }


// ----- inner class KeySet -------------------------------------------------

OldCache::KeySet::KeySet(SafeHashMap::Handle hMap)
        : super((Map::Handle)hMap)
    {
    }

OldCache::KeySet::KeySet(SafeHashMap::View vMap)
        : super((Map::View)vMap)
    {
    }

ObjectArray::Handle OldCache::KeySet::toArray(ObjectArray::Handle hao) const
    {
    // build list of non-expired keys
    ObjectArray::Handle haoAll = hao;
    size32_t            cAll = 0;

    // synchronizing prevents add/remove, keeping size() constant
    OldCache::Holder ohMap = cast<OldCache::Holder>(f_thMapDelegate);
    COH_SYNCHRONIZED (ohMap)
        {
        // create the array to store the map keys
        size32_t c = ohMap->size();
        if (haoAll == NULL || haoAll->length < c)
            {
            haoAll = ObjectArray::create(c);
            }
        else if (haoAll->length > c)
            {
            haoAll[c] = NULL;
            }
        if (c > 0)
            {
            // walk all buckets
            ObjectArray::View vaeBucket     = ohMap->m_haeBucket;
            size32_t          cBuckets      = vaeBucket->length;
            for (size32_t iBucket = 0; iBucket < cBuckets; ++iBucket)
                {
                // walk all entries in the bucket
                Entry::Handle hEntry = cast<Entry::Handle>(vaeBucket[iBucket]);
                while (hEntry != NULL)
                    {
                    if (hEntry->isExpired())
                        {
                        ohMap->removeExpired(hEntry, true);
                        }
                    else
                        {
                        haoAll[cAll++] = hEntry->getKey();
                        }
                    hEntry = cast<OldCache::Entry::Handle>(hEntry->getNext());
                    }
                }
            }
        }

    // if no entries had expired, just return the "work" array
    if (cAll == haoAll->length)
        {
        return haoAll;
        }

    // allocate the necessary array (or stick the null in at the
    // right place) per the Map spec
    return ObjectArray::copy(haoAll, 0, ObjectArray::create(haoAll->length));
    }


// ----- inner class ValuesCollection ---------------------------------------

OldCache::ValuesCollection::ValuesCollection(SafeHashMap::Handle hMap)
        : super((Map::Handle)hMap)
    {
    }

OldCache::ValuesCollection::ValuesCollection(SafeHashMap::View vMap)
        : super((Map::View)vMap)
    {
    }

ObjectArray::Handle OldCache::ValuesCollection::toArray(
        ObjectArray::Handle hao) const
    {
    // build list of non-expired values
    ObjectArray::Handle haoAll = hao;
    size32_t            cAll = 0;

    // synchronizing prevents add/remove, keeping size() constant
    OldCache::Holder thMap = cast<OldCache::Holder >(f_thMapDelegate);
    COH_SYNCHRONIZED (thMap)
        {
        // create the array to store the map values
        size32_t c = thMap->size();
        if (haoAll == NULL || haoAll->length < c)
            {
            haoAll = ObjectArray::create(c);
            }
        else if (haoAll->length > c)
            {
            haoAll[c] = NULL;
            }
        if (c > 0)
            {
            // walk all buckets
            ObjectArray::View vaeBucket     = thMap->m_haeBucket;
            size32_t          cBuckets      = vaeBucket->length;
            for (size32_t iBucket = 0; iBucket < cBuckets; ++iBucket)
                {
                // walk all entries in the bucket
                Entry::Handle hEntry = cast<Entry::Handle>(vaeBucket[iBucket]);
                while (hEntry != NULL)
                    {
                    if (hEntry->isExpired())
                        {
                        thMap->removeExpired(hEntry, true);
                        }
                    else
                        {
                        haoAll[cAll++] = hEntry->getKey();
                        }
                    hEntry = cast<OldCache::Entry::Handle>(hEntry->getNext());
                    }
                }
            }
        }

    // if no entries had expired, just return the "work" array
    if (cAll == haoAll->length)
        {
        return haoAll;
        }

    // allocate the necessary array (or stick the null in at the
    // right place) per the Map spec
    return ObjectArray::copy(haoAll, 0, ObjectArray::create(haoAll->length));
    }


COH_CLOSE_NAMESPACE3
