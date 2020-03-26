/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::ReflectionExtractor;
using coherence::util::filter::EqualsFilter;

using namespace common::test;

/**
* Test Suite for  ReflectionExtractor.
*/
class ReflectionExtractorSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test ReflectionExtractor
        */
        void testReflectionExtractor()
            {
            ReflectionExtractor::View hExtract = ReflectionExtractor::create(
                        String::create("getAge"));
            TS_ASSERT(hExtract->extract(TestPerson::create("Mark", 36))
                    ->equals(Integer32::create(36)));
            }
    };
