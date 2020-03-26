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
#include "coherence/net/ConfigurableCacheFactory.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/util/Map.hpp"
#include "private/coherence/component/net/extend/RemoteCacheService.hpp"
#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"
#include "private/coherence/component/util/SafeCacheService.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/net/LocalMember.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::net::CacheFactory;
using coherence::net::CacheService;
using coherence::net::ConfigurableCacheFactory;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::NamedCache;
using coherence::net::ServiceInfo;
using coherence::net::internal::ScopedReferenceStore;
using coherence::util::Map;
using coherence::component::net::extend::RemoteCacheService;
using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::util::SafeCacheService;
using coherence::component::util::SafeNamedCache;
using coherence::net::LocalMember;

/**
* DefaultConfigurableCacheFactory Test Suite.
*/
class DefaultConfigurableCacheFactoryTest : public CxxTest::TestSuite
    {
    // ----- test fixtures --------------------------------------------------

    public:
        /**
         * Runs after each test
         */
        void tearDown()
            {
            CacheFactory::shutdown();
            }


    public:
        /**
        * Test initialization().
        */
        void testInitialization()
            {
            DefaultConfigurableCacheFactory::Handle hCCF =
                    cast<DefaultConfigurableCacheFactory::Handle>(
                            CacheFactory::getConfigurableCacheFactory());
            TS_ASSERT(hCCF->ensureOperationalContext() != NULL);
            TS_ASSERT(hCCF->ensureOperationalContext()->getLocalMember() != NULL);
            TS_ASSERT(hCCF->ensureOperationalContext()->getEdition() == 1);
            }

        /**
        * Test find scheme mapping.
        */
        void testFindSchemeMapping()
            {
            ConfigurableCacheFactory::Handle hccf = CacheFactory::getConfigurableCacheFactory();
            DefaultConfigurableCacheFactory::Handle dccf = cast<DefaultConfigurableCacheFactory::Handle>(hccf);
            DefaultConfigurableCacheFactory::CacheInfo::View vci = dccf->findSchemeMapping("dist-2343535");
            TS_ASSERT(vci != NULL);
            TS_ASSERT(vci->getCacheName()->equals("dist-2343535"));
            TS_ASSERT(vci->getSchemeName()->equals("extend-dist"));

            vci = dccf->findSchemeMapping("local-test");
            TS_ASSERT(vci != NULL);
            TS_ASSERT(vci->getCacheName()->equals("local-test"));
            TS_ASSERT(vci->getSchemeName()->equals("local-example"));
            }

        // TODO: add test for instancing local named cache as of the moment
        //       local named cache is implemented.
        ///**
        //* Test instancing local named cache.
        //*/
        //public void TestLocalNamedCacheInstancing()
        //{
        //}


        /**
        * Test instancing remote named cache.
        */
        void testRemoteNamedCacheInstancing()
            {
            ConfigurableCacheFactory::Handle hccf = CacheFactory::getConfigurableCacheFactory();
            NamedCache::Handle hcache = hccf->ensureCache("dist-extend-direct");
            TS_ASSERT(hcache != NULL);
            TS_ASSERT(instanceof<SafeNamedCache::Handle>(hcache));

            SafeNamedCache::Handle hSafeCache = cast<SafeNamedCache::Handle>(hcache);
            TS_ASSERT(hSafeCache->getCacheName()->equals("dist-extend-direct"));

            TS_ASSERT(instanceof<RemoteNamedCache::Handle>(hSafeCache->getNamedCache()));
            RemoteNamedCache::Handle hRemoteCache = cast<RemoteNamedCache::Handle>(hSafeCache->getNamedCache());
            TS_ASSERT(hRemoteCache->getCacheName()->equals(hSafeCache->getCacheName()));
            TS_ASSERT(hSafeCache->getCacheService() != NULL);
            TS_ASSERT(instanceof<CacheService::Handle>(hSafeCache->getCacheService()));
            SafeCacheService::Handle hSafeCacheService = cast<SafeCacheService::Handle>(hSafeCache->getCacheService());
            TS_ASSERT(hSafeCacheService->getCacheNames()->size() == 1);
            TS_ASSERT(hSafeCacheService->getCacheNames()->contains(hSafeCache->getCacheName()));

            ScopedReferenceStore::View hStore = hSafeCacheService->getNamedCacheStore();
            TS_ASSERT(hStore->getAllCaches()->size() == 1);
            TS_ASSERT(hStore->getNames()->contains(hSafeCache->getCacheName()));
            TS_ASSERT(hSafeCacheService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(instanceof<RemoteCacheService::View>(hSafeCacheService->getCacheService()));
            TS_ASSERT(hRemoteCache->getCacheService()->equals(hSafeCacheService->getCacheService()));
            RemoteCacheService::View hRemoteService = cast<RemoteCacheService::View>(hSafeCacheService->getCacheService());
            TS_ASSERT(hRemoteService->getServiceName()->equals("ExtendTcpCacheService"));
            }
    };
