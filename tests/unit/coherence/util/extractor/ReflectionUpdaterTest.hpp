/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

#include "common/TestPerson.hpp"

using namespace coherence::lang;

using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::ReflectionUpdater;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::LimitFilter;

using namespace common::test;

/**
* Test Suite for  ReflectionUpdater.
*/
class ReflectionUpdaterSuite : public CxxTest::TestSuite
	{
	public:
 	    /**
        * Test ReflectionUpdater
        */
        void testReflectionUpdater()
            {
            ReflectionUpdater::View hUpdater = ReflectionUpdater::create(
                        String::create("setAge"));
            TestPerson::Handle hPerson = TestPerson::create("Mark", 36);

            hUpdater->update(hPerson, Integer32::create(37));
            TS_ASSERT(hPerson->getAge() == 37);
            }
	};
