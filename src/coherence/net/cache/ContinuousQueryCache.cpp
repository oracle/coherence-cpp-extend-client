/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/ContinuousQueryCache.hpp"

#include "coherence/internal/net/NamedCacheDeactivationListener.hpp"

#include "coherence/util/AbstractMapListener.hpp"
#include "coherence/util/AbstractSet.hpp"
#include "coherence/util/AbstractStableIterator.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/FilterMuterator.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/MapEventTransformer.hpp"
#include "coherence/util/MapListenerSupport.hpp"
#include "coherence/util/MapTriggerListener.hpp"
#include "coherence/util/Muterator.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"
#include "coherence/util/SafeHashMap.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/Supplier.hpp"

#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/KeyAssociatedFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/filter/MapEventTransformerFilter.hpp"
#include "coherence/util/filter/NotFilter.hpp"

#include "coherence/util/processor/ExtractorProcessor.hpp"

#include "coherence/util/transformer/ExtractorEventTransformer.hpp"
#include "coherence/util/transformer/SemiLiteEventTransformer.hpp"

#include "private/coherence/component/util/QueueProcessor.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"
#include "private/coherence/util/ObservableHashMap.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"
#include "private/coherence/util/logging/Logger.hpp"



COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::component::util::QueueProcessor;
using coherence::component::util::SafeNamedCache;
using coherence::internal::net::NamedCacheDeactivationListener;
using coherence::util::AbstractMapListener;
using coherence::util::AbstractSet;
using coherence::util::AbstractStableIterator;
using coherence::util::ArrayList;
using coherence::util::Collections;
using coherence::util::ConcurrentModificationException;
using coherence::util::FilterMuterator;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::InvocableMapHelper;
using coherence::util::Iterator;
using coherence::util::MapEventTransformer;
using coherence::util::MapListenerSupport;
using coherence::util::MapTriggerListener;
using coherence::util::Muterator;
using coherence::util::ObservableHashMap;
using coherence::util::ReadOnlyArrayList;
using coherence::util::SafeHashMap;
using coherence::util::SimpleMapEntry;
using coherence::util::SimpleMapIndex;
using coherence::util::Supplier;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::AndFilter;
using coherence::util::filter::KeyAssociatedFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::filter::MapEventTransformerFilter;
using coherence::util::filter::NotFilter;
using coherence::util::processor::ExtractorProcessor;
using coherence::util::transformer::ExtractorEventTransformer;
using coherence::util::transformer::SemiLiteEventTransformer;

using coherence::util::logging::Logger;

COH_OPEN_NAMESPACE_ANON(ContinuousQueryCache)

/**
* Simple Supplier implementation that returns the NamedCache
* provided at construction.
*/
class COH_EXPORT CacheSupplier
        : public class_spec<CacheSupplier,
          extends<Object>,
          implements<Supplier> >
    {
    friend class factory<CacheSupplier>;

    // ---- constructors ----------------------------------------------------

    protected:
        /**
        * Create a new Supplier that returns the NamedCache
        * provided here.
        *
        * @param hNamedCache the NamedCache
        */
        CacheSupplier(NamedCache::Handle hNamedCache)
                : f_hNamedCache(self(), hNamedCache)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        CacheSupplier(const CacheSupplier&);
    
    // ----- Supplier interface -----------------------------------------
    
    public:
        virtual Object::Holder get() const
            {
            return f_hNamedCache;
            }

        virtual Object::Holder get()
            {
            return f_hNamedCache;
            }
    
    // ----- data members -----------------------------------------------
    
    protected:
        FinalHandle<NamedCache> f_hNamedCache;
    };

COH_CLOSE_NAMESPACE_ANON

// TODO REVIEW MF: Evaluate use of so many mutables, it is likely that
// there should be no such concept as an immutable remote cache; unfortunately
// the object model doesn't help us here.  We somewhat dealt with this in
// LocalCache in a bit cleaner of a way (avoiding all the mutables), but this
// was only doable because those caches are stored in CacheFactories.

// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t ContinuousQueryCache::state_disconnected)
COH_EXPORT_SPEC_MEMBER(const int32_t ContinuousQueryCache::state_configuring)
COH_EXPORT_SPEC_MEMBER(const int32_t ContinuousQueryCache::state_configured)
COH_EXPORT_SPEC_MEMBER(const int32_t ContinuousQueryCache::state_synchronized)


// ----- constructors -------------------------------------------------------

