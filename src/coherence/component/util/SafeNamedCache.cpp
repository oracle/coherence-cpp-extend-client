/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/SafeNamedCache.hpp"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LinkedList.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/MapTriggerListener.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::CacheFactory;
using coherence::security::auth::Subject;
using coherence::util::Iterator;
using coherence::util::LinkedList;
using coherence::util::List;
using coherence::util::Map;
using coherence::util::MapTriggerListener;
using coherence::util::Set;
using coherence::util::logging::Logger;


// ----- constructors/destructor  -------------------------------------------

SafeNamedCache::SafeNamedCache(MapListenerSupport::Handle hSupport)
        : m_hNamedCache(self(),
          (NamedCache::Handle) NULL, true),
          f_vsCacheName(self()),
          m_hCacheService(self(),
          (SafeCacheService::Handle) NULL, true),
          m_fIsReleased(self(), false),
          f_hListenerSupport(self(), hSupport == NULL ? MapListenerSupport::create() : hSupport),
          f_vSubject(self())
    {
    }


// ----- Map interface ------------------------------------------------------

size32_t SafeNamedCache::size() const
    {
    return getRunningNamedCache()->size();
    }

bool SafeNamedCache::isEmpty() const
    {
    return getRunningNamedCache()->isEmpty();
    }

bool SafeNamedCache::containsKey(Object::View vKey) const
    {
    return getRunningNamedCache()->containsKey(vKey);
    }

bool SafeNamedCache::containsValue(Object::View vValue) const
    {
    return getRunningNamedCache()->containsValue(vValue);
    }

Object::Holder SafeNamedCache::get(Object::View vKey) const
    {
    return getRunningNamedCache()->get(vKey);
    }

Object::Holder SafeNamedCache::get(Object::View vKey)
    {
    return getRunningNamedCache()->get(vKey);
    }

Object::Holder SafeNamedCache::put(Object::View vKey, Object::Holder hValue)
    {
    return getRunningNamedCache()->put(vKey, hValue);
    }

Object::Holder SafeNamedCache::remove(Object::View vKey)
    {
    return getRunningNamedCache()->remove(vKey);
    }

void SafeNamedCache::putAll(Map::View vMap)
    {
    getRunningNamedCache()->putAll(vMap);
    }

void SafeNamedCache::clear()
    {
    getRunningNamedCache()->clear();
    }

Set::View SafeNamedCache::keySet() const
    {
    return getRunningNamedCache()->keySet();
    }

Set::Handle SafeNamedCache::keySet()
    {
    return getRunningNamedCache()->keySet();
    }

Collection::View SafeNamedCache::values() const
    {
    return getRunningNamedCache()->values();
    }

Collection::Handle SafeNamedCache::values()
    {
    return getRunningNamedCache()->values();
    }

Set::View SafeNamedCache::entrySet() const
    {
    return getRunningNamedCache()->entrySet();
    }

Set::Handle SafeNamedCache::entrySet()
    {
    return getRunningNamedCache()->entrySet();
    }


// ----- CacheMap interface -------------------------------------------------

Map::View SafeNamedCache::getAll(Collection::View vKeys) const
    {
    return getRunningNamedCache()->getAll(vKeys);
    }

Object::Holder SafeNamedCache::put(Object::View vKey, Object::Holder hValue,
        int64_t cMillis)
    {
    return getRunningNamedCache()->put(vKey, hValue, cMillis);
    }


// ----- NamedCache interface -----------------------------------------------

void SafeNamedCache::release()
    {
    SafeCacheService::Handle hSafeService = m_hCacheService;
    COH_SYNCHRONIZED (hSafeService)
        {
        COH_SYNCHRONIZED (this)
            {
            m_fIsReleased = true;
            releaseListeners();
            hSafeService->releaseCache(this);
            m_hNamedCache = NULL;
            }
        }
    }

void SafeNamedCache::destroy()
    {
    SafeCacheService::Handle hSafeService = m_hCacheService;
    COH_SYNCHRONIZED (hSafeService)
        {
        COH_SYNCHRONIZED (this)
            {
            m_fIsReleased = true;
            hSafeService->destroyCache(this);
            m_hNamedCache = NULL;
            }
        }
    }

void SafeNamedCache::truncate()
    {
    getRunningNamedCache()->truncate();
    }

String::View SafeNamedCache::getCacheName() const
    {
    return f_vsCacheName;
    }

bool SafeNamedCache::isActive() const
    {
    try
        {
        return m_hNamedCache->isActive();
        }
    catch (Exception::View)
        {
        return false;
        }
    }

