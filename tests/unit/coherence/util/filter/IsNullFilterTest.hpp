/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/IsNullFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::IsNullFilter;


/**
* Test Suite for the IsNullFilter.
*/
class IsNullFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test IsNullFilter
        */
        void testIsNullFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            IsNullFilter::Handle hF = IsNullFilter::create(hExtract);
            TS_ASSERT( !hF->evaluate(Float32::create(0.0F)));
            TS_ASSERT( hF->evaluate(NULL));
            }
    };
