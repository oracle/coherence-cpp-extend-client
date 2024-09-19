/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/extractor/UniversalExtractor.hpp"

#include "coherence/util/filter/EqualsFilter.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::extractor::UniversalExtractor;
using coherence::util::filter::EqualsFilter;

using namespace common::test;

/**
 * Test Suite for UniversalExtractor.
 */
class UniversalExtractorTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Test method suffix validation - required suffix is not present
         */
        void testIllegalConstructor()
            {
            ObjectArray::View vaParams = ObjectArray::create(1);

            TS_ASSERT_THROWS(UniversalExtractor::create(String::create("age"), vaParams),
                IllegalArgumentException::View);
            }

        /**
         * Test method suffix validation - required suffix is present
         */
        void testLegalConstructor()
            {
            ObjectArray::View vaParams = ObjectArray::create(1);

            UniversalExtractor::create(String::create("getAge()"), vaParams);
            }

        /**
         * Test method suffix validation - suffix is not required
         */
        void testLegalConstructorNoSuffix()
            {
            UniversalExtractor::create(String::create("age"));
            }
    };