NamedCache::CacheServiceHandle SafeNamedCache::getCacheService()
    {
    return m_hCacheService;
    }

NamedCache::CacheServiceView SafeNamedCache::getCacheService() const
    {
    return m_hCacheService;
    }

void SafeNamedCache::setSafeCacheService(SafeCacheService::Handle cacheService)
    {
    m_hCacheService = cacheService;
    }


// ----- ConcurrentMap interface --------------------------------------------

bool SafeNamedCache::lock(Object::View vKey, int64_t cWait) const
    {
    return getRunningNamedCache()->lock(vKey, cWait);
    }

bool SafeNamedCache::lock(Object::View vKey) const
    {
    return getRunningNamedCache()->lock(vKey);
    }

bool SafeNamedCache::unlock(Object::View vKey) const
    {
    return getRunningNamedCache()->unlock(vKey);
    }


// ----- InvocableMap interface ---------------------------------------------

Object::Holder SafeNamedCache::invoke(Object::View vKey,
        EntryProcessor::Handle hAgent)
    {
    return getRunningNamedCache()->invoke(vKey, hAgent);
    }

Map::View SafeNamedCache::invokeAll(Collection::View vCollKeys,
        EntryProcessor::Handle hAgent)
    {
    return getRunningNamedCache()->invokeAll(vCollKeys, hAgent);
    }

Map::View SafeNamedCache::invokeAll(Filter::View vFilter,
        EntryProcessor::Handle hAgent)
    {
    return getRunningNamedCache()->invokeAll(vFilter, hAgent);
    }

Object::Holder SafeNamedCache::aggregate(Collection::View vCollKeys,
        EntryAggregator::Handle hAgent) const
    {
    return getRunningNamedCache()->aggregate(vCollKeys, hAgent);
    }

Object::Holder SafeNamedCache::aggregate(Filter::View vFilter,
        EntryAggregator::Handle hAgent) const
    {
    return getRunningNamedCache()->aggregate(vFilter, hAgent);
    }


// ----- QueryMap interface -------------------------------------------------

Set::View SafeNamedCache::keySet(Filter::View vFilter) const
    {
    return getRunningNamedCache()->keySet(vFilter);
    }

Set::View SafeNamedCache::entrySet(Filter::View vFilter) const
    {
    return getRunningNamedCache()->entrySet(vFilter);
    }

Set::View SafeNamedCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return getRunningNamedCache()->entrySet(vFilter, vComparator);
    }

void SafeNamedCache::addIndex(ValueExtractor::View vExtractor, bool fOrdered,
        Comparator::View vComparator)
    {
    getRunningNamedCache()->addIndex(vExtractor, fOrdered, vComparator);
    }

void SafeNamedCache::removeIndex(ValueExtractor::View vExtractor)
    {
    getRunningNamedCache()->removeIndex(vExtractor);
    }


// ----- ObservableMap interface --------------------------------------------

void SafeNamedCache::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    if (this == hListener)
        {
        NamedCache::Handle hCache = getNamedCache();
        try
            {
            hCache->addKeyListener(hListener, vKey, fLite);
            }
        catch (Exception::View e)
            {
            if (hCache != NULL && hCache->isActive() &&
                    hCache->getCacheService()->isRunning())
                {
                COH_THROW (e);
                }
            // NamedCache has been invalidated
            }
        }
    else if (instanceof<MapListenerSupport::SynchronousListener::Handle>
            (hListener) || instanceof<MapTriggerListener::Handle>(hListener))
        {
        getRunningNamedCache()->addKeyListener(hListener, vKey, fLite);
        }
    else if (hListener != NULL)
        {
        bool fWasEmpty = false;
        bool fWasLite  = false;

        MapListenerSupport::Handle hSupport = getListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            fWasEmpty = hSupport->isEmpty(vKey);
            fWasLite  = fWasEmpty ||
                    !hSupport->containsStandardListeners(vKey);

            hSupport->addListener(hListener, vKey, fLite);
            }

        if (fWasEmpty || (fWasLite && !fLite))
            {
            try
                {
                addKeyListener(this, vKey, fLite);
                }
            catch (Exception::View e)
                {
                hSupport->removeListener(hListener, vKey);
                COH_THROW (e);
                }
            }
        }
    }

void SafeNamedCache::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    if (this == hListener ||
            instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener))
        {
        NamedCache::Handle hCache = getNamedCache();
        try
            {
            hCache->removeKeyListener(hListener, vKey);
            }
        catch (Exception::View e)
            {
            if (hCache != NULL && hCache->isActive() &&
                    hCache->getCacheService()->isRunning())
                {
                COH_THROW (e);
                }
            // NamedCache has been invalidated
            }
        }
    else if (hListener != NULL)
        {
        bool fEmpty = false;

        MapListenerSupport::Handle hSupport = getListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            hSupport->removeListener(hListener, vKey);
            fEmpty = hSupport->isEmpty(vKey);
            }

        if (fEmpty)
            {
            removeKeyListener(this, vKey);
            }
        }
    }

