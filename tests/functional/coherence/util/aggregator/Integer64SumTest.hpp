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
#include "coherence/util/Filter.hpp"
#include "coherence/util/aggregator/Integer64Sum.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::Integer64Sum;
using coherence::util::Filter;

/**
* Test Suite for the Integer64Sum.
*/
class Integer64SumTest : public CxxTest::TestSuite
  {
  public:
        /**
        * Test Integer64Sum
        */
        void testInteger64Sum()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-filter");

            for (int32_t i = 1; i <= 100; i++)
                {
                hCache->put(Integer32::create(1000*i), Integer32::create(i));
                }

            Integer64::View vlSum = cast<Integer64::View>(hCache->aggregate(
                    (Filter::View) NULL, Integer64Sum::create(
                            (ValueExtractor::View) IdentityExtractor::getInstance())));

            TS_ASSERT(vlSum->getValue() == 5050);
            }
  };
