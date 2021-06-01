/*
 * Copyright (c) 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include <sstream>

using namespace coherence::lang;
using namespace std;

using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::NamedCache;

/**
 * DefaultConfigurableCacheFactory test suite
 */
class DefaultConfigurableCacheFactoryTest : public CxxTest::TestSuite
    {
    public:

        /**
         * COH-23599
         */
        void testConfigThroughEnvironmentVariable()
            {
            String::View vsCacheConfig = System::setProperty("coherence.cacheconfig", "config/system-property-cache-config.xml");
            String::View vsCache       = "foo";

            System::setProperty("test.cachename", vsCache);

            DefaultConfigurableCacheFactory::Handle hCCF = DefaultConfigurableCacheFactory::create();

            hCCF->setConfig(hCCF->getDefaultCacheConfig());

            NamedCache::View vCache = hCCF->ensureCache(vsCache);

            TS_ASSERT(Class::getSimpleClassName(vCache)->equals("LocalNamedCache"));

            hCCF->shutdown();

            // revert the "coherence.cacheconfig" property back to its previous value (or clear it)
            if (NULL == vsCacheConfig)
                {
                System::clearProperty("coherence.cacheconfig");
                }
            else
                {
                System::setProperty("coherence.cacheconfig", vsCacheConfig);
                }
            }
    };