ContinuousQueryCache::ContinuousQueryCache(NamedCache::Handle hCache,
                                           Filter::View vFilter, bool fCacheValues,
                                           MapListener::Handle hListener, ValueExtractor::View vTransformer)
        : m_hCache(self(), NULL, /*fMutable*/ true),
          f_vsName(self(), NULL),
          f_vFilter(self(), vFilter == NULL ? cast<Filter::View>(AlwaysFilter::getInstance()) : vFilter),
          m_fCacheValues(fCacheValues),
          m_fReadOnly(vTransformer != NULL),
          m_cReconnectMillis(0),
          m_ldtConnectionTimestamp(self(), 0, /*fMutable*/ true),
          m_hMapLocal(self(), (ObservableMap::Handle) NULL, /*fMutable*/ true),
          m_nState(self(), state_disconnected, /*fMutable*/ true),
          m_hMapSyncReq(self(), (Map::Handle) NULL, /*fMutable*/ true),
          m_hTaskQueue(self(), NULL, /*fMutable*/ true),
          m_fListeners(false),
          m_vFilterAdd(self(), NULL, /*fMutable*/ true),
          m_vFilterRemove(self(), NULL, /*fMutable*/ true),
          m_whListenerAdd(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_whListenerRemove(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_whListenerService(self(), (MemberListener::Handle) NULL, /*fMutable*/ true),
          m_hListener(self(), hListener, /*fMutable*/ true),
          f_vTransformer(self(), vTransformer),
          f_hMapIndex(self(), SafeHashMap::create()),
          f_hSetKeys(self()),
          f_hSetEntries(self()),
          f_vCacheSupplier(self(), CacheSupplier::create(hCache)),
          m_vCacheNameSupplier(self(), NULL),
          m_hListenerDeactivation(self(), NULL, /*fMutable*/ true)
    {
    }

ContinuousQueryCache::ContinuousQueryCache(Supplier::View vCacheSupplier,
                                           Filter::View vFilter, bool fCacheValues,
                                           MapListener::Handle hListener, ValueExtractor::View vTransformer)
        : m_hCache(self(), NULL, /*fMutable*/ true),
          f_vsName(self(), NULL),
          f_vFilter(self(), vFilter == NULL ? cast<Filter::View>(AlwaysFilter::getInstance()) : vFilter),
          m_fCacheValues(fCacheValues),
          m_fReadOnly(vTransformer != NULL),
          m_cReconnectMillis(0),
          m_ldtConnectionTimestamp(self(), 0, /*fMutable*/ true),
          m_hMapLocal(self(), (ObservableMap::Handle) NULL, /*fMutable*/ true),
          m_nState(self(), state_disconnected, /*fMutable*/ true),
          m_hMapSyncReq(self(), (Map::Handle) NULL, /*fMutable*/ true),
          m_hTaskQueue(self(), NULL, true),
          m_fListeners(false),
          m_vFilterAdd(self(), NULL, /*fMutable*/ true),
          m_vFilterRemove(self(), NULL, /*fMutable*/ true),
          m_whListenerAdd(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_whListenerRemove(self(), (MapListener::Handle) NULL, /*fMutable*/ true),
          m_whListenerService(self(), (MemberListener::Handle) NULL, /*fMutable*/ true),
          m_hListener(self(), hListener, /*fMutable*/ true),
          f_vTransformer(self(), vTransformer),
          f_hMapIndex(self(), SafeHashMap::create()),
          f_hSetKeys(self()),
          f_hSetEntries(self()),
          f_vCacheSupplier(self(), vCacheSupplier),
          m_vCacheNameSupplier(self(), NULL),
          m_hListenerDeactivation(self(), NULL, /*fMutable*/ true)
    {
    }


// ----- accessors ----------------------------------------------------------

NamedCache::View ContinuousQueryCache::getCache() const
    {
    return getCacheInternal();
    }

NamedCache::Handle ContinuousQueryCache::getCache()
    {
    return getCacheInternal();
    }

Filter::View ContinuousQueryCache::getFilter() const
    {
    return f_vFilter;
    }

ValueExtractor::View ContinuousQueryCache::getTransformer() const
    {
    return f_vTransformer;
    }

MapListener::View ContinuousQueryCache::getMapListener() const
    {
    return m_hListener;
    }

bool ContinuousQueryCache::isCacheValues() const
    {
     // standard listener or non-null transformer override initial fCacheValue 
    return m_fCacheValues || isObserved() || f_vTransformer != NULL;          
    }

void ContinuousQueryCache::setCacheValues(bool fCacheValues)
    {
    COH_SYNCHRONIZED(this)
        {
        if (fCacheValues != m_fCacheValues)
            {
            bool fDidCacheValues = isCacheValues();

            // If we are no longer caching the values then we don't need the
            // local indexes.
            if (fDidCacheValues)
                {
                releaseIndexMap();
                }

            m_fCacheValues = fCacheValues;

            if (isCacheValues() != fDidCacheValues)
                {
                configureSynchronization(false);
                }
            }
        }
    }

bool ContinuousQueryCache::isReadOnly() const
    {
    return m_fReadOnly;
    }

void ContinuousQueryCache::setReadOnly(bool fReadOnly)
    {
    COH_SYNCHRONIZED(this)
        {
        if (fReadOnly != isReadOnly())
            {
            // once the cache is read-only, changing its read-only setting is
            // a mutating operation and thus is dis-allowed
            checkReadOnly();

            m_fReadOnly = fReadOnly;
            }
        }
    }

int32_t ContinuousQueryCache::getState() const
    {
    return m_nState;
    }

int64_t ContinuousQueryCache::getReconnectInterval() const
    {
    return m_cReconnectMillis;
    }

void ContinuousQueryCache::setReconnectInterval(int64_t cReconnectMillis)
    {
    m_cReconnectMillis = cReconnectMillis;
    }

Supplier::View ContinuousQueryCache::getCacheNameSupplier() const
    {
    return m_vCacheNameSupplier;
    }

void ContinuousQueryCache::setCacheNameSupplier(Supplier::View hCacheNameSupplier)
    {
    m_vCacheNameSupplier = hCacheNameSupplier;
    }

ObservableMap::Handle ContinuousQueryCache::instantiateInternalCache() const
    {
    return ObservableHashMap::create();
    }

ObservableMap::Handle ContinuousQueryCache::ensureInternalCache() const
    {
    ObservableMap::Handle hMapLocal = m_hMapLocal;
    if (hMapLocal == NULL)
        {
        hMapLocal = m_hMapLocal = instantiateInternalCache();

        MapListener::Handle hListener = m_hListener;
        bool                fLite     = !isCacheValues();
        if (hListener != NULL)
            {
            MapListener::Handle hListener = m_hListener;
            ensureEventQueue();
            hMapLocal->addFilterListener(instantiateEventRouter(m_hListener, fLite), NULL, fLite);
            }
        }
    return hMapLocal;
    }

ObservableMap::View ContinuousQueryCache::getInternalCache() const
    {
    ensureSynchronized(true);
    return m_hMapLocal;
    }

ObservableMap::Handle ContinuousQueryCache::getInternalCache()
    {
    ensureSynchronized(true);
    return m_hMapLocal;
    }

bool ContinuousQueryCache::isObserved() const
    {
    return m_fListeners;
    }

void ContinuousQueryCache::setObserved(bool fObserved)
    {
    COH_SYNCHRONIZED(this)
        {
        if (fObserved != isObserved())
            {
            bool fDidCacheValues = isCacheValues();

            m_fListeners = fObserved;

            if (isCacheValues() != fDidCacheValues)
                {
                configureSynchronization(false);
                }
            }
        }
    }

void ContinuousQueryCache::changeState(int32_t nState) const
    {
    switch (nState)
        {
        case state_disconnected:
            m_hCache = NULL;
            m_nState = state_disconnected;
            break;

        case state_configuring:
            COH_SYNCHRONIZED(this)
                {
                int32_t nStatePrev = m_nState;
                COH_ENSURE(state_disconnected == nStatePrev ||
                        state_synchronized == nStatePrev)

                m_hMapSyncReq = SafeHashMap::create();
                m_nState      = state_configuring;
                }
            break;

        case state_configured:
            COH_SYNCHRONIZED(this)
                {
                if (state_configuring == m_nState)
                    {
                    m_nState = state_configured;
                    }
                else
                    {
                    COH_THROW_STREAM(IllegalStateException, getCacheName() <<
                            " has been invalidated")
                    }
                }
            break;

        case state_synchronized:
            COH_SYNCHRONIZED(this)
                {
                if (state_configured == m_nState)
                    {
                    m_hMapSyncReq = NULL;
                    m_nState = state_synchronized;
                    }
                else
                    {
                    COH_THROW_STREAM(IllegalStateException, getCacheName() <<
                            " has been invalidated")
                    }
                }
            break;

        default:
            COH_THROW_STREAM(IllegalArgumentException, "unknown state: " <<
                    nState)
        }
    }


// ----- Map interface ------------------------------------------------------

Object::Holder ContinuousQueryCache::get(Object::View vKey) const
    {
    return isCacheValues()
        ? getInternalCache()->get(vKey)
        : containsKey(vKey)
            ? getCache()->get(vKey)
            : NULL;
    }

Object::Holder ContinuousQueryCache::put(Object::View vKey,
        Object::Holder ohValue)
    {
    Object::Holder ohOrig = NULL;

    checkReadOnly();
    checkEntry(vKey, ohValue);

    // see if the putAll() optimization will work; this requires the
    // return value to be locally cached, or knowledge that the orig
    // value is null (because it is not present in the
    // ContinuousQueryCache)
    NamedCache::Handle cache       = getCache();
    bool               fLocalCache = isCacheValues();
    bool               fPresent    = containsKey(vKey);
    if (fLocalCache || !fPresent)
        {
        ohOrig = fPresent ? getInternalCache()->get(vKey) : NULL;
        cache->putAll(Collections::singletonMap(vKey, ohValue));
        }
    else
        {
        ohOrig = cache->put(vKey, ohValue);
        if (!InvocableMapHelper::evaluateEntry(getFilter(),
                SimpleMapEntry::create(vKey, ohOrig)))
            {
            ohOrig = NULL;
            }
        }
    return ohOrig;
    }

Object::Holder ContinuousQueryCache::remove(Object::View vKey)
    {
    checkReadOnly();

    Object::Holder ohOrig = NULL;
    if (containsKey(vKey))
        {
        NamedCache::Handle hCache = getCache();
        if (isCacheValues())
            {
            ohOrig = getInternalCache()->get(vKey);
            removeBlind(vKey);
            }
        else
            {
            ohOrig = hCache->remove(vKey);
            }
        }
    return ohOrig;
    }

void ContinuousQueryCache::putAll(Map::View vMap)
    {
    checkReadOnly();
    for (Iterator::Handle hIter = vMap->entrySet()->iterator();
            hIter->hasNext(); )
        {
        checkEntry(cast<Map::Entry::View>(hIter->next()));
        }
    getCache()->putAll(vMap);
    }

void ContinuousQueryCache::clear()
    {
    checkReadOnly();
    getCache()->keySet()->removeAll(getInternalKeySet());
    }

bool ContinuousQueryCache::containsKey(Object::View vKey) const
    {
    return getInternalCache()->containsKey(vKey);
    }

Set::View ContinuousQueryCache::keySet() const
    {
    return f_hSetKeys;
    }

Set::Handle ContinuousQueryCache::keySet()
    {
    return f_hSetKeys;
    }

Set::View ContinuousQueryCache::entrySet() const
    {
    return f_hSetEntries;
    }

Set::Handle ContinuousQueryCache::entrySet()
    {
    return f_hSetEntries;
    }

size32_t ContinuousQueryCache::size() const
    {
    return getInternalCache()->size();
    }


// ----- CacheMap interface -------------------------------------------------

Map::View ContinuousQueryCache::getAll(Collection::View vKeys) const
    {
    Map::Handle hMapResult = NULL;

    Map::View vMapLocal  = getInternalCache();
    if (isCacheValues())
        {
        hMapResult = HashMap::create();
        for (Iterator::Handle hIter = vKeys->iterator(); hIter->hasNext(); )
            {
            Object::Holder ohKey = hIter->next();
            Object::Holder ohVal = vMapLocal->get(ohKey);
            if (ohVal != NULL || containsKey(ohKey))
                {
                hMapResult->put(ohKey, ohVal);
                }
            }
        }
    else if (vKeys->size() <= 1)
        {
        // optimization: the requested set is either empty or the caller
        // is doing a combined "containsKey() and get()"
        hMapResult = HashMap::create();
        for (Iterator::Handle hIter = vKeys->iterator(); hIter->hasNext(); )
            {
            Object::Holder ohKey = hIter->next();
            if (vMapLocal->containsKey(ohKey))
                {
                Object::Holder ohValue = getCache()->get(ohKey);
                if ((ohValue != NULL || vMapLocal->containsKey(ohKey))
                    && InvocableMapHelper::evaluateEntry(getFilter(),
                            SimpleMapEntry::create(ohKey, ohValue)))
                    {
                    hMapResult->put(ohKey, ohValue);
                    }
                }
            }
        }
    else
        {
        // since the values are not cached, delegate the processing to
        // the underlying NamedCache
        Collection::Handle hCollView = ArrayList::create(vKeys);
        hCollView->retainAll(vMapLocal->keySet());
        Map::View vMapResult = getCache()->getAll(hCollView);

        // verify that the returned contents should all be in this
        // cache
        Filter::View vFilter = getFilter();

        if (!vMapResult->isEmpty() && FilterMuterator::create(
                vMapResult->values()->iterator(),
                NotFilter::create(vFilter))->hasNext())
            {
            Iterator::Handle hIter = vMapResult->entrySet()->iterator();
            hMapResult = HashMap::create();
            while (hIter->hasNext())
                {
                Map::Entry::View vEntry  =
                        cast<Map::Entry::View>(hIter->next());
                if (InvocableMapHelper::evaluateEntry(vFilter, vEntry))
                    {
                    hMapResult->put(vEntry->getKey(), vEntry->getValue());
                    }
                }
            }
        else
            {
            return vMapResult;
            }
        }

    return hMapResult;
    }

Object::Holder ContinuousQueryCache::put(Object::View vKey,
        Object::Holder ohValue, int64_t cMillis)
    {
    if (CacheMap::expiry_default == cMillis)
        {
        return put(vKey, ohValue);
        }
    else
        {
        checkReadOnly();
        checkEntry(vKey, ohValue);

        Object::Holder ohOrig = getCache()->put(vKey, ohValue, cMillis);
        return InvocableMapHelper::evaluateEntry(getFilter(),
                SimpleMapEntry::create(vKey, ohOrig))? ohOrig : NULL;
        }
    }


// ----- ObservableMap interface --------------------------------------------

void ContinuousQueryCache::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    COH_ENSURE_PARAM(hListener)

    // need to cache values locally to provide standard (not lite) events
    if (!fLite)
        {
        setObserved(true);
        }

    ensureEventQueue();
    getInternalCache()->addKeyListener(instantiateEventRouter(hListener, fLite),
            vKey, fLite);
    }

void ContinuousQueryCache::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    COH_ENSURE_PARAM(hListener)

    getInternalCache()->removeKeyListener(instantiateEventRouter(hListener, false),
            vKey);
    }

void ContinuousQueryCache::addMapListener(MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }
    
void ContinuousQueryCache::removeMapListener(MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }
    
void ContinuousQueryCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    COH_ENSURE_PARAM(hListener)

    if (instanceof<MapTriggerListener::Handle>(hListener))
        {
        COH_THROW (IllegalArgumentException::create("ContinuousQueryCache does not support MapTriggerListeners"));
        }

    // need to cache values locally to provide event filtering and to
    // provide standard (not lite) events
    if (vFilter != NULL || !fLite)
        {
        setObserved(true);
        }

    ensureEventQueue();

    getInternalCache()->addFilterListener(instantiateEventRouter(hListener, fLite),
            vFilter, fLite);
    }

