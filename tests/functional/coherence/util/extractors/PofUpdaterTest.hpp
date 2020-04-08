/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/RawDate.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/io/pof/reflect/SimplePofPath.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/extractor/PofUpdater.hpp"
#include "coherence/util/processor/UpdaterProcessor.hpp"
#include "common/TestClasses.hpp"

using namespace coherence::lang;

using coherence::io::pof::RawDate;
using coherence::io::pof::SystemPofContext;
using coherence::io::pof::reflect::SimplePofPath;
using coherence::net::NamedCache;
using coherence::util::ValueExtractor;
using coherence::util::extractor::PofUpdater;
using coherence::util::processor::UpdaterProcessor;
using common::test::TestOrder;
using common::test::TestOrderKey;

/**
* Test Suite for the PofUpdater.
*/
class PofUpdaterSuite : public CxxTest::TestSuite
  {
  public:  
    /**
    * Test PofUpdater with Filter
    */
    void testPofUpdaterWithFilter()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-filter");

        TestOrder::Handle    hOrder    = TestOrder::create(Integer32::create(123), String::create("CD"));
        TestOrderKey::Handle hOrderKey = TestOrderKey::create(hOrder);
        
        hCache->put(String::create("p1"), hOrder);
        hCache->put(String::create("p2"), hOrderKey);

        PofUpdater::Handle hUpdater1 = PofUpdater::create(1);
        Array<int32_t>::Handle haIndices = Array<int32_t>::create(2);
        haIndices[0] = 0;
        haIndices[1] = 0;
        PofUpdater::Handle hUpdater2 = PofUpdater::create(SimplePofPath::create(haIndices));
        
        hCache->invoke(String::create("p1"), UpdaterProcessor::create(hUpdater1, String::create("DVD")));
        hCache->invoke(String::create("p2"), UpdaterProcessor::create(hUpdater2, Integer32::create(777)));

        TestOrder::Handle hP1 = cast<TestOrder::Handle>(hCache->get(String::create("p1")));
        TS_ASSERT(hP1->getName()->equals(String::create("DVD")));
        
        TestOrderKey::Handle hP2 = cast<TestOrderKey::Handle>(hCache->get(String::create("p2")));
        TestOrder::View vP1 = cast<TestOrder::View>(hP2->getAssociatedKey());
        TS_ASSERT(vP1->getOrderId()->equals(Integer32::create(777)));
        }
  };
