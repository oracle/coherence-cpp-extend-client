/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::NamedCache;


/**
* Default Serializer Test Suite.
*/
class DefaultSerializerTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Default Serializer.
        */
        void testDefaultPofSerializerCache()
            {
            String::View vsCacheName   = String::create("dist-default");
            String::View vsCacheConfig = String::create("defaultconfig/extend-default-serializer-cache-config.xml");

            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
            NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
            hCache->clear();

            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            CacheFactory::releaseCache(hCache);

            // pof serializer should return known string
            TS_ASSERT(vsGet->equals("TestPofSerializer"));

            setDefaultCacheConfigure();
            }

        /**
        * Test named Default Serializer.
        */
        void testNamedDefaultSerializerCache()
            {
            String::View vsCacheName = String::create("dist-default");
            String::View vsCacheConfig = String::create("defaultconfig/extend-named-default-serializer-cache-config.xml");

            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
            NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
            hCache->clear();

            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            CacheFactory::releaseCache(hCache);

            // pof serializer should return known string
            TS_ASSERT(vsGet->equals("TestPofSerializer"));

            setDefaultCacheConfigure();
            }

        /**
        * Test service, no serializer configured on the service,
        * see if non-pof default is applied.  This test uses a service
        * that connects to a proxy configured with a non-POF serializer.
        */
        void testDefaultSerializerService()
            {
            String::View vsCacheConfig = String::create("defaultconfig/extend-default-notpof-serializer-cache-config.xml");
            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));

            InvocationService::Handle hService = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));

            Member::View vMember =
                    cast<DefaultConfigurableCacheFactory::Handle>
                    (CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext()->getLocalMember();

            IncrementorInvocable::Handle hInvocable = IncrementorInvocable::create(1);

            Map::View vMap = hService->query(hInvocable, NULL);
            TS_ASSERT(vMap->size() == 1);

            Integer32::View vI = cast<Integer32::View>(vMap->get(vMember));

            TS_ASSERT_EQUALS(vI->getInt32Value(), 2);

            setDefaultCacheConfigure();
            }

        /**
        * Test well-known named pof Default Serializer.
        */
        void testDefaultNamedPofSerializerCache()
            {
            String::View vsCacheName = String::create("dist-default");
            String::View vsCacheConfig = String::create("defaultconfig/extend-named-pof-default-serializer-cache-config.xml");

            CacheFactory::configure(CacheFactory::loadXmlFile(vsCacheConfig));
            NamedCache::Handle hCache = CacheFactory::getCache(vsCacheName);
            hCache->clear();

            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);

            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            CacheFactory::releaseCache(hCache);

            // pof serializer should return known string
            TS_ASSERT(vsGet->equals("grid"));

            setDefaultCacheConfigure();
            }

        /**
        * Clean up after the tests - Sunpro compiler does not like cxxtest
        * createSuite() and destroySuite() methods so need to do it this way
        */
        void testCleanup()
            {
            CacheFactory::shutdown();
            }
    };
