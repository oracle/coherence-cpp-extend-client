/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/tests/ProxyControlInvocable.hpp"

#include "coherence/util/Map.hpp"

#include "common/TestUtils.hpp"

using namespace coherence::lang;

using coherence::net::CacheFactory;
using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::net::messaging::ConnectionException;
using coherence::tests::ProxyControlInvocable;
using coherence::util::Map;

/**
 * Check for memory leaks from proxy connection disconnects.
 */
class ProxyConnectionLeakTest : public CxxTest::TestSuite
    {

    // ----- test fixtures --------------------------------------------------
    public:
        /**
         * Runs after each test
         */
        void tearDown()
            {
            CacheFactory::shutdown();
            }

    // ----- tests ----------------------------------------------------------
    public:
        /**
         * Test for RemoteCacheService, Initiator, and Connection cleanup
         * on proxy connection close.
         *
         * See COH-15772
         */
        void testServicesCleanup()
            {
            controlProxy(ProxyControlInvocable::start_operation);

            Map::Handle hNamedCache = ensureCleanCache("dist-proxy-stop-test");

            String::View vsKey   = "hello";
            String::View vsValue = "world";

            // service, peer, and connection classes to track
            String::View vsInitiatorClass          = "coherence::component::util::TcpInitiator";
            String::View vsRemoteCacheServiceClass = "coherence::component::net::extend::RemoteCacheService";
            String::View vsRemoteNameServiceClass  = "coherence::component::net::extend::RemoteNameService";
            String::View vsTcpReader               = "coherence::component::util::TcpInitiator::TcpConnection::TcpReader";
            String::View vsTcpConnection           = "coherence::component::util::TcpInitiator::TcpConnection";

            // expected instance counts after the Proxy connection is closed and processed by the client
            const int64_t cExpectedInitiators  = 2; // RemoteInvocation and RemoteCache (reused) Initiators
            const int64_t cExpectedRCS         = 1; // just one RemoteCacheService (reused)
            const int64_t cExpectedRNS         = 0; // RemoteNameServices should be released up after lookup
            const int64_t cExpectedReaders     = 1; // TcpReader for RemoteInvocationService - others should be released
            const int64_t cExpectedConnections = 1; // TcpConnection for RemoteInvocationService - others should be released

            // issue a put and get to verify that the cache connection is good
            hNamedCache->put(vsKey, vsValue);
            TS_ASSERT(hNamedCache->get(vsKey)->equals(vsValue));

            // stop/start the proxy multiple times checking for leaks
            HeapAnalyzer::Handle   hHA                  = System::getHeapAnalyzer();
            Array<int64_t>::Handle hArr                 = Array<int64_t>::create(10);
            size32_t               cBaselineThreadCount = 0;
            String::View           vsBaselineThreads;
            for (size32_t i = 0; i < hArr->length; ++i)
                {
                controlProxy(ProxyControlInvocable::stop_operation);

                bool fMemClean = false;
                for (int j = 0; j < 30 * (1000/10) /* up to 30 seconds of 10ms sleeps */; ++j)
                    {
                    Map::View vMapSnapshot =
                        cast<ClassBasedHeapAnalyzer::Snapshot::View>(hHA->capture())->getStatsMap();

                    int64_t cInitiators    = cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsInitiatorClass))->getObjectCount();
                    int64_t cRCS           = cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsRemoteCacheServiceClass))->getObjectCount();
                    int64_t cRNS           = cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsRemoteNameServiceClass))->getObjectCount();
                    int64_t cTcpReader     = cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsTcpReader))->getObjectCount();
                    int64_t cTcpConnection = cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsTcpConnection))->getObjectCount();

                    if (cInitiators == cExpectedInitiators &&
                        cRCS == cExpectedRCS &&
                        cRNS == cExpectedRNS &&
                        cTcpReader == cExpectedReaders &&
                        cTcpConnection == cExpectedConnections)
                        {
                        fMemClean = true;
                        break;
                        }
                    Thread::sleep(10);
                    }
                if (!fMemClean)
                    {
                    Map::View vMapSnapshot = cast<ClassBasedHeapAnalyzer::Snapshot::View>(hHA->capture())->getStatsMap();

                    std::cout << "Leaked tracked objects!" << std::endl << vMapSnapshot << std::endl;

                    // asserts to show which one leaked
                    TS_ASSERT_EQUALS(cExpectedInitiators, cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsInitiatorClass))->getObjectCount());
                    TS_ASSERT_EQUALS(cExpectedRCS, cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsRemoteCacheServiceClass))->getObjectCount());
                    TS_ASSERT_EQUALS(cExpectedRNS, cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsRemoteNameServiceClass))->getObjectCount());
                    TS_ASSERT_EQUALS(cExpectedReaders, cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsTcpReader))->getObjectCount());
                    TS_ASSERT_EQUALS(cExpectedConnections, cast<ClassBasedHeapAnalyzer::ClassStats::View>(vMapSnapshot->get(vsTcpConnection))->getObjectCount());

                    TS_FAIL("Leaked tracked objects!");
                    }

                size32_t cThreads = Thread::currentThread()->getThreadGroup()->activeCount();

                if (i == size32_t(0))
                    {
                    // establish a baseline for the thread count
                    cBaselineThreadCount = cThreads;
                    vsBaselineThreads    = Thread::formatStacks();
                    }

                if (cBaselineThreadCount != cThreads)
                    {
                    // TODO: investigate why threads without a Coherence stack are sometimes
                    //       leaked in this test and in other tests.
                    std::cout << "Threads leaked!"                << std::endl
                        << "Baseline: " << vsBaselineThreads      << std::endl
                        << "Current: "  << Thread::formatStacks() << std::endl;
                    // TS_FAIL("Threads leaked!");
                    }

                // record object count after proxy connection closed
                hArr[i] = hHA->getObjectCount();

                // no proxy available currently
                TS_ASSERT_THROWS(hNamedCache->get(vsKey)->equals(vsValue), ConnectionException::View);

                controlProxy(ProxyControlInvocable::start_operation);

                // initiate a new connection now that the proxy is back up
                TS_ASSERT(hNamedCache->get(vsKey)->equals(vsValue));
                }

            // verify that all object counts are within 100 of the second iteration;
            // discard the first iteration
            std::cout << "Object counts per iteration: " << hArr << std::endl;
            int64_t cFirst = hArr[1];
            for (size32_t i = 2; i < hArr->length; ++i)
                {
                int64_t cNext = hArr[i];
                if (cFirst < cNext)
                    {
                    TS_ASSERT_LESS_THAN(cNext - cFirst, int64_t(200));
                    }
                else
                    {
                    TS_ASSERT_LESS_THAN(cFirst - cNext, int64_t(200));
                    }
                }

            controlProxy(ProxyControlInvocable::stop_operation);
            }

    private:
        /**
        * utility method to start/stop/restart the proxy.
        */
        void controlProxy(int32_t nOperation)
            {
            InvocationService::Handle hInvocationService = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpProxyControlService"));

            Invocable::Handle hInvocable = ProxyControlInvocable::create("ExtendTcpProxyServiceCOH8230", nOperation);
            hInvocationService->query(hInvocable, NULL);
            }
    };