void ContinuousQueryCache::removeFilterListener(
        MapListener::Handle hListener, Filter::View vFilter)
    {
    COH_ENSURE_PARAM(hListener)

    getInternalCache()->removeFilterListener(
            instantiateEventRouter(hListener, false), vFilter);
    }


// ----- QueryMap interface -------------------------------------------------

Set::View ContinuousQueryCache::keySet(Filter::View vFilter) const
    {
    return isCacheValues() ?
        InvocableMapHelper::query(this, getIndexMap(), vFilter, false, false, NULL) :
        getCache()->keySet(mergeFilter(vFilter));
    }

Set::View ContinuousQueryCache::entrySet(Filter::View vFilter) const
    {
    return isCacheValues() ?
        InvocableMapHelper::query(this, getIndexMap(), vFilter, true, false, NULL) :
        getCache()->entrySet(mergeFilter(vFilter));
    }

Set::View ContinuousQueryCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return isCacheValues() ?
        InvocableMapHelper::query(this, getIndexMap(), vFilter, true, true, vComparator) :
        getCache()->entrySet(mergeFilter(vFilter), vComparator);
    }

void ContinuousQueryCache::addIndex(ValueExtractor::View vExtractor,
        bool fOrdered, Comparator::View vComparator)
    {
    COH_SYNCHRONIZED (this)
        {
        if (isCacheValues())
            {
            InvocableMapHelper::addIndex(vExtractor, fOrdered, vComparator,
                    getInternalCache(), ensureIndexMap());
            }
        }
    // addIndex is a no-op if many clients are trying to add the same one
    getCache()->addIndex(vExtractor, fOrdered, vComparator);
    }

void ContinuousQueryCache::removeIndex(ValueExtractor::View vExtractor)
    {
    // remove the index locally if we are caching values but do not
    // attempt to remove it from the underlying cache ...
    // removeIndex would kill all the other clients' performance if every
    // client balanced their add and remove index calls, so this cache
    // ignores the suggestion (since it cannot know if it was the cache
    // that originally added the index)
    COH_SYNCHRONIZED (this)
        {
        if (isCacheValues())
            {
            InvocableMapHelper::removeIndex(vExtractor, getInternalCache(), ensureIndexMap());
            }
        }
    }


// ----- InvocableMap interface ---------------------------------------------

Object::Holder ContinuousQueryCache::invoke(Object::View vKey,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    NamedCache::Handle hCache = getCache();
    return hCache->invoke(vKey, hAgent);
    }

Map::View ContinuousQueryCache::invokeAll(Collection::View vCollKeys,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    if (vCollKeys->isEmpty())
        {
        return Collections::emptyMap();
        }

    // verify that the non-existent keys are NOT present in the
    // underlying cache (assumption is most keys in the collection are
    // already in the ContinuousQueryCache)
    NamedCache::Handle hCache = getCache();

    return hCache->invokeAll(vCollKeys, hAgent);
    }

Map::View ContinuousQueryCache::invokeAll(Filter::View vFilter,
        InvocableMap::EntryProcessor::Handle hAgent)
    {
    return getCache()->invokeAll(mergeFilter(vFilter), hAgent);
    }

Object::Holder ContinuousQueryCache::aggregate(Collection::View vCollKeys,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    if (vCollKeys->isEmpty())
        {
        return hAgent->aggregate(Collections::emptySet());
        }

    // verify that the non-existent keys are NOT present in the
    // underlying cache (assumption is most keys in the collection are
    // already in the ContinuousQueryCache)
    NamedCache::View vCache = getCache();
    return vCache->aggregate(vCollKeys, hAgent);
    }

Object::Holder ContinuousQueryCache::aggregate(Filter::View vFilter,
        InvocableMap::EntryAggregator::Handle hAgent) const
    {
    return getCache()->aggregate(mergeFilter(vFilter), hAgent);
    }


// ----- ConcurrentMap interface --------------------------------------------

bool ContinuousQueryCache::lock(Object::View vKey, int64_t cWait) const
    {
    // locking is counted as a mutating operation
    checkReadOnly();

    return getCache()->lock(vKey, cWait);
    }

bool ContinuousQueryCache::lock(Object::View vKey) const
    {
    return lock(vKey, 0);
    }

bool ContinuousQueryCache::unlock(Object::View vKey) const
    {
    // we intentionally don't do the ReadOnly check as you must
    // hold the lock in order to release it
    return getCache()->unlock(vKey);
    }


// ----- NamedCache interface -----------------------------------------------

String::View ContinuousQueryCache::getCacheName() const
    {
    Supplier::View vNameSupplier = m_vCacheNameSupplier;
    return vNameSupplier == NULL ? f_vsName : cast<String::View>(vNameSupplier->get());
    }

CacheService::Handle ContinuousQueryCache::getCacheService()
    {
    return getCache()->getCacheService();
    }

CacheService::View ContinuousQueryCache::getCacheService() const
    {
    return getCache()->getCacheService();
    }

bool ContinuousQueryCache::isActive() const
    {
    NamedCache::Handle hCache = m_hCache;
    return hCache != NULL && hCache->isActive();
    }

void ContinuousQueryCache::release()
    {
    // shut down the event queue
    shutdownEventQueue();

    COH_SYNCHRONIZED(this)
        {
        releaseListeners();

        m_hMapLocal = NULL; 
        m_hCache    = NULL;
        m_nState    = state_disconnected;
        }
    }

void ContinuousQueryCache::destroy()
    {
    // destroys the view but not the underlying cache
    release();
    }

void ContinuousQueryCache::truncate()
    {
    checkReadOnly();
    getCacheInternal()->truncate();
    }


// ----- internal -----------------------------------------------------------

