/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/filter/AnyFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"

using namespace coherence::lang;

using coherence::util::SimpleMapIndex;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AnyFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::LessFilter;


/**
* Test Suite for the AnyFilter.
*/
class AnyFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test AnyFilter
        */
        void testAnyFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            ObjectArray::Handle haFilters = ObjectArray::create(3);
            haFilters[0] = GreaterFilter::create(hExtract,
                        Float32::create(0.0F));
            haFilters[1] = LessFilter::create(hExtract,
                        Float32::create(-100.0F));;
            haFilters[2] = EqualsFilter::create(hExtract,
                        Float32::create(-50.0F));;
            AnyFilter::Handle hFilter = AnyFilter::create(haFilters);
            TS_ASSERT(hFilter->evaluate(Float32::create(-50.0F)));
            }

        /**
        * Test AnyFilter.applyIndex
        */
        void testAnyFilterApplyIndex()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            GreaterFilter::Handle hGFilter = GreaterFilter::create(hExtract,
                    Integer32::create(90));
            LessFilter::Handle hLFilter = LessFilter::create(hExtract,
                    Integer32::create(10));
            ObjectArray::Handle haFilters = ObjectArray::create(2);
            haFilters[0] = hGFilter;
            haFilters[1] = hLFilter;

            AnyFilter::Handle hFilter = AnyFilter::create(haFilters);

            Map::Handle          hmapIndexes = HashMap::create();
            SimpleMapIndex::Handle hIndex    = SimpleMapIndex::create(hExtract, false, (Comparator::View)NULL);
            Set::Handle hsetResults = HashSet::create();
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
            TS_ASSERT(hsetResults->size() == 19);

            for (int32_t i = 0; i < 10; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }
            for (int32_t i = 91; i < 100; ++i)
                {
                TS_ASSERT(hsetResults->contains(Integer32::create(i)));
                }
            }

    };
