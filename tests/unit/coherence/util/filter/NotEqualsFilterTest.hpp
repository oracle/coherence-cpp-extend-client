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
#include "coherence/util/filter/NotEqualsFilter.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::NotEqualsFilter;


/**
* Test Suite for the NotEqualsFilter.
*/
class NotEqualsFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test NotEqualsFilter
        */
        void testNotEqualsFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            NotEqualsFilter::Handle hFilter = NotEqualsFilter::create(hExtract,
                        Float32::create(42.0F));
            TS_ASSERT(hFilter->evaluate(Float32::create(41.0F)));
            }

        /**
        * Test NotEqualsFilter.applyIndex with an unordered, complete index.
        */
        void testNotEqualsFilterApplyIndex_Unordered_Complete()
            {
            testNotEqualsFilterApplyIndex(false, false);
            }

        /**
        * Test NotEqualsFilter.applyIndex with an unordered, partial index.
        */
        void testNotEqualsFilterApplyIndex_Unordered_Partial()
            {
            testNotEqualsFilterApplyIndex(false, true);
            }

        /**
        * Test NotEqualsFilter.applyIndex with an ordered, complete index.
        */
        void testNotEqualsFilterApplyIndex_Ordered_Complete()
            {
            testNotEqualsFilterApplyIndex(true, false);
            }

        /**
        * Test NotEqualsFilter.applyIndex with an ordered, partial index.
        */
        void testNotEqualsFilterApplyIndex_Ordered_Partial()
            {
            testNotEqualsFilterApplyIndex(true, true);
            }

        private:
        void testNotEqualsFilterApplyIndex(bool fOrdered, bool fPartial)
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            NotEqualsFilter::Handle hFilter = NotEqualsFilter::create(hExtract,
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
            TS_ASSERT(hsetResults->size() == 99);
            TS_ASSERT(hIndex->isPartial() == fPartial);

            for (int32_t i = 0; i < 49; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }

            for (int32_t i = 51; i < 100; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }
            }

    };