Filter::View ContinuousQueryCache::mergeFilter(Filter::View vFilter) const
    {
    if (NULL == vFilter)
        {
        return f_vFilter;
        }

    Filter::View vFilterMerged = NULL;

    // strip off key association
    Filter::View   vFilterCQC = getFilter();
    bool           fKeyAssoc  = false;
    Object::Holder ohKeyAssoc = NULL;
    if (instanceof<KeyAssociatedFilter::View>(vFilterCQC))
        {
        KeyAssociatedFilter::View vFilterAssoc =
                cast<KeyAssociatedFilter::View>(vFilterCQC);
        ohKeyAssoc = vFilterAssoc->getHostKey();
        vFilterCQC = vFilterAssoc->getFilter();
        fKeyAssoc = true;

        // if the passed filter is also key-associated, strip it off too
        if (instanceof<KeyAssociatedFilter::View>(vFilter))
            {
            vFilterAssoc = cast<KeyAssociatedFilter::View>(vFilter);
            vFilter = vFilterAssoc->getFilter();
            }
        }
    else if (instanceof<KeyAssociatedFilter::View>(vFilter))
        {
        KeyAssociatedFilter::View vFilterAssoc =
                cast<KeyAssociatedFilter::View>(vFilter);
        ohKeyAssoc = vFilterAssoc->getHostKey();
        vFilter    = vFilterAssoc->getFilter();
        fKeyAssoc  = true;
        }
    if (instanceof<LimitFilter::View>(vFilter))
        {
        // To merge a LimitFilter with the CQC Filter we cannot
        // simply And the two, we must And the CQC Filter with the
        // LimitFilter's internal Filter, and then apply the limit
        // on top of that
        LimitFilter::Handle hFilterNew;
        LimitFilter::Handle hFilterOrig   =
                cast<LimitFilter::Handle>(vFilter->clone());
        int32_t             iPageSize     = hFilterOrig->getPageSize();
        Object::Holder      ohCookie      = hFilterOrig->getCookie();
        if (instanceof<LimitFilter::View>(ohCookie))
            {
            // apply the page size as it could have changed since the
            // wrapper was created
            hFilterNew = cast<LimitFilter::Handle>(ohCookie->clone());
            hFilterNew->setPageSize(iPageSize);
            }
        else
            {
            // cookie either didn't exist, or was not our cookie
            // construct the wrapper and stick it in the cookie for
            // future re-use
            hFilterNew = LimitFilter::create(
                    AndFilter::create(vFilterCQC, hFilterOrig->getFilter()),
                    iPageSize);
            hFilterOrig->setCookie(hFilterNew);
            }

        // apply current page number;
        // all other properites are for use by the query processor
        // and only need to be maintained within the wrapper
        hFilterNew->setPage(hFilterOrig->getPage());

        vFilterMerged = hFilterNew;
        }
    else
        {
        vFilterMerged = AndFilter::create(vFilterCQC, vFilter);
        }
    // apply key association
    if (fKeyAssoc)
        {
        vFilterMerged = KeyAssociatedFilter::create(vFilterMerged, ohKeyAssoc);
        }

    return vFilterMerged;
    }

void ContinuousQueryCache::checkReadOnly() const
    {
    if (isReadOnly())
        {
        COH_THROW_STREAM(IllegalStateException, getCacheName() <<
                " is read-only")
        }
   }

void ContinuousQueryCache::checkEntry(Map::Entry::View vEntry)
    {
    if (!InvocableMapHelper::evaluateEntry(getFilter(), vEntry))
        {
        COH_THROW_STREAM(IllegalArgumentException, getCacheName() <<
                   ": Attempted modification violates filter; key=\"" <<
                   vEntry->getKey() << "\", value=\"" <<
                   vEntry->getValue() << "\"")
        }
    }

void ContinuousQueryCache::checkEntry(Object::View vKey,
        Object::Holder ohValue)
    {
    if (!InvocableMapHelper::evaluateEntry(getFilter(),
            SimpleMapEntry::create(vKey, ohValue)))
        {
        COH_THROW_STREAM(IllegalArgumentException, getCacheName() <<
                ": Attempted modification violates filter; key=\"" <<
                vKey << "\", value=\"" << ohValue << "\"")
        }
    }

String::View ContinuousQueryCache::getStateString(int32_t nState) const
    {
    switch (nState)
        {
        case state_configured:
                return "STATE_CONFIGURED";
            case state_configuring:
                return "STATE_CONFIGURING";
            case state_disconnected:
                return "STATE_DISCONNECTED";
            case state_synchronized:
                return "STATE_SYNCHRONIZED";
            default:
                COH_THROW_STREAM(IllegalStateException, "unknown state: " << nState);
        }
    }

void ContinuousQueryCache::configureSynchronization(bool fReload) const
    {
    COH_SYNCHRONIZED(this)
        {
            ObservableMap::Handle hMapLocal = NULL;
            try
                {
                changeState(state_configuring);
                m_ldtConnectionTimestamp = System::safeTimeMillis();

                NamedCache::Handle hCache       = getCacheInternal();
                Filter::View       vFilter      = getFilter();
                bool               fCacheValues = isCacheValues();
            
                // get the old filters and listeners
                MapEventFilter::View vFilterAddPrev   = m_vFilterAdd;
                MapListener::Handle  hListenerAddPrev = m_whListenerAdd;
            
                // determine if this is initial configuration
                bool  fFirstTime = vFilterAddPrev == NULL;
                if (fFirstTime)
                    {
                    // register for service restart notification
                    registerServiceListener();
                    registerDeactivationListener();
            
                    // create the "remove listener"
                    int32_t nMask = MapEventFilter::e_updated_left
                                  | MapEventFilter::e_deleted;
                    MapEventFilter::View vFilterRemove =
                            MapEventFilter::create(nMask, vFilter);
                    MapListener::Handle hListenerRemove = 
                            instantiateRemoveListener();
                    hCache->addFilterListener(hListenerRemove, vFilterRemove,
                            true);
            
                    m_vFilterRemove    = vFilterRemove;
                    m_whListenerRemove = hListenerRemove;
                    }
                else
                    {
                    hCache->addFilterListener(m_whListenerRemove, m_vFilterRemove,
                            true);
                    }
            
                // configure the "add listener"
                int32_t nMask = MapEventFilter::e_inserted
                              | MapEventFilter::e_updated_entered;
                if (fCacheValues)
                    {
                    nMask |= MapEventFilter::e_updated_within;
                    }
                if (fFirstTime || nMask != vFilterAddPrev->getEventMask())
                    {
                    MapEventFilter::View vFilterAdd =
                        MapEventFilter::create(nMask, vFilter);
                    MapListener::Handle hListenerAdd =
                        instantiateAddListener();
            
                    hCache->addFilterListener(hListenerAdd, 
                               createTransformerFilter(vFilterAdd),
                               !fCacheValues);
            
                    m_vFilterAdd    = vFilterAdd;
                    m_whListenerAdd = hListenerAdd;
            
                    if (hListenerAddPrev != NULL)
                        {
                        COH_ENSURE_RELATION(MapEventFilter::View,
                                vFilterAddPrev, !=, NULL)
                        hCache->removeFilterListener(hListenerAddPrev,
                                createTransformerFilter(vFilterAddPrev));
                        }
                    }
                else
                    {
                    hCache->addFilterListener(hListenerAddPrev,
                                createTransformerFilter(vFilterAddPrev),
                                !fCacheValues);
                    }
            
                // update the local query image
                hMapLocal = ensureInternalCache();
                if (fFirstTime || fReload)
                    {
                    // populate the internal cache
                    if (isCacheValues())
                        {
                        Set::View vSet = ((f_vTransformer == NULL)
                            ? hCache->entrySet(vFilter)
                            : hCache->invokeAll(vFilter,
                                ExtractorProcessor::create(
                                    f_vTransformer))->entrySet());
            
                        // first remove anything that is not in the query
                        if (!hMapLocal->isEmpty())
                            {
                            HashSet::Handle hSetQueryKeys = HashSet::create();
                            for (Iterator::Handle hIter = vSet->iterator();
                                    hIter->hasNext(); )
                                {
                                hSetQueryKeys->add(cast<Map::Entry::View>(
                                        hIter->next())->getKey());
                                }
                            hMapLocal->keySet()->retainAll(hSetQueryKeys);
                            }
            
                        // next, populate the local cache
                        for (Iterator::Handle hIter = vSet->iterator();
                                hIter->hasNext(); )
                            {
                            Map::Entry::View vEntry = cast<Map::Entry::View>(
                                    hIter->next());
                            hMapLocal->put(vEntry->getKey(), vEntry->getValue());
                            }
                        }
                    else
                        {
                        // first remove the keys that are not in the query
                        Set::View vSetQueryKeys = hCache->keySet(vFilter);
                        if (!hMapLocal->isEmpty())
                            {
                            hMapLocal->keySet()->retainAll(vSetQueryKeys);
                            }
            
                        //next, populate the local cache with keys from the query
                        for (Iterator::Handle hIter = vSetQueryKeys->iterator();
                                hIter->hasNext(); )
                            {
                            hMapLocal->put(hIter->next(), NULL);
                            }
                        }
                    }
                else
                    {
                    // not the first time; internal cache is already populated
                    if (fCacheValues)
                        {
                        // used to cache only keys, now caching values too
                        ObjectArray::Handle haoKey;
                        COH_SYNCHRONIZED (hMapLocal) // COH-1418
                            {
                            haoKey = hMapLocal->keySet()->toArray();
                            }
                        Map::View vMapValues = hCache->getAll(ReadOnlyArrayList::create(haoKey));
                        hMapLocal->putAll(vMapValues);
                        }
                    else
                        {
                        // used to cache values, now caching only keys
                        for (Iterator::Handle hIter =
                                hMapLocal->entrySet()->iterator();
                                hIter->hasNext(); )
                            {
                            cast<Map::Entry::Handle>(hIter->next())->setValue(NULL);
                            }
                        }
                    }

                int32_t nCurrentState = getState();
                if (nCurrentState != state_configuring)
                    {
                     // This is possible if the service thread has set the state
                    // to STATE_DISCONNECTED. In this case, throw and let the caller
                    // handle retry logic
                    throw createUnexpectedStateException(state_configuring, nCurrentState);
                    }
                changeState(state_configured);
            
                // resolve all changes that occurred during configuration
                Map::Handle hMapSyncReq = m_hMapSyncReq;
                if (!hMapSyncReq->isEmpty())
                    {
                    ObjectArray::Handle haoKey;
                    COH_SYNCHRONIZED (hMapSyncReq) // COH-1418
                        {
                        haoKey = hMapSyncReq->keySet()->toArray();
                        }
                    Map::View vMapSyncVals = hCache->getAll(ReadOnlyArrayList::create(haoKey));
                    COH_SYNCHRONIZED (hMapSyncReq)
                        {
                        for (Iterator::Handle hIter =
                                hMapSyncReq->keySet()->iterator(); hIter->hasNext(); )
                            {
                            Object::Holder ohKey   = hIter->next();
                            Object::Holder ohValue = vMapSyncVals->get(ohKey);
                            bool           fExists = ohValue != NULL ||
                                vMapSyncVals->containsKey(ohKey);
            
                            // COH-3847 - an update event was received and deferred
                            // while configuring the CQC, but we need to double-check
                            // that the new value satisfies the filter
                            if (fExists && InvocableMapHelper::evaluateEntry(
                                    vFilter, SimpleMapEntry::create(ohKey, ohValue)))
                                {
                                hMapLocal->put(ohKey, ohValue);
                                }
                            else
                                {
                                hMapLocal->remove(ohKey);
                                }
                            }
            
                        // notify other threads that there is nothing to resolve
                        hMapSyncReq->clear();
                        }
                    }

                nCurrentState = getState();
                if (nCurrentState != state_configured)
                    {
                    // This is possible if the service thread has set the state
                    // to STATE_DISCONNECTED. In this case, throw and let the caller
                    // handle retry logic
                    throw createUnexpectedStateException(state_configured, nCurrentState);
                    }
                changeState(state_synchronized);
            }
        catch (Exception::View vEx)
            {
            if (hMapLocal != NULL)
                {
                // exception during initial load (COH-2625) or reconciliation;
                // in either case we need to unregister listeners and
                // start from scratch
                releaseListeners();
                }

            // mark as disconnected    
            changeState(state_disconnected);
            COH_THROW (vEx);
            }
        }
    }

