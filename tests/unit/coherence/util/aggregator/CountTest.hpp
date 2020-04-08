/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/Count.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Count;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;


/**
* Test Suite for the Count.
*/
class CountTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test CountTest
        */
        void testCountTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Integer32::create(42)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Integer32::create(42)));
            Count::Handle hAgg = Count::create();
            Object::View vObj = hAgg->aggregate(hSet);
            TS_ASSERT(Integer32::create(2)->equals(vObj));
            hSet->clear();
            hSet->add(Integer32::create(42));
            hSet->add(Integer32::create(22));
            TS_ASSERT(Integer32::create(64)->equals(
                        hAgg->aggregateResults(hSet)));
            }
    };
