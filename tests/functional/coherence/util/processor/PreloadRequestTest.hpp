/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/processor/PreloadRequest.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::processor::PreloadRequest;

/**
 * Test Suite for the PreloadRequest.
 */
class PreloadRequestSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test PreloadRequest
     */
    void testPreloadRequest()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        for (size32_t x = 0; x < 5; ++x)
            {
            Integer32::View vKey = Integer32::create(x);
            Integer32::View vValue = Integer32::create(1);

            hCache->put(vKey, vValue);
            }
        
        
        PreloadRequest::Handle vPreloadRequest = PreloadRequest::create();
 
        hCache->invoke(Integer32::create(3), vPreloadRequest);

        // TODO : What to assert?
        }
    };