RuntimeException::View ContinuousQueryCache::createUnexpectedStateException(int32_t nExpectedState, int32_t nActualState) const
    {
    return RuntimeException::create(COH_TO_STRING("Unexpected synchronization state.  Expected: "
                                                  << nExpectedState << ", actual: " << nActualState));
    }

Filter::View ContinuousQueryCache::createTransformerFilter(
    MapEventFilter::View vFilterAdd) const
    {
    return MapEventTransformerFilter::create(vFilterAdd,
        f_vTransformer == NULL
            ? ((MapEventTransformer::View)
                   SemiLiteEventTransformer::getInstance())
            : ((MapEventTransformer::View)
                   ExtractorEventTransformer::create(
                       (ValueExtractor::View) NULL, 
                       (ValueExtractor::View) f_vTransformer)));
    }

void ContinuousQueryCache::ensureSynchronized(bool fReload) const
    {
    // configure and synchronize the ContinousQueryCache, if necessary
    if (getState() != state_synchronized)
        {
        int64_t cReconnectMillis = getReconnectInterval();
        bool    fAllowDisconnect = cReconnectMillis > 0;

        if (fAllowDisconnect &&
                System::safeTimeMillis() < m_ldtConnectionTimestamp +
                cReconnectMillis)
            {
            // don't try to re-connect just yet
            return;
            }

        Exception::View vExConfig = NULL;
        int             cAttempts = fAllowDisconnect ? 1 : 3;
        for (int32_t i = 0; i < cAttempts; ++i)
            {
            COH_SYNCHRONIZED (this)
                {
                int32_t nState = getState();
                if (state_disconnected == nState)
                    {
                    try
                        {
                        configureSynchronization(fReload);
                        return;
                        }
                    catch (Exception::View vEx)
                        {
                        vExConfig = vEx;
                        }
                    }
                else
                    {
                    COH_ENSURE_RELATION(int32_t, nState, ==, state_synchronized)
                    return;
                    }
                }
            }

         if (!fAllowDisconnect)
            {
            String::Handle sMsg = String::create("This ContinuousQueryCache is disconnected. Retry the operation again.");
            if (COH_LOGENABLED(Logger::level_d9))
                {
                throw IllegalStateException::create(sMsg, vExConfig);
                }
            else
                {
                throw IllegalStateException::create(sMsg);
                }
            }
        }
    }

bool ContinuousQueryCache::removeBlind(Object::View vKey)
    {
    checkReadOnly();
    return containsKey(vKey) && getCache()->keySet()->remove(vKey);
    }

Set::View ContinuousQueryCache::getInternalKeySet() const
    {
    return getInternalCache()->keySet();
    }

NamedCache::Handle ContinuousQueryCache::getCacheInternal() const
    {
    NamedCache::Handle hCache = m_hCache;
    if (hCache == NULL)
        {
        NamedCache::Holder hCacheHolder = cast<NamedCache::Holder>(f_vCacheSupplier->get());
        hCache = m_hCache = const_cast<NamedCache*>(get_pointer(hCacheHolder));
        if (hCache == NULL)
            {
            COH_THROW_STREAM(IllegalStateException, "NamedCache is not active")
            }
        }
    return hCache;
    }

void ContinuousQueryCache::releaseListeners() const
    {
    NamedCache::Handle hCache = m_hCache;
    if (hCache != NULL)
        {
        unregisterServiceListener();
        unregisterDeactivationListener();

        MapListener::Handle hListenerAdd = m_whListenerAdd;
        if (hListenerAdd != NULL)
            {
            try
                {
                hCache->removeFilterListener(hListenerAdd, createTransformerFilter(m_vFilterAdd));
                }
            catch (Exception::View)
                {
                //do nothing
                }
            m_whListenerAdd = NULL;
            }
         m_vFilterAdd = NULL;

        MapListener::Handle hListenerRemove = m_whListenerRemove;
        if (hListenerRemove != NULL)
            {
            try
                {
                hCache->removeFilterListener(hListenerRemove, m_vFilterRemove);
                }
            catch (Exception::View)
                {
                //do nothing
                }
            m_whListenerRemove = NULL;
            }
         m_vFilterRemove = NULL;
        }
    }


// ----- local class: DeactivationListener ----------------------------------

/**
* DeactivationListener for the underlying NamedCache.
* 
* The primary goal of that listener is invalidation of the named cache when
* the named cache is destroyed or to truncate the local cache if the back 
* cache has been truncated.
*
* @since 12.2.1.4
*/
class COH_EXPORT DeactivationListener
        : public class_spec<DeactivationListener,
          extends<AbstractMapListener>,
          implements<NamedCacheDeactivationListener> >
    {
    friend class factory<DeactivationListener>;

    // ----- constructors ---------------------------------------------------

    protected:
        DeactivationListener(ContinuousQueryCache::Handle queryCache)
                : f_hQueryCache(self(), queryCache)
        {
        }

    private:
        DeactivationListener(const DeactivationListener&);

    // ----- MapListener interface ------------------------------------------

    public:
        void entryDeleted(MapEvent::View)
            {
            // destroy/disconnect event
            ContinuousQueryCache::Handle hQueryCache = f_hQueryCache;
            hQueryCache->changeState(ContinuousQueryCache::state_disconnected);
            }

        void entryUpdated(MapEvent::View)
            {
            // "truncate" event
            ContinuousQueryCache::Handle hQueryCache   = f_hQueryCache;
            ObservableMap::Handle        internalCache = hQueryCache->getInternalCache();
            if (!hQueryCache->isEventDeferred(Class::getTypeName(typeid(this))))
                {
                if (instanceof<ObservableHashMap::Handle>(internalCache))
                    {
                    cast<ObservableHashMap::Handle>(internalCache)->truncate();
                    }
                else
                    {
                    internalCache->clear();
                    }
                }
            };

    // ----- data members ---------------------------------------------------

    protected:
        FinalHandle<ContinuousQueryCache> f_hQueryCache;
    };

