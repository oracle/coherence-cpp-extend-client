/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/NearCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

COH_OPEN_NAMESPACE_ANON(NearCache)

// ----- helper class BackServiceListener -----------------------------------

/**
* MemberListener for the back cache's service.
*
* The primary goal of that listener is invalidation of the front map
* in case of the corresponding CacheService [automatic] restart.
*/
class BackServiceListener
    : public class_spec<BackServiceListener,
        extends<Object>,
        implements<MemberListener> >
    {
    friend class factory<BackServiceListener>;

    // ----- constructor --------------------------------------------

    protected:
        /**
        * Create an BackServiceListener.
        *
        * @param hMap  the associated CachingMap
        */
        BackServiceListener(CachingMap::Handle hMap)
            : m_whMap(self(), hMap)
            {
            }

    // ----- MemberListener interface -------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void memberJoined(MemberEvent::View vEvt)
            {
            if (vEvt->isLocal())
                {
                m_whMap->resetFrontMap();
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeaving(MemberEvent::View /*vEvt*/)
            {
            // nothing on purpose...
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeft(MemberEvent::View vEvt)
            {
            if (vEvt->isLocal() && m_whMap->getInvalidationStrategy() != CachingMap::listen_none)
                {
                m_whMap->resetFrontMap();
                }
            }

        // ---- data members ----------------------------------------

        protected:
            /**
            * The map associated with this listener.
            */
            WeakHandle<CachingMap> m_whMap;
    };

COH_CLOSE_NAMESPACE_ANON

// ----- constructors -------------------------------------------------------

NearCache::NearCache(CacheMap::Handle hMapFront, CacheMap::Handle hMapBack,
        CachingMap::InvalidationStrategy strategy,
        ConcurrentMap::Handle hMapControl)
    : super(hMapFront, hMapBack, strategy, hMapControl),
      f_vsName(self(), cast<NamedCache::View>(hMapBack)->getCacheName()),
      f_hService(self(), cast<NamedCache::Handle>(hMapBack)->getCacheService()),
      f_hListenerBackService(self())
    {
    }

void NearCache::onInit()
    {
    super::onInit();
    initialize(f_hListenerBackService, registerBackServiceListener());
    }

// ----- accessors ----------------------------------------------------------

NamedCache::Handle NearCache::getBackCache()
    {
    return cast<NamedCache::Handle>(getBackMap());
    }

NamedCache::View NearCache::getBackCache() const
    {
    return cast<NamedCache::View>(getBackMap());
    }

// ----- NamedCache interface -----------------------------------------------

String::View NearCache::getCacheName() const
    {
    return f_vsName;
    }

NamedCache::CacheServiceHandle NearCache::getCacheService()
    {
    return f_hService;
    }

NamedCache::CacheServiceView NearCache::getCacheService() const
    {
    return f_hService;
    }

bool NearCache::isActive() const
    {
    try
        {
        return !m_fReleased && getBackCache()->isActive();
        }
    catch (IllegalStateException::View)
        {
        return false;
        }
    }

void NearCache::release()
    {
    NearCache::release(false);
    }

void NearCache::destroy()
    {
    NearCache::release(true);
    }

void NearCache::truncate()
    {
    getBackCache()->truncate();
    }


// ----- ObservableMap interface --------------------------------------------

void NearCache::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    getBackCache()->addKeyListener(hListener, vKey, fLite);
    }

/**
* {@inheritDoc}
*/
void NearCache::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    getBackCache()->removeKeyListener(hListener, vKey);
    }

void NearCache::addMapListener(MapListener::Handle hListener)
    {
    getBackCache()->addMapListener(hListener);
    }

void NearCache::removeMapListener(MapListener::Handle hListener)
    {
    getBackCache()->removeMapListener(hListener);
    }

/**
* {@inheritDoc}
*/
void NearCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    getBackCache()->addFilterListener(hListener, vFilter, fLite);
    }

/**
* {@inheritDoc}
*/
void NearCache::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    getBackCache()->removeFilterListener(hListener, vFilter);
    }


// ----- CacheMap interface -------------------------------------------------

