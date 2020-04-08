/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/transformer/SemiLiteEventTransformer.hpp"

using namespace coherence::lang;

using coherence::util::MapEvent;
using coherence::util::transformer::SemiLiteEventTransformer;


/**
* Test Suite for the SemiLiteEventTransformer.
*/
class SemiLiteEventTransformerSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test SemiLiteEventTransformer
        */
        void testSemiLiteEventTransformer()
            {
            MapEvent::Handle hEvent = MapEvent::create(
                    (ObservableMap::Handle) NULL,
                    MapEvent::entry_updated, String::create("ignore"),
                    Float32::create(42.0F), Float32::create(22.0F));

            SemiLiteEventTransformer::Handle hTransformer =
               SemiLiteEventTransformer::create();
            hEvent = hTransformer->transform(hEvent);
            TS_ASSERT(hEvent->getOldValue() == NULL);
            TS_ASSERT(hEvent->getNewValue()->equals(Float32::create(22.0F)));

            }
    };
