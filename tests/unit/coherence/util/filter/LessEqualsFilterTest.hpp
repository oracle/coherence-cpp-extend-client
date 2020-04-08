/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/tests/TestMapIndex.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::LessEqualsFilter;


/**
* Test Suite for the LessEqualsFilter.
*/
class LessEqualsFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test LessEqualsFilter
        */
        void testGreaterEqualFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            LessEqualsFilter::View hFilter = LessEqualsFilter::create(
                        hExtract, Float32::create(10.0F));
            TS_ASSERT( hFilter->evaluate(Float32::create(7.5F)));
            }

        /**
        * Test LessEqualsFilter.applyIndex with an unordered, complete index.
        */
        void testLessEqualsFilterApplyIndex_Unordered_Complete()
            {
            _testLessEqualsFilterApplyIndex(false, false);
            }

        /**
        * Test LessEqualsFilter.applyIndex with an unordered, partial index.
        */
        void testLessEqualsFilterApplyIndex_Unordered_Partial()
            {
            _testLessEqualsFilterApplyIndex(false, true);
            }

        /**
        * Test LessEqualsFilter.applyIndex with an ordered, complete index.
        */
        void testLessEqualsFilterApplyIndex_Ordered_Complete()
            {
            _testLessEqualsFilterApplyIndex(true, false);
            }

        /**
        * Test LessEqualsFilter.applyIndex with an ordered, partial index.
        */
        void testLessEqualsFilterApplyIndex_Ordered_Partial()
            {
            _testLessEqualsFilterApplyIndex(true, true);
            }

        private:
        void _testLessEqualsFilterApplyIndex(bool fOrdered, bool fPartial)
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            LessEqualsFilter::Handle hFilter = LessEqualsFilter::create(hExtract,
                    Integer32::create(50));

            Map::Handle      hmapIndexes = HashMap::create();
            Set::Handle      hsetResults = HashSet::create();
            MapIndex::Handle hIndex      = TestMapIndex::create(hExtract,
                    fOrdered, fPartial,(Comparator::View)NULL);

            for (int32_t i = 0; i < 100; ++i)
                {
                Object::Handle oKey   = Integer32::create(i);
                Object::Handle oValue = Integer32::create(i);
                hIndex->insert(SimpleMapEntry::create(oKey, oValue));
                hsetResults->add(oKey);
                }
            hmapIndexes->put(hExtract, hIndex);

            Filter::View vFilterReturn = hFilter->applyIndex(hmapIndexes, hsetResults);

            TS_ASSERT(vFilterReturn == NULL);
            TS_ASSERT(hsetResults->size() == 51);
            TS_ASSERT(hIndex->isPartial() == fPartial);

            for (int32_t i = 0; i <= 50; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }

            }
    };
