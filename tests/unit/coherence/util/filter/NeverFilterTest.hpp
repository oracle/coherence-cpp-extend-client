/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;

using coherence::util::filter::NeverFilter;


/**
* Test Suite for the NeverFilter.
*/
class NeverFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test NeverFilter
        */
        void testNeverFilter()
            {
            Float32::View hZero = Float32::create(0.0F);
            NeverFilter::Handle hLF = NeverFilter::create();
            TS_ASSERT(!hLF->evaluate(hZero));
            }
    };
