/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::EqualsFilter;


/**
* Test Suite for the EqualsFilter.
*/
class EqualsFilterSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test EqualsFilter
        */
        void testEqualsFilter()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            size32_t x         = 0;
            size32_t cExpected = 5;

            String::View vFilterValue = String::create("b");
            for (x = 0; x < cExpected; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                hCache->put(vKey, vFilterValue);
                }

              for (size32_t c = x+5; x < c; ++x)
                {
                Integer32::View vKey = Integer32::create(x);
                Object::View vValue  = String::create("c");

                hCache->put(vKey, vValue);
                }

            IdentityExtractor::View vExtract = IdentityExtractor::create();
            EqualsFilter::Handle hFilter = EqualsFilter::create(vExtract, vFilterValue);

            Set::View vFilterEntries = hCache->entrySet(hFilter);
            TS_ASSERT(vFilterEntries->size() == cExpected);
            }
  };
