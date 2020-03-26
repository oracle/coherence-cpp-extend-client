/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/InFilter.hpp"
#include "coherence/util/HashSet.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::InFilter;
using coherence::util::HashSet;


/**
* Test Suite for the InFilter.
*/
class InFilterSuite : public CxxTest::TestSuite
	{
	public:
        /**
        * Test InFilter
        */
        void testInFilter()
            {
            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Noah"));
            hHS->add(String::create("Mark"));
            hHS->add(String::create("Jason"));         
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            InFilter::Handle hFilter = InFilter::create(hExtract, hHS);
            TS_ASSERT(!hFilter->evaluate(String::create("David")));
            TS_ASSERT(hFilter->evaluate(String::create("Mark")));
            }
	};
