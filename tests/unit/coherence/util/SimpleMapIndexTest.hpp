/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/SimpleMapIndex.hpp"

#include "private/coherence/util/SimpleIterator.hpp"
#include "private/coherence/util/SimpleMapEntry.hpp"

using namespace coherence::lang;
using namespace coherence::util;

/**
* Test Suite for SimpleMapIndex.
*/
class SimpleMapIndexTest : public CxxTest::TestSuite
    {
    public:
        void testGetValueExtractor()
            {
            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = SimpleMapIndex::create(hExtractor,
                    false, (Comparator::View) NULL);

            TS_ASSERT(hIndex->getValueExtractor()->equals(hExtractor));
            }

        void testIsOrdered()
            {
            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = SimpleMapIndex::create(hExtractor,
                    false, (Comparator::View) NULL);

            TS_ASSERT(!hIndex->isOrdered());

            hIndex = SimpleMapIndex::create(hExtractor, true,
                    (Comparator::View) NULL);

            TS_ASSERT(hIndex->isOrdered());
            }

            void testIsPartial()
            {
            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = SimpleMapIndex::create(hExtractor,
                    false, (Comparator::View) NULL);

            TS_ASSERT(!hIndex->isPartial());

            hIndex->insert(SimpleMapEntry::create(String::create("key1"), Integer32::create(6)));
            TS_ASSERT(!hIndex->isPartial());

            hIndex->update(SimpleMapEntry::create(String::create("key1"), Integer32::create(4)));
            TS_ASSERT(!hIndex->isPartial());

            hIndex->remove(SimpleMapEntry::create(String::create("key1"), Integer32::create(4)));
            TS_ASSERT(!hIndex->isPartial());
            }

        void testGetIndexContents()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("one"),         Integer32::create(1));
            hMap->put(String::create("another_one"), Integer32::create(1));
            hMap->put(String::create("one_more"),    Integer32::create(1));
            hMap->put(String::create("two"),         Integer32::create(2));
            hMap->put(String::create("three"),       Integer32::create(3));
            hMap->put(String::create("four"),        Integer32::create(4));
            hMap->put(String::create("four_again"),  Integer32::create(4));
            hMap->put(String::create("five"),        Integer32::create(5));
            hMap->put(String::create("five_a"),      Integer32::create(5));
            hMap->put(String::create("five_b"),      Integer32::create(5));
            hMap->put(String::create("five_c"),      Integer32::create(5));
            hMap->put(String::create("five_d"),      Integer32::create(5));

            ValueExtractor::Handle hExtractor     = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex         = createIndex(hMap, hExtractor);
            Map::View              vIndexContents = hIndex->getIndexContents();

            Set::View vSetOne = cast<Set::View>(vIndexContents->get(Integer32::create(1)));
            TS_ASSERT(vSetOne->size()==3);
            TS_ASSERT(vSetOne->contains(String::create("one")));
            TS_ASSERT(vSetOne->contains(String::create("another_one")));
            TS_ASSERT(vSetOne->contains(String::create("one_more")));

            Set::View vSetTwo = cast<Set::View>(vIndexContents->get(Integer32::create(2)));
            TS_ASSERT(vSetTwo->size()==1);
            TS_ASSERT(vSetTwo->contains(String::create("two")));

            Set::View vSetThree = cast<Set::View>(vIndexContents->get(Integer32::create(3)));
            TS_ASSERT(vSetThree->size()==1);
            TS_ASSERT(vSetThree->contains(String::create("three")));

            Set::View vSetFour = cast<Set::View>(vIndexContents->get(Integer32::create(4)));
            TS_ASSERT(vSetFour->size()==2);
            TS_ASSERT(vSetFour->contains(String::create("four")));
            TS_ASSERT(vSetFour->contains(String::create("four_again")));

            Set::View vSetFive = cast<Set::View>(vIndexContents->get(Integer32::create(5)));
            TS_ASSERT(vSetFive->size()==5);
            TS_ASSERT(vSetFive->contains(String::create("five")));
            TS_ASSERT(vSetFive->contains(String::create("five_a")));
            TS_ASSERT(vSetFive->contains(String::create("five_b")));
            TS_ASSERT(vSetFive->contains(String::create("five_c")));
            TS_ASSERT(vSetFive->contains(String::create("five_d")));
            }

        void testGet()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("one"),         Integer32::create(1));
            hMap->put(String::create("two"),         Integer32::create(2));
            hMap->put(String::create("three"),       Integer32::create(3));
            hMap->put(String::create("four"),        Integer32::create(4));
            hMap->put(String::create("five"),        Integer32::create(5));

            ValueExtractor::Handle hExtractor     = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex         = createIndex(hMap, hExtractor);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(Integer32::create(1)));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(Integer32::create(2)));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(Integer32::create(3)));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(Integer32::create(4)));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(Integer32::create(5)));
            }

        void testInsert()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("one"),   Integer32::create(1));
            hMap->put(String::create("two"),   Integer32::create(2));
            hMap->put(String::create("three"), Integer32::create(3));
            hMap->put(String::create("four"),  Integer32::create(4));
            hMap->put(String::create("five"),  Integer32::create(5));

            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = createIndex(hMap, hExtractor);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(Integer32::create(1)));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(Integer32::create(2)));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(Integer32::create(3)));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(Integer32::create(4)));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(Integer32::create(5)));

            TS_ASSERT(hIndex->get(String::create("ten")) == NULL);

            hIndex->insert(SimpleMapEntry::create(String::create("ten"), Integer32::create(10)));

            TS_ASSERT(hIndex->get(String::create("ten"))->equals(Integer32::create(10)));

            Map::View vIndexContents = hIndex->getIndexContents();

            Set::View vSet = cast<Set::View>(vIndexContents->get(Integer32::create(10)));
            TS_ASSERT(vSet->size()==1);
            TS_ASSERT(vSet->contains(String::create("ten")));
            }

        void testUpdate()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("one"),   Integer32::create(1));
            hMap->put(String::create("two"),   Integer32::create(2));
            hMap->put(String::create("three"), Integer32::create(3));
            hMap->put(String::create("four"),  Integer32::create(4));
            hMap->put(String::create("five"),  Integer32::create(5));

            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = createIndex(hMap, hExtractor);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(Integer32::create(1)));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(Integer32::create(2)));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(Integer32::create(3)));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(Integer32::create(4)));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(Integer32::create(5)));

            TS_ASSERT(hIndex->get(String::create("ten")) == NULL);

            hIndex->update(SimpleMapEntry::create(String::create("five"), Integer32::create(55)));

            TS_ASSERT(hIndex->get(String::create("five"))->equals(Integer32::create(55)));

            Map::View vIndexContents = hIndex->getIndexContents();

            Set::View vSet = cast<Set::View>(vIndexContents->get(Integer32::create(55)));
            TS_ASSERT(vSet->size()==1);
            TS_ASSERT(vSet->contains(String::create("five")));

            vSet = cast<Set::View>(vIndexContents->get(Integer32::create(5)));
            TS_ASSERT(NULL == vSet || vSet->size() == 0);
            }

        void testRemove()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("one"),   Integer32::create(1));
            hMap->put(String::create("two"),   Integer32::create(2));
            hMap->put(String::create("three"), Integer32::create(3));
            hMap->put(String::create("four"),  Integer32::create(4));
            hMap->put(String::create("five"),  Integer32::create(5));

            ValueExtractor::Handle hExtractor = IdentityExtractor::create();
            SimpleMapIndex::Handle hIndex     = createIndex(hMap, hExtractor);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(Integer32::create(1)));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(Integer32::create(2)));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(Integer32::create(3)));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(Integer32::create(4)));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(Integer32::create(5)));

            TS_ASSERT(hIndex->get(String::create("ten")) == NULL);

            hIndex->remove(SimpleMapEntry::create(String::create("three"), Integer32::create(3)));

            TS_ASSERT(hIndex->get(String::create("three")) == NULL);

            Map::View vIndexContents = hIndex->getIndexContents();

            Set::View vSet = cast<Set::View>(vIndexContents->get(Integer32::create(3)));
            TS_ASSERT(NULL == vSet || vSet->size() == 0);
            }

        void testInsertWithCollection()
            {
            insertUpdateWithCollection(false);
            }

        void testUpdateWithArray()
            {
            insertUpdateWithArray(true);
            }

        /**
        * Internal method called by test methods to test insert/update of a
        * Collection in a SimpleMapIndex.  Verify the following :
        * 1) the index contains the new value for a key after an update
        * 2) if multiple equivalent Collection instances are inserted/updated
        *    in the index for different keys, verify that only one copy of the
        *    Collection exists in the index
        *
        * @param fUpdate  if true then do an insert followed by an update; else
        *                 do an insert only
        *
        * @throws Exception  rethrow any exception to be caught by test framework
        */
        static void insertUpdateWithCollection(bool fUpdate)
            {
            Object::View vKey    = String::create("key");
            Object::View vValue  = String::create("value");
            Object::View vKey2   = String::create("key2");
            Object::View vValue2 = String::create("value2");
            Object::View vKey3   = String::create("key3");
            Object::View vValue3 = String::create("value3");
            Object::View vKey4   = String::create("key4");
            Object::View vValue4 = String::create("value4");

            ValueExtractor::View vExtractor = IdentityExtractor::create();

            // create the SimpleMapIndex to be tested
            SimpleMapIndex::Handle hMapIndex = SimpleMapIndex::create(vExtractor,
                    false, (Comparator::View) NULL);

            // define and populate the Collection values for the mock entries
            Collection::Handle hCollection     = LinkedList::create();
            Collection::Handle hCollection2    = LinkedList::create();
            Collection::Handle hCollection3    = LinkedList::create();
            Collection::Handle hCollection4    = LinkedList::create();
            Collection::Handle hCollectionInit = LinkedList::create();

            // Values used to populate the Collection values
            Integer32::View vCollectionItem1 = Integer32::create(1);
            Integer32::View vCollectionItem2 = Integer32::create(2);
            Integer32::View vCollectionItem3 = Integer32::create(3);
            Integer32::View vCollectionItem4 = Integer32::create(4);
            Integer32::View vCollectionItem5 = Integer32::create(5);

            // use new String to get a different instance than nCollectionItem1
            Integer32::View sCollectionItem1a = Integer32::create(1);

            hCollection->add(vCollectionItem1);
            hCollection->add(vCollectionItem2);
            hCollection->add(vCollectionItem3);

            hCollection2->add(sCollectionItem1a);
            hCollection2->add(vCollectionItem4);
            hCollection2->add(vCollectionItem5);

            hCollection3->add(sCollectionItem1a);
            hCollection3->add(vCollectionItem2);
            hCollection3->add(vCollectionItem3);
            hCollection3->add(vCollectionItem4);
            hCollection3->add(vCollectionItem5);

            hCollection4->add(sCollectionItem1a);
            hCollection4->add(vCollectionItem2);
            hCollection4->add(vCollectionItem3);

            hCollectionInit->add(vCollectionItem1);

            // begin test

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hMapIndex->get(String::create("key")) == NULL);
            TS_ASSERT(hMapIndex->get(String::create("key2")) == NULL);
            TS_ASSERT(hMapIndex->get(String::create("key3")) == NULL);
            TS_ASSERT(hMapIndex->get(String::create("key4")) == NULL);

            SimpleMapEntry::View vEntry  = SimpleMapEntry::create(vKey,  hCollection);
            SimpleMapEntry::View vEntry2 = SimpleMapEntry::create(vKey2, hCollection2);
            SimpleMapEntry::View vEntry3 = SimpleMapEntry::create(vKey3, hCollection3);
            SimpleMapEntry::View vEntry4 = SimpleMapEntry::create(vKey4, hCollection4);

            if (fUpdate)
                {
                // insert into the index
                hMapIndex->insert(SimpleMapEntry::create(vKey,  hCollectionInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey2, hCollectionInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey3, hCollectionInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey4, hCollectionInit));

                // update the index
                hMapIndex->update(vEntry);
                hMapIndex->update(vEntry2);
                hMapIndex->update(vEntry3);
                hMapIndex->update(vEntry4);
                }
            else
                {
                // insert into the index
                hMapIndex->insert(vEntry);
                hMapIndex->insert(vEntry2);
                hMapIndex->insert(vEntry3);
                hMapIndex->insert(vEntry4);
                }

            Collection::View vCollIndexValue  = cast<Collection::View>(hMapIndex->get(vKey));
            Collection::View vCollIndexValue2 = cast<Collection::View>(hMapIndex->get(vKey2));
            Collection::View vCollIndexValue3 = cast<Collection::View>(hMapIndex->get(vKey3));
            Collection::View vCollIndexValue4 = cast<Collection::View>(hMapIndex->get(vKey4));

            SafeHashMap::View hMapInverse = cast<SafeHashMap::View>(hMapIndex->getIndexContents());

            assertCollectionInverseMap(vKey,  vCollIndexValue,  hMapInverse);
            assertCollectionInverseMap(vKey2, vCollIndexValue2, hMapInverse);
            assertCollectionInverseMap(vKey3, vCollIndexValue3, hMapInverse);
            assertCollectionInverseMap(vKey4, vCollIndexValue4, hMapInverse);

            TS_ASSERT(!vCollIndexValue->equals(vCollIndexValue2));
            TS_ASSERT(!vCollIndexValue->equals(vCollIndexValue3));
            TS_ASSERT(vCollIndexValue->equals(vCollIndexValue4));

            TS_ASSERT(!vCollIndexValue2->equals(vCollIndexValue3));
            TS_ASSERT(!vCollIndexValue2->equals(vCollIndexValue4));

            TS_ASSERT(!vCollIndexValue3->equals(vCollIndexValue4));
            }

        /**
        * Internal method called by test methods to test insert/update of a Object
        * array in a SimpleMapIndex.  Verify the following :
        * 1) the index contains the new value for a key after an update
        * 2) if multiple equivalent Object arrays are inserted/updated in the
        *    index for different keys, verify that only one copy of the Object
        *    array exists in the index
        *
        * @param fUpdate  if true then do an insert followed by an update; else
        *                 do an insert only
        *
        * @throws Exception  rethrow any exception to be caught by test framework
        */
        static void insertUpdateWithArray(bool fUpdate)
            {
            Object::View vKey    = String::create("key");
            Object::View vValue  = String::create("value");
            Object::View vKey2   = String::create("key2");
            Object::View vValue2 = String::create("value2");
            Object::View vKey3   = String::create("key3");
            Object::View vValue3 = String::create("value3");
            Object::View vKey4   = String::create("key4");
            Object::View vValue4 = String::create("value4");

            ValueExtractor::View vExtractor = IdentityExtractor::create();

            // create the SimpleMapIndex to be tested
            SimpleMapIndex::Handle hMapIndex = SimpleMapIndex::create(vExtractor,
                    false, (Comparator::View) NULL);

            // define and populate the Object array values for the mock entries
            ObjectArray::Handle ha     = ObjectArray::create(3);
            ObjectArray::Handle ha2    = ObjectArray::create(3);
            ObjectArray::Handle ha3    = ObjectArray::create(5);
            ObjectArray::Handle ha4    = ObjectArray::create(3);
            ObjectArray::Handle haInit = ObjectArray::create(1);

            // Values used to populate the Object array values
            Integer32::View nArrayItem1  = Integer32::create(1);
            Integer32::View nArrayItem2  = Integer32::create(2);
            Integer32::View nArrayItem3  = Integer32::create(3);
            Integer32::View nArrayItem4  = Integer32::create(4);
            Integer32::View nArrayItem5  = Integer32::create(5);
            Integer32::View nArrayItem1a = Integer32::create(1);

            ha[0] = nArrayItem1;
            ha[1] = nArrayItem2;
            ha[2] = nArrayItem3;

            ha2[0] = nArrayItem1a;
            ha2[1] = nArrayItem4;
            ha2[2] = nArrayItem5;

            ha3[0] = nArrayItem1a;
            ha3[1] = nArrayItem2;
            ha3[2] = nArrayItem3;
            ha3[3] = nArrayItem4;
            ha3[4] = nArrayItem5;

            ha4[0] = nArrayItem1a;
            ha4[1] = nArrayItem2;
            ha4[2] = nArrayItem3;

            haInit[0] = nArrayItem1;

            // begin test

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hMapIndex->get(String::create("key")) == NULL);

            TS_ASSERT(hMapIndex->get(String::create("key2")) == NULL);

            TS_ASSERT(hMapIndex->get(String::create("key3")) == NULL);

            TS_ASSERT(hMapIndex->get(String::create("key4")) == NULL);

            SimpleMapEntry::View vEntry  = SimpleMapEntry::create(vKey,  ha);
            SimpleMapEntry::View vEntry2 = SimpleMapEntry::create(vKey2, ha2);
            SimpleMapEntry::View vEntry3 = SimpleMapEntry::create(vKey3, ha3);
            SimpleMapEntry::View vEntry4 = SimpleMapEntry::create(vKey4, ha4);

            if (fUpdate)
                {
                // insert into the index
                hMapIndex->insert(SimpleMapEntry::create(vKey,  haInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey2, haInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey3, haInit));
                hMapIndex->insert(SimpleMapEntry::create(vKey4, haInit));

                // update the index
                hMapIndex->update(vEntry);
                hMapIndex->update(vEntry2);
                hMapIndex->update(vEntry3);
                hMapIndex->update(vEntry4);
                }
            else
                {
                // insert into the index
                hMapIndex->insert(vEntry);
                hMapIndex->insert(vEntry2);
                hMapIndex->insert(vEntry3);
                hMapIndex->insert(vEntry4);
                }

            ObjectArray::View vaIndexValue  = cast<ObjectArray::View>(hMapIndex->get(vKey));
            ObjectArray::View vaIndexValue2 = cast<ObjectArray::View>(hMapIndex->get(vKey2));
            ObjectArray::View vaIndexValue3 = cast<ObjectArray::View>(hMapIndex->get(vKey3));
            ObjectArray::View vaIndexValue4 = cast<ObjectArray::View>(hMapIndex->get(vKey4));

            SafeHashMap::View vMapInverse = cast<SafeHashMap::View>(hMapIndex->getIndexContents());

            // verify that the inverse map contains an entry for each value in the collection
            assertArrayInverseMap(vKey,  vaIndexValue,  vMapInverse);
            assertArrayInverseMap(vKey2, vaIndexValue2, vMapInverse);
            assertArrayInverseMap(vKey3, vaIndexValue3, vMapInverse);
            assertArrayInverseMap(vKey4, vaIndexValue4, vMapInverse);

            TS_ASSERT(!vaIndexValue->equals(vaIndexValue2));
            TS_ASSERT(!vaIndexValue->equals(vaIndexValue3));

            // verify that indexed collection value is reused.
            TS_ASSERT(vaIndexValue->equals(vaIndexValue4));

            TS_ASSERT(!vaIndexValue2->equals(vaIndexValue3));
            TS_ASSERT(!vaIndexValue2->equals(vaIndexValue4));

            TS_ASSERT(!vaIndexValue3->equals(vaIndexValue4));
            }

        /**
        * Internal method called by test methods to verify that the given inverse
        * map contains an entry for each value (key) in the collection.  Also
        * verify that the value of each entry is a Set that contains the given key.
        *
        * @param vKey         the key being checked
        * @param vCollection  the collection which is the extracted value
        * @param vMapInverse  the index inverse map
        */
        static void assertCollectionInverseMap(Object::View vKey,
                Collection::View vCollection, SafeHashMap::View vMapInverse)
            {
            Iterator::Handle hIterator = vCollection->iterator();

            while (hIterator->hasNext())
                {
                Integer32::View sCollectionItem = cast<Integer32::View>(hIterator->next());

                Map::Entry::View vInverseEntry = vMapInverse->getEntry(sCollectionItem);

                TS_ASSERT(vInverseEntry != NULL);

                Set::View vSet = cast<Set::View>(vMapInverse->get(sCollectionItem));

                TS_ASSERT(vSet->contains(vKey));
                }
            }

        /**
        * Internal method called by test methods to verify that the given inverse
        * map contains an entry for each value (key) in the collection.  Also
        * verify that the value of each entry is a Set that contains the given key.
        *
        * @param vKey         the key being checked
        * @param vCollection  the collection which is the extracted value
        * @param vMapInverse  the index inverse map
        */
        static void assertArrayInverseMap(Object::View vKey,
                ObjectArray::View vCollection, SafeHashMap::View vMapInverse)
            {
            Iterator::Handle hIterator = cast<Iterator::Handle>(SimpleIterator::create(vCollection));

            while (hIterator->hasNext())
                {
                Integer32::View sCollectionItem = cast<Integer32::View>(hIterator->next());

                Map::Entry::View vInverseEntry = vMapInverse->getEntry(sCollectionItem);

                TS_ASSERT(vInverseEntry != NULL);

                Set::View vSet = cast<Set::View>(vMapInverse->get(sCollectionItem));

                TS_ASSERT(vSet->contains(vKey));
                }
            }

        static SimpleMapIndex::Handle createIndex(Map::Handle hMap, ValueExtractor::View vExtractor)
        {
        SimpleMapIndex::Handle hIndex = SimpleMapIndex::create(vExtractor,
                true, (Comparator::View) NULL);
        for (Iterator::Handle hIter = hMap->entrySet()->iterator(); hIter->hasNext();)
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
            hIndex->insert(vEntry);
            }
        return hIndex;
        }
    };
