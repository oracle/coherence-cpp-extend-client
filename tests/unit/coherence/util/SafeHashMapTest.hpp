/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SafeHashMap.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for SafeHashMap.
*/
class SafeHashMapTest : public CxxTest::TestSuite
    {

    class SafeHashMapInfo
        : public class_spec<SafeHashMapInfo,
            extends<SafeHashMap> >
        {
        friend class factory<SafeHashMapInfo>;

        // constructor ------------------------------------------------------
        protected:

            SafeHashMapInfo(size32_t cInitialBuckets = 17,
                    float32_t flLoadFactor = 1.0F, float32_t flGrowthRate = 3.0F)
                : super(cInitialBuckets, flLoadFactor, flGrowthRate)
                {
                }

        public:
            size32_t getCapacity()
                {
                return m_cCapacity;
                }
        };


    public:
        void testCreate()
            {
            SafeHashMap::Handle hMap = SafeHashMap::create();
            TS_ASSERT(hMap != NULL);
            }

        void testPut()
            {
            SafeHashMap::Handle hMap   = SafeHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->put(vKey, NULL) == hValue);
            }

        void testGet()
            {
            SafeHashMap::Handle hMap   = SafeHashMap::create();
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
            SafeHashMap::Handle hMap   = SafeHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->containsKey(vKey) == false);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->containsKey(vKey) == true);
            }

        void testSize()
            {
            SafeHashMap::Handle hMap   = SafeHashMap::create();
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
            SafeHashMap::Handle hMap   = SafeHashMap::create();
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
            SafeHashMap::Handle hMap   = SafeHashMap::create();
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
            SafeHashMap::Handle hMap   = SafeHashMap::create();
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

        void testClone()
            {
            SafeHashMap::Handle hMap   = SafeHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            SafeHashMap::Handle hClone = cast<SafeHashMap::Handle>(hMap->clone());

            Collection::Handle hColl = hMap->values();
            Collection::Handle hCollClone = hClone->values();

            TS_ASSERT(hColl->containsAll(hCollClone));
            TS_ASSERT(hCollClone->containsAll(hColl));
            }

        void testGrow()
            {
            SafeHashMapInfo::Handle hMap = SafeHashMapInfo::create(3, 1.0F, 1.0F);

            Integer32::Handle h1 = Integer32::create(1);
            Integer32::Handle h2 = Integer32::create(2);
            Integer32::Handle h3 = Integer32::create(3);
            Integer32::Handle h4 = Integer32::create(4);

            TS_ASSERT(hMap->getCapacity() == 3);
            hMap->put(h1,h1);
            hMap->put(h2,h2);
            hMap->put(h3,h3);

            TS_ASSERT(hMap->getCapacity() == 3);

            // Force the hashmap to copy the entries while growing
            Iterator::Handle hIter = hMap->entrySet()->iterator();
            int x = 0;
            TS_ASSERT(hIter->hasNext());
            hIter->next();
            x++;

            // Force the map to grow
            hMap->put(h4,h4);

            // prove that teh map grew
            TS_ASSERT(hMap->getCapacity() == 17);

            // prove that we can finish iterating
            while(hIter->hasNext())
                {
                hIter->next();
                x++;
                }
            TS_ASSERT(x == 3);

            // Prove that our insert worked
            TS_ASSERT(hMap->size() == 4);

            // Make sure the map still holds on to its original entries
            TS_ASSERT(hMap->get(h1) == h1);
            TS_ASSERT(hMap->get(h2) == h2);
            TS_ASSERT(hMap->get(h3) == h3);
            TS_ASSERT(hMap->get(h4) == h4);
            }

        void testGetOrDefault()
            {
            SafeHashMap::Handle hMap   = SafeHashMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->get(vKey) == NULL);
            Object::Holder ohObject = hMap->getOrDefault(vKey, hValue);
            TS_ASSERT(ohObject->equals(hValue));
            }

        void testPutIfAbsent()
            {
            SafeHashMap::Handle hMap    = SafeHashMap::create();
            Integer32::View     vKey    = Integer32::create(123);
            Integer32::Handle   hValue1 = Integer32::create(456);
            Integer32::Handle   hValue2 = Integer32::create(789);

            TS_ASSERT(hMap->get(vKey) == NULL);

            Object::Holder ohObject = hMap->putIfAbsent(vKey, hValue1);
            TS_ASSERT(ohObject == NULL);
            TS_ASSERT(hMap->get(vKey)->equals(hValue1));

            ohObject = hMap->putIfAbsent(vKey, hValue2);
            TS_ASSERT(ohObject->equals(hValue1));
            }

        void testRemove()
            {
            SafeHashMap::Handle hMap  = SafeHashMap::create();

            Integer32::View   vKey    = Integer32::create(123);
            Integer32::View   vValue1 = Integer32::create(456);
            Integer32::View   vValue2 = Integer32::create(789);

            hMap->put(vKey, vValue2);

            TS_ASSERT(!hMap->remove(vKey, vValue1));
            TS_ASSERT(hMap->remove(vKey, vValue2));
            }

        void testReplaceOldWithNew()
            {
            SafeHashMap::Handle hMap    = SafeHashMap::create();

            Integer32::View     vKey    = Integer32::create(123);
            Integer32::Handle   hValue1 = Integer32::create(456);
            Integer32::Handle   hValue2 = Integer32::create(789);

            hMap->put(vKey, hValue1);     
            TS_ASSERT(hMap->replace(vKey, hValue1, hValue2));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));

            hMap->put(vKey, hValue2);
            TS_ASSERT(!hMap->replace(vKey, hValue1, hValue2));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));
            }

        void testReplaceWithValue()
            {
            SafeHashMap::Handle hMap    = SafeHashMap::create();
            Integer32::View     vKey    = Integer32::create(123);
            Integer32::Handle   hValue1 = Integer32::create(456);
            Integer32::Handle   hValue2 = Integer32::create(789);

            TS_ASSERT(hMap->replace(vKey, hValue1) == NULL);

            hMap->put(vKey, hValue1);
            TS_ASSERT(hMap->replace(vKey, hValue2)->equals(hValue1));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));
            }

    };
