/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ValueManipulator.hpp"
#include "coherence/util/processor/PriorityProcessor.hpp"
#include "coherence/util/processor/NumberMultiplier.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::ValueManipulator;
using coherence::util::processor::PriorityProcessor;
using coherence::util::processor::NumberMultiplier;

/**
 * Test Suite for the PriorityProcessor.
 */
class PriorityProcessorSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test PriorityProcessor
     */
    void testPriorityProcessor()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        for (size32_t x = 0; x < 5; ++x)
            {
            Integer32::View vKey = Integer32::create(x);
            Integer32::View vValue = Integer32::create(8);

            hCache->put(vKey, vValue);
            }

        NumberMultiplier::Handle vNumberMultiplier =
                NumberMultiplier::create((ValueManipulator::View) NULL,
                        Integer32::create(7), false);

        Integer32::View vKey = Integer32::create(3);
        PriorityProcessor::Handle vPriorityProcessor =
                PriorityProcessor::create(vNumberMultiplier);

        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(8)));
        hCache->invoke(vKey, vPriorityProcessor);
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(56)));
        }
    };
