/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/MapEventTransformerFilter.hpp"
#include "coherence/util/transformer/SemiLiteEventTransformer.hpp"

using namespace coherence::lang;

using coherence::util::MapEvent;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::MapEventTransformerFilter;
using coherence::util::transformer::SemiLiteEventTransformer;


/**
* Test Suite for the MapEventTransformerFilter.
*/
class MapEventTransformerFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test MapEventTransformerFilter
        */
        void testMapEventTransformerFilter()
            {
            MapEvent::Handle hEvent = MapEvent::create(
                    (ObservableMap::Handle) NULL,
                    MapEvent::entry_updated, String::create("ignore"),
                    Float32::create(42.0F), Float32::create(22.0F));

            SemiLiteEventTransformer::Handle hTransformer =
               SemiLiteEventTransformer::create();
            MapEventTransformerFilter::Handle hFilter =
                MapEventTransformerFilter::create(
                        AlwaysFilter::create(),
                        hTransformer);

            TS_ASSERT(hFilter->evaluate(hEvent));
            hEvent = hTransformer->transform(hEvent);
            TS_ASSERT(hEvent->getOldValue() == NULL);
            TS_ASSERT(hEvent->getNewValue()->equals(Float32::create(22.0F)));

            }
    };
