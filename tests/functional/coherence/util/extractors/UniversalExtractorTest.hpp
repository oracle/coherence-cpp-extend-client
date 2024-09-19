/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/NamedCache.hpp"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/UniversalExtractor.hpp"

#include "coherence/util/filter/EqualsFilter.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::UniversalExtractor;
using coherence::util::filter::EqualsFilter;

/**
* Test Suite for the UniversalExtractor.
*/
class UniversalExtractorTest : public CxxTest::TestSuite
  {
  public:
        void testUniversalExtractor()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            Float32::View           vFilterValue = Float32::valueOf(42.0F);
            IdentityExtractor::View vExtract     = IdentityExtractor::create();
            EqualsFilter::Handle    hFilter1     = EqualsFilter::create(vExtract,
                vFilterValue);
            EqualsFilter::Handle    hFilter2     = EqualsFilter::create(vExtract,
                Float32::valueOf(666.0F));

            hCache->put(String::create("EqualsFilter1"), hFilter1);
            hCache->put(String::create("EqualsFilter2"), hFilter2);

            UniversalExtractor::View vUExtract =
                UniversalExtractor::create(String::create("getValue()"));
            EqualsFilter::View       hFilter  = EqualsFilter::create(vUExtract,
                vFilterValue);

            Set::View vFilterEntries = hCache->entrySet(hFilter);
            TS_ASSERT_EQUALS(size32_t(1), vFilterEntries->size());

            Iterator::Handle hIter  = vFilterEntries->iterator();
            Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

            hFilter = cast<EqualsFilter::View>(vEntry->getValue());
            TS_ASSERT(hFilter->getValue()->equals(vFilterValue));
            }
  };
