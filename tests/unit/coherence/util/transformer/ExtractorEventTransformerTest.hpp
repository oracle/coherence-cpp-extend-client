/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/transformer/ExtractorEventTransformer.hpp"

using namespace coherence::lang;

using coherence::util::extractor::IdentityExtractor;
using coherence::util::MapEvent;
using coherence::util::transformer::ExtractorEventTransformer;


/**
* Test Suite for the ExtractorEventTransformer.
*/
class ExtractorEventTransformerSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test ExtractorEventTransformer
        */
        void testExtractorEventTransformer()
            {
            IdentityExtractor::View hExtract = IdentityExtractor::create();
            MapEvent::Handle hEvent = MapEvent::create(
                    (ObservableMap::Handle)  NULL,
                    MapEvent::entry_updated, String::create("ignore"),
                    Float32::create(42.0F), Float32::create(22.0F));

            ExtractorEventTransformer::Handle hTransformer =
               ExtractorEventTransformer::create(hExtract);
            hEvent = hTransformer->transform(hEvent);
            TS_ASSERT(hEvent->getOldValue()->equals(Float32::create(42.0F)));
            TS_ASSERT(hEvent->getNewValue()->equals(Float32::create(22.0F)));

            }
    };
