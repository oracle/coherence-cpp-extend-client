/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/ContainsAllFilter.hpp"
#include "coherence/util/HashSet.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::ContainsAllFilter;
using coherence::util::HashSet;
using coherence::util::SimpleMapIndex;


/**
* Test Suite for the ContainsAllFilter.
*/
class ContainsAllFilterSuite : public CxxTest::TestSuite
	{
	public:
        /**
        * Test ContainsAllFilter
        */
        void testContainsAllFilter()
            {
            HashSet::Handle hHSValue = HashSet::create();
            hHSValue->add(String::create("Noah"));
            hHSValue->add(String::create("Mark"));
            ContainsAllFilter::Handle hFilter = ContainsAllFilter::create(
                        IdentityExtractor::getInstance(), hHSValue);
            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Jason"));
            hHS->add(String::create("Noah"));
             TS_ASSERT(!hFilter->evaluate(hHS));
            hHS->add(String::create("Mark"));
            TS_ASSERT(hFilter->evaluate(hHS));

            ObjectArray::Handle haStrings = ObjectArray::create(3);
            haStrings[0] = String::create("David");
            haStrings[1] = String::create("Noah");
            haStrings[2] = String::create("Jason");
            TS_ASSERT(!hFilter->evaluate(haStrings));

            haStrings[2] = String::create("Mark");
            TS_ASSERT(hFilter->evaluate(haStrings));

            }

        /**
        * Test ContainsAllFilter.applyIndex
        */
        void testContainsAllFilterApplyIndex()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();

            HashSet::Handle hSet = HashSet::create();
            hSet->add(String::create("Monkey"));
            hSet->add(String::create("Runner"));

            ContainsAllFilter::Handle hFilter = ContainsAllFilter::create(hExtract, hSet);

            Map::Handle          hmapIndexes = HashMap::create();
            SimpleMapIndex::Handle hIndex    = SimpleMapIndex::create(hExtract, false, (Comparator::View)NULL);
            Set::Handle hsetResults = HashSet::create();


            HashSet::Handle hValue1 = HashSet::create();
            hValue1->add(String::create("Monkey"));
            hValue1->add(String::create("Star"));
            Object::Handle oKey1 = Integer32::create(1);
            hIndex->insert(SimpleMapEntry::create(oKey1, hValue1));
            hsetResults->add(oKey1);

            HashSet::Handle hValue2 = HashSet::create();
            hValue2->add(String::create("Runner"));
            hValue2->add(String::create("Pancake"));
            hValue2->add(String::create("Monkey"));
            Object::Handle oKey2 = Integer32::create(2);
            hIndex->insert(SimpleMapEntry::create(oKey2, hValue2));
            hsetResults->add(oKey2);

            HashSet::Handle hValue3 = HashSet::create();
            hValue3->add(String::create("Picture"));
            hValue3->add(String::create("Mouse"));
            Object::Handle oKey3 = Integer32::create(3);
            hIndex->insert(SimpleMapEntry::create(oKey3, hValue3));
            hsetResults->add(oKey3);

            hmapIndexes->put(hExtract, hIndex);

            Filter::View vFilterReturn = hFilter->applyIndex(hmapIndexes, hsetResults);

            TS_ASSERT(vFilterReturn == NULL);
            TS_ASSERT(hsetResults->size() == 1);

            TS_ASSERT(hsetResults->contains(Integer32::create(2)));
            }

	};
