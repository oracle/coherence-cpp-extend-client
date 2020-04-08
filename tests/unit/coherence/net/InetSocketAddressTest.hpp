/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"
#include "coherence/net/InetSocketAddress.hpp"
#include "coherence/net/UnknownHostException.hpp"


#include <iostream>

using namespace coherence::lang;
using namespace std;

using coherence::net::InetSocketAddress;
using coherence::net::UnknownHostException;

/**
* InetSocketAddressTest test suite
*/
class InetSocketAddressTest : public CxxTest::TestSuite
    {
    public:
        void testGetResolved()
            {
            String::View      vsLocal  = "localhost";
            InetSocketAddress::View vAddr    = InetSocketAddress::create(vsLocal, 8088);
            TS_ASSERT(vAddr->getHostName()->equals(vsLocal));
            TS_ASSERT(!vAddr->isUnresolved());
            }

        void testGetUnresolved()
            {
            String::View      vsLocal  = "localhost";
            InetSocketAddress::View vAddr    = InetSocketAddress::create(vsLocal, 8088, false);
            TS_ASSERT(vAddr->getHostName()->equals(vsLocal));
            TS_ASSERT(vAddr->isUnresolved());
            }

        void testLateResolve()
            {
            String::View      vsLocal  = "localhost";
            InetSocketAddress::View vAddr    = InetSocketAddress::create(vsLocal, 8088, false);
            TS_ASSERT(vAddr->getHostName()->equals(vsLocal));
            TS_ASSERT(vAddr->isUnresolved());
            vAddr = cast<InetSocketAddress::View>(vAddr->resolve());
            TS_ASSERT(!vAddr->isUnresolved());
            }

        void testBadResolve()
            {
            String::View      vsBad  = "non.existant.host.name.neverever";
            InetSocketAddress::View vAddr = InetSocketAddress::create(vsBad, 8088);
            TS_ASSERT(vAddr->getHostName()->equals(vsBad));
            TS_ASSERT(vAddr->isUnresolved());
            try
                {
                vAddr = cast<InetSocketAddress::View>(vAddr->resolve());
                TS_ASSERT(false);
                }
            catch (UnknownHostException::View vex)
                {
                }
            }
    };
