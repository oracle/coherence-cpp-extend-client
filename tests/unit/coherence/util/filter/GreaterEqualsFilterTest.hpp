/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/tests/TestMapIndex.hpp"

#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::GreaterEqualsFilter;


/**
* Test Suite for the GreaterEqualsFilter.
*/
class GreaterEqualsFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test GreaterEqualsFilter
        */
        void testGreaterEqualFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            GreaterEqualsFilter::View hFilter = GreaterEqualsFilter::create(
                        hExtract, Float32::create(5.0F));
            TS_ASSERT( hFilter->evaluate(Float32::create(7.5F)));
            }

        /**
        * Test GreaterEqualsFilter.applyIndex with an unordered, complete index.
        */
        void testGreaterEqualsFilterApplyIndex_Unordered_Complete()
            {
            _testGreaterEqualsFilterApplyIndex(false, false);
            }

        /**
        * Test GreaterEqualsFilter.applyIndex with an unordered, partial index.
        */
        void testGreaterEqualsFilterApplyIndex_Unordered_Partial()
            {
            _testGreaterEqualsFilterApplyIndex(false, true);
            }

        /**
        * Test GreaterEqualsFilter.applyIndex with an ordered, complete index.
        */
        void testGreaterEqualsFilterApplyIndex_Ordered_Complete()
            {
            _testGreaterEqualsFilterApplyIndex(true, false);
            }

        /**
        * Test GreaterEqualsFilter.applyIndex with an ordered, partial index.
        */
        void testGreaterEqualsFilterApplyIndex_Ordered_Partial()
            {
            _testGreaterEqualsFilterApplyIndex(true, true);
            }


        private:

        void _testGreaterEqualsFilterApplyIndex(bool fOrdered, bool fPartial)
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            GreaterEqualsFilter::Handle hFilter = GreaterEqualsFilter::create(hExtract,
                    Integer32::create(50));
            TS_ASSERT(hFilter->evaluate(Integer32::create(50)));


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

            for (int32_t i = 50; i < 100; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }

            }

    };
