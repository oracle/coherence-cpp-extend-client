/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/lang.ns"

#include "coherence/lang.ns"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/InFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/OrFilter.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::InFilter;


/**
* Test Suite for the InFilter.
*/
class InFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test EqualsFilter
        */
        void testInFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            hCache->put(Integer32::create(12345), String::create("Moe"));
            hCache->put(Integer32::create(23456), String::create("Larry"));
            hCache->put(Integer32::create(34567), String::create("Curly"));
            hCache->put(Integer32::create(45678), String::create("David"));

            HashSet::Handle hHS = HashSet::create();
            hHS->add(String::create("Noah"));
            hHS->add(String::create("Mark"));
            hHS->add(String::create("David"));         
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            InFilter::Handle hFilter = InFilter::create(hExtract, hHS);

            Set::View vFilterEntries = hCache->entrySet(hFilter);
            TS_ASSERT(vFilterEntries->size() == 1);
            }
  };
