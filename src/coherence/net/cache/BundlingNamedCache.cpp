/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/BundlingNamedCache.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

BundlingNamedCache::BundlingNamedCache(NamedCache::Handle hCache)
    : super(hCache), m_hGetBundler(self()),
      m_hPutBundler(self()), m_hRemoveBundler(self())
    {
    }

// ----- initiators ---------------------------------------------------------

AbstractBundler::Handle BundlingNamedCache::ensureGetBundler(int32_t cBundleThreshold)
    {
    COH_SYNCHRONIZED(this)
        {
        if (cBundleThreshold > 0)
            {
            GetBundler::Handle hBundler = m_hGetBundler;
            if (hBundler == NULL)
                {
                m_hGetBundler = hBundler = GetBundler::create(this);
                }
            hBundler->setSizeThreshold(cBundleThreshold);
            return hBundler;
            }
        else
            {
            return m_hGetBundler = NULL;
            }
        }
    }

AbstractBundler::Handle BundlingNamedCache::ensurePutBundler(int32_t cBundleThreshold)
    {
    COH_SYNCHRONIZED(this)
        {
        if (cBundleThreshold > 0)
            {
            PutBundler::Handle hBundler = m_hPutBundler;
            if (hBundler == NULL)
                {
                m_hPutBundler = hBundler = PutBundler::create(this);
                }
            hBundler->setSizeThreshold(cBundleThreshold);
            return hBundler;
            }
        else
            {
            return m_hPutBundler = NULL;
            }
        }
    }

AbstractBundler::Handle BundlingNamedCache::ensureRemoveBundler(int32_t cBundleThreshold)
    {
    COH_SYNCHRONIZED(this)
        {
        if (cBundleThreshold > 0)
            {
            RemoveBundler::Handle hBundler = m_hRemoveBundler;
            if (hBundler == NULL)
                {
                m_hRemoveBundler = hBundler = RemoveBundler::create(this);
                }
            hBundler->setSizeThreshold(cBundleThreshold);
            return hBundler;
            }
        else
            {
            return m_hRemoveBundler = NULL;
            }
        }
    }

// ----- accessors ----------------------------------------------------------

AbstractBundler::Handle BundlingNamedCache::getGetBundler()
    {
    return m_hGetBundler;
    }

AbstractBundler::Handle BundlingNamedCache::getPutBundler()
    {
    return m_hPutBundler;
    }

AbstractBundler::Handle BundlingNamedCache::getRemoveBundler()
    {
    return m_hRemoveBundler;
    }

// ----- various bundleable NamedCache methods ------------------------------

Object::Holder BundlingNamedCache::get(Object::View vKey)
    {
    GetBundler::Handle hBundler = m_hGetBundler;
    return hBundler == NULL ?
            super::get(vKey) : hBundler->process(vKey);
    }

Map::View BundlingNamedCache::getAll(Collection::View vColKeys)
    {
    GetBundler::Handle hBundler = m_hGetBundler;
    return hBundler == NULL ?
            super::getAll(vColKeys) : hBundler->processAll(vColKeys);
    }

Object::Holder BundlingNamedCache::put(Object::View vKey, Object::Holder ohValue)
    {
    PutBundler::Handle hBundler = m_hPutBundler;
    if (hBundler == NULL)
        {
        super::putAll(Collections::singletonMap(vKey, ohValue));
        }
    else
        {
        hBundler->process(vKey, ohValue);
        }
    return NULL;
    }

void BundlingNamedCache::putAll(Map::View vMap)
    {
    PutBundler::Handle hBundler = m_hPutBundler;
    if (hBundler == NULL)
        {
        super::putAll(vMap);
        }
    else
        {
        hBundler->processAll(vMap);
        }
    }

Object::Holder BundlingNamedCache::remove(Object::View vKey)
    {
    RemoveBundler::Handle hBundler = m_hRemoveBundler;
    if (hBundler == NULL)
        {
        super::remove(vKey);
        }
    else
        {
        hBundler->process(vKey);
        }
    return NULL;
    }

// ----- NamedCache interface -----------------------------------------------

void BundlingNamedCache::release()
    {
    super::release();
    m_hGetBundler = NULL;
    m_hPutBundler = NULL;
    m_hRemoveBundler = NULL;
    }

void BundlingNamedCache::destroy()
    {
    super::destroy();
    m_hGetBundler = NULL;
    m_hPutBundler = NULL;
    m_hRemoveBundler = NULL;
    }

// ----- inner classes --------------------------------------------------

BundlingNamedCache::GetBundler::GetBundler(BundlingNamedCache::Handle hBundlingNamedCache)
    : f_hBundlingNamedCache(self(), hBundlingNamedCache)
    {
    AbstractBundler::Bundle::Handle hBundle = instantiateBundle();
    init(hBundle);
    }

Map::View BundlingNamedCache::GetBundler::bundle(Collection::View vColKeys)
    {
    return getBundlingNamedCache()->BundlingNamedCache::super::getAll(vColKeys);
    }

Object::Holder BundlingNamedCache::GetBundler::unbundle(Object::View vKey) const
    {
    return getBundlingNamedCache()->BundlingNamedCache::super::get(vKey);
    }

Object::Holder BundlingNamedCache::GetBundler::unbundle(Object::View vKey)
    {
    return getBundlingNamedCache()->BundlingNamedCache::super::get(vKey);
    }

BundlingNamedCache::Handle BundlingNamedCache::GetBundler::getBundlingNamedCache()
    {
    return f_hBundlingNamedCache;
    }

BundlingNamedCache::View BundlingNamedCache::GetBundler::getBundlingNamedCache() const
    {
    return f_hBundlingNamedCache;
    }

BundlingNamedCache::PutBundler::PutBundler(BundlingNamedCache::Handle hBundlingNamedCache)
    : f_hBundlingNamedCache(self(), hBundlingNamedCache)
    {
    AbstractBundler::Bundle::Handle hBundle = instantiateBundle();
    init(hBundle);
    }

void BundlingNamedCache::PutBundler::bundle(Map::View vMap)
    {
    getBundlingNamedCache()->BundlingNamedCache::super::putAll(vMap);
    }

BundlingNamedCache::Handle BundlingNamedCache::PutBundler::getBundlingNamedCache()
    {
    return f_hBundlingNamedCache;
    }

BundlingNamedCache::View BundlingNamedCache::PutBundler::getBundlingNamedCache() const
    {
    return f_hBundlingNamedCache;
    }

BundlingNamedCache::RemoveBundler::RemoveBundler(BundlingNamedCache::Handle hBundlingNamedCache)
    : f_hBundlingNamedCache(self(), hBundlingNamedCache)
    {
    AbstractBundler::Bundle::Handle hBundle = instantiateBundle();
    init(hBundle);
    }

Map::View BundlingNamedCache::RemoveBundler::bundle(Collection::View vColKeys)
    {
    getBundlingNamedCache()->keySet()->removeAll(vColKeys);
    return NULL;
    }

Object::Holder BundlingNamedCache::RemoveBundler::unbundle(Object::View vKey)
    {
    getBundlingNamedCache()->keySet()->remove(vKey);
    return NULL;
    }

BundlingNamedCache::Handle BundlingNamedCache::RemoveBundler::getBundlingNamedCache()
    {
    return f_hBundlingNamedCache;
    }

BundlingNamedCache::View BundlingNamedCache::RemoveBundler::getBundlingNamedCache() const
    {
    return f_hBundlingNamedCache;
    }

COH_CLOSE_NAMESPACE3
