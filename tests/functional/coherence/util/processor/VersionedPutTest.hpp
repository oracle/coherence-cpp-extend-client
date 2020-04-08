/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"
#include "coherence/net/NamedCache.hpp"
#include "coherence/util/processor/VersionedPut.hpp"
#include "coherence/tests/VersionedObject.hpp"

using namespace coherence::lang;

using coherence::net::NamedCache;
using coherence::util::processor::VersionedPut;
using coherence::tests::VersionedObject;

/**
 * Test Suite for the VersionedPut.
 */
class VersionedPutSuite : public CxxTest::TestSuite
    {
public:
    /**
     * Test VersionedPut
     */
    void testVersionedPut()
        {
        NamedCache::Handle hCache = ensureCleanCache("dist-processor");

        Integer32::View vKey = Integer32::create(3);
        
        VersionedObject::Handle hVersioned = VersionedObject::create(1);
        
        hCache->put(vKey, hVersioned);

        VersionedPut::Handle vVersionedPut = 
                VersionedPut::create(hVersioned, true, true);
        
        TS_ASSERT(1 == hVersioned->getVersion());
        
        hCache->invoke(vKey, vVersionedPut);

        hVersioned = cast<VersionedObject::Handle>(hCache->get(vKey));

        TS_ASSERT(2 == hVersioned->getVersion());
        }
    };
