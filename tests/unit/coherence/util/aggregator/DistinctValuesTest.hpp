/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/aggregator/DistinctValues.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/ValueUpdater.hpp"
#include "fake/aggregator/FakeInvocableMapEntry.hpp"


using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::DistinctValues;
using coherence::util::HashSet;
using coherence::util::ValueExtractor;
using coherence::util::ValueUpdater;
using fake::aggregator::FakeInvocableMapEntry;


/**
* Test Suite for the DistinctValues.
*/
class DistinctValuesTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test DistinctValuesTest
        */
        void testDistinctValuesTest()
            {
            HashSet::Handle hSet = HashSet::create();
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("Noah"),
                    Integer32::create(42)));
            hSet->add(FakeInvocableMapEntry::create(
                    String::create("David"),
                    Integer32::create(42)));
            DistinctValues::Handle hAgg = DistinctValues::create(
                        IdentityExtractor::getInstance());
            Set::View vSet = cast<Set::View>(hAgg->aggregate(hSet));
            TS_ASSERT(vSet->size() == 1);
            }
    };
