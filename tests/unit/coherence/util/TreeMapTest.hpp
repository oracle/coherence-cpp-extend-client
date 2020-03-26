/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/TreeMap.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

#include <sstream>
#include <iostream>

using namespace std;

using coherence::util::Iterator;
using coherence::util::TreeMap;

/**
* Test Suite for TreeMap
*/
class TreeMapTest : public CxxTest::TestSuite,
    public class_spec<TreeMapTest>
    {
    public:
        void testSetGet()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();
            Object::Handle  h        = Object::create();
            Object::View    vKey     = String::create("Key");

            hTreeMap->put(vKey, h);
            TS_ASSERT(hTreeMap->get(vKey) == h);

            Object::Handle h2   = Object::create();
            Object::View   vOld = hTreeMap->put(vKey, h2);
            TS_ASSERT(hTreeMap->get(vKey) == h2);
            TS_ASSERT(vOld->equals(h));

            Object::View vKey2 = String::create("Key2");
            TS_ASSERT(hTreeMap->get(vKey2) == NULL);

            hTreeMap->put(vKey2, h);
            TS_ASSERT(hTreeMap->get(vKey2) == h);
            }

        void testSize()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(hTreeMap->size() == 0);

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());
            hTreeMap->put(String::create("Key4"), Object::create());
            hTreeMap->put(String::create("Key5"), Object::create());
            TS_ASSERT(hTreeMap->size() == 5);

            hTreeMap->remove(String::create("Key2"));
            TS_ASSERT(hTreeMap->size() == 4);

            hTreeMap->put(String::create("Key5"), Object::create());
            TS_ASSERT(hTreeMap->size() == 4);

            hTreeMap->put(String::create("Key6"), Object::create());
            TS_ASSERT(hTreeMap->size() == 5);

            hTreeMap->clear();
            TS_ASSERT(hTreeMap->size() == 0);
            }

        void testIsEmpty()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(hTreeMap->isEmpty());

            hTreeMap->put(String::create("Key1"), Object::create());
            TS_ASSERT(!hTreeMap->isEmpty());

            hTreeMap->remove(String::create("Key1"));
            TS_ASSERT(hTreeMap->isEmpty());

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());
            TS_ASSERT(!hTreeMap->isEmpty());

            hTreeMap->clear();
            TS_ASSERT(hTreeMap->isEmpty());
            }

        void testContainsKey()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(!hTreeMap->containsKey(String::create("Key1")));

            hTreeMap->put(String::create("Key1"), Object::create());
            TS_ASSERT(hTreeMap->containsKey(String::create("Key1")));

            hTreeMap->remove(String::create("Key1"));
            TS_ASSERT(!hTreeMap->containsKey(String::create("Key1")));

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());
            TS_ASSERT(hTreeMap->containsKey(String::create("Key1")));
            TS_ASSERT(hTreeMap->containsKey(String::create("Key2")));
            TS_ASSERT(hTreeMap->containsKey(String::create("Key3")));

            hTreeMap->clear();
            TS_ASSERT(!hTreeMap->containsKey(String::create("Key1")));
            TS_ASSERT(!hTreeMap->containsKey(String::create("Key2")));
            TS_ASSERT(!hTreeMap->containsKey(String::create("Key3")));
            }


        void testRemove()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(hTreeMap->isEmpty());

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());

            TS_ASSERT(hTreeMap->size() == 3);

            hTreeMap->remove(String::create("Key1"));
            TS_ASSERT(hTreeMap->get(String::create("Key1")) == NULL);
            TS_ASSERT(hTreeMap->size() == 2);

            hTreeMap->remove(String::create("Key3"));
            TS_ASSERT(hTreeMap->get(String::create("Key3")) == NULL);
            TS_ASSERT(hTreeMap->size() == 1);

            hTreeMap->remove(String::create("Key2"));
            TS_ASSERT(hTreeMap->get(String::create("Key2")) == NULL);
            TS_ASSERT(hTreeMap->size() == 0);
            }

        void testClear()
            {
            HeapAnalyzer::Snapshot::View vSnap = HeapAnalyzer::ensureHeap();
                {
                TreeMap::Handle hTreeMap = TreeMap::create();

                TS_ASSERT(hTreeMap->isEmpty());

                hTreeMap->put(String::create("Key1"), Object::create());
                hTreeMap->put(String::create("Key2"), Object::create());
                hTreeMap->put(String::create("Key3"), Object::create());
                TS_ASSERT(!hTreeMap->isEmpty());

                hTreeMap->clear();
                TS_ASSERT(hTreeMap->isEmpty());
                }
            HeapAnalyzer::ensureHeap(vSnap);
            }

        void testEntrySet()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            Set::Handle hEntrySet = hTreeMap->entrySet();
            TS_ASSERT(hEntrySet->size() == 0);

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());

            TS_ASSERT(hEntrySet->size() == 3);

            hTreeMap->remove(String::create("Key2"));

            TS_ASSERT(hEntrySet->size() == 2);

            Map::Entry::View vEntry = SimpleMapEntry::create(String::create("Key1"), Object::create());
            hEntrySet->remove(vEntry);

            TS_ASSERT(hEntrySet->size() == 1);

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());

            TS_ASSERT(hEntrySet->size() == 3);

            int32_t c = 0;
            for (Iterator::Handle hIter = hEntrySet->iterator(); hIter->hasNext(); ++c)
                {
                Map::Entry::View vE = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(vE != NULL);
                TS_ASSERT(hEntrySet->contains(vE));
                }
            TS_ASSERT(c == 3);

            c = 0;
            for (Muterator::Handle hIter = hEntrySet->iterator(); hIter->hasNext(); ++c)
                {
                Map::Entry::View vE = cast<Map::Entry::View>(hIter->next());
                TS_ASSERT(vE != NULL);
                TS_ASSERT(hEntrySet->contains(vE));
                hIter->remove();
                }
            TS_ASSERT(c == 3);
            TS_ASSERT(hTreeMap->size() == 0);

            hTreeMap->put(String::create("Key1"), Object::create());
            hTreeMap->put(String::create("Key2"), Object::create());
            hTreeMap->put(String::create("Key3"), Object::create());

            TS_ASSERT(hEntrySet->size() == 3);

            vEntry = SimpleMapEntry::create(String::create("Key10"), Object::create());
            hEntrySet->add(vEntry);

            TS_ASSERT(hEntrySet->size() == 4);
            TS_ASSERT(hTreeMap->size() == 4);

            hEntrySet->clear();
            TS_ASSERT(hEntrySet->size() == 0);
            TS_ASSERT(hTreeMap->size() == 0);
            }

        void testComparator()
            {
            Comparator::Handle hComparator = SafeComparator::getInstance();
            TreeMap::Handle hTreeMap = TreeMap::create(hComparator);
            TS_ASSERT(hTreeMap->comparator() == hComparator);
            }

        void testFirstKey()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            try
                {
                hTreeMap->firstKey();
                TS_ASSERT(false);
                }
            catch (NoSuchElementException::View)
                {
                }

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(3), Object::create());

            TS_ASSERT(hTreeMap->firstKey()->equals(Integer32::create(1)));

            hTreeMap->remove(Integer32::create(1));

            TS_ASSERT(hTreeMap->firstKey()->equals(Integer32::create(2)));
            }

        void testLastKey()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            try
                {
                hTreeMap->lastKey();
                TS_ASSERT(false);
                }
            catch (NoSuchElementException::View)
                {
                }

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(3), Object::create());

            TS_ASSERT(hTreeMap->lastKey()->equals(Integer32::create(3)));

            hTreeMap->remove(Integer32::create(3));

            TS_ASSERT(hTreeMap->lastKey()->equals(Integer32::create(2)));
            }

        void testCeilingHigherKey()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(hTreeMap->ceilingKey(Integer32::create(1)) == NULL);
            TS_ASSERT(hTreeMap->higherKey(Integer32::create(1)) == NULL);

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(4), Object::create());

            TS_ASSERT(hTreeMap->ceilingKey(Integer32::create(1))->equals(Integer32::create(1)));
            TS_ASSERT(hTreeMap->higherKey(Integer32::create(1))->equals(Integer32::create(2)));

            hTreeMap->remove(Integer32::create(1));

            TS_ASSERT(hTreeMap->ceilingKey(Integer32::create(1))->equals(Integer32::create(2)));
            TS_ASSERT(hTreeMap->higherKey(Integer32::create(2))->equals(Integer32::create(4)));
           }

        void testFloorLowerKey()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            TS_ASSERT(hTreeMap->floorKey(Integer32::create(1)) == NULL);
            TS_ASSERT(hTreeMap->lowerKey(Integer32::create(1)) == NULL);

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(4), Object::create());

            TS_ASSERT(hTreeMap->floorKey(Integer32::create(4))->equals(Integer32::create(4)));
            TS_ASSERT(hTreeMap->lowerKey(Integer32::create(4))->equals(Integer32::create(2)));

            hTreeMap->remove(Integer32::create(4));

            TS_ASSERT(hTreeMap->floorKey(Integer32::create(4))->equals(Integer32::create(2)));
            TS_ASSERT(hTreeMap->lowerKey(Integer32::create(2))->equals(Integer32::create(1)));
           }

        void testHeadMap()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            SortedMap::Handle hHeadMap = hTreeMap->headMap(Integer32::create(4));
            SortedMap::Handle hHeadMap2 = hTreeMap->headMap(Integer32::create(4), true);

            TS_ASSERT(hHeadMap->size() == 0);

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(3), Object::create());

            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hHeadMap->size() == 3);
            TS_ASSERT(hHeadMap2->size() == 3);

            hTreeMap->put(Integer32::create(4), Object::create());
            hTreeMap->put(Integer32::create(5), Object::create());
            hTreeMap->put(Integer32::create(6), Object::create());

            TS_ASSERT(hTreeMap->size() == 6);
            TS_ASSERT(hHeadMap->size() == 3);
            TS_ASSERT(hHeadMap2->size() == 4);

            hTreeMap->remove(Integer32::create(2));
            TS_ASSERT(hTreeMap->size() == 5);
            TS_ASSERT(hHeadMap->size() == 2);
            TS_ASSERT(hHeadMap2->size() == 3);

            hTreeMap->remove(Integer32::create(5));
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hHeadMap->size() == 2);
            TS_ASSERT(hHeadMap2->size() == 3);

            hHeadMap->remove(Integer32::create(1));
            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hHeadMap->size() == 1);
            TS_ASSERT(hHeadMap2->size() == 2);

            hHeadMap->put(Integer32::create(1), Object::create());
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hHeadMap->size() == 2);
            TS_ASSERT(hHeadMap2->size() == 3);

            hHeadMap->remove(Integer32::create(4));
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hHeadMap->size() == 2);
            TS_ASSERT(hHeadMap2->size() == 3);

            SortedMap::Handle hSubHeadMap = hHeadMap->headMap(Integer32::create(2));
            TS_ASSERT(hSubHeadMap->size() == 1);
            }

        void testTailMap()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            SortedMap::Handle hTailMap = hTreeMap->tailMap(Integer32::create(4));
            SortedMap::Handle hTailMap2 = hTreeMap->tailMap(Integer32::create(4), false);

            TS_ASSERT(hTailMap->size() == 0);
            TS_ASSERT(hTailMap2->size() == 0);

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(3), Object::create());

            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hTailMap->size() == 0);
            TS_ASSERT(hTailMap2->size() == 0);

            hTreeMap->put(Integer32::create(4), Object::create());
            hTreeMap->put(Integer32::create(5), Object::create());
            hTreeMap->put(Integer32::create(6), Object::create());

            TS_ASSERT(hTreeMap->size() == 6);
            TS_ASSERT(hTailMap->size() == 3);
            TS_ASSERT(hTailMap2->size() == 2);

            hTreeMap->remove(Integer32::create(2));
            TS_ASSERT(hTreeMap->size() == 5);
            TS_ASSERT(hTailMap->size() == 3);
            TS_ASSERT(hTailMap2->size() == 2);

            hTreeMap->remove(Integer32::create(5));
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hTailMap->size() == 2);
            TS_ASSERT(hTailMap2->size() == 1);

            hTailMap->remove(Integer32::create(4));
            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hTailMap->size() == 1);
            TS_ASSERT(hTailMap2->size() == 1);

            hTailMap->put(Integer32::create(4), Object::create());
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hTailMap->size() == 2);
            TS_ASSERT(hTailMap2->size() == 1);

            SortedMap::Handle hSubTailMap = hTailMap->tailMap(Integer32::create(5));
            SortedMap::Handle hSubTailMap2 = hTailMap2->tailMap(Integer32::create(5));
            TS_ASSERT(hSubTailMap->size() == 1);
            TS_ASSERT(hSubTailMap2->size() == 1);
            }

        void testSubMap()
            {
            TreeMap::Handle hTreeMap = TreeMap::create();

            SortedMap::Handle hSubMap = hTreeMap->subMap(Integer32::create(2), Integer32::create(15));
            SortedMap::Handle hSubMap2 = hTreeMap->subMap(Integer32::create(2), false, Integer32::create(15), true);

            TS_ASSERT(hSubMap->size() == 0);
            TS_ASSERT(hSubMap2->size() == 0);

            try
                {
                hSubMap->firstKey();
                TS_ASSERT(false);
                }
            catch (NoSuchElementException::View)
                {
                }

            try
                {
                hSubMap->lastKey();
                TS_ASSERT(false);
                }
            catch (NoSuchElementException::View)
                {
                }

            hTreeMap->put(Integer32::create(1), Object::create());
            hTreeMap->put(Integer32::create(2), Object::create());
            hTreeMap->put(Integer32::create(3), Object::create());

            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap2->size() == 1);

            TS_ASSERT(hSubMap->firstKey()->equals(Integer32::create(2)));
            TS_ASSERT(hSubMap->lastKey()->equals(Integer32::create(3)));

            TS_ASSERT(hSubMap2->firstKey()->equals(Integer32::create(3)));
            TS_ASSERT(hSubMap2->lastKey()->equals(Integer32::create(3)));

            hTreeMap->put(Integer32::create(14), Object::create());
            hTreeMap->put(Integer32::create(15), Object::create());
            hTreeMap->put(Integer32::create(16), Object::create());

            TS_ASSERT(hTreeMap->size() == 6);
            TS_ASSERT(hSubMap->size() == 3);
            TS_ASSERT(hSubMap2->size() == 3);

            TS_ASSERT(hSubMap->firstKey()->equals(Integer32::create(2)));
            TS_ASSERT(hSubMap->lastKey()->equals(Integer32::create(14)));

            TS_ASSERT(hSubMap2->firstKey()->equals(Integer32::create(3)));
            TS_ASSERT(hSubMap2->lastKey()->equals(Integer32::create(15)));

            hTreeMap->remove(Integer32::create(2));
            TS_ASSERT(hTreeMap->size() == 5);
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap2->size() == 3);

            hTreeMap->remove(Integer32::create(15));
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap2->size() == 2);

            hSubMap->remove(Integer32::create(14));
            TS_ASSERT(hTreeMap->size() == 3);
            TS_ASSERT(hSubMap->size() == 1);
            TS_ASSERT(hSubMap->lastKey()->equals(Integer32::create(3)));
            TS_ASSERT(hSubMap2->size() == 1);
            TS_ASSERT(hSubMap2->lastKey()->equals(Integer32::create(3)));

            String::Handle hObject = String::create("foo");
            hSubMap->put(Integer32::create(14), hObject);
            TS_ASSERT(hTreeMap->size() == 4);
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap->lastKey()->equals(Integer32::create(14)));
            TS_ASSERT(hSubMap->get(Integer32::create(14))->equals(hObject));

            SortedMap::Handle hSubSubMap = hSubMap->subMap(Integer32::create(3), Integer32::create(14));
            TS_ASSERT(hSubSubMap->firstKey()->equals(Integer32::create(3)));
            TS_ASSERT(hSubSubMap->lastKey()->equals(Integer32::create(3)));

            hTreeMap->put(Integer32::create(17), Object::create());
            hTreeMap->put(Integer32::create(10), Object::create());
            hSubMap->put(Integer32::create(11), Object::create());
            hSubSubMap->put(Integer32::create(12), Object::create());
            TS_ASSERT(hTreeMap->size() == 8);
            TS_ASSERT(hSubMap->size() == 5);
            TS_ASSERT(hSubSubMap->size() == 4);

            TS_ASSERT(hSubSubMap->firstKey()->equals(Integer32::create(3)));
            TS_ASSERT(hSubSubMap->lastKey()->equals(Integer32::create(12)));

            Set::Handle hEntrySet = hSubSubMap->entrySet();

            TS_ASSERT(hEntrySet->size() == 4);

            try
                {
                hSubSubMap->put(Integer32::create(20), Object::create());
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View)
                {
                }

            hSubSubMap->remove(Integer32::create(11));
            TS_ASSERT(hSubSubMap->size() == 3);
            TS_ASSERT(hEntrySet->size() == 3);

            Map::Entry::View vEntry = SimpleMapEntry::create(Integer32::create(11), Object::create());
            hEntrySet->add(vEntry);
            TS_ASSERT(hSubSubMap->size() == 4);
            TS_ASSERT(hEntrySet->size() == 4);
            }

        void testGetOrDefault()
            {
            TreeMap::Handle     hMap   = TreeMap::create();
            Integer32::View     vKey   = Integer32::create(123);
            Integer32::Handle   hValue = Integer32::create(456);

            TS_ASSERT(hMap->get(vKey) == NULL);
            Object::Holder ohObject = hMap->getOrDefault(vKey, hValue);
            TS_ASSERT(ohObject->equals(hValue));
            }


        void testPutIfAbsent()
            {
            TreeMap::Handle     hMap    = TreeMap::create();
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

        void testRemoveIfPresent()
            {
            TreeMap::Handle   hMap    = TreeMap::create();
            Integer32::View   vKey    = Integer32::create(123);
            Integer32::View   vValue1 = Integer32::create(456);
            Integer32::View   vValue2 = Integer32::create(789);

            hMap->put(vKey, vValue2);

            TS_ASSERT(!hMap->remove(vKey, vValue1));
            TS_ASSERT(hMap->remove(vKey, vValue2));
            }

        void testReplaceOldWithNew()
            {
            TreeMap::Handle     hMap    = TreeMap::create();
            Integer32::View     vKey    = Integer32::create(123);
            Integer32::Handle   hValue1 = Integer32::create(456);
            Integer32::Handle   hValue2 = Integer32::create(789);

            hMap->put(vKey, hValue1);     
            TS_ASSERT(((Map::Handle)hMap)->replace(vKey, hValue1, hValue2));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));

            hMap->put(vKey, hValue2);
            TS_ASSERT(!((Map::Handle)hMap)->replace(vKey, hValue1, hValue2));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));
            }

        void testReplaceWithValue()
            {
            TreeMap::Handle     hMap    = TreeMap::create();
            Integer32::View     vKey    = Integer32::create(123);
            Integer32::Handle   hValue1 = Integer32::create(456);
            Integer32::Handle   hValue2 = Integer32::create(789);

            TS_ASSERT(((Map::Handle)hMap)->replace(vKey, hValue1) == NULL);

            hMap->put(vKey, hValue1);
            TS_ASSERT(((Map::Handle)hMap)->replace(vKey, hValue2)->equals(hValue1));
            TS_ASSERT(hMap->get(vKey)->equals(hValue2));
            }
    };
