/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/CompositeUpdater.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/filter/NotFilter.hpp"

using namespace coherence::lang;

using coherence::util::comparator::SafeComparator;
using coherence::util::extractor::CompositeUpdater;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::filter::NotFilter;


/**
* Test Suite for  CompositeUpdater.
*/
class CompositeUpdaterSuite : public CxxTest::TestSuite
	{
	public:
 	    /**
        * Test CompositeUpdater
        */
        void testCompositeUpdater()
            {     
            CompositeUpdater::View hUpdater =
                    CompositeUpdater::create("getSpouse.setAge");
            MarriedPerson::Handle hPerson = MarriedPerson::create("Mark", 36, MarriedPerson::create("Otti", 37));
            hUpdater->update(hPerson, Integer32::create(36));

            TS_ASSERT(hPerson->getSpouse()->getAge() == 36);
            }
	};
