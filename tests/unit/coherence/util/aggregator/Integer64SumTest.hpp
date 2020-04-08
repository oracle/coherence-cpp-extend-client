/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/Integer64Sum.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the Integer64Sum.
*/
class Integer64SumTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Integer64SumTest
        */
        void testInteger64SumTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Integer32::create(42)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Integer32::create(42)));
            Integer64Sum::Handle hAgg = Integer64Sum::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Integer64::create(84)->equals(vObj));
            hSet->clear();
            hSet->add(Integer32::create(42));
            hSet->add(Integer32::create(22));
            TS_ASSERT(Integer64::create(64)->equals(
                        hAgg->aggregateResults(hSet)));
            }

        /**
        * Test Integer64Sum String constructor
        */
        void testInteger64SumStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();
            
            hSet->add(FakeInvocableMapEntry::create(
                String::create("Noah"),
                TestPerson::create(String::create("Noah"), (int32_t) 23)));
            hSet->add(FakeInvocableMapEntry::create(
                String::create("David"),
                TestPerson::create(String::create("David"), (int32_t) 13)));
            Integer64Sum::Handle hAgg = Integer64Sum::create(
                (String::View) String::create("getAge"));
            TS_ASSERT(hAgg->aggregate(hSet)->equals(Integer64::create(36)));
            }
    };
