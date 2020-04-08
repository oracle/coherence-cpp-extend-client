/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/cache/SimpleCacheStatistics.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)


// ----- constructors ---------------------------------------------------

SimpleCacheStatistics::SimpleCacheStatistics()
        : m_cCacheHits(0L),
          m_cCacheMisses(0L),
          m_cHitsMillis(0L),
          m_cMissesMillis(0L),
          m_cCachePuts(0L),
          m_cPutsMillis(0L),
          m_cCachePrunes(0L),
          m_cCachePrunesMillis(0L)
    {
    }


// ----- CacheStatistics interface --------------------------------------

 int64_t SimpleCacheStatistics::getTotalGets() const
    {
    return m_cCacheHits + m_cCacheMisses;
    }

 int64_t SimpleCacheStatistics::getTotalGetsMillis() const
    {
    return m_cHitsMillis + m_cMissesMillis;
    }

 float64_t SimpleCacheStatistics::getAverageGetMillis() const
    {
    float64_t cMillis = (float64_t) m_cHitsMillis +
            (float64_t) m_cMissesMillis;
    float64_t cGets   = (float64_t) m_cCacheHits  +
            (float64_t) m_cCacheMisses;
    return Float64::isZero(cGets) ? 0.0 : cMillis / cGets;
    }

 int64_t SimpleCacheStatistics::getTotalPuts() const
    {
    return m_cCachePuts;
    }

 int64_t SimpleCacheStatistics::getTotalPutsMillis() const
    {
    return m_cPutsMillis;
    }

 float64_t SimpleCacheStatistics::getAveragePutMillis() const
    {
    float64_t cMillis = (float64_t) m_cPutsMillis;
    float64_t cPuts   = (float64_t) m_cCachePuts;
    return Float64::isZero(cPuts) ? 0.0 : cMillis / cPuts;
    }

 int64_t SimpleCacheStatistics::getCacheHits() const
    {
    return m_cCacheHits;
    }

 int64_t SimpleCacheStatistics::getCacheHitsMillis() const
    {
    return m_cHitsMillis;
    }

 float64_t SimpleCacheStatistics::getAverageHitMillis() const
    {
    float64_t cMillis = (float64_t) m_cHitsMillis;
    float64_t cGets   = (float64_t) m_cCacheHits;
    return Float64::isZero(cGets) ? 0.0 : cMillis / cGets;
    }

 int64_t SimpleCacheStatistics::getCacheMisses() const
    {
    return m_cCacheMisses;
    }

 int64_t SimpleCacheStatistics::getCacheMissesMillis() const
    {
    return m_cMissesMillis;
    }

 float64_t SimpleCacheStatistics::getAverageMissMillis() const
    {
    float64_t cMillis = (float64_t) m_cMissesMillis;
    float64_t cGets   = (float64_t) m_cCacheMisses;
    return Float64::isZero(cGets) ? 0.0 : cMillis / cGets;
    }

 float64_t SimpleCacheStatistics::getHitProbability() const
    {
    float64_t cHits   = (float64_t) m_cCacheHits;
    float64_t cTotal  = cHits + (float64_t) m_cCacheMisses;
    return Float64::isZero(cTotal) ? 0.0 : cHits / cTotal;
    }

 int64_t SimpleCacheStatistics::getCachePrunes() const
    {
    return m_cCachePrunes;
    }

 int64_t SimpleCacheStatistics::getCachePrunesMillis() const
    {
    return m_cCachePrunesMillis;
    }

 float64_t SimpleCacheStatistics::getAveragePruneMillis() const
    {
    float64_t cMillis = (float64_t) m_cCachePrunesMillis;
    float64_t cPrunes = (float64_t) m_cCachePrunes;
    return Float64::isZero(cPrunes) ? 0.0 : cMillis / cPrunes;
    }

 void SimpleCacheStatistics::resetHitStatistics()
    {
    m_cCacheHits         = 0L;
    m_cCacheMisses       = 0L;
    m_cHitsMillis        = 0L;
    m_cMissesMillis      = 0L;
    m_cPutsMillis        = 0L;
    m_cCachePuts         = 0L;
    m_cCachePrunes       = 0L;
    m_cCachePrunesMillis = 0L;
    }


