/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"

using namespace coherence::lang;

using coherence::util::SimpleMapIndex;
using coherence::util::filter::AlwaysFilter;


/**
* Test Suite for the AlwaysFilter.
*/
class AlwaysFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test AlwaysFilter
        */
        void testAlwaysFilter()
            {
            Float32::View hZero = Float32::create(0.0F);
            AlwaysFilter::Handle hLF = AlwaysFilter::create();
            TS_ASSERT(hLF->evaluate(hZero));
            }

        /**
        * Test AlwaysFilter.applyIndex
        */
        void testAlwaysFilterApplyIndex()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            AlwaysFilter::Handle hFilter = AlwaysFilter::create();
            TS_ASSERT(hFilter->evaluate(Integer32::create(5)));


            Map::Handle          hmapIndexes = HashMap::create();
            SimpleMapIndex::Handle hIndex    = SimpleMapIndex::create(hExtract, false, (Comparator::View)NULL);
            Set::Handle hsetResults = HashSet::create();
            for (int32_t i = 0; i < 100; ++i)
                {
                Object::Handle oKey   = Integer32::create(i);
                Object::Handle oValue = Integer32::create(i % 10);
                hIndex->insert(SimpleMapEntry::create(oKey, oValue));
                hsetResults->add(oKey);
                }
            hmapIndexes->put(hExtract, hIndex);

            Filter::View vFilterReturn = hFilter->applyIndex(hmapIndexes, hsetResults);

            TS_ASSERT(vFilterReturn == NULL);
            TS_ASSERT(hsetResults->size() == 100);
            for (int32_t i = 0; i < 100; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }
            }
    };
