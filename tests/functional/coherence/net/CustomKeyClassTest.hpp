/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CUSTOM_KEY_CLASS_TEST
#define COH_CUSTOM_KEY_CLASS_TEST

#include "coherence/lang.ns"

#include "cxxtest/TestSuite.h"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/tests/CustomKeyClass.hpp"

#include "RemoteNamedCacheTest.hpp"

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::NamedCache;

using coherence::tests::CustomKeyClass;



/**
* A collection of functional tests for Coherence*Extend that go through the
* RemoteNamedCacheTest tests using a custom key class which is defined at the
* extend client but not at the PartitionedCache.
*
* Class header needed for test generation perl script:
*
* class CustomKeyClassTest : public CxxTest::TestSuite
*
* @author phf 2001.09.06
*/
class CustomKeyClassTest : public RemoteNamedCacheTest
    {
    // ----- RemoteNamedCacheTest interface ---------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getKeyObject(Object::View v) const
            {
            // use a key class which is only defined in the extend client to
            // verify that the PartitionedCache does not deserialize the key
            return CustomKeyClass::create(v);
            }


    // ----- RemoteNamedCacheTest tests -------------------------------------

    public:
        /**
        * Add test method names - needed for perl test generation script
        *
        * void testInitialize()
        * void testNamedCacheProperties()
        * void testNamedCacheMethods()
        * void testNamedCacheKeysCollections()
        * void testNamedCacheValuesCollections()
        * void testNamedCacheEntryCollection()
        * void testNamedCacheIndex()
        * void testNamedCacheGetAllWithSameKeys()
        * void testCacheTriggerListener()
        * void testNamedCacheKeySet()
        * void testNamedCacheEntrySet()
        * void testNamedCacheExpiry()
        * void testNamedCacheSynchronousListeners()
        * void testNamedCacheMapListeners()
        * void testPortableException()
        */


    // ----- CustomKeyClassTest tests ---------------------------------------

    public:
        /**
        * Verify that put with a simple key class succeeds but with a custom
        * key class throws an exception when the defer-key-association-check
        * element is set to true.
        */
        void testDeferKeyAssociationCheck()
            {
            NamedCache::Handle cache = ensureCleanCache("defer-key-association-check");

            cache->put(String::create("key"), String::create("value"));
            TS_ASSERT_THROWS(
                    cache->put(CustomKeyClass::create(String::create("key")),
                            String::create("value")),
                    Exception::View);
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

#endif // COH_CUSTOM_KEY_CLASS_TEST