void ContinuousQueryCache::registerDeactivationListener() const
   {
   // automatic named cache clean up (upon cache destruction)
   // requires a NamedCacheDeactivationListener implementation
   CacheService::View hService = getCacheInternal()->getCacheService();
   if (hService != NULL)
       {
       try
           {
           MapListener::Handle hDeactivationListener;
           hDeactivationListener = m_hListenerDeactivation = DeactivationListener::create(const_cast<ContinuousQueryCache*>(this));
           m_hCache->addMapListener(hDeactivationListener);
           }
       catch(UnsupportedOperationException::View)
           {
           }
       }
   }   

void ContinuousQueryCache::unregisterDeactivationListener() const
   {
   MapListener::Handle hDeactivationListener = m_hListenerDeactivation;
   if (hDeactivationListener != NULL)
       {
       try
           {
           NamedCache::Handle hCache = m_hCache;
           if (hCache != NULL)
               {
               hCache->removeMapListener(hDeactivationListener);
               }
           }
       catch(RuntimeException::View)
           {
           }
       }
   }

bool ContinuousQueryCache::isEventDeferred(Object::View vKey) const
    {
    bool fDeferred = false;

    Map::Handle hMapSyncReq = m_hMapSyncReq;
    if (hMapSyncReq != NULL)
        {
        if (getState() <= state_configuring)
            {
            // handle a truncation event being received during configuration
            // clear any currently pending events.
            if (Class::getTypeName(typeid(DeactivationListener))->equals(vKey))
                {
                hMapSyncReq->clear();
                }
            else
                {
                // since the listeners are being configured and the local
                // cache is being populated, assume that the event is
                // being processed out-of-order and requires a subsequent
                // synchronization of the corresponding value
                hMapSyncReq->put(vKey, NULL);
                }
            fDeferred = true;
            }
        else
            {
            // since an event has arrived after the configuration
            // completed, the event automatically resolves the sync
            // requirement
            hMapSyncReq->keySet()->remove(vKey);
            }
        }

    return fDeferred;
    }


// ----- Object interface ---------------------------------------------------

void ContinuousQueryCache::onInit()
    {
    super::onInit();

    Supplier::View vCacheSupplier = f_vCacheSupplier;
    Filter::View   vFilter        = f_vFilter;

    COH_ENSURE_RELATION(Supplier::View, vCacheSupplier, !=, NULL)
    COH_ENSURE_RELATION(Filter::View, vFilter, !=, NULL)

    NamedCache::View hNamedCache = cast<NamedCache::View>(vCacheSupplier->get());

    COH_ENSURE_RELATION(NamedCache::Holder, hNamedCache, !=, NULL)

    if (instanceof<LimitFilter::View>(vFilter))
        {
        // FUTURETODO: it would be nice to eventually be able to have a
        // cache of the "top ten" items, etc.
        COH_THROW_STREAM(UnsupportedOperationException,
                         "LimitFilter may not be used")
        }

    initialize(f_vsName, COH_TO_STRING("ContinuousQueryCache{Cache=" << hNamedCache->getCacheName() <<
                                       ", Filter=" << vFilter << ", Transformer=" << f_vTransformer << "}"));

    // key/entry set can't be lazy as we need to pass non-const this reference
    initialize(f_hSetKeys, instantiateKeySet());
    initialize(f_hSetEntries, instantiateEntrySet());

    // was a standard (non-lite) listener passed at construction time?
    m_fListeners = m_hListener != NULL && isCacheValues();

    ensureInternalCache();
    ensureSynchronized(false);
    }


// ----- local class: AddListener -------------------------------------------

/**
* A MapListener for evicting items from the query.
*/
class COH_EXPORT AddListener
    : public class_spec<AddListener,
        extends<MultiplexingMapListener>,
        implements<MapListenerSupport::SynchronousListener> >
    {
    friend class factory<AddListener>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new Status object.
        *
        * @param vCache  the outer ContinuousQueryCache
        */
        AddListener(ContinuousQueryCache::View vCache)
            : f_vCache(self(), vCache)
            {
            }

    // ----- MultiplexingMapListener interface --------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onMapEvent(MapEvent::View vEvent)
            {
            Object::View vKey = vEvent->getKey();

            if (!f_vCache->isEventDeferred(vKey))
                {
                // guard against possible NPE; one could theoretically occur
                // during construction or after release; one occurred during
                // testing of a deadlock issue (COHCPP-300)
                Map::Handle hMap = f_vCache->m_hMapLocal;
                if (hMap != NULL)
                    {
                    hMap->put(vKey, f_vCache->isCacheValues() ?
                            vEvent->getNewValue() : (Object::View) NULL);
                    }
                }
            }

    // ----- Object interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("AddListener[" << f_vCache << "]");
            }

    // ---- data members ------------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        FinalView<ContinuousQueryCache> f_vCache;
    };

MapListener::Handle ContinuousQueryCache::instantiateAddListener() const
    {
    return AddListener::create(this);
    }


// ----- local class: RemoveListener ----------------------------------------

/**
* A MapListener for evicting items from the query.
*/
class COH_EXPORT RemoveListener
    : public class_spec<RemoveListener,
        extends<MultiplexingMapListener>,
        implements<MapListenerSupport::SynchronousListener> >
    {
    friend class factory<RemoveListener>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new Status object.
        *
        * @param vCache  the outer ContinuousQueryCache
        */
        RemoveListener(ContinuousQueryCache::View vCache)
            : f_vCache(self(), vCache)
            {
            }

    // ----- MultiplexingMapListener interface --------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onMapEvent(MapEvent::View vEvent)
            {
            ContinuousQueryCache::View vCache = f_vCache;
            Object::View               vKey   = vEvent->getKey();
            if (!vCache->isEventDeferred(vKey))
                {
                // guard against possible NPE; one could theoretically occur
                // during construction or after release; one occurred during
                // testing of a deadlock issue (COHCPP-300)
                Map::Handle hMap = vCache->m_hMapLocal;
                if (hMap != NULL)
                    {
                    hMap->remove(vKey);
                    }
                }
            }

    // ----- Object interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("RemoveListener[" << f_vCache << "]");
            }

    // ---- data members ------------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        FinalView<ContinuousQueryCache> f_vCache;
    };

MapListener::Handle ContinuousQueryCache::instantiateRemoveListener() const
    {
    return RemoveListener::create(this);
    }


// ----- local class: ServiceListener ---------------------------------------

