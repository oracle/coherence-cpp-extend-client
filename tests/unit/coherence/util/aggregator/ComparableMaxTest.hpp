/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/ComparableMax.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::ComparableMax;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the ComparableMax.
*/
class ComparableMaxTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test ComparableMaxTest
        */
        void testComparableMaxTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                String::create("Noah"),
                String::create("sane")));
            hSet->add(FakeInvocableMapEntry::create(
                String::create("David"),
                String::create("crazy")));
            ComparableMax::Handle hAgg = ComparableMax::create(
                (ValueExtractor::View) IdentityExtractor::getInstance());
            TS_ASSERT(String::create("sane")->equals(
                hAgg->aggregate(hSet)));
            hSet->clear();
            hSet->add(String::create("sane"));
            hSet->add(String::create("zaney"));
            TS_ASSERT(String::create("zaney")->equals(
                hAgg->aggregateResults(hSet)));
            }

        /**
        * Test ComparableMax String constructor
        */
        void testComparableMaxStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();
            
            hSet->add(FakeInvocableMapEntry::create(
                String::create("Noah"),
                TestPerson::create(String::create("Noah"), (int32_t) 23)));
            hSet->add(FakeInvocableMapEntry::create(
                String::create("David"),
                TestPerson::create(String::create("David"), (int32_t) 13)));
            ComparableMax::Handle hAgg = ComparableMax::create(
                (String::View) String::create("getAge"),
                (Comparator::View) NULL);
            TS_ASSERT(hAgg->aggregate(hSet)->equals(Integer32::create(23)));
            }
    };
