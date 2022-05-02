/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "mock/CommonMocks.hpp"

#include "coherence/net/cache/CacheEvent.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableMap.hpp"

using namespace std;
using namespace coherence::util;
using namespace mock;

class CacheEventTest : public CxxTest::TestSuite
    {
    public:

        void testIsSynthetic()
            {
            CacheEvent::View vCache = CacheEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"), false);
            TS_ASSERT(!vCache->isSynthetic());

            vCache = CacheEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"), true);
            TS_ASSERT(vCache->isSynthetic());
            }

        void testIsExpired()
            {
            CacheEvent::View vCache = CacheEvent::create(
                TestObservableMap::create(), 1, String::create("key"),
                String::create("old"), String::create("new"), false);
            TS_ASSERT(!vCache->isExpired());

            vCache = CacheEvent::create(
                TestObservableMap::create(), 1, String::create("key"),
                String::create("old"), String::create("new"), false, CacheEvent::transformable, false, false);
            TS_ASSERT(!vCache->isExpired());

            vCache = CacheEvent::create(
                TestObservableMap::create(), 1, String::create("key"),
                String::create("old"), String::create("new"), false, CacheEvent::transformable, false, true);
            TS_ASSERT(vCache->isExpired());
            }

        void testGetDescription()
            {
            CacheEvent::View vCache = CacheEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"), false);
            TS_ASSERT(vCache->getDescription()->indexOf(" added: key=key, value=new") != String::npos);

            vCache = CacheEvent::create(
                    TestObservableMap::create(), 1, String::create("key"),
                    String::create("old"), String::create("new"), true);
            TS_ASSERT(vCache->getDescription()->indexOf(" added: key=key, value=new, synthetic") != String::npos);
            }

    };


