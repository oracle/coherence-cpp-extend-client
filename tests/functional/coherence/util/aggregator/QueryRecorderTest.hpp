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
#include "coherence/util/QueryRecord.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/OrFilter.hpp"
#include "coherence/util/aggregator/QueryRecorder.hpp"

using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::QueryRecord;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::QueryRecorder;
using coherence::util::Filter;
using coherence::util::filter::GreaterEqualsFilter;
using coherence::util::filter::LessEqualsFilter;
using coherence::util::filter::OrFilter;

using namespace std;

/**
* Test Suite for the Integer64Sum.
*/
class QueryRecorderTest : public CxxTest::TestSuite
  {
  public:
        /**
        * Test Integer64Sum
        */
        void testExplain()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-explain");

            for (int32_t i = 1; i <= 100; i++)
                {
                hCache->put(Integer32::create(1000*i), Integer32::create(i));
                }

            IdentityExtractor::View hExtract =
                IdentityExtractor::getInstance();
            OrFilter::Handle hFilter = OrFilter::create(
                        GreaterEqualsFilter::create(hExtract,
                                Integer32::create(50)),
                        LessEqualsFilter::create(hExtract,
                                Integer32::create(20)));

            QueryRecord::View vRecord = cast<QueryRecord::View>(hCache->aggregate(
                    (Filter::View) hFilter, QueryRecorder::create(QueryRecorder::explain)));


            cout << vRecord;

            TS_ASSERT(vRecord->getResults()->size() == 1);
            TS_ASSERT(cast<QueryRecord::PartialResult::View>(vRecord->getResults()->get(0))->getSteps()->size() == 1);
            TS_ASSERT(cast<QueryRecord::PartialResult::Step::View>(cast<QueryRecord::PartialResult::View>(vRecord->getResults()->get(0))->getSteps()->get(0))->getSteps()->size() == 2);


            vRecord = cast<QueryRecord::View>(hCache->aggregate(
                    (Filter::View) hFilter, QueryRecorder::create(QueryRecorder::trace)));


            cout << vRecord;

            TS_ASSERT(vRecord->getResults()->size() == 1);
            TS_ASSERT(cast<QueryRecord::PartialResult::View>(vRecord->getResults()->get(0))->getSteps()->size() == 2);
            TS_ASSERT(cast<QueryRecord::PartialResult::Step::View>(cast<QueryRecord::PartialResult::View>(vRecord->getResults()->get(0))->getSteps()->get(0))->getSteps()->size() == 2);
            TS_ASSERT(cast<QueryRecord::PartialResult::Step::View>(cast<QueryRecord::PartialResult::View>(vRecord->getResults()->get(0))->getSteps()->get(1))->getSteps()->size() == 2);


            }
  };
