/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/tests/ConfiguredMapListener.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;

using coherence::tests::ConfiguredMapListener;

/**
* ConfiguredMapListener Test Suite.
*
* Tests the configuration that can be defined:
*
* - map listener instance with and without parameters
* - factory instance methods with and without parameters
* - factory static methods with and without parameters
*
* Each test method gets a specific cache and puts a
* name/value in the cache.  The test listener modifies
* the event, depending on how it was configured.
*
* If the cache is configured with a parameter, the parameter
* is the cache-name, which is passed to the test listener,
* and is returned as the event value.  
*
* If the cache is configured without a parameter, the
* test listener is instantiated with a test string, which 
* is returned as the event value.
*
* @since Coherence 12.1.3
* @author par 10/25/2013
*/
class ConfiguredMapListenerTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test defining a MapListener in the config file, no params.
        */
        void testConfiguredMapListenerInstance()
            {
            String::View vsCacheName = String::create("instance-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = "grid";
            hCache->put(vsKey, vsValue);
          
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals("ConfiguredMapListenerInstanceNoParameters"));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, with parameters.
        */
        void testConfiguredMapListenerInstanceWithParameters()
            {
            String::View vsCacheName = String::create("instance-params-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = "grid";
            hCache->put(vsKey, vsValue);

            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals(vsCacheName));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, using
        * factory method, no parameters.
        */
        void testConfiguredMapListenerFactoryInstance()
            {
            String::View vsCacheName = String::create("factory-instance-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = "grid";
            hCache->put(vsKey, vsValue);

            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals("createConfiguredMapListenerFactoryInstance"));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, using
        * factory method, with parameters.
        */
        void testConfiguredMapListenerFactoryInstanceWithParameters()
            {
            String::View vsCacheName = String::create("factory-instance-params-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = vsCacheName;
            hCache->put(vsKey, vsValue);

            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals(vsCacheName));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, using
        * factory static method, no parameters.
        */
        void testConfiguredMapListenerFactoryStatic()
            {
            String::View vsCacheName = String::create("factory-static-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = "grid";
            hCache->put(vsKey, vsValue);

            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals("createConfiguredMapListenerFactoryStatic"));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
            setDefaultCacheConfigure();
            }

        /**
        * Test defining a MapListener in the config file, using
        * factory static method, with parameters.
        */
        void testConfiguredMapListenerFactoryStaticWithParameters()
            {
            String::View vsCacheName   = String::create("factory-static-params-configured-test");
            NamedCache::Handle hCache 
                    = ensureCleanCache(vsCacheName, "defaultconfig/extend-configured-maplistener-cache-config.xml");

            String::View vsKey   = "hello";
            String::View vsValue = vsCacheName;
            hCache->put(vsKey, vsValue);

            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            TS_ASSERT(vsGet->equals(vsValue));

            TS_ASSERT(ConfiguredMapListener::getEvent()->equals(vsCacheName));

            ConfiguredMapListener::clearEvent();
            CacheFactory::destroyCache(hCache);

            // restore the default cache configure
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
