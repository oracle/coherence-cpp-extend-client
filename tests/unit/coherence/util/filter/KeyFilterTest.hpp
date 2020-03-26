/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/KeyFilter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Set.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::KeyFilter;
using coherence::util::HashSet;
using coherence::util::Map;
using coherence::util::HashMap;
using coherence::util::Set;


/**
* Test Suite for the KeyFilter.
*/
class KeyFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test KeyFilter
        */
        void testKeyFilter()
            {
            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Noah"));
            hHS->add(String::create("Mark"));
            hHS->add(String::create("Jason"));
            KeyFilter::Handle hFilter = KeyFilter::create(hHS);

            HashMap::Handle hMap = HashMap::create();
            hMap->put(String::create("Noah"),Integer32::create(42));
            Set::View         vSet = hMap->entrySet();
            Iterator::Handle  iter = vSet->iterator();
            Map::Entry::View vME = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(hFilter->evaluateEntry(vME));

            hMap = HashMap::create();
            hMap->put(String::create("David"),Integer32::create(42));
            vSet = hMap->entrySet();
            iter = vSet->iterator();
            vME  = cast<Map::Entry::View>(iter->next());
            TS_ASSERT(!hFilter->evaluateEntry(vME));
            }
    };
