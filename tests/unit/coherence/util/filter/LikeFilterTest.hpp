/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/LikeFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::LikeFilter;


/**
* Test Suite for the LikeFilter.
*/
class LikeFilterSuite : public CxxTest::TestSuite
	{
	public:
        /**
        * Test LikeFilter
        */
        void testLikeFilter()
            {
            LikeFilter::Handle hFilter;
             hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("David"));
            TS_ASSERT(hFilter->evaluate(String::create("David")));

            hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("David"));
            TS_ASSERT(!hFilter->evaluate(String::create("DaviD")));

            hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("_avid"));
            TS_ASSERT(hFilter->evaluate(String::create("David")));

            hFilter = LikeFilter::create(
                         IdentityExtractor::getInstance(),
                         String::create("Davi_"));
             TS_ASSERT(hFilter->evaluate(String::create("David")));
             
             hFilter = LikeFilter::create(
                          IdentityExtractor::getInstance(),
                          String::create("Da_iD"));
              TS_ASSERT(!hFilter->evaluate(String::create("David")));

            hFilter = LikeFilter::create(
                         IdentityExtractor::getInstance(),
                         String::create("%vid"));
             TS_ASSERT(hFilter->evaluate(String::create("David")));

            hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("D%d"));
            TS_ASSERT(hFilter->evaluate(String::create("David")));
            
            hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("%v%"));
            TS_ASSERT(hFilter->evaluate(String::create("David")));

            //test case for COH-4020
            hFilter = LikeFilter::create(
                        IdentityExtractor::getInstance(),
                        String::create("%GetLogonData_getAccountIndicativeInfoInput_7137B2C9070C4951AE00EE5C4F01435A%"));
            TS_ASSERT(hFilter->evaluate(String::create("xGetLogonDataxgetAccountIndicativeInfoInputx7137B2C9070C4951AE00EE5C4F01435Axxx")));
            }
	};
