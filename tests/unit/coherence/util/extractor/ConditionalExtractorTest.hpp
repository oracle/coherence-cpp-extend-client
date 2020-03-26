/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/ConditionalIndex.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/ConditionalExtractor.hpp"
#include "coherence/util/extractor/TypedExtractor.hpp"
#include "coherence/util/filter/EqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"

#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::util::ConditionalIndex;
using coherence::util::Filter;
using coherence::util::ValueExtractor;
using coherence::util::extractor::ConditionalExtractor;
using coherence::util::extractor::TypedExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::GreaterFilter;

using common::test::Bird;
using common::test::Fish;


/**
* Test Suite for ConditionalExtractor.
*/
class ConditionalExtractorSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test createIndex
        */
        void testCreateIndex()
            {
            ValueExtractor::View vExtract = IdentityExtractor::create();
            Filter::View         vFilter  = EqualsFilter::create(vExtract, Float32::create(42.0F));
            Map::Handle          hMap     = HashMap::create();

            ConditionalExtractor::Handle hCondExtract = ConditionalExtractor::create(vFilter, vExtract, true);
            try
                {
                MapIndex::Handle hIndex = hCondExtract->createIndex(false, (Comparator::View) NULL, hMap);
                TS_ASSERT(instanceof<ConditionalIndex::Handle>(hIndex));
                TS_ASSERT(vFilter == cast<ConditionalIndex::Handle>(hIndex)->getFilter());
                TS_ASSERT(vExtract == hIndex->getValueExtractor());

                // make sure that the index map has been updated with the created index
                MapIndex::Handle hIndex2 = cast<MapIndex::Handle>(hMap->get(vExtract));
                TS_ASSERT(hIndex2 != NULL);
                TS_ASSERT(hIndex == hIndex2);
                }
            catch (UnsupportedOperationException::View)
                {
                TS_ASSERT(42 == 42);
                }
            }

        /**
        * Test destroyIndex
        */
        void testDestroyIndex()
            {
            ValueExtractor::View vExtract = IdentityExtractor::create();
            Filter::View         vFilter  = GreaterFilter::create(vExtract, Integer32::create(5));
            Map::Handle          hMap     = HashMap::create();

            ConditionalExtractor::Handle hCondExtract = ConditionalExtractor::create(vFilter, vExtract, true);
            MapIndex::Handle             hIndex       = hCondExtract->createIndex(false, (Comparator::View) NULL, hMap);

            // make sure that the index map has been updated with the created
            // index
            MapIndex::Handle hIndex2 = cast<MapIndex::Handle>(hMap->get(vExtract));
            TS_ASSERT(hIndex2 != NULL);
            TS_ASSERT(hIndex == hIndex2);

            hCondExtract->destroyIndex(hMap);

            // make sure that the index has been removed from the index map
            TS_ASSERT(hMap->get(vExtract) == NULL);
            }

        /**
        * Test extract
        */
        void testExtract()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(123), Integer32::create(456));

            Collection::View          vValues  = ((Map::View) hMap)->values();
            ReflectionExtractor::View vExtract = COH_TYPED_EXTRACTOR(Collection::View, Map, values);
            TS_ASSERT(vExtract->getMethodName()->equals("values"));

            EqualsFilter::View           vFilter      = EqualsFilter::create(vExtract, vValues);
            ConditionalExtractor::Handle hCondExtract = ConditionalExtractor::create(vFilter, vExtract, true);

            try
                {
                hCondExtract->extract(hMap);
                TS_ASSERT(false);  //an exception must be thrown by extract
                }
            catch (UnsupportedOperationException::View)
                {
                TS_ASSERT(42 == 42);
                }
            }

        /**
        * Regression test for COH-5516.
        */
        void testCoh5516()
            {
            ReflectionExtractor::View    vTypeExtract =
                    ReflectionExtractor::create(String::create("getType"));
            Filter::View                 vFilter      =
                    EqualsFilter::create(vTypeExtract, String::create("bird"));
            ReflectionExtractor::View    vExtract     =
                    ReflectionExtractor::create(String::create("getWings"));
            ConditionalExtractor::Handle hCondExtract =
                    ConditionalExtractor::create(vFilter, vExtract, false);
            Map::Handle                  hMap         = HashMap::create();

            MapIndex::Handle hIndex = hCondExtract->createIndex(
                    false, (Comparator::View) NULL, hMap);

            Bird::Handle         hBird   = Bird::create();
            Fish::Handle         hFish   = Fish::create();
            SimpleMapEntry::View vEntry1 = SimpleMapEntry::create(
                    Integer32::create(0), hBird, hBird);
            SimpleMapEntry::View vEntry2 = SimpleMapEntry::create(
                    Integer32::create(1), hFish, hFish);

            // add entries of type Bird and Fish - only
            // Bird should get indexed
            hIndex->insert(vEntry1);
            hIndex->insert(vEntry2);

            // remove entries of type Bird and Fish - only
            // Bird should get indexed
            hIndex->remove(vEntry1);
            hIndex->remove(vEntry2);
            }
    };