// ----- accessors, mutators and helpers --------------------------------

 void SimpleCacheStatistics::registerHit()
    {
    m_cCacheHits = m_cCacheHits + 1;
    }

 void SimpleCacheStatistics::registerHit(int64_t lStartMillis)
    {
    m_cCacheHits = m_cCacheHits + 1;

    int64_t lStopMillis = System::safeTimeMillis();
    if (lStopMillis > lStartMillis)
        {
        m_cHitsMillis = m_cHitsMillis + (lStopMillis - lStartMillis);
        }
    }

 void SimpleCacheStatistics::registerHits(int32_t cHits, int64_t lStartMillis)
    {
    m_cCacheHits = m_cCacheHits + cHits;
    if (lStartMillis > 0)
        {
        int64_t lStopMillis = System::safeTimeMillis();
        if (lStopMillis > lStartMillis)
            {
            m_cHitsMillis = m_cHitsMillis + (lStopMillis - lStartMillis);
            }
        }
    }

 void SimpleCacheStatistics::registerMiss()
    {
    m_cCacheMisses = m_cCacheMisses + 1;
    }

 void SimpleCacheStatistics::registerMiss(int64_t lStartMillis)
    {
    m_cCacheMisses       = m_cCacheMisses + 1;
    int64_t lStopMillis  = System::safeTimeMillis();
    if (lStopMillis > lStartMillis)
        {
        m_cMissesMillis = m_cMissesMillis + (lStopMillis - lStartMillis);
        }
    }

 void SimpleCacheStatistics::registerMisses(int32_t cMisses,
         int64_t lStartMillis)
    {
    m_cCacheMisses = m_cCacheMisses + cMisses;
    if (lStartMillis > 0)
        {
        int64_t lStopMillis = System::safeTimeMillis();
        if (lStopMillis > lStartMillis)
            {
            m_cMissesMillis = m_cMissesMillis + (lStopMillis - lStartMillis);
            }
        }
    }

 void SimpleCacheStatistics::registerPut(int64_t lStartMillis)
    {
    m_cCachePuts = m_cCachePuts + 1;
    if (lStartMillis > 0)
        {
        int64_t lStopMillis = System::safeTimeMillis();
        if (lStopMillis > lStartMillis)
            {
            m_cPutsMillis = m_cPutsMillis + (lStopMillis - lStartMillis);
            }
        }
    }

 void SimpleCacheStatistics::registerPuts(int32_t cPuts, int64_t lStartMillis)
    {
    m_cCachePuts = m_cCachePuts + cPuts;
    if (lStartMillis > 0)
        {
        int64_t lStopMillis = System::safeTimeMillis();
        if (lStopMillis > lStartMillis)
            {
            m_cPutsMillis = m_cPutsMillis + (lStopMillis - lStartMillis);
            }
        }
    }

 void SimpleCacheStatistics::registerCachePrune(int64_t lStartMillis)
    {
    m_cCachePrunes = m_cCachePrunes + 1;

    if (lStartMillis > 0)
        {
        int64_t lStopMillis = System::safeTimeMillis();
        if (lStopMillis > lStartMillis)
            {
            m_cCachePrunesMillis = m_cCachePrunesMillis + (lStopMillis - lStartMillis);
            }
        }
    }

 TypedHandle<const String> SimpleCacheStatistics::toString() const
     {
     return COH_TO_STRING("CacheStatistics {TotalGets="
         << getTotalGets()
         << ", TotalGetsMillis="
         << getTotalGetsMillis()
         << ", AverageGetMillis="
         << getAverageGetMillis()
         << ", TotalPuts="
         << getTotalPuts()
         << ", TotalPutsMillis="
         << getTotalPutsMillis()
         << ", AveragePutMillis="
         << getAveragePutMillis()
         << ", CacheHits="
         << getCacheHits()
         << ", CacheHitsMillis="
         << getCacheHitsMillis()
         << ", AverageHitMillis="
         << getAverageHitMillis()
         << ", CacheMisses="
         << getCacheMisses()
         << ", CacheMissesMillis="
         << getCacheMissesMillis()
         << ", AverageMissMillis="
         << getAverageMissMillis()
         << ", HitProbability="
         << getHitProbability()
         << ", CachePrunes="
         << getCachePrunes()
         << ", CachePrunesMillis="
         << getCachePrunesMillis()
         << ", AveragePruneMillis="
         << getAveragePruneMillis()
         << '}');
     }

COH_CLOSE_NAMESPACE3
