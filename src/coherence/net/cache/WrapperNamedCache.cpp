/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/WrapperNamedCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

// ----- constructors/destructor  -------------------------------------------

WrapperNamedCache::WrapperNamedCache(NamedCache::Handle hCache)
    : f_hNamedCache(self(), (NamedCache::Handle) hCache)
    {
    }

// ----- Map interface ------------------------------------------------------

size32_t WrapperNamedCache::size() const
    {
    return getNamedCache()->size();
    }

bool WrapperNamedCache::isEmpty() const
    {
    return getNamedCache()->isEmpty();
    }

bool WrapperNamedCache::containsKey(Object::View vKey) const
    {
    return getNamedCache()->containsKey(vKey);
    }

bool WrapperNamedCache::containsValue(Object::View vValue) const
    {
    return getNamedCache()->containsValue(vValue);
    }

Object::Holder WrapperNamedCache::get(Object::View vKey) const
    {
    return getNamedCache()->get(vKey);
    }

Object::Holder WrapperNamedCache::get(Object::View vKey)
    {
    return getNamedCache()->get(vKey);
    }

Object::Holder WrapperNamedCache::put(Object::View vKey, Object::Holder ohValue)
    {
    return getNamedCache()->put(vKey, ohValue);
    }

Object::Holder WrapperNamedCache::remove(Object::View vKey)
    {
    return getNamedCache()->remove(vKey);
    }

void WrapperNamedCache::putAll(Map::View vMap)
    {
    getNamedCache()->putAll(vMap);
    }

void WrapperNamedCache::clear()
    {
    getNamedCache()->clear();
    }

Set::View WrapperNamedCache::keySet() const
    {
    return getNamedCache()->keySet();
    }

Set::Handle WrapperNamedCache::keySet()
    {
    return getNamedCache()->keySet();
    }

Collection::View WrapperNamedCache::values() const
    {
    return getNamedCache()->values();
    }

Collection::Handle WrapperNamedCache::values()
    {
    return getNamedCache()->values();
    }

Set::View WrapperNamedCache::entrySet() const
    {
    return getNamedCache()->entrySet();
    }

Set::Handle WrapperNamedCache::entrySet()
    {
    return getNamedCache()->entrySet();
    }

// ----- CacheMap interface -------------------------------------------------

Map::View WrapperNamedCache::getAll(Collection::View vKeys) const
    {
    return getNamedCache()->getAll(vKeys);
    }

Object::Holder WrapperNamedCache::put(Object::View vKey, Object::Holder hValue,
        int64_t cMillis)
    {
    return getNamedCache()->put(vKey, hValue, cMillis);
    }

// ----- NamedCache interface -----------------------------------------------

void WrapperNamedCache::release()
    {
    getNamedCache()->release();
    }

void WrapperNamedCache::destroy()
    {
    getNamedCache()->destroy();
    }

void WrapperNamedCache::truncate()
    {
    getNamedCache()->truncate();
    }

String::View WrapperNamedCache::getCacheName() const
    {
    return getNamedCache()->getCacheName();
    }

bool WrapperNamedCache::isActive() const
    {
    try
        {
        return f_hNamedCache->isActive();
        }
    catch (Exception::View)
        {
        return false;
        }
    }

NamedCache::CacheServiceHandle WrapperNamedCache::getCacheService()
    {
    return getNamedCache()->getCacheService();
    }

NamedCache::CacheServiceView WrapperNamedCache::getCacheService() const
    {
    return getNamedCache()->getCacheService();
    }

// ----- ConcurrentMap interface --------------------------------------------

bool WrapperNamedCache::lock(Object::View vKey, int64_t cWait) const
    {
    return getNamedCache()->lock(vKey, cWait);
    }

bool WrapperNamedCache::lock(Object::View vKey) const
    {
    return getNamedCache()->lock(vKey);
    }

bool WrapperNamedCache::unlock(Object::View vKey) const
    {
    return getNamedCache()->unlock(vKey);
    }

// ----- InvocableMap interface ---------------------------------------------

Object::Holder WrapperNamedCache::invoke(Object::View vKey,
        EntryProcessor::Handle hAgent)
    {
    return getNamedCache()->invoke(vKey, hAgent);
    }

