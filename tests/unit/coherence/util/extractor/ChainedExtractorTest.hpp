/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/ChainedExtractor.hpp"
#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::ChainedExtractor;

using namespace common::test;

/**
* Test Suite for  ChainedExtractor.
*/
class ChainedExtractorSuite : public CxxTest::TestSuite
    {
    public:
         /**
        * Test ChainedExtractor
        */
        void testChainedExtractor()
            {
            ChainedExtractor::View hExtract =
                    ChainedExtractor::create(ChainedExtractor::createExtractors(
                            "getSpouse.getAge"));
            TS_ASSERT(hExtract->extract(
                    MarriedPerson::create("Mark", 36, MarriedPerson::create("Otti", 37)))
                    ->equals(Integer32::create(37)));
            }
    };
