/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/Float64Min.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Float64Min;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;

using namespace common::test;


/**
* Test Suite for the Float64Min.
*/
class Float64MinTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test Float64MinTest
        */
        void testFloat64MinTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Float32::create(22.0F)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Float32::create(42.0F)));
            Float64Min::Handle hAgg = Float64Min::create(
                    (ValueExtractor::View) IdentityExtractor::getInstance());
            TS_ASSERT(Float64::create(22.0)->equals(
                        hAgg->aggregate(hSet)));
            hSet->clear();
            hSet->add(Float32::create(42.0F));
            hSet->add(Float32::create(22.0F));
            TS_ASSERT(Float64::create(22.0)->equals(
                        hAgg->aggregateResults(hSet)));
            }

        /**
        * Test Float64MinStringConstructor
        */
        void testFloat64MinStringConstructor()
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
            Float64Min::Handle hAgg = Float64Min::create(
                    (String::View) String::create("getSalary"));
            TS_ASSERT(Float64::create(1000.0)->equals(
                        hAgg->aggregate(hSet)));
            }
    };
