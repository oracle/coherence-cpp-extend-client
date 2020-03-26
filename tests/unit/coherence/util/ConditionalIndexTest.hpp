/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/ConditionalIndex.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/ConditionalExtractor.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"

using namespace coherence::lang;

using coherence::util::MapIndex;
using coherence::util::extractor::ConditionalExtractor;
using coherence::util::extractor::KeyExtractor;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::LessFilter;


/**
* Test Suite for ConditionalIndex.
*/
class ConditionalIndexSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the testGetValueExtractor() method.
        */
        void testGetValueExtractor()
            {
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = GreaterFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                    vExtractor, false, (Comparator::View) NULL, true);

            TS_ASSERT(vExtractor == hIndex->getValueExtractor());
            }

        /**
        * Test the testGetFilter() method.
        */
        void testGetFilter()
            {
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = GreaterFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                    vExtractor, false, (Comparator::View) NULL, true);

            TS_ASSERT(vFilter == hIndex->getFilter());
            }

        /**
        * Test the testIsOrdered() method.
        */
        void testIsOrdered()
            {
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = GreaterFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                    vExtractor, false, (Comparator::View) NULL, true);

            TS_ASSERT(!hIndex->isOrdered());

            hIndex = ConditionalIndex::create(vFilter, vExtractor, true, (Comparator::View) NULL, true);

            TS_ASSERT(hIndex->isOrdered());
            }

        void testIsPartial()
            {
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = GreaterFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                vExtractor, false, (Comparator::View) NULL, true);

            TS_ASSERT(hIndex->isPartial() == false);

            hIndex->insert(SimpleMapEntry::create(String::create("key1"), Integer32::create(6)));

            TS_ASSERT(hIndex->isPartial() == false);

            hIndex->insert(SimpleMapEntry::create(String::create("key2"), Integer32::create(4)));

            TS_ASSERT(hIndex->isPartial());

            hIndex = ConditionalIndex::create(vFilter, vExtractor, false, (Comparator::View) NULL, true);

            TS_ASSERT(hIndex->isPartial() == false);

            hIndex->insert(SimpleMapEntry::create(String::create("key1"), Integer32::create(6)));

            TS_ASSERT(hIndex->isPartial() == false);

            hIndex->update(SimpleMapEntry::create(String::create("key1"), Integer32::create(4)));

            TS_ASSERT(hIndex->isPartial());
            }

        /**
        * Test getIndexContents.
        */
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

            ValueExtractor::View     vExtractor     = IdentityExtractor::create();
            Filter::View             vFilter        = LessFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex         = createIndex(hMap, vFilter, vExtractor, true);
            Map::View                vIndexContents = hIndex->getIndexContents();

            Set::View setOne = cast<Set::View>(vIndexContents->get(Integer32::create(1)));
            TS_ASSERT(3 == setOne->size());
            TS_ASSERT(setOne->contains(String::create("one")));
            TS_ASSERT(setOne->contains(String::create("another_one")));
            TS_ASSERT(setOne->contains(String::create("one_more")));

            Set::View setTwo = cast<Set::View>(vIndexContents->get(Integer32::create(2)));
            TS_ASSERT(1 == setTwo->size());
            TS_ASSERT(setTwo->contains(String::create("two")));

            Set::View setThree = cast<Set::View>(vIndexContents->get(Integer32::create(3)));
            TS_ASSERT(1 == setThree->size());
            TS_ASSERT(setThree->contains(String::create("three")));

            Set::View setFour = cast<Set::View>(vIndexContents->get(Integer32::create(4)));
            TS_ASSERT(2 == setFour->size());
            TS_ASSERT(setFour->contains(String::create("four")));
            TS_ASSERT(setFour->contains(String::create("four_again")));

            Set::View setFive = cast<Set::View>(vIndexContents->get(Integer32::create(5)));
            TS_ASSERT(NULL == setFive);
            }

        /**
        * Test get.
        */
        void testGet()
            {
            Map::Handle hMap = HashMap::create();

            hMap->put(String::create("one"),   Integer32::create(1));
            hMap->put(String::create("two"),   Integer32::create(2));
            hMap->put(String::create("three"), Integer32::create(3));
            hMap->put(String::create("four"),  Integer32::create(4));
            hMap->put(String::create("five"),  Integer32::create(5));

            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(5));
            ConditionalIndex::Handle hIndex     = createIndex(hMap, vFilter, vExtractor, true);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(Integer32::create(1)));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(Integer32::create(2)));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(Integer32::create(3)));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(Integer32::create(4)));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(MapIndex::getNoValue()));

            // forward map support == false
            hIndex = createIndex(hMap, vFilter, vExtractor, false);

            TS_ASSERT(hIndex->get(String::create("one"))->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(String::create("two"))->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(String::create("three"))->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(String::create("four"))->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(String::create("five"))->equals(MapIndex::getNoValue()));
            }

        /**
        * Test insert into a ConditionalIndex.  Verify the following :
        * 1) the index contains a value for a key after an insert
        * 2) if multiple equivalent values are inserted into the index
        *    for different keys, verify that only one copy of the value exists in
        *    the index
        * 3) extracted values from entries that do not pass the filter test are
        *    not added to the index
        */
        void testInsert()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, true);

            // define the keys and values for the mock entries
            Object::View oKey        = String::create("key");
            Object::View oValue      = Integer32::create(11);
            Object::View oKey2       = String::create("key2");
            Object::View oValue2     = oValue;
            Object::View oKey3       = String::create("key3");
            Object::View oValue3     = Integer32::create(25);

            // begin test

            SimpleMapEntry::View vEntry  = SimpleMapEntry::create(oKey, oValue);
            SimpleMapEntry::View vEntry2 = SimpleMapEntry::create(oKey2, oValue2);
            SimpleMapEntry::View vEntry3 = SimpleMapEntry::create(oKey3, oValue3);

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hIndex->get(oKey)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(oKey2)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(oKey3)->equals(MapIndex::getNoValue()));

            // assert the the inverse map does not contain an entry for the extracted values
            TS_ASSERT(!hIndex->getIndexContents()->containsKey(oValue));
            TS_ASSERT(!hIndex->getIndexContents()->containsKey(oValue2));

            // insert into the index
            hIndex->insert(vEntry);
            hIndex->insert(vEntry2);
            hIndex->insert(vEntry3);

            // verify the value from the index for key
            Object::View oIndexValue = hIndex->get(oKey);
            TS_ASSERT(oValue->equals(oIndexValue));

            // verify the value from the index for key2
            Object::Holder oIndexValue2 = hIndex->get(oKey2);
            TS_ASSERT(oValue2->equals(oIndexValue2));

            // verify no value in the index for key3
            Object::Holder oIndexValue3 = hIndex->get(oKey3);
            TS_ASSERT(oIndexValue3->equals(MapIndex::getNoValue()));

            // verify that the value for key and key2 is the same instance
            TS_ASSERT(oIndexValue->equals(oIndexValue2));

            // get the inverse map
            Map::View vMapInverse = hIndex->getIndexContents();

            // get the set of keys from the inverse map keyed by the extracted
            // value for key
            Set::View vSet = cast<Set::View>(vMapInverse->get(oIndexValue));

            // verify that the set of keys contains key
            TS_ASSERT(vSet->contains(oKey));

            // get the set of keys from the inverse map keyed by the extracted
            // value for key2
            vSet = cast<Set::View>(vMapInverse->get(oIndexValue2));

            // verify that the set of keys contains key2
            TS_ASSERT(vSet->contains(oKey2));
            }

        /**
        * Test insert into a ConditionalIndex.  Verify the following :
        * 1) the index contains a value for a key after an insert
        * 2) if multiple equivalent values are inserted into the index
        *    for different keys, verify that only one copy of the value exists in
        *    the index
        * 3) extracted values from entries that do not pass the filter test are
        *    not added to the index
        */
        void testInsert_forwardIndexFalse()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hIndex     = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, false);

            // define the keys and values for the mock entries
            Object::View oKey    = String::create("key");
            Object::View oValue  = Integer32::create(1);
            Object::View oKey2   = String::create("key2");
            Object::View oValue2 = Integer32::create(2);
            Object::View oKey3   = String::create("key3");
            Object::View oValue3 = Integer32::create(25);

            SimpleMapEntry::View vEntry  = SimpleMapEntry::create(oKey, oValue);
            SimpleMapEntry::View vEntry2 = SimpleMapEntry::create(oKey2, oValue2);
            SimpleMapEntry::View vEntry3 = SimpleMapEntry::create(oKey3, oValue3);

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hIndex->get(oKey)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(oKey2)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hIndex->get(oKey3)->equals(MapIndex::getNoValue()));

            // insert into the index
            hIndex->insert(vEntry);
            hIndex->insert(vEntry2);
            hIndex->insert(vEntry3);

            // all gets should return NO_VALUE since forward index is not supported
            // verify no value from the index for key
            Object::View oIndexValue = hIndex->get(oKey);
            TS_ASSERT(oIndexValue->equals(MapIndex::getNoValue()));

            // verify no value from the index for key2
            Object::View oIndexValue2 = hIndex->get(oKey2);
            TS_ASSERT(oIndexValue2->equals(MapIndex::getNoValue()));

            // verify no value in the index for key3
            Object::View oIndexValue3 = hIndex->get(oKey3);
            TS_ASSERT(oIndexValue3->equals(MapIndex::getNoValue()));

            // verify that the value for key and key2 is the same instance
            TS_ASSERT(oIndexValue->equals(oIndexValue2));

            // get the inverse map
            Map::View vMapInverse = hIndex->getIndexContents();

            // get the set of keys from the inverse map keyed by the extracted
            // value for key
            Set::View vSet = cast<Set::View>(vMapInverse->get(oValue));

            // verify that the set of keys contains key
            TS_ASSERT(vSet->contains(oKey));

            vSet = cast<Set::View>(vMapInverse->get(oValue2));
            TS_ASSERT(vSet->contains(oKey2));
            }

        /**
        * Test update on a ConditionalIndex.  Verify the following :
        * 1) the index contains the new value for a key after an update
        * 2) if multiple equivalent values are inserted into the index
        *    for different keys, verify that only one copy of the value exists in
        *    the index
        * 3) extracted values from entries that do not pass the filter test are
        *    not added to the index
        */
        void testUpdate()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hMapIndex  = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, true);

            // define the keys and values
            Object::View oKey           = String::create("key");
            Object::View oValue         = Integer32::create(0);
            Object::View oExtracted     = Integer32::create(0);
            Object::View oKey2          = String::create("key2");
            Object::View oValue2        = Integer32::create(11);
            Object::View oExtracted2    = Integer32::create(11);
            Object::View oKey3          = String::create("key3");
            Object::View oValue3        = Integer32::create(21);
            Object::View oExtracted3    = Integer32::create(21);
            Object::View oNewValue      = oValue2;
            Object::View oExtractedNew  = Integer32::create(11);
            Object::View oNewValue2     = Integer32::create(30);
            Object::View oExtractedNew2 = Integer32::create(30);

            SimpleMapEntry::View vEntry     = SimpleMapEntry::create(oKey, oValue);
            SimpleMapEntry::View vEntry2    = SimpleMapEntry::create(oKey2, oValue2);
            SimpleMapEntry::View vEntry3    = SimpleMapEntry::create(oKey3, oValue3);
            SimpleMapEntry::View vEntryNew  = SimpleMapEntry::create(oKey, oNewValue, oValue);
            SimpleMapEntry::View vEntryNew2 = SimpleMapEntry::create(oKey2, oNewValue2, oValue2);

            // begin test

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hMapIndex->get(oKey)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hMapIndex->get(oKey2)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hMapIndex->get(oKey3)->equals(MapIndex::getNoValue()));

            // insert into the index
            hMapIndex->insert(vEntry);   //key  (extracted value : 0)
            hMapIndex->insert(vEntry2);  //key2 (extracted value : 2)
            hMapIndex->insert(vEntry3);  //key3 (extracted value : 21)

            // verify the value from the index for key
            Object::View oIndexValue = hMapIndex->get(oKey);
            TS_ASSERT(oExtracted->equals(oIndexValue));

            // verify the value from the index for key2
            Object::View oIndexValue2 = hMapIndex->get(oKey2);
            TS_ASSERT(oExtracted2->equals(oIndexValue2));

            // since the extracted value (21) for key3 fails the filter check
            // LessFilter(vExtractor, Integer32::create(15)), it should not be part of
            // the index
            Object::View oIndexValue3 = hMapIndex->get(oKey3);
            TS_ASSERT(oIndexValue3->equals(MapIndex::getNoValue()));

            // get the inverse map
            Map::View vMapInverse = hMapIndex->getIndexContents();

            // assert the the inverse map does contain an entry for the
            // extracted values for key
            TS_ASSERT(vMapInverse->containsKey(oExtracted));

            // assert that the set mapped to the extracted value for key contains
            // key
            Set::View vSet = cast<Set::View>(vMapInverse->get(oExtracted));
            TS_ASSERT(vSet->contains(oKey));

            // assert the the inverse map does contain an entry for the
            // extracted values for key2
            TS_ASSERT(vMapInverse->containsKey(oExtracted2));

            // assert that the set mapped to the extracted value for key2 contains
            // key2
            vSet = cast<Set::View>(vMapInverse->get(oExtracted2));
            TS_ASSERT(vSet->contains(oKey2));

            // assert the the inverse map does not contain an entry for the
            // extracted value for key3
            TS_ASSERT(!vMapInverse->containsKey(oExtracted3));

            // update the index
            hMapIndex->update(vEntryNew);   // key  (extracted value : 11)
            hMapIndex->update(vEntryNew2);  // key2 (extracted value : 30)

            // assert the the index now contains the updated value for key
            oIndexValue = hMapIndex->get(oKey);
            TS_ASSERT(oExtractedNew->equals(oIndexValue));

            // assert that the instance for the extracted value 11 is reused
            TS_ASSERT(oIndexValue->equals(oIndexValue2));

            // verify the value for key2 is no longer available from the index
            // since the updated extracted value (30) for key2 fails the filter
            // check : LessFilter(vExtractor, Integer32::create(15)), it should not be
            // part of the index
            oIndexValue2 = hMapIndex->get(oKey2);
            TS_ASSERT(oIndexValue2->equals(MapIndex::getNoValue()));

            // assert the inverse map does contain an entry for the
            // extracted value for key
            vMapInverse = hMapIndex->getIndexContents();
            TS_ASSERT(vMapInverse->containsKey(oExtractedNew));

            // assert that the set mapped to the old extracted value for key
            // no longer contains key... result of update
            vSet = cast<Set::View>(vMapInverse->get(oExtracted));
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey));

            // assert that the set mapped to the extracted value for key contains
            // key
            vSet = cast<Set::View>(vMapInverse->get(oExtractedNew));
            TS_ASSERT(vSet->contains(oKey));

            // assert that the set mapped to the old extracted value for key2
            // no longer contains key2... result of update
            vSet = cast<Set::View>(vMapInverse->get(oExtracted2));
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey2));

            // assert the the inverse map does not contain an entry for the new
            // extracted value for key2... fails filter check
            vSet = cast<Set::View>(vMapInverse->get(oExtractedNew2));
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey2));
            }

        /**
        * Test update on a ConditionalIndex.  Verify the following :
        * 1) the index contains the new value for a key after an update
        * 2) if multiple equivalent values are inserted into the index
        *    for different keys, verify that only one copy of the value exists in
        *    the index
        * 3) extracted values from entries that do not pass the filter test are
        *    not added to the index
        * 4) keys are no longer associated with the old extracted values in the
        *    inverse mapping after the update
        */
        void testUpdate_forwardIndexFalse()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hMapIndex  = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, false);

            // define the keys and values
            Object::View oKey           = String::create("key");
            Object::View oValue         = Integer32::create(0);
            Object::View oExtracted     = Integer32::create(0);
            Object::View oNewValue      = Integer32::create(1);
            Object::View oExtractedNew  = Integer32::create(1);
            Object::View oKey2          = String::create("key2");
            Object::View oValue2        = Integer32::create(2);
            Object::View oExtracted2    = Integer32::create(2);
            Object::View oKey3          = String::create("key3");
            Object::View oValue3        = Integer32::create(21);
            Object::View oNewValue2     = Integer32::create(4);
            Object::View oExtractedNew2 = Integer32::create(4);

            SimpleMapEntry::View vEntry     = SimpleMapEntry::create(oKey, oValue);
            SimpleMapEntry::View vEntry2    = SimpleMapEntry::create(oKey2, oValue2);
            SimpleMapEntry::View vEntry3    = SimpleMapEntry::create(oKey3, oValue3);
            SimpleMapEntry::View vEntryNew  = SimpleMapEntry::create(oKey, oNewValue, oValue);
            SimpleMapEntry::View vEntryNew2 = SimpleMapEntry::create(oKey2, oNewValue2, oValue2);

            // begin test

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hMapIndex->get(oKey)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hMapIndex->get(oKey2)->equals(MapIndex::getNoValue()));
            TS_ASSERT(hMapIndex->get(oKey3)->equals(MapIndex::getNoValue()));

            // insert into the index
            hMapIndex->insert(vEntry);  // key, oExtracted
            hMapIndex->insert(vEntry2); // key, oExtracted2
            hMapIndex->insert(vEntry3); // key, oExtracted3

            // all gets should return NO_VALUE since forward index is not supported
            // verify no value from the index for key
            Object::View oIndexValue = hMapIndex->get(oKey);
            TS_ASSERT(oIndexValue->equals(MapIndex::getNoValue()));

            TS_ASSERT(!vEntry->isOriginalPresent());

            // verify no value from the index for key2
            Object::View oIndexValue2 = hMapIndex->get(oKey2);
            TS_ASSERT(oIndexValue2->equals(MapIndex::getNoValue()));

            // verify no value in the index for key3
            Object::View oIndexValue3 = hMapIndex->get(oKey3);
            TS_ASSERT(oIndexValue3->equals(MapIndex::getNoValue()));

            // update the index
            hMapIndex->update(vEntryNew);   // key, oExtractedNew
            hMapIndex->update(vEntryNew2);  // key2, oExtractedNew2

            // all gets should return NO_VALUE since forward index is not supported
            // verify no value from the index for key
            oIndexValue = hMapIndex->get(oKey);
            TS_ASSERT(oIndexValue->equals(MapIndex::getNoValue()));

            TS_ASSERT(vEntryNew->isOriginalPresent());

            oIndexValue2 = hMapIndex->get(oKey2);
            TS_ASSERT(oIndexValue2->equals(MapIndex::getNoValue()));

            // get the inverse map
            Map::View vMapInverse = hMapIndex->getIndexContents();

            // get the set of keys from the inverse map keyed by the extracted
            // value for key
            Set::View vSet = cast<Set::View>(vMapInverse->get(oExtractedNew));

            // verify that the set of keys contains key
            TS_ASSERT(vSet->contains(oKey));

            // get the set of keys from the inverse map keyed by the old extracted
            // value for key
            vSet = cast<Set::View>(vMapInverse->get(oExtracted));

            // verify that the set of keys does not contain key
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey));

            // get the set of keys from the inverse map keyed by the extracted
            // value for key2
            vSet = cast<Set::View>(vMapInverse->get(oExtractedNew2));

            // verify that the set of keys contains key2
            TS_ASSERT(vSet->contains(oKey2));

            // get the set of keys from the inverse map keyed by the old extracted
            // value for key2
            vSet = cast<Set::View>(vMapInverse->get(oExtracted2));

            // verify that the set of keys does not contain key2
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey2));
        }

        /**
        * Test delete from a ConditionalIndex.  Verify that the index does not
        * contain a value for a key after an delete.
        */
        void testDelete()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hMapIndex  = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, true);

            // define the keys and values
            Object::View oKey       = String::create("key");
            Object::View oValue     = Integer32::create(1);
            Object::View oExtracted = Integer32::create(1);

            SimpleMapEntry::View vEntry = SimpleMapEntry::create(oKey, oValue, oValue);

            // begin test

            // verify that the index does not contain a value for the tested keys
            TS_ASSERT(hMapIndex->get(oKey)->equals(MapIndex::getNoValue()));

            // insert into the index
            hMapIndex->insert(vEntry);

            Object::View extractedValue = hMapIndex->get(oKey);
            TS_ASSERT(oExtracted->equals(extractedValue));

            hMapIndex->remove(vEntry);

            TS_ASSERT(hMapIndex->get(oKey)->equals(MapIndex::getNoValue()));
            }

        /**
        * Test delete from a ConditionalIndex.  Verify that the index does not
        * contain a value for a key after an delete.
        */
        void testDelete_forwardIndexFalse()
            {
            // create the ConditionalIndex to be tested
            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = LessFilter::create(vExtractor, Integer32::create(15));
            ConditionalIndex::Handle hMapIndex  = ConditionalIndex::create(vFilter,
                    vExtractor, true, (Comparator::View) NULL, false);

            // define the keys and values for the mock entries
            Object::View oKey          = String::create("key");
            Object::View oValue        = Integer32::create(1);
            Object::View oExtracted    = Integer32::create(1);

            SimpleMapEntry::View vEntry    = SimpleMapEntry::create(oKey, oValue);
            SimpleMapEntry::View vDelEntry = SimpleMapEntry::create(oKey, oValue, oValue);

            // begin test

            // assert the the inverse map does not contain an entry for the extracted value
            TS_ASSERT(!hMapIndex->getIndexContents()->containsKey(oExtracted));

            // insert into the index
            hMapIndex->insert(vEntry);

            // assert the the inverse map does contain an entry for the extracted value
            TS_ASSERT(hMapIndex->getIndexContents()->containsKey(oExtracted));

            hMapIndex->remove(vDelEntry);

            // get the inverse map
            Map::View vMapInverse = hMapIndex->getIndexContents();

            // get the set of keys from the inverse map keyed by the extracted
            // value for key
            Set::View vSet = cast<Set::View>(vMapInverse->get(oExtracted));

            // verify that the set of keys does not contain key
            TS_ASSERT(vSet == NULL || !vSet->contains(oKey));
            }

        /**
        * Test index creation with a dataset larger than 50. This is to test the
        * fix done in COH-4954 that uses a threshold of 50 to determine whether to
        * scan the forward index.
        */
        void testCoh4954()
            {
            Map::Handle hMap = HashMap::create();
            Set::Handle hSet = HashSet::create();
            hSet->add(Integer32::create(1));
            for (int i = 0; i < 52; i++)
                {
                hMap->put(Integer32::create(i), hSet);
                }

            ValueExtractor::View     vExtractor = IdentityExtractor::create();
            Filter::View             vFilter    = AlwaysFilter::create();
            ConditionalIndex::Handle hIndex     = createIndex(hMap, vFilter, vExtractor, true);

            // forward map support == false
            hIndex = createIndex(hMap, vFilter, vExtractor, false);
            }

    private:
        static ConditionalIndex::Handle createIndex(Map::Handle hMap,
            Filter::View vFilter, ValueExtractor::View vExtractor, bool fSupportForwardMap)
            {
                ConditionalIndex::Handle hIndex = ConditionalIndex::create(vFilter,
                        vExtractor, false, (Comparator::View) NULL, fSupportForwardMap);

                Set::Handle hEntrySet = hMap->entrySet();
                for (Iterator::Handle hIter = hEntrySet->iterator(); hIter->hasNext();)
                {
                    Map::Entry::View vE = cast<Map::Entry::View>(hIter->next());
                    hIndex->insert(vE);
                }
                return hIndex;
            }

    };

