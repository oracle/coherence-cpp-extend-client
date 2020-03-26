/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/processor/ConditionalRemove.hpp"
#include "coherence/util/filter/AlwaysFilter.hpp"
#include "coherence/util/filter/NeverFilter.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::processor::ConditionalRemove;
using coherence::util::filter::AlwaysFilter;
using coherence::util::filter::NeverFilter;

/**
 * Test Suite for the ConditionalRemove.
 */
class ConditionalRemoveSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test ConditionalRemove
     */
    void testConditionalRemove()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        for (size32_t x = 0; x < 5; ++x)
            {
            Integer32::View vKey = Integer32::create(x);
            Integer32::View vValue = Integer32::create(1);

            hCache->put(vKey, vValue);
            }
        
        Integer32::View vKey = Integer32::create(3);
        
        NeverFilter::View hNeverFilter = NeverFilter::create();
        ConditionalRemove::Handle vNeverConditionalRemove = 
                ConditionalRemove::create(hNeverFilter, true);
 
        AlwaysFilter::View hAlwaysFilter = AlwaysFilter::create();
        ConditionalRemove::Handle vAlwaysConditionalRemove = 
                ConditionalRemove::create(hAlwaysFilter, true);
        
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(1)));

        TS_ASSERT(NULL == hCache->invoke(vKey, vNeverConditionalRemove));
        TS_ASSERT(hCache->get(vKey)->equals(Integer32::create(1)));

  
        TS_ASSERT(NULL == hCache->invoke(vKey, vAlwaysConditionalRemove));
        TS_ASSERT(NULL == hCache->get(vKey));
        }
    };
