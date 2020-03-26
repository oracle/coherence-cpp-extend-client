/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/Float64Sum.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Float64Sum;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the Float64Sum.
*/
class Float64SumTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Float64SumTest
        */
        void testFloat64SumTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Float32::create(22.0F)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Float32::create(42.0F)));
            Float64Sum::Handle hAgg = Float64Sum::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Float64::create(64.0)->equals(vObj));
            hSet->clear();
            hSet->add(Float32::create(42.0F));
            hSet->add(Float32::create(22.0F));
            TS_ASSERT(Float64::create(64.0)->equals(
                        hAgg->aggregateResults(hSet)));
            }

        /**
        * Test Float64SumStringConstructor
        */
        void testFloat64SumStringConstructor()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    SalariedPerson::create(String::create("Noah"),
                        (int32_t) 23, (float64_t) 1000.0)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    SalariedPerson::create(String::create("David"),
                        (int32_t) 42, (float64_t) 200000.0)));
            Float64Sum::Handle hAgg = Float64Sum::create(
                    (String::View) String::create("getSalary"));
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Float64::create(201000.0)->equals(vObj));
            }
    };
