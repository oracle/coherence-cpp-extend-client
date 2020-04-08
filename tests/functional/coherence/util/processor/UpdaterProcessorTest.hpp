/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/extractor/ReflectionUpdater.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/LimitFilter.hpp"
#include "coherence/util/processor/UpdaterProcessor.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::extractor::ReflectionUpdater;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::LimitFilter;
using coherence::util::processor::UpdaterProcessor;

/**
 * Test Suite for the UpdaterProcessor.
 */
class UpdaterProcessorSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test UpdaterProcessor
     */
    void testUpdaterProcessor()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        Integer32::View vKey = Integer32::create(3);
        LimitFilter::Handle vValue = LimitFilter::create(
                    AlwaysFilter::getInstance(), 32);
        
        hCache->put(vKey, vValue);
        
        ReflectionUpdater::View vUpdater = ReflectionUpdater::create(
                    String::create("setPageSize"));

        UpdaterProcessor::Handle vUpdaterProcessor = 
                UpdaterProcessor::create(vUpdater, Integer32::create(99));

        TS_ASSERT(vValue->getPageSize() == 32);

        hCache->invoke(vKey, vUpdaterProcessor);

        vValue = cast<LimitFilter::Handle>(hCache->get(vKey));
        
        TS_ASSERT(vValue->getPageSize() == 99);
        }
    };
