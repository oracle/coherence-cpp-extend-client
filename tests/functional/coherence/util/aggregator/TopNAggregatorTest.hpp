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
#include "coherence/util/NullImplementation.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/aggregator/TopNAggregator.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"


using namespace coherence::lang;

using coherence::io::pof::SystemPofContext;
using coherence::net::NamedCache;
using coherence::util::Filter;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::NullImplementation;
using coherence::util::Set;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::aggregator::TopNAggregator;

/**
* Test Suite for the TopNAggregator.
*/
class TopNAggregatorTest : public CxxTest::TestSuite
  {
  protected:
        void assertArrayEquals(ObjectArray::View vaoExpect, ObjectArray::View vaoActual)
            {
            TS_ASSERT_EQUALS(vaoExpect->length, vaoActual->length);

            size32_t cElems = vaoExpect->length;
            for (size32_t i = 0; i < cElems; i++)
                {
                TS_ASSERT(Object::equals(vaoExpect[i], vaoActual[i]));
                }
            }

        void doTestTopN(String::View vsCache)
            {
            NamedCache::Handle     hCache = ensureCleanCache(vsCache);
            TopNAggregator::Handle hAgent =
                TopNAggregator::create(IdentityExtractor::getInstance(),
                                       SafeComparator::getInstance(), 10);

            ObjectArray::Handle haoEmpty = ObjectArray::create(0);
            ObjectArray::Handle hao1     = ObjectArray::create(1);
            hao1[0] = Integer32::create(1);

            ObjectArray::Handle haoResult;

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Collection::View) NullImplementation::getSet(), hAgent));
            assertArrayEquals(haoEmpty, haoResult);

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Collection::View) Collections::singletonList(COH_TO_STRING(1)), hAgent));
            assertArrayEquals(haoEmpty, haoResult);

            haoResult = cast<ObjectArray::Handle>(hCache->aggregate((Filter::View) NULL, hAgent));
            assertArrayEquals(haoEmpty, haoResult);

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Filter::View) AlwaysFilter::getInstance(), hAgent));
            assertArrayEquals(haoEmpty, haoResult);

            // use a new cache to force the splitting of the aggregator (bypass
            // the $Storage.QuerySizeCache)
            hCache = ensureCleanCache(COH_TO_STRING(vsCache << "-topN"));

            Map::Handle hMap  = HashMap::create();
            int32_t     cKeys = 10000;
            for (int32_t i = 1; i <= cKeys; i++)
                {
                hMap->put(COH_TO_STRING(i), Integer32::create(i));
                }
            hCache->putAll(hMap);

            ObjectArray::Handle haoTop10 = ObjectArray::create(10);
            for (int i = 0; i < 10; i++)
                {
                haoTop10[i] = Integer32::create(cKeys - i);
                }

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Collection::View) NullImplementation::getSet(), hAgent));
            assertArrayEquals(haoEmpty, haoResult);

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Collection::View) Collections::singletonList(COH_TO_STRING(1)), hAgent));
            assertArrayEquals(hao1, haoResult);

            haoResult = cast<ObjectArray::Handle>(hCache->aggregate((Filter::View) NULL, hAgent));
            assertArrayEquals(haoTop10, haoResult);

            haoResult = cast<ObjectArray::Handle>(
                    hCache->aggregate((Filter::View) AlwaysFilter::getInstance(), hAgent));
            assertArrayEquals(haoTop10, haoResult);

            // test duplicate values
            hCache->clear();

            cKeys = 100;
            hMap  = HashMap::create();
            for (int i = 1; i <= cKeys; ++i)
            {
                hMap->put(COH_TO_STRING(i), Integer32::create(i / 2));
            }
            hCache->putAll(hMap);

            haoTop10 = ObjectArray::create(10);
            for (int i = 0; i < 10; ++i)
            {
                haoTop10[i] = Integer32::create((cKeys - i) / 2);
            }

            haoResult = cast<ObjectArray::Handle>(hCache->aggregate((Filter::View) NULL, hAgent));
            assertArrayEquals(haoTop10, haoResult);
            }


  public:
        void testTopN()
            {
            doTestTopN("dist-filter");
            doTestTopN("local-filter");
            }
  };