/**
* A MapListener for evicting items from the query.
*/
class COH_EXPORT ServiceListener
    : public class_spec<ServiceListener,
        extends<Object>,
        implements<MemberListener> >
    {
    friend class factory<ServiceListener>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new Status object.
        *
        * @param vCache  the outer ContinuousQueryCache
        */
        ServiceListener(ContinuousQueryCache::View vCache)
            : f_vCache(self(), vCache)
            {
            }

    // ----- MemberListener interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void memberJoined(MemberEvent::View /*vEvt*/)
            {
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeaving(MemberEvent::View /*vEvt*/)
            {
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeft(MemberEvent::View vEvt)
            {
            if (vEvt->isLocal())
                {
                f_vCache->changeState(ContinuousQueryCache::state_disconnected);
                }
            }

    // ----- Object interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("ServiceListener[" << f_vCache << "]");
            }

    // ---- data members ------------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        FinalView<ContinuousQueryCache> f_vCache;
    };

void ContinuousQueryCache::registerServiceListener() const
    {
    // automatic front map clean up (upon service restart)
    // requires a MemberListener implementation

    CacheService::Handle hService = getCacheInternal()->getCacheService();
    if (hService != NULL)
        {
        try
            {
            MemberListener::Handle hListener = ServiceListener::create(this);
            hService->addMemberListener(hListener);
            m_whListenerService = hListener;
            }
        catch (UnsupportedOperationException::View)
            {
            // do nothing
            }
        }
    }

void ContinuousQueryCache::unregisterServiceListener() const
    {
    try
        {
        cast<CacheService::Handle>(getCacheInternal()->getCacheService())->removeMemberListener(m_whListenerService);
        }
    catch (RuntimeException::View)
        {
        // do nothing
        }
    }

Map::Handle ContinuousQueryCache::ensureIndexMap()
    {
    return getIndexMap();
    }

Map::View ContinuousQueryCache::getIndexMap() const
    {
    return f_hMapIndex;
    }

Map::Handle ContinuousQueryCache::getIndexMap()
    {
    return f_hMapIndex;
    }

void ContinuousQueryCache::releaseIndexMap()
    {
    Map::Handle hMapIndex = getIndexMap();
    if (hMapIndex != NULL)
        {
        ObjectArray::View aoExtractors = hMapIndex->keySet()->toArray();
        for (size32_t i = 0; i < aoExtractors->length; ++i)
            {
            removeIndex(cast<ValueExtractor::View>(aoExtractors[i]));
            }
        }
    }


// ----- inner class: AsynchronousEvent -------------------------------------

ContinuousQueryCache::AsynchronousEvent::AsynchronousEvent(
        MapEvent::Handle hEvent, MapListener::Handle hListener)
        : f_hEvent(self(), hEvent), f_hListener(self(), hListener)
    {
    }

void ContinuousQueryCache::AsynchronousEvent::dispatch()
    {
    MapListener::Handle hListener = f_hListener;
    f_hEvent->dispatch(hListener);
    }


// ----- local class: EventQueue --------------------------------------------

/**
* Used to dispatch asynchronous messages.
*/
class COH_EXPORT EventQueue
    : public class_spec<EventQueue,
        extends<QueueProcessor>,
        implements<ContinuousQueryCache::TaskDaemon> >
    {
    friend class factory<EventQueue>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new EventQueue object.
        */
        EventQueue()
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        EventQueue(const EventQueue&);

    // ----- EventQueue interface ---------------------------------------

    public:
        /**
        * Schedule an event for dispatch.
        *
        * @param hEvent  the event to dispatch
        */
        virtual void scheduleDispatch(
                ContinuousQueryCache::AsynchronousEvent::Handle hEvent)
            {
            getQueue()->add(hEvent);
            }

        /**
        * {@inheritDoc}
        */
        virtual void start()
            {
            QueueProcessor::start();
            }

        /**
        * {@inheritDoc}
        */
        virtual void stop()
            {
            QueueProcessor::stop();
            }

    // ----- QueueProcessor interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onNotify()
            {
            ContinuousQueryCache::AsynchronousEvent::Handle hEvent = NULL;
            while ((hEvent = cast<ContinuousQueryCache::AsynchronousEvent::Handle>
                    (getQueue()->removeNoWait())) != NULL)
                {
                hEvent->dispatch();
                }
            }
    };

ContinuousQueryCache::TaskDaemon::Handle
        ContinuousQueryCache::instantiateEventQueue() const
    {
    return EventQueue::create();
    }

ContinuousQueryCache::TaskDaemon::Handle
        ContinuousQueryCache::getEventQueue() const
    {
    return m_hTaskQueue;
    }

ContinuousQueryCache::TaskDaemon::Handle
        ContinuousQueryCache::ensureEventQueue() const
    {
    COH_SYNCHRONIZED(this)
        {
        TaskDaemon::Handle hQueue = getEventQueue();
        if (NULL == hQueue)
            {
            m_hTaskQueue = hQueue = instantiateEventQueue();
            hQueue->start();
            }
        return hQueue;
        }
    }

void ContinuousQueryCache::shutdownEventQueue()
    {
    TaskDaemon::Handle hEventQueue = getEventQueue();
    if (hEventQueue != NULL)
        {
        m_hTaskQueue = NULL;
        hEventQueue->stop();
        }
    }

// ----- local class: EventRouter -------------------------------------------

/**
* A MapListener for evicting items from the query.
*/
class COH_EXPORT EventRouter
    : public class_spec<EventRouter,
        extends<MultiplexingMapListener> >
    {
    friend class factory<EventRouter>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new Status object.
        *
        * @param hCache    the outer ContinuousQueryCache
        * @param listener  a client listener
        * @param fLite     true to indicate that the MapEvent objects do
        *                  not have to include the OldValue and 
        *                  NewValue property values in order to allow optimizations
        */
        EventRouter(ContinuousQueryCache::Handle hCache,
                MapListener::Handle hListener, bool fLite)
            : f_hCache(self(), hCache),
              m_hListener(self(), hListener),
              m_fLite(fLite)
            {
            }

    // ----- MultiplexingMapListener interface --------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onMapEvent(MapEvent::View vEvent)
            {
            //TODO: const references?
            MapListener::Handle          hListener = m_hListener;
            MapEvent::Handle             hEvtRoute;
            ContinuousQueryCache::Handle hMap      = f_hCache;

            // internal note:  The implementation that intializes hEvtRoute differs from
            //                 the Java and .NET implementations due to differences in the
            //                 existing APIs.
            if (instanceof<MapListenerSupport::FilterEvent::View>(vEvent))
                {
                hEvtRoute = MapListenerSupport::FilterEvent::create(hMap,
                                                                    vEvent->getId(),
                                                                    vEvent->getKey(),
                                                                    m_fLite ? NULL : vEvent->getOldValue(),
                                                                    m_fLite ? NULL : vEvent->getNewValue(),
                                                                    false,
                                                                    false,
                                                                    cast<MapListenerSupport::FilterEvent::View>(vEvent)->getFilter());
                }
            else
                {
                hEvtRoute = MapEvent::create(hMap,
                                             vEvent->getId(),
                                             vEvent->getKey(),
                                             m_fLite ? NULL : vEvent->getOldValue(),
                                             m_fLite ? NULL : vEvent->getNewValue());
                }
            
            if (instanceof<MapListenerSupport::SynchronousListener::Handle>
                    (hListener))
                {
                try
                    {
                    hEvtRoute->dispatch(hListener);
                    }
                catch (RuntimeException::View vEx)
                    {
                    COH_LOGEX(vEx, Logger::level_warning)
                    }
                }
            else
                {
                ContinuousQueryCache::TaskDaemon::Handle hEventQueue = hMap->getEventQueue();

                // COH-2413 - guard against IllegalStateException after release()
                if (hEventQueue != NULL)
                    {
                    ContinuousQueryCache::AsynchronousEvent::Handle hEvent =
                        ContinuousQueryCache::AsynchronousEvent::create(
                            hEvtRoute, hListener);

                    hEventQueue->scheduleDispatch(hEvent);
                    }
                }
            }

    // ----- Object interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
            {
            EventRouter::View vThat = cast<EventRouter::View>(v, false);
            return vThat != NULL && m_hListener->equals(vThat->m_hListener);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return m_hListener->hashCode();
            }

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("EventRouter[" << m_hListener <<"]");
            }


    // ----- data members -----------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        FinalHandle<ContinuousQueryCache> f_hCache;

        /**
        * The MapListener to route to.
        */
        FinalHandle<MapListener> m_hListener;

        /**
         * Flag indicating MapEvent objects do not have to include the 
         * OldValue and NewValue property values in order to 
         * allow optimizations.
         * 
         * @since 12.2.1.4
         */
        bool m_fLite;
    };

MultiplexingMapListener::Handle ContinuousQueryCache::instantiateEventRouter(
        MapListener::Handle hListener, bool fLite) const
    {
    return EventRouter::create(const_cast<ContinuousQueryCache*>(this), hListener, fLite);
    }


// ----- local class: KeySetMuterator ---------------------------------------