void SafeNamedCache::addMapListener(MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }

void SafeNamedCache::removeMapListener(MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }

void SafeNamedCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    if (this == hListener)
        {
        NamedCache::Handle hCache = getNamedCache();
        try
            {
            hCache->addFilterListener(hListener, vFilter, fLite);
            }
        catch (Exception::View e)
            {
            if (hCache != NULL && hCache->isActive() &&
                    hCache->getCacheService()->isRunning())
                {
                COH_THROW (e);
                }
            // NamedCache has been invalidated
            }
        }
    else if (instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener)
          || instanceof<MapTriggerListener::Handle>(hListener))
        {
        getRunningNamedCache()->addFilterListener(hListener, vFilter, fLite);
        }
    else if (hListener != NULL)
        {
        bool fWasEmpty = false;
        bool fWasLite  = false;

        MapListenerSupport::Handle hSupport = getListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            fWasEmpty = hSupport->isEmpty(vFilter);
            fWasLite  = fWasEmpty ||
                    !hSupport->containsStandardListeners(vFilter);

            hSupport->addListener(hListener, vFilter, fLite);
            }

        if (fWasEmpty || (fWasLite && !fLite))
            {
            try
                {
                addFilterListener(this, vFilter, fLite);
                }
            catch (Exception::View e)
                {
                hSupport->removeListener(hListener, vFilter);
                COH_THROW (e);
                }
            }
        }
    }

void SafeNamedCache::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    if (this == hListener  ||
            instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener) ||
            instanceof<MapTriggerListener::Handle>(hListener))
        {
        NamedCache::Handle hCache = getNamedCache();
        try
            {
            hCache->removeFilterListener(hListener, vFilter);
            }
        catch (Exception::View e)
            {
            if (hCache != NULL && hCache->isActive() &&
                    hCache->getCacheService()->isRunning())
                {
                COH_THROW (e);
                }
            // NamedCache has been invalidated
            }
        }
    else if (hListener != NULL)
        {
        bool fEmpty = false;

        MapListenerSupport::Handle hSupport = getListenerSupport();
        COH_SYNCHRONIZED (hSupport)
            {
            hSupport->removeListener(hListener, vFilter);
            fEmpty = hSupport->isEmpty(vFilter);
            }

        if (fEmpty)
            {
            removeFilterListener(this, vFilter);
            }
        }
    }


// ----- MapListener interface ----------------------------------------------

void SafeNamedCache::entryInserted(MapEvent::View vEvent)
    {
    translateMapEvent(vEvent);
    }

void SafeNamedCache::entryUpdated(MapEvent::View vEvent)
    {
    translateMapEvent(vEvent);
    }


