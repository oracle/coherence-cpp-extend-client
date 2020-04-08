/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

using namespace coherence::lang;

using coherence::util::SimpleMapEntry;
using coherence::util::SimpleMapIndex;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::EqualsFilter;


/**
* Test Suite for the EqualsFilter.
*/
class EqualsFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test EqualsFilter
        */
        void testEqualsFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            EqualsFilter::Handle hFilter = EqualsFilter::create(hExtract,
                        Float32::create(42.0F));
            TS_ASSERT(hFilter->evaluate(Float32::create(42.0F)));
            }

        /**
        * Test EqualsFilter.applyIndex
        */
        void testEqualsFilterApplyIndex()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            EqualsFilter::Handle hFilter = EqualsFilter::create(hExtract,
                    Integer32::create(5));
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
            TS_ASSERT(hsetResults->size() == 10);
            for (int32_t i = 5; i < 100; i+=10)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }
            }

    };
