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
#include "coherence/util/filter/XorFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::XorFilter;


/**
* Test Suite for the XOrFilter.
*/
class XorFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test XOrFilter
        */
        void testXorFilter()
            {
             IdentityExtractor::View hExtract = IdentityExtractor::create();
             XorFilter::Handle hFilter = XorFilter::create(
                     EqualsFilter::create(hExtract, Float32::create(5.0F)),
                     EqualsFilter::create(hExtract, Float32::create(6.0F)));
            TS_ASSERT( hFilter->evaluate(Float32::create(5.0F)));
            TS_ASSERT(hFilter->evaluate(Float32::create(6.0F)));
            hFilter = XorFilter::create(
                    EqualsFilter::create(hExtract, Float32::create(5.0F)),
                    EqualsFilter::create(hExtract, Float32::create(5.0F)));
            TS_ASSERT(!hFilter->evaluate(Float32::create(5.0F)));
            }
    };
