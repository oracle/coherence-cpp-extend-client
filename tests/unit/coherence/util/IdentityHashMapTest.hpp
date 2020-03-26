/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/IdentityHashMap.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for WeakHashMap.
*/
class IdentityHashMapTest : public CxxTest::TestSuite
    {
    public:
        void testPutGet()
            {
            Map::Handle hMap = IdentityHashMap::create();

            Object::View vKey1 = Integer32::create(123);
            Object::View vKey2 = Integer32::create(123);

            hMap->put(vKey1, vKey1);
            hMap->put(vKey2, vKey2);

            TS_ASSERT(hMap->get(vKey1) == vKey1);
            TS_ASSERT(hMap->get(vKey2) == vKey2);
            TS_ASSERT(hMap->get(vKey1) != vKey2);
            TS_ASSERT(hMap->get(vKey2) != vKey1);
            }

        void testRemove()
            {
            Map::Handle hMap = IdentityHashMap::create();

            Object::View vKey1 = Integer32::create(123);
            Object::View vKey2 = Integer32::create(123);

            hMap->put(vKey1, vKey1);
            hMap->put(vKey2, vKey2);

            TS_ASSERT(hMap->size() == 2);
            TS_ASSERT(hMap->remove(vKey1) == vKey1);
            TS_ASSERT(hMap->size() == 1);
            TS_ASSERT(hMap->get(vKey1) == NULL);

            TS_ASSERT(hMap->remove(vKey2) == vKey2);
            TS_ASSERT(hMap->size() == 0);
            TS_ASSERT(hMap->get(vKey2) == NULL);
            }
    };
