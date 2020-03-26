/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/WeakHashMap.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for WeakHashMap.
*/
class WeakHashMapTest : public CxxTest::TestSuite
    {
    public:
        void testCreate()
            {
            WeakHashMap::Handle hMap = WeakHashMap::create();
            TS_ASSERT(hMap != NULL);
            }

        void testPut()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->put(vKey, NULL) == hValue);
            }

        void testGet()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->get(vKey) == NULL);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->get(vKey) == hValue);
            TS_ASSERT(cast<Integer32::Handle>(hMap->get(vKey)) == hValue);
            TS_ASSERT(hMap->put(vKey, NULL) == hValue);
            TS_ASSERT(hMap->get(vKey) == NULL);
            }

        void testContainsKey()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->containsKey(vKey) == false);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->containsKey(vKey) == true);
            }

        void testSize()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);

            TS_ASSERT(hMap->size() == 1);
            hMap->remove(NULL);
            TS_ASSERT(hMap->size() == 1);
            hMap->remove(vKey);
            TS_ASSERT(hMap->size() == 0);

            hMap->put(vKey, hValue);
            hMap->clear();
            TS_ASSERT(hMap->size() == 0);
            }

        void testEntrySet()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            Set::Handle hSet = hMap->entrySet();

            TS_ASSERT(hSet->size() == 2);
            int32_t c = 0;
            for (Iterator::Handle hIter = hSet->iterator(); hIter->hasNext(); ++c)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(vEntry != NULL);
                TS_ASSERT(hSet->contains(vEntry));
                Object::View vKeyEntry = vEntry->getKey();
                if (vKeyEntry == vKey)
                    {
                    TS_ASSERT(vEntry->getValue() == hValue);
                    }
                else
                    {
                    TS_ASSERT(vKeyEntry == hValue)
                    TS_ASSERT(vEntry->getValue() == vKey);
                    }

                // test mutation propogation
                hMap->put(vKeyEntry, vKeyEntry);
                TS_ASSERT(vEntry->getValue() == vKeyEntry);

                TS_ASSERT(hSet->remove(vEntry));
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            c = 0;
            for (Muterator::Handle hIter = hSet->iterator(); hIter->hasNext(); ++c)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(vEntry != NULL);
                TS_ASSERT(hSet->contains(vEntry));
                TS_ASSERT(hMap->containsKey(vEntry->getKey()));

                // test mutation via muterator
                hIter->remove();
                TS_ASSERT(!hSet->contains(vEntry));
                TS_ASSERT(!hMap->containsKey(vEntry->getKey()));
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());


            try
                {
                hSet->add(vKey);
                TS_ASSERT(false);
                }
            catch (UnsupportedOperationException::View)
                {
                }
            }

        void testKeySet()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            Set::Handle hSet = hMap->keySet();

            TS_ASSERT(hSet->size() == 2);
            int32_t c = 0;
            for (Iterator::Handle hIter = hSet->iterator(); hIter->hasNext(); ++c)
                {
                Object::View vKeyEntry = hIter->next();
                TS_ASSERT(vKeyEntry != NULL);
                TS_ASSERT(hSet->contains(vKeyEntry));

                // test mutation
                TS_ASSERT(hSet->remove(vKeyEntry));
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);
            TS_ASSERT(hSet->size() == 2);

            c = 0;
            for (Muterator::Handle hIter = hSet->iterator(); hIter->hasNext(); ++c)
                {
                Object::View vKeyEntry = hIter->next();
                TS_ASSERT(vKeyEntry != NULL);
                TS_ASSERT(hSet->contains(vKeyEntry));
                TS_ASSERT(hMap->containsKey(vKeyEntry));

                // test mutation via muterator
                hIter->remove();
                TS_ASSERT(!hSet->contains(vKeyEntry));
                TS_ASSERT(!hMap->containsKey(vKeyEntry));
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());

            try
                {
                hSet->add(vKey);
                TS_ASSERT(false);
                }
            catch (UnsupportedOperationException::View)
                {
                }
            }

        void testValues()
            {
            WeakHashMap::Handle hMap   = WeakHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            Collection::Handle hColl = hMap->values();

            TS_ASSERT(hColl->size() == 2);
            int32_t c = 0;
            for (Iterator::Handle hIter = hColl->iterator(); hIter->hasNext(); ++c)
                {
                Object::View vValueEntry = hIter->next();
                TS_ASSERT(vValueEntry != NULL);
                TS_ASSERT(hColl->contains(vValueEntry));

                // test mutation
                if (vValueEntry == hValue)
                    {
                    TS_ASSERT(hMap->remove(vKey) == hValue);
                    }
                else
                    {
                    TS_ASSERT(hMap->remove(hValue) == vKey);
                    }
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            c = 0;
            for (Muterator::Handle hIter = hColl->iterator(); hIter->hasNext(); ++c)
                {
                Object::View vValueEntry = hIter->next();
                TS_ASSERT(vValueEntry != NULL);
                TS_ASSERT(hColl->contains(vValueEntry));

                // test mutation via muterator
                hIter->remove();
                }
            TS_ASSERT(c == 2);
            TS_ASSERT(hMap->isEmpty());


            try
                {
                hColl->add(hValue);
                TS_ASSERT(false);
                }
            catch (UnsupportedOperationException::View)
                {
                }
            }

        void testRelease()
            {
            WeakHashMap::Handle hMap = WeakHashMap::create();
            Object::View vKey;
            {
            vKey = Integer32::create(123);
            Object::View vVal = Integer32::create(456);

            hMap->put(vKey, vVal);

            TS_ASSERT(hMap->get(vKey) == vVal);
            TS_ASSERT(hMap->get(Integer32::create(123)) == vVal);

            vKey = Integer32::create(123);
            }

            TS_ASSERT(hMap->containsKey(vKey) == false);
            TS_ASSERT(hMap->get(vKey) == NULL);
            }

        void testReclaim()
            {
            WeakHashMap::Handle hMap    = WeakHashMap::create();
            for (size32_t i = 0; i < 1000; ++i)
                {
                hMap->put(Integer32::create(i), NULL);
                }

            // all keys are reclaimable, so we if were to insert another
            // big batch of keys, we should have somewhere between 0 and the
            // new batch size
            for (size32_t i = 1000; i < 2000; ++i)
                {
                hMap->put(Integer32::create(i), NULL);
                }

            TS_ASSERT(hMap->size() <= 1000);
            }
    };
