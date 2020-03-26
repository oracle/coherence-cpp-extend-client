/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;


/**
* Test Suite for  IdentityExtractor.
*/
class IdentityExtractorSuite : public CxxTest::TestSuite
    {
    public:
         /**
        * Test IdentityExtractor
        */
        void testIdentityExtractor()
            {
            Float32::Handle hObj = Float32::create(0.0F); // the king of numbers
            IdentityExtractor::Handle hExtract = IdentityExtractor::create();
            Object::View ans = hExtract->extract(hObj);
            TS_ASSERT(hObj == ans);
            }
     };
