/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/NotFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::NotFilter;


/**
* Test Suite for the NotFilter.
*/
class NotFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test NotFilter
        */
        void testNotFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            NotFilter::Handle hFilter = NotFilter::create(
                     EqualsFilter::create(hExtract,
                                 Float32::create(42.0F)));
            TS_ASSERT(!hFilter->evaluate(Float32::create(42.0F)));
            }
    };
