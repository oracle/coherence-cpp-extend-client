/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/ComparisonValueExtractor.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/filter/LessFilter.hpp"

using namespace coherence::lang;

using coherence::util::extractor::ComparisonValueExtractor;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::filter::LessFilter;


/**
* Test Suite for  ComparisonValueExtractor.
*/
class ComparisonValueExtractorSuite : public CxxTest::TestSuite
	{
	public:
        void testComparisonValueExtractor2()
            {     
            IdentityExtractor::View hExtract = 
                            IdentityExtractor::getInstance();
             ComparisonValueExtractor::View  hCompExtract = 
                        ComparisonValueExtractor::create(
                                hExtract, hExtract);
            Object::View vAns;
            vAns = hCompExtract->extract(Integer16::valueOf(42));           
            TS_ASSERT(Integer32::valueOf(0)->compareTo(vAns) == 0);

            vAns = hCompExtract->extract(Integer32::valueOf(42));           
             TS_ASSERT(Integer32::valueOf(0)->compareTo(vAns) == 0);
 
            vAns = hCompExtract->extract(Integer64::valueOf(42));          
            TS_ASSERT(Integer64::valueOf(0)->compareTo(vAns) == 0);

            vAns = hCompExtract->extract(Float32::valueOf(42.0));           
            TS_ASSERT(Float32::valueOf(0)->compareTo(vAns) == 0);

            vAns = hCompExtract->extract(Float64::valueOf(42.0));          
            TS_ASSERT(Float64::valueOf(0)->compareTo(vAns) == 0);

            vAns = hCompExtract->extract(String::create("Hello World"));          
            TS_ASSERT(Integer32::valueOf(0)->compareTo(vAns) == 0);

            }
	};
