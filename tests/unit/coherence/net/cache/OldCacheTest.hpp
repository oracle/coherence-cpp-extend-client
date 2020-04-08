/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/cache/OldCache.hpp"

#include "coherence/util/ArrayList.hpp"

#include "common/TestListener.hpp"

class OldCacheSuite : public CxxTest::TestSuite
    {
    public:
        void testKeyListener()
            {
            OldCache::Handle     hCache    = OldCache::create();
            TestListener::Handle hListener = TestListener::create();
            Integer32::Handle    hInt1     = Integer32::create(1);
            Integer32::Handle    hInt2     = Integer32::create(2);

            // Test that we heard an event for hInt1;
            hCache->addKeyListener(hListener, hInt1, false);
            hCache->put(hInt1, hInt1);
            MapEvent::View vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == NULL);
            TS_ASSERT(vEvent->getNewValue() == hInt1);
            hListener->setEvent(NULL);

            TS_ASSERT(hCache->get(hInt1) == hInt1);

            // Test that we hear an update for hInt1;
            hCache->put(hInt1, hInt2);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == hInt1);
            TS_ASSERT(vEvent->getNewValue() == hInt2);
            hListener->setEvent(NULL);

            // Test that we hear an event removing hInt1
            hCache->remove(hInt1);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == hInt2);
            TS_ASSERT(vEvent->getNewValue() == NULL);
            hListener->setEvent(NULL);

            // Test that we don't hear an event for hInt2;
            hCache->put(hInt2, hInt2);
            vEvent = hListener->getEvent();
            TS_ASSERT(hListener->getEvent() == NULL);

            // Remove the event and ensure we no longer see updates
            hCache->removeKeyListener(hListener, hInt1);
            hCache->put(hInt1, hInt1);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL == vEvent);
            }

        void testFilterListener()
            {
            OldCache::Handle     hCache    = OldCache::create();
            TestListener::Handle hListener = TestListener::create();
            Integer32::Handle    hInt1     = Integer32::create(1);
            Integer32::Handle    hInt2     = Integer32::create(2);

            // Test that we heard an event for hInt1;
            hCache->addFilterListener(hListener);
            hCache->put(hInt1, hInt1);
            MapEvent::View vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == NULL);
            TS_ASSERT(vEvent->getNewValue() == hInt1);
            hListener->setEvent(NULL);

            // Test that we hear an update for hInt1;
            hCache->put(hInt1, hInt2);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == hInt1);
            TS_ASSERT(vEvent->getNewValue() == hInt2);
            hListener->setEvent(NULL);

            // Test that we hear an event removing hInt1
            hCache->remove(hInt1);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == hInt2);
            TS_ASSERT(vEvent->getNewValue() == NULL);
            hListener->setEvent(NULL);

            // Test that we hear an event for hInt2;
            hCache->put(hInt2, hInt2);
            vEvent = hListener->getEvent();
            TS_ASSERT(NULL != vEvent);
            TS_ASSERT(vEvent->getOldValue() == NULL);
            TS_ASSERT(vEvent->getNewValue() == hInt2);
            hListener->setEvent(NULL);
            }

        void testclear()
            {
            OldCache::Handle hCache = OldCache::create();
            for (size32_t x = 0; x < 10; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            TS_ASSERT(hCache->size() == 10);
            hCache->clear();
            TS_ASSERT(hCache->size() == 0);
            TS_ASSERT(hCache->get(Integer32::create(1)) == NULL);
            }

        void testContainsKey()
            {
            OldCache::Handle hCache = OldCache::create();
            Integer32::Handle hI = Integer32::create(1);
            hCache->put(hI, hI);

            TS_ASSERT(hCache->containsKey(hI) == true);
            }

        void testEvict()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);
            Integer32::Handle hI2    = Integer32::create(2);
            hCache->setExpiryDelay(50);
            hCache->put(hI, hI);
            Thread::sleep(100);
            hCache->setExpiryDelay(3600000);
            hCache->put(hI2, hI2);
            hCache->evict();
            TS_ASSERT(hCache->size() == 1);
            TS_ASSERT(hCache->get(hI) == NULL);
            TS_ASSERT(hCache->get(hI2) == hI2);
            }

        void testEvictKey()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            hCache->evict(hI);
            TS_ASSERT(hCache->size() == 0);
            TS_ASSERT(hCache->get(hI) == NULL);
            }

        void testEvictAll()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);
            Integer32::Handle hI2    = Integer32::create(2);
            hCache->put(hI, hI);
            hCache->put(hI2, hI2);

            ArrayList::Handle hList = ArrayList::create();
            hList->add(hI);
            hList->add(hI2);

            hCache->evictAll(hList);
            TS_ASSERT(hCache->size() == 0);
            TS_ASSERT(hCache->get(hI) == NULL);
            TS_ASSERT(hCache->get(hI2) == NULL);
            }

        void testGet()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(hCache->get(hI) == hI)
            TS_ASSERT(hCache->get(Integer32::create(2)) == NULL);
            }

        void testGetCacheHits()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(hCache->getCacheHits() == 0);

            for(int x = 0; x < 15; ++x)
                {
                hCache->get(hI);
                }
            TS_ASSERT(hCache->getCacheHits() == 15);
            }

        void testGetCacheMisses()
            {
            OldCache::Handle  hCache = OldCache::create();

            TS_ASSERT(hCache->getCacheMisses() == 0);
            for(int x = 0; x < 15; ++x)
                {
                hCache->get(NULL);
                }

            TS_ASSERT(hCache->getCacheMisses() == 15);
            }

        void testHitProbability()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(Float64::compare(hCache->getHitProbability(), 0) == 0);

            for(int x = 0; x < 8; ++x)
                {
                hCache->get(hI);
                }
            hCache->get(NULL);
            hCache->get(NULL);
            TS_ASSERT(Float64::compare(hCache->getHitProbability(), 0.8) == 0);
            }

        void testGetUnits()
            {
            OldCache::Handle  hCache = OldCache::create();
            for(int x = 0; x < 25; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            TS_ASSERT(hCache->getUnits() == 25);
            }

        void testPut()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(hCache->get(hI) == hI);
            }

        void testPutMillis()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI, 50);
            TS_ASSERT(hCache->get(hI) == hI);
            Thread::sleep(100);
            TS_ASSERT(hCache->get(hI) == NULL);
            }

        void testRemove()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(hCache->get(hI) == hI);
            hCache->remove(hI);
            TS_ASSERT(hCache->get(hI) == NULL);
            }

        void testResetCacheStatistics()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);

            hCache->put(hI, hI);
            TS_ASSERT(Float64::compare(hCache->getHitProbability(), 0) == 0);

            for(int x = 0; x < 8; ++x)
                {
                hCache->get(hI);
                }
            hCache->get(NULL);
            hCache->get(NULL);
            TS_ASSERT(Float64::compare(hCache->getHitProbability(), 0.8) == 0);
            TS_ASSERT(hCache->getCacheHits() == 8);
            TS_ASSERT(hCache->getCacheMisses() == 2);

            hCache->resetHitStatistics();
            TS_ASSERT(Float64::compare(hCache->getHitProbability(), 0) == 0);
            TS_ASSERT(hCache->getCacheHits() == 0);
            TS_ASSERT(hCache->getCacheMisses() == 0);
            }

        void testSetExpiryAndFlushDelay()
            {
            OldCache::Handle  hCache = OldCache::create();
            Integer32::Handle hI     = Integer32::create(1);
            Integer32::Handle hI2    = Integer32::create(2);
            hCache->setExpiryDelay(50);
            hCache->put(hI, hI);
            hCache->setFlushDelay(10);
            Thread::sleep(100);
            TS_ASSERT(hCache->size() == 0);
            }

        void testCacheSizeLimit()
            {
            OldCache::Handle hCache = OldCache::create(100,
                    3600000, 1.0F);

            for(int x = 0; x < 100; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            TS_ASSERT(hCache->size() == 100);

            for(int x = 100; x < 200; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);
                hCache->put(hI, hI);
                }

            TS_ASSERT(hCache->size() <= 100);
            }

        void testRelease()
            {
            HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
                {
                OldCache::Handle hCache = OldCache::create();
                hCache->put(Object::create(), Object::create());
                hCache->release();
                }
            HeapAnalyzer::ensureHeap(vSnap);
            }
    };
