/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/run/xml/XmlElement.hpp"

#include "private/coherence/component/util/TcpInitiator.hpp"
#include "private/coherence/run/xml/SimpleParser.hpp"

using namespace coherence::lang;
using namespace std;

using coherence::component::util::TcpInitiator;
using coherence::net::DefaultOperationalContext;
using coherence::run::xml::SimpleParser;
using coherence::run::xml::XmlElement;


/**
* TcpInitiator Unit Test Suite.
*/
class TcpInitiatorTest : public CxxTest::TestSuite
    {

    // ----- test fixtures --------------------------------------------------

    public:
        /**
        * Test ConnectTimeout.
        */
        void testConnectTimeout()
            {
            // case 1: both ConnectTimeout and RequestTimeout configured, ConnectTimeout shows ConnectTimeout value
            stringstream ss1;
                ss1 << "         <initiator-config>"
                    << "            <tcp-initiator>"
                    << "               <remote-addresses>"
                    << "                  <socket-address>"
                    << "                     <address system-property=\"tangosol.coherence.proxy.address\">127.0.0.1</address>"
                    << "                     <port system-property=\"tangosol.coherence.proxy.port\">32000</port>"
                    << "                  </socket-address>"
                    << "               </remote-addresses>"
                    << "            </tcp-initiator>"
                    << "            <outgoing-message-handler>"
                    << "               <request-timeout>30s</request-timeout>"
                    << "            </outgoing-message-handler>"
                    << "            <connect-timeout>1s</connect-timeout>"
                    << "         </initiator-config>";
            TcpInitiator::Handle hInitiator= loadConfig(&ss1);
            TS_ASSERT(hInitiator->getConnectTimeout() != hInitiator->getRequestTimeout());

            // case 2: only ConnectTimeout configured, shows ConnectTimeout value
            stringstream ss2;
                ss2 << "         <initiator-config>"
                    << "            <tcp-initiator>"
                    << "               <remote-addresses>"
                    << "                  <socket-address>"
                    << "                     <address system-property=\"tangosol.coherence.proxy.address\">127.0.0.1</address>"
                    << "                     <port system-property=\"tangosol.coherence.proxy.port\">32000</port>"
                    << "                  </socket-address>"
                    << "               </remote-addresses>"
                    << "            </tcp-initiator>"
                    << "            <connect-timeout>1s</connect-timeout>"
                    << "         </initiator-config>";
            hInitiator= loadConfig(&ss2);
            TS_ASSERT(hInitiator->getConnectTimeout() != hInitiator->getRequestTimeout());

            // case 3: only RequestTimeout configured, ConnectTimeout shows RequestTimeoutvalue
            stringstream ss3;
                ss3 << "         <initiator-config>"
                    << "            <tcp-initiator>"
                    << "               <remote-addresses>"
                    << "                  <socket-address>"
                    << "                     <address system-property=\"tangosol.coherence.proxy.address\">127.0.0.1</address>"
                    << "                     <port system-property=\"tangosol.coherence.proxy.port\">32000</port>"
                    << "                  </socket-address>"
                    << "               </remote-addresses>"
                    << "            </tcp-initiator>"
                    << "            <outgoing-message-handler>"
                    << "               <request-timeout>30s</request-timeout>"
                    << "            </outgoing-message-handler>"
                    << "         </initiator-config>";
            hInitiator= loadConfig(&ss3);
            TS_ASSERT(hInitiator->getConnectTimeout() == hInitiator->getRequestTimeout());

            // case 4: neither ConnectTimeout or RequestTimeout configured, ConnectTimeout shows default RequestTimeout value
            stringstream ss4;
                ss4 << "         <initiator-config>"
                    << "            <tcp-initiator>"
                    << "               <remote-addresses>"
                    << "                  <socket-address>"
                    << "                     <address system-property=\"tangosol.coherence.proxy.address\">127.0.0.1</address>"
                    << "                     <port system-property=\"tangosol.coherence.proxy.port\">32000</port>"
                    << "                  </socket-address>"
                    << "               </remote-addresses>"
                    << "            </tcp-initiator>"
                    << "         </initiator-config>";
            hInitiator= loadConfig(&ss4);
            TS_ASSERT(hInitiator->getConnectTimeout() == hInitiator->getRequestTimeout());
            }

    private:

        /**
         * load test config
         */
        TcpInitiator::Handle loadConfig(stringstream *ss)
            {
            TcpInitiator::Handle hInitiator = TcpInitiator::create();
            hInitiator->setOperationalContext(DefaultOperationalContext::create());

            XmlElement::Handle hXml = SimpleParser::create()->parseXml(*ss);

            hInitiator->configure(hXml);
            return hInitiator;
            }
    };
