/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/MapEventFilter.hpp"

using namespace coherence::lang;

using coherence::util::MapEvent;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::filter::EqualsFilter;
using coherence::util::filter::MapEventFilter;


/**
* Test Suite for the MapEventFilter.
*/
class MapEventFilterSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test MapEventFilter
        */
        void testMapEventFilter()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
             EqualsFilter::Handle hEFilter = EqualsFilter::create(hExtract,
                         Float32::create(22.0F));

            MapEvent::Handle hEvent = MapEvent::create(
                    (ObservableMap::Handle) NULL,
                    MapEvent::entry_updated, String::create("ignore"),
                    Float32::create(42.0F), Float32::create(22.0F));

             MapEventFilter::Handle hFilter =
                MapEventFilter::create(MapEventFilter::e_updated_entered,
                        hEFilter);

            TS_ASSERT(hFilter->evaluate(hEvent));

            }

        /**
        * Test equals
        */
        void testEquals()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
             EqualsFilter::Handle hEFilter = EqualsFilter::create(hExtract,
                         Float32::create(22.0F));

            MapEvent::Handle hEvent = MapEvent::create(
                    (ObservableMap::Handle) NULL, MapEvent::entry_updated,
                    String::create("ignore"),
                    Float32::create(42.0F), Float32::create(22.0F));

            MapEventFilter::Handle hFilter1 =
                MapEventFilter::create(MapEventFilter::e_updated_entered,
                        hEFilter);

            MapEventFilter::Handle hFilter2 =
                MapEventFilter::create(MapEventFilter::e_updated_entered,
                        hEFilter);

            TS_ASSERT(hFilter1->equals(hFilter2));
            TS_ASSERT(hFilter2->equals(hFilter1));

            MapEventFilter::Handle hFilter3 =
                MapEventFilter::create(MapEventFilter::e_deleted,
                        hEFilter);

            TS_ASSERT(!hFilter1->equals(hFilter3));
            TS_ASSERT(!hFilter3->equals(hFilter1));

            MapEventFilter::Handle hFilter4 =
                MapEventFilter::create(MapEventFilter::e_deleted, (Filter::View) NULL);

            MapEventFilter::Handle hFilter5 =
                MapEventFilter::create(MapEventFilter::e_deleted);

            TS_ASSERT(hFilter4->equals(hFilter5));
            TS_ASSERT(hFilter5->equals(hFilter4));

            TS_ASSERT(!hFilter4->equals(hFilter3));
            TS_ASSERT(!hFilter3->equals(hFilter4));

            }
    };
