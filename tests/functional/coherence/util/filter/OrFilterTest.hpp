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
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/OrFilter.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::AndFilter;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterEqualsFilter;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::OrFilter;


/**
* Test Suite for the OrFilter.
*/
class OrFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test EqualsFilter
        */
        void testOrFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            hCache->put(Integer32::create(12345), Float64::create(5.0));
            hCache->put(Integer32::create(23456), Float64::create(75.0));
            hCache->put(Integer32::create(34567), Float64::create(115.0));

            IdentityExtractor::View hExtract =
                IdentityExtractor::getInstance();
            OrFilter::Handle hFilter = OrFilter::create(
               EqualsFilter::create(hExtract, Float64::create(5.0)),
               AndFilter::create(
                       GreaterEqualsFilter::create(hExtract,
                               Float64::create(50.0)),
                       LessEqualsFilter::create(hExtract,
                               Float64::create(100.0))));

            Set::View vFilterEntries = hCache->entrySet(hFilter);
            TS_ASSERT(vFilterEntries->size() == 2);
            }
  };