void SafeNamedCache::entryDeleted(MapEvent::View vEvent)
    {
    translateMapEvent(vEvent);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> SafeNamedCache::toString() const
    {
    return COH_TO_STRING(super::toString() << ": " << getNamedCache());
    }


// ----- internal methods ---------------------------------------------------

NamedCache::Handle SafeNamedCache::getRunningNamedCache() const
    {
    NamedCache::Handle   hCache   = m_hNamedCache;
    CacheService::Handle hService = NULL == hCache
            ? CacheService::Handle(NULL)
            : hCache->getCacheService();

    if (NULL == hService || !hService->isRunning() || !hCache->isActive())
        {
        SafeService::Handle hSafeService = m_hCacheService;
        COH_SYNCHRONIZED(hSafeService)
            {
            COH_SYNCHRONIZED(this)
                {
                hCache   = m_hNamedCache;
                hService = NULL == hCache
                        ? CacheService::Handle(NULL)
                        : hCache->getCacheService();

                if (NULL == hService || !hService->isRunning() ||
                        !hCache->isActive())
                    {
                    if (m_fIsReleased)
                        {
                        COH_THROW (UnsupportedOperationException::create(
                                "SafeNamedCache was explicitly released."));
                        }
                    else
                        {
                        // restart the actual named cache
                        if (hCache != NULL)
                            {
                            m_hNamedCache = NULL;
                            COH_LOG("Restarting NamedCache: " <<
                                    getCacheName(), Logger::level_info);
                            }
                        m_hNamedCache = hCache = restartNamedCache();
                        }
                    }
                }
            }
        hSafeService->drainEvents();
        }
    return hCache;
    }

NamedCache::Handle SafeNamedCache::restartNamedCache() const
    {
    /*
    * We're going to cast away constness in restartNamedCache so that we
    * can support expiry from const methods.
    */
    SafeNamedCache::Handle hThis = const_cast<SafeNamedCache*>(this);
    COH_ENSURE(hThis != NULL);

    // In case the underlying cache is scoped by Subject, use the original
    // Subject
    COH_RUN_AS (getSubject())
        {
        return hThis->restartNamedCache();
        }
    }

NamedCache::Handle SafeNamedCache::restartNamedCache()
    {
    NamedCache::Handle hCache =
            m_hCacheService->ensureRunningCacheService(false)->
            ensureCache(f_vsCacheName);

    ObjectArray::View        vaFilter;
    ObjectArray::View        vaoKey;
    MapListenerSupport::View vSupport = getListenerSupport();

    COH_SYNCHRONIZED (vSupport)
        {
        if (!vSupport->isEmpty())
            {
            vaFilter = vSupport->getFilterSet()->toArray();
            vaoKey   = vSupport->getKeySet()->toArray();
            }
        }

    if (NULL != vaFilter)
        {
        for (size32_t i = 0, c = vaFilter->length; i < c; i++)
            {
            Filter::View vFilter = cast<Filter::View>(vaFilter[i]);

            hCache->addFilterListener(this, vFilter,
                !vSupport->containsStandardListeners(vFilter));
            }
        }

    if (NULL != vaoKey)
        {
        for (size32_t i = 0, c = vaoKey->length; i < c; i++)
            {
            Object::View vKey = vaoKey[i];

            hCache->addKeyListener(this, vKey,
                !vSupport->containsStandardListeners(vKey));
            }
        }

    return hCache;
    }

void SafeNamedCache::translateMapEvent(MapEvent::View vEvt)
    {
    if (vEvt->getSource() == getNamedCache())
        {
        // ensure lazy event data access
        MapEvent::Handle hEvt = MapListenerSupport::convertEvent(
                vEvt, this, NULL, NULL);
        getListenerSupport()->fireEvent(hEvt, true);
        }
    }

void SafeNamedCache::releaseListeners()
    {
    MapListenerSupport::Handle hSupport = getListenerSupport();
    if (!hSupport->isEmpty())
        {
        List::Handle hListFilter = LinkedList::create();
        List::Handle hListKeys   = LinkedList::create();

        COH_SYNCHRONIZED (hSupport)
            {
            if (!hSupport->isEmpty())
                {
                hListFilter->addAll(hSupport->getFilterSet());
                hListKeys->addAll(hSupport->getKeySet());
                hSupport->clear();
                }
            }

        for (Iterator::Handle hIter = hListFilter->iterator(); hIter->hasNext(); )
            {
            removeFilterListener(this, cast<Filter::View>(hIter->next()));
            }
        for (Iterator::Handle hIter = hListKeys->iterator(); hIter->hasNext(); )
            {
            removeKeyListener(this, hIter->next());
            }
        }
    }


// ----- property getters/setters -------------------------------------------

MapListenerSupport::Handle SafeNamedCache::getListenerSupport()
    {
    return f_hListenerSupport;
    }

MapListenerSupport::View SafeNamedCache::getListenerSupport() const
    {
    return f_hListenerSupport;
    }

void SafeNamedCache::setListenerSupport(
        MapListenerSupport::Handle hListenerSupport)
    {
    initialize(f_hListenerSupport, hListenerSupport);
    }

NamedCache::Handle SafeNamedCache::getNamedCache()
    {
    return m_hNamedCache;
    }

NamedCache::View SafeNamedCache::getNamedCache() const
    {
    return m_hNamedCache;
    }

void SafeNamedCache::setNamedCache(NamedCache::Handle hNamedCache)
    {
    m_hNamedCache = hNamedCache;
    }

void SafeNamedCache::setCacheName(String::View vsCacheName)
    {
    initialize(f_vsCacheName, vsCacheName);
    }

SafeCacheService::View SafeNamedCache::getSafeCacheService() const
    {
    return m_hCacheService;
    }

bool SafeNamedCache::isReleased() const
    {
    return m_fIsReleased;
    }

void SafeNamedCache::setReleased(bool fReleased)
    {
    COH_SYNCHRONIZED (this)
        {
        m_fIsReleased = fReleased;
        }
    }

Subject::View SafeNamedCache::getSubject() const
    {
    return f_vSubject;
    }

void SafeNamedCache::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

COH_CLOSE_NAMESPACE3
