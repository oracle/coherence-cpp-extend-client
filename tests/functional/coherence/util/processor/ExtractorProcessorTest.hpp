/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/extractor/IdentityExtractor.hpp"
#include "coherence/util/processor/ExtractorProcessor.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::extractor::IdentityExtractor;
using coherence::util::processor::ExtractorProcessor;

/**
 * Test Suite for the ExtractorProcessor.
 */
class ExtractorProcessorSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test ExtractorProcessor
     */
    void testExtractorProcessor()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        Integer32::View vKey = Integer32::create(3);
        Integer32::View vValue = Integer32::create(99);
        hCache->put(vKey, vValue);
        
        IdentityExtractor::View vExtractor = IdentityExtractor::create();
        ExtractorProcessor::Handle vExtractorProcessor = 
                ExtractorProcessor::create(vExtractor);

        TS_ASSERT(hCache->get(vKey)->equals(vValue));

        TS_ASSERT(hCache->invoke(vKey, vExtractorProcessor)->equals(vValue));
        }
    };
