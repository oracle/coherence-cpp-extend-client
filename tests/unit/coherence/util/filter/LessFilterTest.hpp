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
#include "coherence/util/filter/LessFilter.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::LessFilter;


/**
* Test Suite for the LessFilter.
*/
class LessFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test LessFilter
        */
        void testLessFilter()
            {
            Float32::View hZero = Float32::create(0.0F);
            Float32::View hOne = Float32::create(1.0F);
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            LessFilter::Handle hLF = LessFilter::create(hExtract,hOne);
            TS_ASSERT(hLF->evaluate(hZero));
            }

        /**
        * Test LessFilter.applyIndex with an unordered, complete index.
        */
        void testLessFilterApplyIndex_Unordered_Complete()
            {
            _testLessFilterApplyIndex(false, false);
            }

        /**
        * Test LessFilter.applyIndex with an unordered, partial index.
        */
        void testLessFilterApplyIndex_Unordered_Partial()
            {
            _testLessFilterApplyIndex(false, true);
            }

        /**
        * Test LessFilter.applyIndex with an ordered, complete index.
        */
        void testLessFilterApplyIndex_Ordered_Complete()
            {
            _testLessFilterApplyIndex(true, false);
            }

        /**
        * Test LessFilter.applyIndex with an ordered, partial index.
        */
        void testLessFilterApplyIndex_Ordered_Partial()
            {
            _testLessFilterApplyIndex(true, true);
            }

        private:
        void _testLessFilterApplyIndex(bool fOrdered, bool fPartial)
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            LessFilter::Handle hFilter = LessFilter::create(hExtract,
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
            TS_ASSERT(hsetResults->size() == 50);
            TS_ASSERT(hIndex->isPartial() == fPartial);

            for (int32_t i = 0; i < 50; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }

            }
    };
