/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/Hashtable.hpp"

#include "private/coherence/util/logging/Logger.hpp"

using namespace coherence::lang;
using namespace coherence::util;

using coherence::util::logging::Logger;

/**
* Test Suite for Hashtable.
*/
class HashtableTest : public CxxTest::TestSuite
    {
    public:
        void testCreate()
            {
            Hashtable::Handle hMap = Hashtable::create();
            TS_ASSERT(hMap != NULL);
            }

        void testPut()
            {
            Hashtable::Handle hMap   = Hashtable::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->put(vKey, NULL) == hValue);
            }

        void testGet()
            {
            Hashtable::Handle hMap   = Hashtable::create();
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
            Hashtable::Handle hMap   = Hashtable::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->containsKey(vKey) == false);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->containsKey(vKey) == true);
            }

        void testRemove()
            {
            Hashtable::Handle hMap   = Hashtable::create();
            Integer32::View   vKey   = Integer32::create(123);
            Integer32::Handle hValue = Integer32::create(456);

            TS_ASSERT(hMap->containsKey(vKey) == false);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->remove(vKey) == hValue);
            TS_ASSERT(hMap->isEmpty());
            TS_ASSERT(hMap->get(vKey) == NULL);
            TS_ASSERT(hMap->put(vKey, hValue) == NULL);
            TS_ASSERT(hMap->size() == 1);
            TS_ASSERT(hMap->get(vKey) != NULL);
            TS_ASSERT(hMap->get(vKey) == hValue);
            }

        void testSize()
            {
            Hashtable::Handle hMap   = Hashtable::create();
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
            HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
                {
                Hashtable::Handle hMap   = Hashtable::create();
                Integer32::View     vKey   = Integer32::create(123);
                Integer32::Handle   hValue = Integer32::create(456);

                hMap->put(vKey, hValue);

                TS_ASSERT(hMap->get(vKey) == hValue);

                hMap->put(hValue, vKey);

                TS_ASSERT(hMap->get(hValue) == vKey);

                Set::Handle hSet = hMap->entrySet();

                TS_ASSERT(hSet->size() == 2);
                int32_t c = 0;
                for (Iterator::Handle hIter = hSet->iterator(); hIter->hasNext(); ++c)
                    {
                    Map::Entry::Handle hEntry = cast<Map::Entry::Handle>(hIter->next());
                    TS_ASSERT(hEntry != NULL);
                    TS_ASSERT(hSet->contains(hEntry));
                    Object::View vKeyEntry = hEntry->getKey();
                    if (vKeyEntry == vKey)
                        {
                        TS_ASSERT(hEntry->getValue() == hValue);
                        }
                    else
                        {
                        TS_ASSERT(vKeyEntry == hValue)
                        TS_ASSERT(hEntry->getValue() == vKey);
                        }

                    // test mutation propogation
                    hEntry->setValue(vKeyEntry);
                    TS_ASSERT(hEntry->getValue() == vKeyEntry);
                    TS_ASSERT(hMap->get(vKeyEntry) == vKeyEntry);

                    TS_ASSERT(hSet->remove(hEntry));
                    }
                TS_ASSERT(c == 2);
                TS_ASSERT(hMap->isEmpty());

                hMap->put(vKey, hValue);
                TS_ASSERT(hMap->size() == 1);
                TS_ASSERT(hMap->containsKey(vKey));
                TS_ASSERT(hMap->get(vKey) == hValue);

                hMap->put(hValue, vKey);

                TS_ASSERT(hMap->get(hValue) == vKey);

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
            HeapAnalyzer::ensureHeap(vSnap);
            }

        void testKeySet()
            {
            Hashtable::Handle hMap   = Hashtable::create();
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
            Hashtable::Handle hMap   = Hashtable::create();
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

        void testValuesEquality()
            {
            Map::Handle hMap = Hashtable::create();
            hMap->put(Integer32::create(123), Integer32::create(456));
            hMap->put(Integer32::create(234), Integer32::create(567));
            Collection::View vColl  = hMap->values();
            TS_ASSERT(vColl->equals(vColl));

            Collection::Handle hCollCopy = HashSet::create(vColl);
            TS_ASSERT(vColl->equals(hCollCopy));
            }

        void testKeySetEquality()
            {
            Map::Handle hMap = Hashtable::create();
            hMap->put(Integer32::create(123), Integer32::create(456));
            hMap->put(Integer32::create(234), Integer32::create(567));
            Collection::View vColl  = hMap->keySet();
            TS_ASSERT(vColl->equals(vColl));

            Collection::Handle hCollCopy = HashSet::create(vColl);
            TS_ASSERT(vColl->equals(hCollCopy));
            }

        void testEntrySetEquality()
            {
            Map::Handle hMap = Hashtable::create();
            hMap->put(Integer32::create(123), Integer32::create(456));
            hMap->put(Integer32::create(234), Integer32::create(567));
            Collection::View vColl  = hMap->entrySet();
            TS_ASSERT(vColl->equals(vColl));

            Collection::Handle hCollCopy = HashSet::create(vColl);
            TS_ASSERT(vColl->equals(hCollCopy));
            }

        void testClone()
            {
            Hashtable::Handle hMap   = Hashtable::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            hMap->put(vKey, hValue);
            hMap->put(hValue, vKey);

            Hashtable::Handle hClone = cast<Hashtable::Handle>(hMap->clone());

            Collection::Handle hColl = hMap->values();
            Collection::Handle hCollClone = hClone->values();

            TS_ASSERT(hColl->containsAll(hCollClone));
            TS_ASSERT(hCollClone->containsAll(hColl));
            }

        void testGrow()
            {
            Hashtable::Handle hMap = Hashtable::create(17, 0.9F);

            for (int32_t i = 0; i < 17; ++i)
                {
                hMap->put(Integer32::valueOf(i), Integer32::valueOf(i));
                }

            // Force the hashmap to copy the entries while growing
            Iterator::Handle hIter = hMap->entrySet()->iterator();
            TS_ASSERT(hIter->hasNext());
            hIter->next();

            // Force the map to grow
            for (int32_t i = 17; i < 32; ++i)
                {
                hMap->put(Integer32::valueOf(i), Integer32::valueOf(i));
                }

            // continue iteration CME is expected
            try
                {
                while(hIter->hasNext())
                    {
                    hIter->next();
                    }
                TS_ASSERT(false);
                }
            catch (ConcurrentModificationException::View e)
                {
                }

            // Prove that our insert worked
            COH_ENSURE_EQUALITY(hMap->size(), 32);

            // Make sure the map still holds on to its original entries
            for (int32_t i = 0; i < 32; ++i)
                {
                TS_ASSERT(hMap->get(Integer32::valueOf(i)) == Integer32::valueOf(i));
                }
            }

    class BadHash
        : public class_spec<BadHash>
        {
        friend class factory<BadHash>;

        protected:
            BadHash(int32_t n)
                : m_n(n)
                {}

        public:
            virtual size32_t hashCode() const
                {
                return 17;
                }

            virtual bool equals(Object::View vThat) const
                {
                return cast<BadHash::View>(vThat)->m_n == m_n;
                }

        protected:
            int32_t m_n;
        };

    void testBadHash()
        {
        Hashtable::Handle hMap = Hashtable::create();

        // add a few BadHashes
        for (int32_t i = 0; i < 128; ++i)
            {
            Object::View vBad = BadHash::create(i);
            hMap->put(vBad, vBad);
            }

        TS_ASSERT(hMap->get(BadHash::create(127)) != NULL);

        TS_ASSERT(hMap->size() == 128);

        // pull them back
        for (int32_t i = 0; i < 128; ++i)
            {
            Object::View vBad = BadHash::create(i);
            TS_ASSERT(hMap->get(vBad)->equals(vBad));
            }
        }

    void testEscape()
        {
        Hashtable::Handle hMap = Hashtable::create();

        Object::View vA = Object::create();
        Object::View vB = Object::create();
        Object::View vC = Object::create();
        Object::View vD = Object::create();

        hMap->put(vA, vB); // attaches vA, vB
        System::assertAttachCount(vA, 0, 1 + 1);
        System::assertAttachCount(vB, 0, 1 + 1);

        TS_ASSERT(hMap->_isEscaped() == false);
        TS_ASSERT(vA->_isEscaped() == false);
        TS_ASSERT(vB->_isEscaped() == false);

        MemberHandle<Map> mhMap(System::common(), hMap);

        TS_ASSERT(hMap->_isEscaped());
        TS_ASSERT(vA->_isEscaped());
        TS_ASSERT(vB->_isEscaped());

        System::assertAttachCount(vA, 0, 1 + 2);
        System::assertAttachCount(vB, 0, 1 + 2);

        hMap->put(vC, vD); // escape attaches to vC, vD

        TS_ASSERT(vC->_isEscaped());
        TS_ASSERT(vD->_isEscaped());
        System::assertAttachCount(vC, 0, 1 + 2);
        System::assertAttachCount(vD, 0, 1 + 2);

        hMap->remove(vC); // recapture vC, vD
        TS_ASSERT(vC->_isEscaped() == false);
        TS_ASSERT(vD->_isEscaped() == false);
        System::assertAttachCount(vC, 0, 1);
        System::assertAttachCount(vD, 0, 1);

        // overwrite vA->vB to vA->vC
        hMap->put(vA, vC);

        TS_ASSERT(vC->_isEscaped());
        System::assertAttachCount(vA, 0, 1 + 2);
        System::assertAttachCount(vC, 0, 1 + 2);

        // vB was captured
        TS_ASSERT(vB->_isEscaped() == false);
        System::assertAttachCount(vB, 0, 1);

        hMap->remove(vA); // this will trigger the capture

        TS_ASSERT(vA->_isEscaped() == false);
        System::assertAttachCount(vA, 0, 1);
        System::assertAttachCount(vC, 0, 1);
        }

    void testSizeOf()
        {
        Hashtable::Handle hMap = Hashtable::create();

        Object::View vA = Object::create();
        Object::View vB = Object::create();
        Object::View vC = Object::create();
        Object::View vD = Object::create();

        size64_t cbShallow = hMap->sizeOf(false);
        size64_t cbEmpty = hMap->sizeOf(true);

        TS_ASSERT(cbShallow <= cbEmpty);

        size64_t cbLast = hMap->sizeOf(true);
        hMap->put(vA, vA);
        TS_ASSERT(cbLast < hMap->sizeOf(true));
        cbLast = hMap->sizeOf(true);

        hMap->put(vB, vB);
        TS_ASSERT(cbLast < hMap->sizeOf(true));
        cbLast = hMap->sizeOf(true);

        hMap->put(vC, vC);
        TS_ASSERT(cbLast < hMap->sizeOf(true));
        cbLast = hMap->sizeOf(true);

        hMap->put(vD, vD);
        TS_ASSERT(cbLast < hMap->sizeOf(true));
        cbLast = hMap->sizeOf(true);

        TS_ASSERT(cbShallow = hMap->sizeOf(false));
        }

    void testGetOrDefault()
        {
        Hashtable::Handle hMap   = Hashtable::create();
        Integer32::View   vKey   = Integer32::create(123);
        Integer32::Handle hValue = Integer32::create(456);

        TS_ASSERT(hMap->get(vKey) == NULL);
        Object::Holder ohObject = hMap->getOrDefault(vKey, hValue);
        TS_ASSERT(ohObject->equals(hValue));
        }

    void testPutIfAbsent()
        {
        Hashtable::Handle hMap    = Hashtable::create();
        Integer32::View   vKey    = Integer32::create(123);
        Integer32::Handle hValue1 = Integer32::create(456);
        Integer32::Handle hValue2 = Integer32::create(789);

        TS_ASSERT(hMap->get(vKey) == NULL);

        Object::Holder ohObject = hMap->putIfAbsent(vKey, hValue1);
        TS_ASSERT(ohObject == NULL);
        TS_ASSERT(hMap->get(vKey)->equals(hValue1));

        ohObject = hMap->putIfAbsent(vKey, hValue2);
        TS_ASSERT(ohObject->equals(hValue1));
        }

    void testRemoveIfPresent()
        {
        Hashtable::Handle hMap    = Hashtable::create();

        Integer32::View   vKey    = Integer32::create(123);
        Integer32::View   vValue1 = Integer32::create(456);
        Integer32::View   vValue2 = Integer32::create(789);

        hMap->put(vKey, vValue2);

        TS_ASSERT(!hMap->remove(vKey, vValue1));
        TS_ASSERT(hMap->remove(vKey, vValue2));
        }

    void testReplaceOldWithNew()
        {
        Hashtable::Handle   hMap    = Hashtable::create();

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
        Hashtable::Handle   hMap    = Hashtable::create();
        Integer32::View     vKey    = Integer32::create(123);
        Integer32::Handle   hValue1 = Integer32::create(456);
        Integer32::Handle   hValue2 = Integer32::create(789);

        TS_ASSERT(hMap->replace(vKey, hValue1) == NULL);

        hMap->put(vKey, hValue1);
        TS_ASSERT(hMap->replace(vKey, hValue2)->equals(hValue1));
        TS_ASSERT(hMap->get(vKey)->equals(hValue2));
        }
    };
