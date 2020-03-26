/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/ConfigurableAddressProvider.hpp"
#include "coherence/net/InetAddress.hpp"
#include "coherence/net/InetSocketAddress.hpp"

#include "coherence/run/xml/XmlElement.hpp"

#include "coherence/util/Collection.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LiteSet.hpp"

#include <iostream>
#include <sstream>

using namespace coherence::lang;
using namespace std;

using coherence::net::CacheFactory;
using coherence::net::ConfigurableAddressProvider;
using coherence::net::InetAddress;
using coherence::net::InetSocketAddress;
using coherence::run::xml::XmlElement;
using coherence::util::Collection;
using coherence::util::Iterator;
using coherence::util::LiteSet;

/**
* ConfigurableAddressProviderTest test suite
*
* @author pp  2010.09.10
*/
class ConfigurableAddressProviderTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Assert that address rejection followed by address acceptance will
        * cause the getNextAddress call to start at the beginning of the
        * iteration instead of returning null.
        */
        void testAcceptLast()
            {
            ConfigurableAddressProvider::Handle hProvider
                    = ConfigurableAddressProvider::create(createConfiguration(2));

            InetSocketAddress::View vAddr = hProvider->getNextAddress();
            TS_ASSERT(NULL != vAddr);
            hProvider->reject(NULL);
            vAddr = hProvider->getNextAddress();
            TS_ASSERT(NULL != vAddr);
            hProvider->accept();
            vAddr = hProvider->getNextAddress();
            TS_ASSERT(NULL != vAddr);
            }

        /**
        * Assert that iteration of addresses without calling accept will result
        * in NULL being returned after all addresses have been iterated.
        */
        void testIteration()
            {
            int32_t cAddresses = 5;
            ConfigurableAddressProvider::Handle hProvider
                    = ConfigurableAddressProvider::create(createConfiguration(cAddresses));

            for (int32_t i = 0; i < cAddresses; i++)
                {
                hProvider->getNextAddress();
                }

            // Expected null after iterating all addresses
            TS_ASSERT(NULL == hProvider->getNextAddress());
            }

        void testConfigWithHostname()
            {
            // try a few different hostnames hoping for one that returns more than one IP
            Collection::Handle hColHosts = LiteSet::create();

            hColHosts->add(String::create("wpad"));
            hColHosts->add(String::create("time.apple.com"));

            for (Iterator::Handle hIter = hColHosts->iterator(); hIter->hasNext(); )
                {
                String::View                        vsHost        = cast<String::View>(hIter->next());
                int                                 nAddressCount = 0;
                ConfigurableAddressProvider::Handle hProvider     =
                    ConfigurableAddressProvider::create(createHostnameConfiguration(vsHost));

                for (InetSocketAddress::View vSockAddr = hProvider->getNextAddress(); vSockAddr != NULL;
                        vSockAddr = hProvider->getNextAddress())
                    {
                    ++nAddressCount;
                    TS_ASSERT_DIFFERS(vSockAddr->getAddress(), (InetAddress::View) NULL);
                    }
                std::cerr << std::endl << "ConfigurableAddressProvider returned " << nAddressCount
                    << " address(es) for host \"" << vsHost << "\"." << std::endl;
                if (nAddressCount > 1)
                    {
                    return;
                    }
                }

            std::cerr << std::endl << "Warning: unable to obtain a host with multiple IP addresses from "
                << hColHosts << std::endl;
            }

        /**
        * Assert that calling accept after iterating each address will result
        * in the first address being returned again.
        */
        void testIterationWithAccept()
            {
            int32_t cAddresses = 5;
            ConfigurableAddressProvider::Handle hProvider
                    = ConfigurableAddressProvider::create(createConfiguration(cAddresses));

            InetSocketAddress::View vAddr = hProvider->getNextAddress();
            hProvider->accept();

            for (int32_t i = 0; i < cAddresses - 1; i++)
                {
                hProvider->getNextAddress();
                hProvider->accept();
                }

            TS_ASSERT(vAddr->equals(hProvider->getNextAddress()));
            }


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Create a ConfigurableAddressProvider XML configuration.
        *
        * @param cAddresses  number of addresses to include in configuration;
        *                    values from 1 to 9 are supported
        *
        * @return XML configuration containing the number of addresses specified
        */
        XmlElement::Handle createConfiguration(int32_t cAddresses)
            {
            if (cAddresses < 1 || cAddresses > 9)
                {
                COH_THROW(IllegalArgumentException::create("Supported values are 1 to 9"));
                }

            stringstream builder;

            builder << "<socket-addresses>";
            for (int32_t i = 0; i < cAddresses; i++)
                {
                builder << "<socket-address><address>localhost</address><port>80"
                        << i << "</port></socket-address>";
                }
            builder << "</socket-addresses>" << ends;

            return CacheFactory::loadXml(builder);
            }

        /**
        * Create a ConfigurableAddressProvider XML configuration.
        *
        * @param cAddresses  number of addresses to include in configuration;
        *                    values from 1 to 9 are supported
        *
        * @return XML configuration containing the number of addresses specified
        */
        XmlElement::Handle createHostnameConfiguration(String::View vsHostname)
            {
            stringstream builder;

            builder << "<socket-addresses>";
                builder << "<socket-address><address>" << vsHostname << "</address><port>80"
                        << "</port></socket-address>";
            builder << "</socket-addresses>" << ends;

            return CacheFactory::loadXml(builder);
            }

    };
