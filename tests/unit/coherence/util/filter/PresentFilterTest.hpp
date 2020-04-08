/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/filter/PresentFilter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Set.hpp"

using namespace coherence::lang;

using coherence::util::filter::PresentFilter;
using coherence::util::Map;
using coherence::util::HashMap;
using coherence::util::Set;


/**
* Test Suite for the PresentFilter.
*/
class PresentFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test PresentFilter
        */
        void testPresentFilter()
            {
             PresentFilter::Handle hFilter = PresentFilter::create();

            HashMap::Handle hMap = HashMap::create();
            hMap->put(String::create("David"),Integer32::create(42));
            Set::View         vSet = hMap->entrySet();
            Iterator::Handle  iter = vSet->iterator();
            Map::Entry::View hME = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(hFilter->evaluateEntry(hME));
            }
    };
