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
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/OrFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AndFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterEqualsFilter;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::OrFilter;


/**
* Test Suite for the OrFilter.
*/
class OrFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test OrFilter
        */
        void testOrFilter()
            {
             IdentityExtractor::View hExtract = IdentityExtractor::create();
             OrFilter::Handle hFilter = OrFilter::create(
                EqualsFilter::create(hExtract, Float32::create(5.0F)),
                AndFilter::create(
                         GreaterEqualsFilter::create(hExtract,
                                     Float32::create(50.0F)),
                         LessEqualsFilter::create(hExtract,
                                     Float32::create(100.0F))));
            TS_ASSERT( hFilter->evaluate(Float32::create(5.0F)));
            TS_ASSERT(hFilter->evaluate(Float32::create(75.0F)));
            }
    };
