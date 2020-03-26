/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collections.hpp"
#include "coherence/util/TreeMap.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include "private/coherence/run/xml/SimpleElement.hpp"

using namespace coherence::run::xml;
using namespace coherence::util;

COH_PRAGMA_PUSH // Windows generates warnings related to use of env variables

class CollectionsTestStringComparator
    : public class_spec<CollectionsTestStringComparator,
        extends<Object>,
        implements<Comparator> >
{
friend class factory<CollectionsTestStringComparator>;

 virtual int32_t compare(Object::View v1, Object::View v2) const
     {
     return StringHelper::compare(cast<String::View>(v1), cast<String::View>(v2), true);
     }
};

class CollectionsTestNonComparable
    : public class_spec<CollectionsTestNonComparable>
{
friend class factory<CollectionsTestNonComparable>;
};


class CollectionsTestNonSortableList
    : public class_spec<CollectionsTestNonSortableList,
        extends<ArrayList> >
{
friend class factory<CollectionsTestNonSortableList>;

public:
 virtual Object::Holder set(size32_t /*idx*/, Object::Holder /*oh*/)
     {
     COH_THROW(UnsupportedOperationException::create(
             "The list does not support modification operations"));
     }
};

/**
* Test Suite for the Collections object.
*/
class CollectionsTest : public CxxTest::TestSuite
    {
    public:
        void testReverse()
            {
            List::Handle hList = ArrayList::create();

            // empty list
            Collections::reverse(hList);

            // singleton list
            hList->add(Integer32::create(1));
            Collections::reverse(hList);
            TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(0))->getInt32Value(), int32_t(1));
            hList->clear();

            // list smaller than coh_reverse_threshold
            int cListSize = 10;
            for (int i = 0; i < cListSize; ++i)
                {
                hList->add(Integer32::create(i));
                }

            Collections::reverse(hList);

            for (int i = 0; i < cListSize; ++i)
                {
                TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(i))->getInt32Value(), int32_t((cListSize - 1) - i));
                }

            // list larger than coh_reverse_threshold
            cListSize = 1000;

            hList->clear();
            for (int i = 0; i < cListSize; ++i)
                {
                hList->add(Integer32::create(i));
                }

            Collections::reverse(hList);

            for (int i = 0; i < cListSize; ++i)
                {
                TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(i))->getInt32Value(), int32_t((cListSize - 1) - i));
                }
            }

        void testReverseNonSettable()
            {
            List::Handle hList = CollectionsTestNonSortableList::create();

            // empty list
            Collections::reverse(hList);

            // singleton list
            hList->add(Integer32::create(1));
            Collections::reverse(hList);
            TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(0))->getInt32Value(), int32_t(1));
            hList->clear();

            hList->add(String::create("Hello"));
            hList->add(String::create("World"));

            TS_ASSERT_THROWS(Collections::reverse(hList), UnsupportedOperationException::View);
            }

        /**
        * Test sort
        */
        void testSort()
            {
            ArrayList::Handle hList = ArrayList::create();

            hList->add(String::create("EEEE"));
            hList->add(String::create("BBBB"));
            hList->add(String::create("FFFF"));
            hList->add(String::create("DDDD"));
            hList->add(String::create("GGGG"));
            hList->add(String::create("AAAA"));
            hList->add(String::create("HHHH"));
            hList->add(String::create("CCCC"));

            TS_ASSERT(hList->size() == 8);

            Collections::sort(hList);

            TS_ASSERT(hList->size() == 8);

            TS_ASSERT(hList->get(0)->equals(String::create("AAAA")));
            TS_ASSERT(hList->get(1)->equals(String::create("BBBB")));
            TS_ASSERT(hList->get(2)->equals(String::create("CCCC")));
            TS_ASSERT(hList->get(3)->equals(String::create("DDDD")));
            TS_ASSERT(hList->get(4)->equals(String::create("EEEE")));
            TS_ASSERT(hList->get(5)->equals(String::create("FFFF")));
            TS_ASSERT(hList->get(6)->equals(String::create("GGGG")));
            TS_ASSERT(hList->get(7)->equals(String::create("HHHH")));
            }

        /**
        * Test sort with a non comparable object
        */
        void testSortNonComparable()
            {
            ArrayList::Handle hList = ArrayList::create();

            CollectionsTestNonComparable::Handle hNonComp = CollectionsTestNonComparable::create();
            hList->add(hNonComp);
            CollectionsTestNonComparable::Handle hNonComp2 = CollectionsTestNonComparable::create();
            hList->add(hNonComp2);

            try
                {
                Collections::sort(hList);
                TS_FAIL("Expected an exception : calling sort with a non comparable object.");
                }
            catch (ClassCastException::View)
                {
                //expected
                }
            }

        /**
        * Test sort with a list that can not be sorted
        */
        void testSortNonSortable()
            {
            String::Handle hObject = String::create("EEEE");

            ArrayList::Handle hList = CollectionsTestNonSortableList::create();

            hList->add(String::create("BBBB"));
            hList->add(String::create("AAAA"));
            hList->add(String::create("CCCC"));

            try
                {
                Collections::sort(hList);
                TS_FAIL("Expected an exception : calling sort on a non-sortable list.");
                }
            catch (Exception::View)
                {
                //expected
                }
            }

        /**
        * Test sort with an empty list
        */
        void testSortEmptyList()
            {
            ArrayList::Handle hList = ArrayList::create();

            TS_ASSERT(hList->size() == 0);

            Collections::sort(hList);

            TS_ASSERT(hList->size() == 0);
            }

        /**
        * Test sort with a comparator
        */
        void testSortWithComparator()
            {
            CollectionsTestStringComparator::Handle hComparator = CollectionsTestStringComparator::create();

            ArrayList::Handle hList = ArrayList::create();

            hList->add(String::create("EEEE"));
            hList->add(String::create("BBBB"));
            hList->add(String::create("FFFF"));
            hList->add(String::create("DDDD"));
            hList->add(String::create("GGGG"));
            hList->add(String::create("AAAA"));
            hList->add(String::create("HHHH"));
            hList->add(String::create("CCCC"));

            TS_ASSERT(hList->size() == 8);


            Collections::sort(hList, hComparator);

            TS_ASSERT(hList->size() == 8);

            TS_ASSERT(hList->get(0)->equals(String::create("AAAA")));
            TS_ASSERT(hList->get(1)->equals(String::create("BBBB")));
            TS_ASSERT(hList->get(2)->equals(String::create("CCCC")));
            TS_ASSERT(hList->get(3)->equals(String::create("DDDD")));
            TS_ASSERT(hList->get(4)->equals(String::create("EEEE")));
            TS_ASSERT(hList->get(5)->equals(String::create("FFFF")));
            TS_ASSERT(hList->get(6)->equals(String::create("GGGG")));
            TS_ASSERT(hList->get(7)->equals(String::create("HHHH")));

            }

        /**
        * Test sort with a list that can not be sorted
        */
        void testSortNonSortableWithComparator()
            {
            CollectionsTestStringComparator::Handle hComparator =
                    CollectionsTestStringComparator::create();
            String::Handle hObject = String::create("EEEE");

            ArrayList::Handle hList = CollectionsTestNonSortableList::create();

            hList->add(String::create("BBBB"));
            hList->add(String::create("AAAA"));
            hList->add(String::create("CCCC"));

            try
                {
                Collections::sort(hList, hComparator);
                TS_FAIL("Expected an exception : calling sort on a non-sortable list.");
                }
            catch (Exception::View)
                {
                //expected
                }
            }

        /**
        * Test sort with a comparator
        */
        void testSortEmptyListWithComparator()
            {
            CollectionsTestStringComparator::Handle hComparator =
                    CollectionsTestStringComparator::create();

            ArrayList::Handle hList = ArrayList::create();

            TS_ASSERT(hList->size() == 0);

            Collections::sort(hList, hComparator);

            TS_ASSERT(hList->size() == 0);
            }

        void testSwap()
            {
            List::Handle hList = ArrayList::create();

            for (int i = 0; i < 10; ++i)
                {
                hList->add(Integer32::create(i));
                }

            Collections::swap(hList, 0, 1);

            TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(0))->getInt32Value(), 1);
            TS_ASSERT_EQUALS(cast<Integer32::View>(hList->get(1))->getInt32Value(), 0);

            TS_ASSERT_THROWS(Collections::swap(hList,  0, 10), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(Collections::swap(hList, 10,  0), IndexOutOfBoundsException::View);
            TS_ASSERT_THROWS(Collections::swap(hList, -1, -1), IndexOutOfBoundsException::View);
            }

        /**
        * Test singleton
        */
        void testSingleton()
            {
            String::Handle hObject = String::create("EEEE");

            Set::View vSingleton = Collections::singleton(hObject);

            TS_ASSERT(vSingleton->size() == 1);
            TS_ASSERT(vSingleton->contains(hObject));

            Iterator::Handle hIterator = vSingleton->iterator();

            TS_ASSERT(hIterator->hasNext());
            TS_ASSERT(hIterator->next()->equals(hObject));
            TS_ASSERT(!hIterator->hasNext());

            Set::View vCloneSingleton = cast<Set::View>(vSingleton->clone());
            TS_ASSERT(vCloneSingleton->equals(vSingleton));
            }

        /**
        * Test singletonList
        */
        void testSingletonList()
            {
            String::Handle hObject = String::create("EEEE");

            List::View vSingleton = Collections::singletonList(hObject);

            TS_ASSERT(vSingleton->size() == 1);
            TS_ASSERT(vSingleton->contains(hObject));

            ListIterator::Handle hIterator = vSingleton->listIterator();

            TS_ASSERT(hIterator->hasNext());
            TS_ASSERT(hIterator->next()->equals(hObject));
            TS_ASSERT(!hIterator->hasNext());
            TS_ASSERT(hIterator->hasPrevious());
            TS_ASSERT(hIterator->previous()->equals(hObject));
            TS_ASSERT(!hIterator->hasPrevious());
            TS_ASSERT(hIterator->hasNext());

            List::View vCloneSingleton = cast<List::View>(vSingleton->clone());
            TS_ASSERT(vCloneSingleton->equals(vSingleton));
            }

        /**
        * Test singletonMap
        */
        void testSingletonMap()
            {
            String::Handle hKey = String::create("key");
            String::Handle hObject = String::create("EEEE");

            Map::View vSingleton = Collections::singletonMap(hKey, hObject);

            TS_ASSERT(vSingleton->size() == 1);
            TS_ASSERT(vSingleton->containsKey(hKey));
            TS_ASSERT(vSingleton->containsValue(hObject));

            Set::View vEntrySet = vSingleton->entrySet();

            TS_ASSERT(vEntrySet->size() == 1);

            Iterator::Handle hIterator = vEntrySet->iterator();

            TS_ASSERT(hIterator->hasNext());
            Map::Entry::View vEntry =
                    cast<Map::Entry::View>(hIterator->next());

            TS_ASSERT(vEntry->getKey()->equals(String::create("key")));
            TS_ASSERT(vEntry->getValue()->equals(String::create("EEEE")));

            Map::View vCloneSingleton = cast<Map::View>(vSingleton->clone());
            TS_ASSERT(vCloneSingleton->equals(vSingleton));
            }

        /**
        * Test emptySet
        */
        void testEmptySet()
            {
            Set::View vEmpty = Collections::emptySet();

            TS_ASSERT(vEmpty->size() == 0);

            TS_ASSERT(!vEmpty->contains(String::create("foo")));

            Iterator::Handle hIterator = vEmpty->iterator();

            TS_ASSERT(!hIterator->hasNext());
            try
                {
                hIterator->next();
                TS_FAIL("Expected NoSuchElementException from empty iterator!");
                }
            catch (NoSuchElementException::View)
                {
                //expected
                }
            Set::View vCloneEmpty = cast<Set::View>(vEmpty->clone());
            TS_ASSERT(vCloneEmpty->equals(vEmpty));
            }

        /**
        * Test emptyMap
        */
        void testEmptyMap()
            {
            Map::View vEmpty = Collections::emptyMap();

            TS_ASSERT(vEmpty->size() == 0);

            TS_ASSERT(NULL == vEmpty->get(String::create("key")));

            Set::View vEntrySet = vEmpty->entrySet();

            TS_ASSERT(vEntrySet->size() == 0);

            Iterator::Handle hIterator = vEntrySet->iterator();

            TS_ASSERT(!hIterator->hasNext());
            try
                {
                hIterator->next();
                TS_FAIL("Expected NoSuchElementException from empty iterator!");
                }
            catch (NoSuchElementException::View)
                {
                //expected
                }
            Map::View vCloneEmpty = cast<Map::View>(vEmpty->clone());
            TS_ASSERT(vCloneEmpty->equals(vEmpty));
            }

        /**
        * Test synchronizedMap
        */
        void testSynchronizedMap()
            {
            String::Handle hKey1 = String::create("key1");
            String::Handle hKey2 = String::create("key2");
            String::Handle hKey3 = String::create("key3");
            String::Handle hObject1 = String::create("EEEE");
            String::Handle hObject2 = String::create("FFFF");
            String::Handle hObject3 = String::create("GGGG");

            Map::Handle hMap = HashMap::create();
            hMap->put(hKey1, hObject1);
            hMap->put(hKey2, hObject2);
            hMap->put(hKey3, hObject3);
            Map::Handle hSyncMap = Collections::synchronizedMap(hMap);

            TS_ASSERT(hSyncMap->size() == 3);
            TS_ASSERT(hSyncMap->containsKey(hKey1));;
            TS_ASSERT(hSyncMap->containsValue(hObject2));

            Set::View vEntrySet = hSyncMap->entrySet();
            TS_ASSERT(vEntrySet->size() == 3);

            Map::Handle hCloneSyncMap = cast<Map::Handle>(hSyncMap->clone());
            TS_ASSERT(hCloneSyncMap->equals(hSyncMap));

            Collection::View vValues = hSyncMap->values();
            hSyncMap->remove(hKey1);
            TS_ASSERT(vValues->size() == 2);

            hSyncMap->clear();
            TS_ASSERT(hSyncMap->isEmpty());
            }

        /**
        * Test synchronizedSortedMap
        */
        void testSynchronizedSortedMap()
            {
            String::Handle hKey1 = String::create("key1");
            String::Handle hKey2 = String::create("key2");
            String::Handle hKey3 = String::create("key3");
            String::Handle hKey4 = String::create("key4");
            String::Handle hKey5 = String::create("key5");
            String::Handle hObject1 = String::create("EEEE");
            String::Handle hObject2 = String::create("FFFF");
            String::Handle hObject3 = String::create("GGGG");
            String::Handle hObject4 = String::create("HHHH");
            String::Handle hObject5 = String::create("IIII");

            SortedMap::Handle hMap = TreeMap::create();
            hMap->put(hKey3, hObject3);
            hMap->put(hKey1, hObject1);
            hMap->put(hKey2, hObject2);
            hMap->put(hKey5, hObject5);
            hMap->put(hKey4, hObject4);
            SortedMap::Handle hSyncMap = Collections::synchronizedSortedMap(hMap);

            TS_ASSERT(hSyncMap->size() == 5);
            TS_ASSERT(hSyncMap->containsKey(hKey1));
            TS_ASSERT(hSyncMap->containsValue(hObject2));
            TS_ASSERT(hSyncMap->firstKey()->equals(String::create("key1")));
            TS_ASSERT(hSyncMap->lastKey()->equals(String::create("key5")));

            Set::View vEntrySet = hSyncMap->entrySet();
            TS_ASSERT(vEntrySet->size() == 5);

            Iterator::Handle hIterator = vEntrySet->iterator();

            TS_ASSERT(hIterator->hasNext());
            Map::Entry::View vEntry =
                    cast<Map::Entry::View>(hIterator->next());

            TS_ASSERT(vEntry->getKey()->equals(String::create("key1")));
            TS_ASSERT(vEntry->getValue()->equals(String::create("EEEE")));

            Map::Handle hCloneSyncMap = cast<Map::Handle>(hSyncMap->clone());
            TS_ASSERT(hCloneSyncMap->equals(hSyncMap));

            SortedMap::Handle hSubMap = cast<SortedMap::Handle>(hSyncMap->headMap(hKey3));
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap->containsKey(hKey1));

            SortedMap::View vSubMap = cast<SortedMap::View>(hSyncMap->headMap(hKey3));
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap->containsKey(hKey1));

            hSubMap = hSyncMap->tailMap(hKey2);
            TS_ASSERT(hSubMap->size() == 4);
            TS_ASSERT(hSubMap->containsKey(hKey3));

            hSubMap = hSyncMap->subMap(hKey2, hKey4);
            TS_ASSERT(hSubMap->size() == 2);
            TS_ASSERT(hSubMap->containsKey(hKey3));

            hCloneSyncMap->clear();
            TS_ASSERT(hCloneSyncMap->isEmpty());
            }
        };