/**
* A mutable iterator backed by the key set of this cache.
*/
class COH_EXPORT KeySetMuterator
    : public class_spec<KeySetMuterator,
        extends<AbstractStableIterator> >
    {
    friend class factory<KeySetMuterator>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new KeySetMuterator.
        *
        * @param hCache  the ContinuousQueryCache
        * @param hIter   the underlying key set iterator
        */
        KeySetMuterator(ContinuousQueryCache::Handle hCache,
                Iterator::Handle hIter)
            : m_whCache(self(), hCache),
              f_hIter(self(), hIter)
            {
            }

    // ----- AbstractStableIterator interface ---------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove(Object::Holder ohPrev)
            {
            m_whCache->removeBlind(ohPrev);
            }

        using AbstractStableIterator::remove;

        /**
        * {@inheritDoc}
        */
        virtual void advance()
            {
            if (f_hIter->hasNext())
                {
                setNext(f_hIter->next());
                }
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The ContinuousQueryCache.
        */
        WeakHandle<ContinuousQueryCache> m_whCache;

        /**
        * The undelying key set iterator
        */
        FinalHandle<Iterator> f_hIter;
    };

// ----- local class: KeySet ------------------------------------------------

/**
* Key set backed by the ContinuousQueryCache
*/
class COH_EXPORT KeySet
    : public class_spec<KeySet,
        extends<AbstractSet> >
    {
    friend class factory<KeySet>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new KeySet object.
        *
        * @param hCache  the outer ContinuousQueryCache
        */
        KeySet(ContinuousQueryCache::Handle hCache)
            : m_whCache(self(), hCache)
            {
            }

    // ----- Set interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            m_whCache->clear();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            return m_whCache->containsKey(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const
            {
            return m_whCache->isEmpty();
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            //TODO: empty set optimization?
            return m_whCache->getInternalKeySet()->iterator();
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            //TODO: empty key set optimization
            return KeySetMuterator::create((ContinuousQueryCache::Handle) m_whCache,
                    m_whCache->getInternalKeySet()->iterator());
            }

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v)
            {
            return m_whCache->removeBlind(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return m_whCache->size();
            }

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(ObjectArray::Handle hao) const
            {
            return m_whCache->getInternalKeySet()->toArray(hao);
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        WeakHandle<ContinuousQueryCache> m_whCache;
    };

Set::Handle ContinuousQueryCache::instantiateKeySet()
    {
    return KeySet::create(this);
    }


// ----- local class: Entry -------------------------------------------------

/**
* A Map Entry implementation that defers its value acquisition from the
* containing map (via Map::get(Object)) if the Entry is constructed with a
* NULL value.
*/
class COH_EXPORT Entry
    : public class_spec<Entry,
        extends<SimpleMapEntry> >
    {
    friend class factory<Entry>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new Entry object.
        *
        * @param hCache  the outer ContinuousQueryCache
        * @param vKey    the key
        * @param ohVal   that value
        */
        Entry(ContinuousQueryCache::Handle hCache, Object::View vKey,
                Object::Holder ohVal)
            : super(vKey, ohVal),
              m_whCache(self(), hCache),
              m_ohCachedValue(self(), NULL, /*fMutable*/ true)
            {
            }

    // ----- Entry interface --------------------------------------------

    public:
        /**
        * Returns the value corresponding to this entry.
        *
        * @return the value corresponding to this entry
        */
        virtual Object::Holder getValue() const
            {
            Object::Holder ohValue = SimpleMapEntry::getValue();
            if (NULL == ohValue)
                {
                // REVIEW: MF I'd really prefer to avoid fighting constness like
                // this, we should either not try to cache the value, or not
                // derive from SimpleMapEntry.
                // cant directly set the value on the entry because of const
                ohValue = m_ohCachedValue != NULL ? m_ohCachedValue :
                    (m_ohCachedValue = m_whCache->get(getKey()));
                }
            return ohValue;
            }

        /**
        * Returns the value corresponding to this entry.
        *
        * @return the value corresponding to this entry
        */
        virtual Object::Holder getValue()
            {
            Object::Holder ohValue = SimpleMapEntry::getValue();
            if (NULL == ohValue)
                {
                ohValue = m_ohCachedValue;
                if (NULL == ohValue)
                    {
                    ohValue = m_whCache->get(getKey());
                    setValue(ohValue);
                    }
                }
            return ohValue;
            }

        /**
        * Replaces the value corresponding to this entry with the specified
        * value (optional operation).  (Writes through to the map.)
        *
        * @param ohValue  new value to be stored in this entry
        *
        * @return old value corresponding to the entry
        */
        virtual Object::Holder setValue(Object::Holder ohValue)
            {
            Object::Holder ohValueOrig = m_whCache->put(getKey(), ohValue);
            SimpleMapEntry::setValue(ohValue);
            return ohValueOrig;
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder ohValue,
               bool fSynthetic)
            {
            Object::Holder ohValueOrig = m_whCache->put(getKey(), ohValue);
            SimpleMapEntry::setValue(ohValue, fSynthetic);
            return ohValueOrig;
            }

        /**
        * Returns the hash code value for this map entry.
        *
        * @return the hash code value for this map entry.
        */
        virtual size32_t hashCode() const
            {
            return Object::hashCode(getKey());
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        WeakHandle<ContinuousQueryCache> m_whCache;

        /**
        * The cached value.
        */
        mutable MemberHolder<Object> m_ohCachedValue;
    };


// ----- local class: EntrySetIterator --------------------------------------

/**
* A non-mutable iterator backed by the entry set of this cache.
*/
class COH_EXPORT EntrySetIterator
    : public class_spec<EntrySetIterator,
        extends<Object>,
        implements<Iterator> >
    {
    friend class factory<EntrySetIterator>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new EntrySetIterator.
        *
        * @param hCache  the outer ContinuousQueryCache
        */
        EntrySetIterator(ContinuousQueryCache::Handle hCache)
            : m_whCache(self(), hCache),
              f_hIterKeys(self(), hCache->keySet()->iterator())
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        EntrySetIterator(const EntrySetIterator&);

    // ----- Iterator interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool hasNext() const
            {
            return f_hIterKeys->hasNext();
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder next()
            {
            return Entry::create((ContinuousQueryCache::Handle) m_whCache,
                    f_hIterKeys->next(), (Object::Handle) NULL);
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The ContinuousQueryCache.
        */
        WeakHandle<ContinuousQueryCache> m_whCache;

        /**
        * An iterator backed by the ContinuousQueryCache key set.
        */
        //TODO: Iterator with a Muterator member
        FinalHandle<Muterator> f_hIterKeys;
    };


// ----- local class: EntrySetMuterator -------------------------------------

/**
* A mutable iterator backed by the entry set of this cache.
*/
class COH_EXPORT EntrySetMuterator
    : public class_spec<EntrySetMuterator,
        extends<EntrySetIterator>,
        implements<Muterator> >
    {
    friend class factory<EntrySetMuterator>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new EntrySetMuterator.
        *
        * @param hCache  the outer ContinuousQueryCache
        */
        EntrySetMuterator(ContinuousQueryCache::Handle hCache)
            : super(hCache)
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        EntrySetMuterator(const EntrySetMuterator&);

    // ----- Muterator interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void remove()
            {
            f_hIterKeys->remove();
            }
    };


// ----- local class: EntrySet ----------------------------------------------

/**
* Entry set backed by the CQC
*/
class COH_EXPORT EntrySet
    : public class_spec<EntrySet,
        extends<AbstractSet> >
    {
    friend class factory<EntrySet>;

    // ----- constructors/destructor ------------------------------------

    protected:
        /**
        * Create a new EntrySet object.
        *
        * @param hCache  the outer ContinuousQueryCache
        */
        EntrySet(ContinuousQueryCache::Handle hCache)
            : m_whCache(self(), hCache, /*fMutable*/ true)
            {
            }

    // ----- Set interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void clear()
            {
            getCache()->clear();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(v, false);
            if (vEntry != NULL)
                {
                Object::View               vKey   = vEntry->getKey();
                Object::View               vValue = vEntry->getValue();
                ContinuousQueryCache::View vMap   = getCache();

                return Object::equals(vValue, vMap->get(vKey)) &&
                        (vValue != NULL || vMap->containsKey(vKey));
                }

            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const
            {
            return getCache()->isEmpty();
            }

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle iterator() const
            {
            //TODO: empty set optimization
            return EntrySetIterator::create(getCache());
            }

        /**
        * {@inheritDoc}
        */
        virtual Muterator::Handle iterator()
            {
            //TODO: empty set optimization
            return EntrySetMuterator::create(getCache());
            }

        /**
        * {@inheritDoc}
        */
        virtual bool remove(Object::View v)
            {
            if (contains(v))
                {
                getCache()->remove(cast<Map::Entry::View>(v)->getKey());
                return true;
                }
            else
                {
                return false;
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t size() const
            {
            return getCache()->size();
            }

        /**
        * {@inheritDoc}
        */
        virtual ObjectArray::Handle toArray(ObjectArray::Handle hao) const
            {
            ContinuousQueryCache::View vMap = getCache();
            ObjectArray::Handle ha = vMap->keySet()->toArray(hao);
            for (size32_t i = 0, c = ha->length; i < c; ++i)
                {
                Object::View vKey = ha[i];
                // snapshot!
                ha[i] = Entry::create(getCache(), vKey, vMap->get(vKey));
                }

            return ha;
            }

    // ----- helpers ----------------------------------------------------

    protected:
        /**
        * Return the cache associated with this Entry.
        *
        * @return the cache associated with this Entry
        */
        virtual ContinuousQueryCache::Handle getCache() const
            {
            return m_whCache;
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The associated ContinuousQueryCache.
        */
        mutable WeakHandle<ContinuousQueryCache> m_whCache;
    };

Set::Handle ContinuousQueryCache::instantiateEntrySet()
    {
    return EntrySet::create(this);
    }

COH_CLOSE_NAMESPACE3

