/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "common/TestPofSerializer.hpp"
#include "common/TestSerializer.hpp"
#include "common/TestSerializerXmlConfigurable.hpp"
#include "common/TestUtils.hpp"

using namespace coherence::lang;

/**
* Custom Serializer Test Suite.
*/
class CustomSerializerTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test custom Serializer.
        */
        void testCustomSerializer()
            {
            String::View vsCacheName = "custom-serializer";
            NamedCache::Handle hCache = ensureCleanCache(vsCacheName);
            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            // Custom serializer should have converted string
            TS_ASSERT(vsGet->equals("TestSerializer"));
            }

        /**
        * Test custom XML Configurable Serializer.
        */
        void testCustomSerializerXML()
            {
            String::View vsCacheName = "custom-serializer-xmlconfigurable";
            NamedCache::Handle hCache = ensureCleanCache(vsCacheName);
            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            // Custom serializer should have converted string
            TS_ASSERT(vsGet->equals("TestSerializerXmlConfigurable"));
            }

        /**
        * Test custom POF serializer().
        */
        void testCustomPofSerializer()
            {
            String::View vsCacheName = "custom-pof-serializer";
            NamedCache::Handle hCache = ensureCleanCache(vsCacheName);
            // create a key, and value
            String::View vsKey   = "hello";
            String::View vsValue = "grid";

            // insert the pair into the cache
            hCache->put(vsKey, vsValue);
            // read back the value, casting to the expected value type
            String::View vsGet = cast<String::View>(hCache->get(vsKey));
            // Custom serializer should have converted string
            TS_ASSERT(vsGet->equals("TestPofSerializer"));
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
