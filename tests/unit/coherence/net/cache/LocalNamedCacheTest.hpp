/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "private/coherence/net/cache/LocalNamedCache.hpp"

#include "common/TestListener.hpp"
#include "common/TestEvenFilter.hpp"

using coherence::util::comparator::EntryComparator;
using coherence::util::ValueManipulator;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::processor::NumberIncrementor;

class LocalNamedCacheSuite : public CxxTest::TestSuite
    {
    public:
        void testSetGetCacheName()
            {
            LocalNamedCache::Handle hCache = LocalNamedCache::create();

            String::View vsName = String::create("local-foo");
            hCache->setCacheName(vsName);
            TS_ASSERT(hCache->getCacheName() == vsName);
            }
        void testGetCacheService()
            {
            LocalNamedCache::Handle hCache = LocalNamedCache::create();
            TS_ASSERT(hCache->getCacheService() == NULL);
            }
        void testIsActive()
            {
            LocalNamedCache::Handle hCache = LocalNamedCache::create();
            TS_ASSERT(hCache->isActive() == true);
            hCache->release();
            TS_ASSERT(hCache->isActive() == false);
            hCache = LocalNamedCache::create();
            TS_ASSERT(hCache->isActive() == true);
            hCache->destroy();
            TS_ASSERT(hCache->isActive() == false);
            }
        void testRelease()
            {
            LocalNamedCache::Handle hCache = LocalNamedCache::create();
            Integer32::Handle hI = Integer32::create(1);
            hCache->release();
            TS_ASSERT_THROWS(hCache->put(hI, hI), IllegalStateException::View);
            }
        void testDestroy()
            {
            LocalNamedCache::Handle hCache = LocalNamedCache::create();
            Integer32::Handle hI = Integer32::create(1);
            hCache->destroy();
            TS_ASSERT_THROWS(hCache->put(hI, hI), IllegalStateException::View);
            }
    };
