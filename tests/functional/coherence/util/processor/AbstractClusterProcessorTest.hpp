/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/NamedCache.hpp"

#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/UUID.hpp"

#include "coherence/tests/ProcessorPrintUUIDTimestamp.hpp"

using namespace coherence::lang;
using coherence::net::InetAddress;
using coherence::net::NamedCache;
using coherence::io::pof::PortableObject;
using coherence::util::UUID;
using coherence::tests::ProcessorPrintUUIDTimestamp;

/**
 * Test Suite for the AbstractClusterProcessor.
 */
class AbstractClusterProcessorSuite : public CxxTest::TestSuite
    {
    public:
        /**
        * Test AbstractClusterProcessor
        */
        void testAbstractClusterProcessor()
            {
            NamedCache::Handle hCache = ensureCleanCache("dist-processor");

            // identical UUID's
            UUID::View vUUID  = UUID::create(System::currentTimeMillis(), InetAddress::getLocalHost(), 12345, 67890);
            String::View vKey = String::create("UUID");

            hCache->put(vKey, vUUID);

            UUID::View vCachedUUID = cast<UUID::View>(hCache->get(vKey));

            TS_ASSERT(vUUID->equals(vCachedUUID));

            TS_ASSERT_EQUALS(vUUID->getTimestamp(),
                    (cast<Integer64::View> (hCache->invoke(vKey, ProcessorPrintUUIDTimestamp::create())))->getInt64Value());
            }
    };