Map::View NearCache::getAll(Collection::View vColKeys) const
    {
    return CachingMap::getAll(vColKeys);
    }

Object::Holder NearCache::put(Object::View vKey,
        Object::Holder ohValue, int64_t cMillis)
    {
    return CachingMap::put(vKey, ohValue, true, cMillis);
    }


// ----- ConcurrentMap interface --------------------------------------------

bool NearCache::lock(Object::View vKey, int64_t cWait) const
    {
    if (getBackCache()->lock(vKey, cWait))
        {
        // back map listeners are always synchronous, so if there is one
        // the front map invalidation is not necessary
        if (getInvalidationStrategy() == CachingMap::listen_none)
            {
            getFrontMap()->remove(vKey);
            }
        return true;
        }
    else
        {
        return false;
        }
    }

bool NearCache::lock(Object::View vKey) const
    {
    return lock(vKey, 0L);
    }

bool NearCache::unlock(Object::View vKey) const
    {
    return getBackCache()->unlock(vKey);
    }


// ----- QueryMap interface -------------------------------------------------

Set::View NearCache::keySet(Filter::View vFilter) const
    {
    return getBackCache()->keySet(vFilter);
    }

Set::View NearCache::entrySet(Filter::View vFilter) const
    {
    return getBackCache()->entrySet(vFilter);
    }

Set::View NearCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return getBackCache()->entrySet(vFilter, vComparator);
    }

void NearCache::addIndex(ValueExtractor::View vExtractor, bool fOrdered,
        Comparator::View vComparator)
    {
    getBackCache()->addIndex(vExtractor, fOrdered, vComparator);
    }

void NearCache::removeIndex(ValueExtractor::View vExtractor)
    {
    getBackCache()->removeIndex(vExtractor);
    }


// ----- InvocableMap interface ---------------------------------------------

Object::Holder NearCache::invoke(Object::View vKey,
        EntryProcessor::Handle hAgent)
    {
    return getBackCache()->invoke(vKey, hAgent);
    }

Map::View NearCache::invokeAll(Collection::View vCollKeys,
        EntryProcessor::Handle hAgent)
    {
    return getBackCache()->invokeAll(vCollKeys, hAgent);
    }

Map::View NearCache::invokeAll(Filter::View vFilter,
        EntryProcessor::Handle hAgent)
    {
    return getBackCache()->invokeAll(vFilter, hAgent);
    }

Object::Holder NearCache::aggregate(Collection::View vCollKeys,
        EntryAggregator::Handle hAgent) const
    {
    return getBackCache()->aggregate(vCollKeys, hAgent);
    }

Object::Holder NearCache::aggregate(Filter::View vFilter,
        EntryAggregator::Handle hAgent) const
    {
    return getBackCache()->aggregate(vFilter, hAgent);
    }


// ----- internal helpers ---------------------------------------------------

void NearCache::release(bool fDestroy)
    {
    try
        {
        NamedCache::Handle hCache = getBackCache();
        unregisterBackServiceListener();
        inherited::release();
        if (fDestroy)
            {
            hCache->destroy();
            }
        else
            {
            hCache->release();
            }
        }
        catch (RuntimeException::View)
            {
            // one of the following should be ignored:
            //   IllegalStateException("Cache is not active");
            //   RuntimeException("Storage is not configured");
            //   RuntimeException("Service has been terminated");
            }
    }

MemberListener::Handle NearCache::registerBackServiceListener()
    {
    // automatic front map clean up (upon service restart)
    // requires a MemberListener implementation
    CacheService::Handle hService = getCacheService();
    if (hService != NULL)
        {
        try
            {
            MemberListener::Handle hListener =
                    BackServiceListener::create(this);
            hService->addMemberListener(hListener);
            return hListener;
            }
        catch (UnsupportedOperationException::View)
            {
            // do nothing
            }
        }

    return NULL;
    }

void NearCache::unregisterBackServiceListener()
    {
    try
        {
        getCacheService()->removeMemberListener(f_hListenerBackService);
        }
    catch (RuntimeException::View)
        {
        // do nothing
        }
    }

COH_CLOSE_NAMESPACE3
