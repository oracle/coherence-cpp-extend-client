/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/ComparableMin.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::ComparableMin;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the ComparableMin.
*/
class ComparableMinTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test ComparableMinTest
        */
        void testComparableMinTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    String::create("sane")));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    String::create("crazy")));
            ComparableMin::Handle hAgg = ComparableMin::create(
                (ValueExtractor::View) IdentityExtractor::getInstance());
            TS_ASSERT(String::create("crazy")->equals(
                        hAgg->aggregate(hSet)));
            hSet->clear();
            hSet->add(String::create("crazy"));
            hSet->add(String::create("zaney"));
            TS_ASSERT(String::create("crazy")->equals(
                        hAgg->aggregateResults(hSet)));
            }

        /**
        * Test ComparableMin String constructor
        */
        void testComparableMinStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();
            
            hSet->add(FakeInvocableMapEntry::create(
                String::create("Noah"),
                TestPerson::create(String::create("Noah"), (int32_t) 23)));
            hSet->add(FakeInvocableMapEntry::create(
                String::create("David"),
                TestPerson::create(String::create("David"), (int32_t) 13)));
            ComparableMin::Handle hAgg = ComparableMin::create(
                (String::View) String::create("getAge"),
                (Comparator::View) NULL);
            TS_ASSERT(hAgg->aggregate(hSet)->equals(Integer32::create(13)));
            }
    };
