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
#include "coherence/util/processor/ConditionalProcessor.hpp"
#include "coherence/util/processor/NumberMultiplier.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::ValueManipulator;
using coherence::util::processor::ConditionalProcessor;
using coherence::util::processor::NumberMultiplier;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;

/**
 * Test Suite for the ConditionalProcessor.
 */
class ConditionalProcessorSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test ConditionalProcessor
     */
    void testConditionalProcessor()
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
        NeverFilter::View hNeverFilter = NeverFilter::create();
        ConditionalProcessor::Handle vNeverConditionalProcessor =
                ConditionalProcessor::create(hNeverFilter, vNumberMultiplier);

        AlwaysFilter::View hAlwaysFilter = AlwaysFilter::create();
        ConditionalProcessor::Handle vAlwaysConditionalProcessor =
                ConditionalProcessor::create(hAlwaysFilter, vNumberMultiplier);

        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(8)));
        hCache->invoke(vKey, vNeverConditionalProcessor);
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(8)));
        hCache->invoke(vKey, vAlwaysConditionalProcessor);
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(56)));
        }
    };
