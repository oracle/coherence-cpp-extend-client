/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/ContainsFilter.hpp"
#include "coherence/util/HashSet.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::ContainsFilter;
using coherence::util::HashSet;


/**
* Test Suite for the ContainsFilter.
*/
class ContainsFilterSuite : public CxxTest::TestSuite
	{
	public:
        /**
        * Test ContainsFilter
        */
        void testContainsFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            ContainsFilter::Handle hFilter = ContainsFilter::create(hExtract,
                        String::create("David"));
            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Noah"));
            hHS->add(String::create("Mark"));
            hHS->add(String::create("Jason"));         
            TS_ASSERT(!hFilter->evaluate(hHS)); 
            hHS->add(String::create("David"));
            TS_ASSERT(hFilter->evaluate(hHS));
            }
	};
