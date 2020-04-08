/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/processor/ConditionalPut.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::processor::ConditionalPut;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;

/**
 * Test Suite for the ConditionalPut.
 */
class ConditionalPutSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test ConditionalPut
     */
    void testConditionalPut()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        for (size32_t x = 0; x < 5; ++x)
            {
            Integer32::View vKey = Integer32::create(x);
            Integer32::View vValue = Integer32::create(1);

            hCache->put(vKey, vValue);
            }
        
        Integer32::View vKey = Integer32::create(3);
        Integer32::View vValue = Integer32::create(99);
        
        NeverFilter::View hNeverFilter = NeverFilter::create();
        ConditionalPut::Handle vNeverConditionalPut = 
                ConditionalPut::create(hNeverFilter, vValue, true);
 
        AlwaysFilter::View hAlwaysFilter = AlwaysFilter::create();
        ConditionalPut::Handle vAlwaysConditionalPut = 
                ConditionalPut::create(hAlwaysFilter, vValue, true);
        
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(1)));

        TS_ASSERT(hCache->invoke(vKey, vNeverConditionalPut)->equals(Integer32::create(1)));
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(1)));

  
        TS_ASSERT(NULL == hCache->invoke(vKey, vAlwaysConditionalPut));
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(99)));
        }
    };
