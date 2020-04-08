/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "common/TestMapListener.hpp"
#include "common/TestUtils.hpp"

using namespace coherence::lang;

using coherence::io::IOException;
using coherence::net::messaging::ConnectionException;

using common::test::TestMapListener;

/**
* MessageSize Test Suite.
*/
class MessageSizeTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test CacheData.
        */
        void testCacheData()
            {
            NamedCache::Handle hNamedCache = ensureCleanCache("dist-extend", "defaultconfig/extend-cache-config-msg-size.xml");
            HashMap::Handle    hMap        = HashMap::create();

            for (int32_t i = 0; i < 100; i++)
                {
                hMap->put(COH_TO_STRING("key" << i), Integer32::create(i));
                }

            hNamedCache->putAll(hMap);

            setDefaultCacheConfigure();
            }

        /**
        * Test GetLargeData.
        * Retrieve a large set of data from cache. The result message exceeds
        * the max-message-size of the initiator's incoming message handler.
        */
        void testGetLargeData()
            {
            ArrayList::Handle hKeys = ArrayList::create();
            HashMap::Handle   hMap  = HashMap::create();
            for (int32_t i = 0; i < 1000; i++)
                {
                String::View vKey = COH_TO_STRING("key" << i);
                hKeys->add(vKey);
                hMap->put(vKey, Integer32::create(i));
                }

            NamedCache::Handle hNamedCache = ensureCleanCache("dist-extend", "defaultconfig/extend-cache-config-msg-size.xml");
            try
                {
                hNamedCache->putAll(hMap);
                Map::View hEntrySet = hNamedCache->getAll(hKeys);
                TS_FAIL("expected exception");
                }
            catch (ConnectionException::View /* ve */)
                {
                }

            setDefaultCacheConfigure();
            }

        /**
        * Test PutLargeData.
        * Put a large set of data to cache. The request message exceeds the
        * max-message-size of the initiator's outgoing message handler.
        */
        void testPutLargeData()
            {
            HashMap::Handle hMap = HashMap::create();
            for (int32_t i = 0; i < 1500; i++)
                {
                hMap->put(COH_TO_STRING("key" << i), Integer32::create(i));
                }

            NamedCache::Handle hNamedCache = ensureCleanCache("dist-extend", "defaultconfig/extend-cache-config-msg-size.xml");
            try
                {
                hNamedCache->putAll(hMap);
                TS_FAIL("expected exception");
                }
            catch (IOException::View /* ve */)
                {
                }

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
