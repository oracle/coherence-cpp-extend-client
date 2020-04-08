/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/CacheService.hpp"
#include "coherence/net/NamedCache.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::net::CacheFactory;
using coherence::net::CacheService;
using coherence::net::NamedCache;

/**
* RemoteNamedCache test suite.
*/
class LoadBalancerTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test LoadBalancer.
        */
        void testLoadBalancer()
            {
            CacheService::Handle hService1 = cast<CacheService::Handle> 
                    (CacheFactory::getService("ExtendTcpCacheService"));

            // short sleep to allow load state to propagate
            Thread::sleep(1000);

            CacheService::Handle hService2 = cast<CacheService::Handle> 
                    (CacheFactory::getService("ExtendTcpCacheService2"));

            String::View vsCacheName = "local-test";
            NamedCache::Handle hCache1 = hService1->ensureCache(vsCacheName);
            NamedCache::Handle hCache2 = hService2->ensureCache(vsCacheName);

            hCache1->clear();
            hCache2->clear();

            // create a key, and value
            String::View vsKey   = "key";
            String::View vsValue = "value";

            // insert the pair into the hCache1
            hCache1->put(vsKey, vsValue);

            TS_ASSERT(hCache2->containsKey(vsKey) == false);
            }
    };
