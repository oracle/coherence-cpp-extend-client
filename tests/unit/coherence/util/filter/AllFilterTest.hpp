/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/tests/TestMapIndex.hpp"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/Set.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/AllFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

using namespace coherence::lang;

using coherence::tests::TestMapIndex;

using coherence::util::Comparator;
using coherence::util::Filter;
using coherence::util::HashMap;
using coherence::util::HashSet;
using coherence::util::MapIndex;
using coherence::util::Set;
using coherence::util::SimpleMapEntry;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AllFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::LessFilter;


/**
* Test Suite for the AllFilter.
*/
class AllFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test AllFilter
        */
        void testAllFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            ObjectArray::Handle haFilters    = ObjectArray::create(3);
            haFilters[0] = GreaterFilter::create(hExtract,
                        Float32::create(0.0F));
            haFilters[1] = LessFilter::create(hExtract,
                        Float32::create(100.0F));
            haFilters[2] = EqualsFilter::create(hExtract,
                        Float32::create(50.0F));
            AllFilter::Handle hFilter = AllFilter::create(haFilters);
            TS_ASSERT(hFilter->evaluate(Float32::create(50.0F)));
            }

        /**
        * Test AllFilter.applyIndex with an unordered, complete index.
        */
        void testAllFilterApplyIndex_Unordered_Complete()
            {
            _testAllFilterApplyIndex(false, false);
            }

        /**
        * Test AllFilter.applyIndex with an unordered, partial index.
        */
        void testAllFilterApplyIndex_Unordered_Partial()
            {
            _testAllFilterApplyIndex(false, true);
            }

        /**
        * Test AllFilter.applyIndex with an ordered, complete index.
        */
        void testAllFilterApplyIndex_Ordered_Complete()
            {
            _testAllFilterApplyIndex(true, false);
            }

        /**
        * Test AllFilter.applyIndex with an ordered, partial index.
        */
        void testAllFilterApplyIndex_Ordered_Partial()
            {
            _testAllFilterApplyIndex(true, true);
            }

        private:

        void _testAllFilterApplyIndex(bool fOrdered, bool fPartial)
            {

            IdentityExtractor::View hExtract = IdentityExtractor::create();
            GreaterFilter::Handle hGFilter   = GreaterFilter::create(hExtract,
                    Integer32::create(50));
            LessFilter::Handle hLFilter      = LessFilter::create(hExtract,
                    Integer32::create(60));
            ObjectArray::Handle haFilters    = ObjectArray::create(2);
            haFilters[0] = hGFilter;
            haFilters[1] = hLFilter;
            AllFilter::Handle hFilter        = AllFilter::create(haFilters);

            Map::Handle      hMapIndexes = HashMap::create();
            Set::Handle      hSetResults = HashSet::create();
            MapIndex::Handle hIndex      = TestMapIndex::create(hExtract,
                    fOrdered, fPartial,(Comparator::View)NULL);

            for (int32_t i = 0; i < 100; ++i)
                {
                Object::Handle oKey   = Integer32::create(i);
                Object::Handle oValue = Integer32::create(i);
                hIndex->insert(SimpleMapEntry::create(oKey, oValue));
                hSetResults->add(oKey);
                }
            hMapIndexes->put(hExtract, hIndex);

            Filter::View vFilterReturn = hFilter->applyIndex(hMapIndexes, hSetResults);

            TS_ASSERT(vFilterReturn == NULL);
            TS_ASSERT(hSetResults->size() == 9);
            TS_ASSERT(hIndex->isPartial() == fPartial);

            for (int32_t i = 51; i < 60; ++i)
                {
                TS_ASSERT(hSetResults->contains(Integer32::create(i)));
                }
            }

    };
