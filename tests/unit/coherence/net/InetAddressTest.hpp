/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"
#include "coherence/net/InetAddress.hpp"
#include "coherence/net/UnknownHostException.hpp"

#include "coherence/util/Arrays.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace std;

using coherence::net::InetAddress;
using coherence::net::UnknownHostException;
using coherence::util::Arrays;

/**
* InetAddressTest test suite
*/
class InetAddressTest : public CxxTest::TestSuite
    {
    public:
        void testGetLocalHost()
            {
            InetAddress::View vAddr = InetAddress::getLocalHost();
            TS_ASSERT(NULL != vAddr);
            TS_ASSERT(vAddr->getHostName()->length() > 0);
            TS_ASSERT(vAddr->getAddress()->length == 4 || vAddr->getAddress()->length == 16);
            TS_ASSERT(vAddr->getHostAddress()->length() > 0);
            TS_ASSERT(InetAddress::getByName(vAddr->getHostAddress())->equals(vAddr));
            }

        void testGetByIPText()
            {
            String::View vsLocal       = "127.0.0.1";
            String::View vsLocal6      = "0:0:0:0:0:0:0:1";

            InetAddress::View vAddr   = InetAddress::getByName(vsLocal);
            TS_ASSERT(vAddr->getHostName()->equals(vsLocal)  || 
                      vAddr->getHostName()->equals(vsLocal6));
            TS_ASSERT(vAddr->getAddress()->length == 4 ||
                      vAddr->getAddress()->length == 16);

            Array<octet_t>::View vaIP = vAddr->getAddress();
            if (vAddr->getAddress()->length == 4)
                {
                TS_ASSERT(vAddr->getHostAddress()->equals(vsLocal));
                TS_ASSERT(vaIP[0] == 127);
                TS_ASSERT(vaIP[1] == 0);
                TS_ASSERT(vaIP[2] == 0);
                TS_ASSERT(vaIP[3] == 1);
                }
            else
                {
                // could be 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 or
                //          0 0 0 0 0 0 0 0 0 0 0xff 0xff 0x7f 0 0 0x1
                for (size32_t i = 0; i < 10; ++i)
                    {
                    TS_ASSERT_EQUALS(vaIP[i], 0);
                    }
                TS_ASSERT(vaIP[10] == 0 || vaIP[10] == 0xff);
                TS_ASSERT(vaIP[11] == 0 || vaIP[11] == 0xff);
                TS_ASSERT(vaIP[12] == 0 || vaIP[12] == 0x7f);
                TS_ASSERT_EQUALS(vaIP[13], 0);
                TS_ASSERT_EQUALS(vaIP[14], 0);
                TS_ASSERT_EQUALS(vaIP[15], 1);
                }


            }

        void testGetByName()
            {
            String::View      vsLocal   = "localhost";
            String::View      vsLocal4  = "127.0.0.1";
            String::View      vsLocal6a = "0:0:0:0:0:0:0:1";
            String::View      vsLocal6b = "0:0:0:0:0:ffff:7f00:1";

            InetAddress::View vAddr = InetAddress::getByName(vsLocal);
            TS_ASSERT(vAddr->getHostName()->equals(vsLocal));
            TS_ASSERT(vAddr->getHostAddress()->equals(vsLocal4)    || 
                      vAddr->getHostAddress()->equals(vsLocal6a)   ||
                      vAddr->getHostAddress()->equals(vsLocal6b));

            Array<octet_t>::View vaIP = vAddr->getAddress();
            if (vaIP->length == 4)
                {
                TS_ASSERT(vaIP[0] == 127);
                TS_ASSERT(vaIP[1] == 0);
                TS_ASSERT(vaIP[2] == 0);
                TS_ASSERT(vaIP[3] == 1);
                }
            else
                {
                if (vAddr->getHostAddress()->equals(vsLocal6a))
                    {
                for (size32_t i = 0; i < 15; ++i)
                    {
                    TS_ASSERT_EQUALS(vaIP[i], 0);
                    }
                TS_ASSERT_EQUALS(vaIP[15], 1);
                    }
                else if (vAddr->getHostAddress()->equals(vsLocal6b))
                    {
                    for (size32_t i = 0; i < 10; ++i)
                        {
                        TS_ASSERT_EQUALS(vaIP[i], 0);
                        }
                    TS_ASSERT_EQUALS(vaIP[10], 0xff);
                    TS_ASSERT_EQUALS(vaIP[11], 0xff);
                    TS_ASSERT_EQUALS(vaIP[12], 0x7f);
                    TS_ASSERT_EQUALS(vaIP[13], 0);
                    TS_ASSERT_EQUALS(vaIP[14], 0);
                    TS_ASSERT_EQUALS(vaIP[15], 1);
                    }
                }
            }

        void testGetAllByName()
            {
            try
                {
                InetAddress::getAllByName("non.existant.host.name.neverever");
                TS_ASSERT(false);
                }
            catch (UnknownHostException::View)
                {
                }

            ObjectArray::View voaAddr = InetAddress::getAllByName(NULL);
            TS_ASSERT(voaAddr->length == 1);
            TS_ASSERT(cast<InetAddress::View>(voaAddr[0])->equals(InetAddress::getLoopbackAddress()));

            voaAddr = InetAddress::getAllByName("127.0.0.1");
            TS_ASSERT(voaAddr->length == 1);
            TS_ASSERT(cast<InetAddress::View>(voaAddr[0])->equals(InetAddress::getLoopbackAddress()));

            TS_ASSERT(Arrays::asList(InetAddress::getAllByName("localhost"))
                ->contains(InetAddress::getByName("localhost")));
            }

        void testGetLoopbackAddress()
            {
            TS_ASSERT(InetAddress::getLoopbackAddress()->equals(InetAddress::getByName("127.0.0.1")));
            }

        void testBadName()
            {
            try
                {
                InetAddress::getByName("non.existant.host.name.neverever");
                TS_ASSERT(false);
                }
            catch (UnknownHostException::View)
                {
                }
            }

        void testBadIPv4()
            {
            try
                {
                InetAddress::getByName("1.2.3.4.5");
                TS_ASSERT(false);
                }
            catch (UnknownHostException::View)
                {
                }
            }

        void testBadIPv6()
            {
            try
                {
                InetAddress::getByName("1:2:3:4:5:6:7:8:9");
                TS_ASSERT(false);
                }
            catch (UnknownHostException::View)
                {
                }
            }
    };
