/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AndFilter;
using coherence::util::filter::GreaterEqualsFilter;
using coherence::util::filter::LessEqualsFilter;


/**
* Test Suite for the AndFilter.
*/
class AndFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test AndFilter
        */
        void testAndFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            AndFilter::Handle hFilter = AndFilter::create(
                GreaterEqualsFilter::create(hExtract, Float32::create(5.0F)),
                LessEqualsFilter::create(hExtract,Float32::create(10.0F)));
            TS_ASSERT( hFilter->evaluate(Float32::create(7.5F)));
            }
    };
