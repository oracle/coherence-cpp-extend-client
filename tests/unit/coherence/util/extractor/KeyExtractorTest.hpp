/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

#include "private/coherence/util/SimpleMapEntry.hpp"

using namespace coherence::lang;

using coherence::util::extractor::KeyExtractor;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::SimpleMapEntry;

/**
* Test Suite for  KeyExtractor.
*/
class KeyExtractorSuite : public CxxTest::TestSuite
	{
	public:
 	    /**
        * Test KeyExtractor
        */
        void testKeyExtractor()
            {
            Map::Entry::Handle hEntry = SimpleMapEntry::create(
                    TestPerson::create("Mark", 36), String::create("the value"));

            KeyExtractor::Handle hExtractor = KeyExtractor::create(
                    ReflectionExtractor::create("getName"));

            TS_ASSERT(hExtractor->extract(hEntry->getKey())->equals(String::create("Mark")));
            }
	};
