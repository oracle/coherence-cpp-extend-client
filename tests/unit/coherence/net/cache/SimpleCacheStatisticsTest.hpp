/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/net/cache/SimpleCacheStatistics.hpp"

using coherence::util::HashMap;
using coherence::net::cache::SimpleCacheStatistics;

class SimpleCacheStatisticsTest : public CxxTest::TestSuite
    {
    public:

    void testGetTotalGets()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getTotalGets() == 0 );

        hStats->registerHit();

        TS_ASSERT(hStats->getTotalGets() == 1 );

        hStats->registerHit();

        TS_ASSERT(hStats->getTotalGets() == 2 );

        hStats->registerMiss();

        TS_ASSERT(hStats->getTotalGets() == 3 );
        }


    void testGetTotalGetsMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getTotalGetsMillis() == 0 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;

        hStats->registerHit(lStartMillis);

        TS_ASSERT(hStats->getTotalGetsMillis() >= 10 );

        hStats->registerMiss(lStartMillis);

        TS_ASSERT(hStats->getTotalGetsMillis() >= 20 );
        }

    void testGetAverageGetMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(Float64::isZero(hStats->getAverageGetMillis()));

        int64_t lStartMillis = System::safeTimeMillis() - 10;

        hStats->registerHit(lStartMillis);
        hStats->registerHit(lStartMillis);
        hStats->registerHit(lStartMillis);
        hStats->registerHit(lStartMillis);

        TS_ASSERT(hStats->getAverageGetMillis() >= 10 );

        }

    void testGetTotalPuts()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getTotalPuts() == 0 );


        int64_t lStartMillis = System::safeTimeMillis() - 10;

        hStats->registerPut(lStartMillis);

        TS_ASSERT(hStats->getTotalPuts() == 1 );

        hStats->registerPut(lStartMillis);

        TS_ASSERT(hStats->getTotalPuts() == 2 );

        hStats->registerPuts(10, lStartMillis);

        TS_ASSERT(hStats->getTotalPuts() == 12 );

        }

    void testGetTotalPutsMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getTotalPutsMillis() == 0 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;

        hStats->registerPut(lStartMillis);

        TS_ASSERT(hStats->getTotalPutsMillis() >= 0 );
        }

    void testGetAveragePutMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(Float64::isZero(hStats->getAveragePutMillis()));

        int64_t lStartMillis = System::safeTimeMillis() - 10;

        hStats->registerPut(lStartMillis);
        hStats->registerPut(lStartMillis);
        hStats->registerPut(lStartMillis);

        TS_ASSERT(hStats->getAveragePutMillis() >= 0 );
        }

    void testGetCacheHits()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getCacheHits() == 0 );

        hStats->registerHit();

        TS_ASSERT(hStats->getCacheHits() == 1 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerHit(lStartMillis);

        TS_ASSERT(hStats->getCacheHits() == 2 );

        }

    void testGetCacheHitsMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getCacheHitsMillis() == 0 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerHit(lStartMillis);

        TS_ASSERT(hStats->getCacheHitsMillis() >= 10 );
        }

    void testGetAverageHitMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(Float64::isZero(hStats->getAverageHitMillis()));

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerHit(lStartMillis);
        hStats->registerHit(lStartMillis);
        hStats->registerHit(lStartMillis);

        TS_ASSERT(hStats->getAverageHitMillis() >= 10 );
        }

    void testGetCacheMisses()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getCacheMisses() == 0 );

        hStats->registerMiss();

        TS_ASSERT(hStats->getCacheMisses() == 1 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerMiss(lStartMillis);

        TS_ASSERT(hStats->getCacheMisses() == 2 );
        }

    void testGetCacheMissesMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(hStats->getCacheMissesMillis() == 0 );

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerMiss(lStartMillis);

        TS_ASSERT(hStats->getCacheMissesMillis() >= 10 );
        }

    void testGetAverageMissMillis()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(Float64::isZero(hStats->getAverageMissMillis()));

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerMiss(lStartMillis);
        hStats->registerMiss(lStartMillis);
        hStats->registerMiss(lStartMillis);

        TS_ASSERT(hStats->getAverageMissMillis() >= 10 );
        }

    void testGetHitProbability()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        TS_ASSERT(Float64::isZero(hStats->getHitProbability()));

        hStats->registerHit();

        TS_ASSERT(Float64::compare(hStats->getHitProbability(), 1.0) == 0);

        hStats->registerHit();

        TS_ASSERT(Float64::compare(hStats->getHitProbability(), 1.0) == 0);

        hStats->registerMiss();
        hStats->registerMiss();

        TS_ASSERT(Float64::compare(hStats->getHitProbability(), .50) == 0 );
        }


    void testResetHitStatistics()
        {
        SimpleCacheStatistics::Handle hStats = SimpleCacheStatistics::create();

        int64_t lStartMillis = System::safeTimeMillis() - 10;
        hStats->registerHit(lStartMillis);
        hStats->registerMiss(lStartMillis);
        hStats->registerPut(lStartMillis);

        TS_ASSERT(hStats->getTotalGetsMillis() != 0 );
        TS_ASSERT(!Float64::isZero(hStats->getAverageGetMillis()));
        TS_ASSERT(hStats->getTotalPuts() != 0 );
        TS_ASSERT(hStats->getTotalPutsMillis() != 0 );
        TS_ASSERT(!Float64::isZero(hStats->getAveragePutMillis()));
        TS_ASSERT(hStats->getCacheHits() != 0 );
        TS_ASSERT(hStats->getCacheHitsMillis() != 0 );
        TS_ASSERT(!Float64::isZero(hStats->getAverageHitMillis()));
        TS_ASSERT(hStats->getCacheMisses() != 0 );
        TS_ASSERT(hStats->getCacheMissesMillis() != 0 );
        TS_ASSERT(!Float64::isZero(hStats->getAverageMissMillis()));
        TS_ASSERT(!Float64::isZero(hStats->getHitProbability()));

        hStats->resetHitStatistics();

        TS_ASSERT(hStats->getTotalGetsMillis() == 0 );
        TS_ASSERT(Float64::isZero(hStats->getAverageGetMillis()));
        TS_ASSERT(hStats->getTotalPuts() == 0 );
        TS_ASSERT(hStats->getTotalPutsMillis() == 0 );
        TS_ASSERT(Float64::isZero(hStats->getAveragePutMillis()));
        TS_ASSERT(hStats->getCacheHits() == 0 );
        TS_ASSERT(hStats->getCacheHitsMillis() == 0 );
        TS_ASSERT(Float64::isZero(hStats->getAverageHitMillis()));
        TS_ASSERT(hStats->getCacheMisses() == 0 );
        TS_ASSERT(hStats->getCacheMissesMillis() == 0 );
        TS_ASSERT(Float64::isZero(hStats->getAverageMissMillis()));
        TS_ASSERT(Float64::isZero(hStats->getHitProbability()));
        }

    };


