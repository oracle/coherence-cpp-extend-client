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
#include "coherence/util/comparator/SafeComparator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/extractor/ReflectionExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::extractor::ReflectionExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;
using coherence::util::filter::LessEqualsFilter;


/**
* Test Suite for the ReflectionExtractor.
*/
class ReflectionExtractorSuite : public CxxTest::TestSuite
  {
  public:
        /**
        * Test EqualsFilter
        */
        void testReflectionExtractor()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            Float32::View vFilterValue = Float32::create(42.0F);
            IdentityExtractor::View vExtract = IdentityExtractor::create();
            EqualsFilter::Handle hFilter1 = EqualsFilter::create(vExtract,
                    vFilterValue);
            EqualsFilter::Handle hFilter2 = EqualsFilter::create(vExtract,
                    Float32::create(666.0F));

            hCache->put(String::create("EqualsFilter1"), hFilter1);
            hCache->put(String::create("EqualsFilter2"), hFilter2);

            ReflectionExtractor::View vRExtract =
                ReflectionExtractor::create(String::create("getValue"));
            EqualsFilter::View hFilter = EqualsFilter::create(vRExtract,
                    vFilterValue);

            Set::View vFilterEntries = hCache->entrySet(hFilter);
            if (vFilterEntries->size() == 1)
                {
                Iterator::Handle hIter = vFilterEntries->iterator();
                Map::Entry::View vEntry = cast<Map::Entry::View>(
                        hIter->next());
                hFilter = cast<EqualsFilter::View>(vEntry->getValue());
                TS_ASSERT(hFilter->getValue()->equals(Float32::create(42.0F)));
                }
            else
                {
                TS_ASSERT(false);
                }
            }
  };
