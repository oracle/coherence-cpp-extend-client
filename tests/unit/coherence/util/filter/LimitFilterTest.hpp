/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::LimitFilter;


/**
* Test Suite for the LimitFilter.
*/
class LimitFilterTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test LimitFilter
        */
        void testLimitFilter()
            {
            EqualsFilter::Handle hEFilter = EqualsFilter::create(
                        IdentityExtractor::getInstance(),
                        Float32::create(42.0F));
            LimitFilter::Handle hFilter = LimitFilter::create(hEFilter, 32);
            TS_ASSERT(hFilter->evaluate(Float32::create(42.0F)));
            TS_ASSERT(!hFilter->evaluate(Float32::create(2.0F)));
            }

        /**
        * Test LimitFilter
        */
        void testExtractPage()
            {
            EqualsFilter::Handle hEFilter = EqualsFilter::create(
                        IdentityExtractor::getInstance(),
                        Float32::create(42.0F));
            int32_t pageSize = 5;
            LimitFilter::Handle hFilter = LimitFilter::create(hEFilter, pageSize);

            ObjectArray::Handle haEntry = ObjectArray::create(64);

            for (int x = 0; x < 64; ++x)
                {
                Integer32::Handle hI = Integer32::create(x);

                haEntry[x] = SimpleMapEntry::create(hI, hI);
                }

            ObjectArray::Handle haResult = hFilter->extractPage(haEntry);
            while (haResult->length > 0)
                {
                TS_ASSERT(haResult->length <= 5);

                for (size32_t i = 0; i < haResult->length; ++i)
                    {
                    SimpleMapEntry::Handle entry = cast<SimpleMapEntry::Handle>(haResult[i]);
                    int32_t index = hFilter->getPage() * pageSize + i;
                    TS_ASSERT(Integer32::create(index)->equals(entry->getValue()));
                    }
                hFilter->nextPage();
                haResult = hFilter->extractPage(haEntry);
                }
            }

        void testToString()
            {
            // start with a minimal LimitFilter
            LimitFilter::Handle hFilter = LimitFilter::create(AlwaysFilter::getInstance(), 5);

            TSM_ASSERT(hFilter->toString()->getCString(), hFilter->toString()->equals("LimitFilter: (AlwaysFilter [pageSize=5, pageNum=0])"));

            // set the page
            hFilter->setPage(1);

            TSM_ASSERT(hFilter->toString()->getCString(), hFilter->toString()->equals("LimitFilter: (AlwaysFilter [pageSize=5, pageNum=1])"));

            // add a Comparator
            hFilter->setComparator(DummyComparator::create());

            TSM_ASSERT(hFilter->toString()->getCString(),
                hFilter->toString()->startsWith("LimitFilter: (AlwaysFilter [pageSize=5, pageNum=1, top=NULL, bottom=NULL, comparator=mock::DummyComparator"));
            }
    };
