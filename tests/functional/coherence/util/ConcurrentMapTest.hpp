/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ConcurrentMap.hpp"

#include "common/TestUtils.hpp"

using coherence::net::NamedCache;
using coherence::util::ConcurrentMap;


/**
* ConcurrentMap Test Suite.
*/
class ConcurrentMapTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test lock().
        */
        void testLock()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-lock");

            String::View vsKey = "key";

            bool f = hCache->lock(vsKey);
            TS_ASSERT(f == true);

            f = hCache->lock(vsKey);
            TS_ASSERT(f == true);

            f = hCache->unlock(vsKey);
            TS_ASSERT(f == true);
            }

        /**
        * Test lock() with timeout.
        */
        void testLockTimeout()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-lock");

            String::View vsKey = "key";

            bool f = hCache->lock(vsKey, 1000L);
            TS_ASSERT(f == true);

            f = hCache->lock(vsKey, 1000L);
            TS_ASSERT(f == true);

            f = hCache->unlock(vsKey);
            TS_ASSERT(f == true);
            }

        /**
        * Test lock() with global lock.
        */
        void testLockAll()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-lock");

            try
                {
                hCache->lock(ConcurrentMap::getLockAll());
                TS_ASSERT(false);
                }
            catch (...)
                {
                }

            try
                {
                hCache->lock(ConcurrentMap::getLockAll(), 1000L);
                TS_ASSERT(false);
                }
            catch (...)
                {
                }
            }
    };