Map::View WrapperNamedCache::invokeAll(Collection::View vCollKeys,
        EntryProcessor::Handle hAgent)
    {
    return getNamedCache()->invokeAll(vCollKeys, hAgent);
    }

Map::View WrapperNamedCache::invokeAll(Filter::View vFilter,
        EntryProcessor::Handle hAgent)
    {
    return getNamedCache()->invokeAll(vFilter, hAgent);
    }

Object::Holder WrapperNamedCache::aggregate(Collection::View vCollKeys,
        EntryAggregator::Handle hAgent) const
    {
    return getNamedCache()->aggregate(vCollKeys, hAgent);
    }

Object::Holder WrapperNamedCache::aggregate(Filter::View vFilter,
        EntryAggregator::Handle hAgent) const
    {
    return getNamedCache()->aggregate(vFilter, hAgent);
    }

// ----- QueryMap interface -------------------------------------------------

Set::View WrapperNamedCache::keySet(Filter::View vFilter) const
    {
    return getNamedCache()->keySet(vFilter);
    }

Set::View WrapperNamedCache::entrySet(Filter::View vFilter) const
    {
    return getNamedCache()->entrySet(vFilter);
    }

Set::View WrapperNamedCache::entrySet(Filter::View vFilter,
        Comparator::View vComparator) const
    {
    return getNamedCache()->entrySet(vFilter, vComparator);
    }

void WrapperNamedCache::addIndex(ValueExtractor::View vExtractor, bool fOrdered,
        Comparator::View vComparator)
    {
    getNamedCache()->addIndex(vExtractor, fOrdered, vComparator);
    }

void WrapperNamedCache::removeIndex(ValueExtractor::View vExtractor)
    {
    getNamedCache()->removeIndex(vExtractor);
    }

// ----- ObservableMap interface --------------------------------------------

void WrapperNamedCache::addKeyListener(MapListener::Handle hListener,
        Object::View vKey, bool fLite)
    {
    COH_ENSURE(NULL != hListener);
    NamedCache::Handle hCache = getNamedCache();
    try
        {
        hCache->addKeyListener(hListener, vKey, fLite);
        }
    catch (Exception::View e)
        {
        COH_THROW (e);
        }
    }

void WrapperNamedCache::removeKeyListener(MapListener::Handle hListener,
        Object::View vKey)
    {
    COH_ENSURE(NULL != hListener);
    NamedCache::Handle hCache = getNamedCache();
    try
        {
        hCache->removeKeyListener(hListener, vKey);
        }
    catch (Exception::View e)
        {
        COH_THROW (e);
        }
    }

void WrapperNamedCache::addMapListener(MapListener::Handle hListener)
    {
    addFilterListener(hListener, (Filter::View) NULL, false);
    }

void WrapperNamedCache::removeMapListener(MapListener::Handle hListener)
    {
    removeFilterListener(hListener, (Filter::View) NULL);
    }

void WrapperNamedCache::addFilterListener(MapListener::Handle hListener,
        Filter::View vFilter, bool fLite)
    {
    COH_ENSURE(NULL != hListener);
    NamedCache::Handle hCache = getNamedCache();
    try
        {
        hCache->addFilterListener(hListener, vFilter, fLite);
        }
    catch (Exception::View e)
        {
        COH_THROW (e);
        }
    }

void WrapperNamedCache::removeFilterListener(MapListener::Handle hListener,
        Filter::View vFilter)
    {
    COH_ENSURE(NULL != hListener);
    NamedCache::Handle hCache = getNamedCache();
    try
        {
        hCache->removeFilterListener(hListener, vFilter);
        }
    catch (Exception::View e)
        {
        COH_THROW (e);
        }
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> WrapperNamedCache::toString() const
    {
    return COH_TO_STRING(super::toString() << "{" << getNamedCache() << "}");
    }

// ----- property getters/setters -------------------------------------------

NamedCache::Handle WrapperNamedCache::getNamedCache()
    {
    return f_hNamedCache;
    }

NamedCache::View WrapperNamedCache::getNamedCache() const
    {
    return f_hNamedCache;
    }

COH_CLOSE_NAMESPACE3
