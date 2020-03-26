/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/MultiExtractor.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::MultiExtractor;

using namespace common::test;

/**
* Test Suite for  MultiExtractor.
*/
class MultiExtractorSuite : public CxxTest::TestSuite
	{
	public:
 	    /**
        * Test MultiExtractor
        */
        void testMultiExtractor()
            {     
            ObjectArray::Handle ha = ObjectArray::create(2);
            ha[0] = ReflectionExtractor::create("getName");
            ha[1] = ReflectionExtractor::create("getAge");

            MultiExtractor::View hExtract = MultiExtractor::create(ha);
            TestPerson::Handle hPerson = TestPerson::create("Mark", 36);
            List::View vList = cast<List::View>(hExtract->extract(hPerson));
            TS_ASSERT(vList->get(0)->equals(hPerson->getName()));
            TS_ASSERT(vList->get(1)->equals(Integer32::create(36)));
            }
	};
