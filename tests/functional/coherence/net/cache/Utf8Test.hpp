/*
 * Copyright (c) 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/NamedCache.hpp"

#include "common/TestClasses.hpp"

using coherence::net::CacheFactory;
using coherence::net::NamedCache;
using common::test::TestUtf8Processor;

/**
 * Verify that UTF-8 POF serialization matches that of Java.
 */
class Utf8Test : public CxxTest::TestSuite
    {
    public:
        /**
         * Insert a String with 4 byte UTF-8 encoded characters and then compare the entry
         * using Java POF encoding.
         */
        void test4ByteUtf8()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-cache");

            // create UTF-8 4-byte characters: 0xf0938080, 0xf09f8ebf, 0xf09f8f80, 0xf09f8e89, 0xf09f9294
            char five[21];
            // 0xf0938080
            five[0]  = (char) 0xf0;
            five[1]  = (char) 0x93;
            five[2]  = (char) 0x80;
            five[3]  = (char) 0x80;
            // 0xf09f8ebf
            five[4]  = (char) 0xf0;
            five[5]  = (char) 0x9f;
            five[6]  = (char) 0x8e;
            five[7]  = (char) 0xbf;
            // 0xf09f8f80
            five[8]  = (char) 0xf0;
            five[9]  = (char) 0x9f;
            five[10] = (char) 0x8f;
            five[11] = (char) 0x80;
            // 0xf09f8e89
            five[12] = (char) 0xf0;
            five[13] = (char) 0x9f;
            five[14] = (char) 0x8e;
            five[15] = (char) 0x89;
            // 0xf09f9294
            five[16] = (char) 0xf0;
            five[17] = (char) 0x9f;
            five[18] = (char) 0x92;
            five[19] = (char) 0x94;
            five[20] =        '\0';
            std::string data(five);

            Integer32::View vNid   = Integer32::valueOf(1);
            String::View    vsTest = String::create(data);;

            hCache->put(vNid, vsTest);

            // compare the Java serialized value to the C++ serialized value
            Object::View vResult = hCache->invoke(vNid, TestUtf8Processor::create());

            TS_ASSERT(vResult->equals(Boolean::valueOf(true)));
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
