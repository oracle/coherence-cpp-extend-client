/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/Integer64Max.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Integer64Max;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the Integer64Max.
*/
class Integer64MaxTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Integer64MaxTest
        */
        void testInteger64MaxTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Integer32::create(22)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Integer32::create(42)));
            Integer64Max::Handle hAgg = Integer64Max::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());
            TS_ASSERT(Integer64::create(42)->equals(
                        hAgg->aggregate(hSet)));
            hSet->clear();
            hSet->add(Integer32::create(42));
            hSet->add(Integer32::create(22));
            TS_ASSERT(Integer64::create(42)->equals(
                        hAgg->aggregateResults(hSet)));
            }

        /**
        * Test Integer64MaxStringConstructor
        */
        void testInteger64MaxStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();

            hSet->add(FakeInvocableMapEntry::create(
                String::create("Noah"),
                TestPerson::create(String::create("Noah"), (int32_t) 23)));
            hSet->add(FakeInvocableMapEntry::create(
                String::create("David"),
                TestPerson::create(String::create("David"), (int32_t) 13)));
            Integer64Max::Handle hAgg = Integer64Max::create(
                (String::View) String::create("getAge"));
            TS_ASSERT(Integer64::create(23)->equals(
                        hAgg->aggregate(hSet)));
            }
    };
