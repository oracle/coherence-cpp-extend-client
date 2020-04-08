/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "mock/CommonMocks.hpp"
#include "coherence/lang.ns"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "private/coherence/component/net/extend/RemoteService.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/ConnectionEvent.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include "private/coherence/component/util/SafeInvocationService.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/component/util/SafeService.hpp"

using namespace coherence::lang;
using namespace std;
using namespace mock;

using coherence::component::util::SafeInvocationService;
using coherence::component::util::SafeNamedCache;
using coherence::component::util::SafeService;
using coherence::net::messaging::Channel;
using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::util::Iterator;
using coherence::util::Map;
using coherence::util::Set;
using coherence::util::logging::Logger;
using coherence::component::net::extend::RemoteService;
using coherence::net::messaging::ConnectionEvent;
using mock::BaseMock;
using mock::Expectation;

class SafeInvocationServiceTest : public CxxTest::TestSuite
    {

    public:

    void testQuery()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hService->setService(hMockWrappedService);
        hMockWrappedService->setStrict(true);

        //set expectations
        Set::View set = DummySet::create();
        Invocable::Handle invocable = DummyInvocable::create();

        hMockWrappedService->isRunning();
        hMockWrappedService->lastExpectation()->setBoolReturn(true);

        hMockWrappedService->query(invocable, set);

        //replay
        hMockWrappedService->replay();
        hService->query(invocable, set);

        //verify
        hMockWrappedService->verify();
        }

    void testGetConfig()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        SimpleElement::Handle hConfig = SimpleElement::create();
        hService->setConfig(hConfig);
        TS_ASSERT(hConfig == hService->getConfig());
        }

    void testGetSafeServiceState()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        hService->setSafeServiceState(SafeService::started);
        TS_ASSERT(SafeService::started == hService->getSafeServiceState());
        }

    void testGetService()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hService->setService(hMockWrappedService);
        TS_ASSERT(hMockWrappedService == hService->getService());
        }

    void testGetServiceName()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        String::Handle hName = "YoYoYo";
        hService->setServiceName(hName);
        TS_ASSERT(hName == hService->getServiceName());
        }

    void testGetServiceType()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        hService->setServiceType(ServiceInfo::remote_invocation);
        TS_ASSERT(ServiceInfo::remote_invocation == hService->getServiceType());
        }

    void testGetInfo()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hService->setService(hMockWrappedService);

        //set expectations
        ServiceInfo::View info = DummyServiceInfo::create();
        hMockWrappedService->getInfo();
        hMockWrappedService->lastExpectation()->setObjectReturn(info);

        //replay
        hMockWrappedService->replay();
        TS_ASSERT(hService->getInfo() == info);

        //verify
        hMockWrappedService->verify();
        }

    void testGetSetUserContext()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hService->setService(hMockWrappedService);
        hMockWrappedService->setStrict(true);

        //set expectations
        String::Handle hUserCtx = String::create("user-context");
        hMockWrappedService->setUserContext(hUserCtx);

        //replay
        hMockWrappedService->replay();
        hService->setUserContext(hUserCtx);
        TS_ASSERT(hUserCtx == hService->getUserContext());

        //verify
        hMockWrappedService->verify();
        }

    void testIsRunning()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        TS_ASSERT(!hService->isRunning());
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();

        hMockWrappedService->setStrict(true);

        hService->setService(hMockWrappedService);

        //set expectations
        hMockWrappedService->isRunning();
        hMockWrappedService->lastExpectation()->setBoolReturn(false);

        hMockWrappedService->isRunning();
        hMockWrappedService->lastExpectation()->setBoolReturn(true);

        //replay
        hMockWrappedService->replay();
        TS_ASSERT(!hService->isRunning());
        TS_ASSERT(hService->isRunning());

        //verify
        hMockWrappedService->verify();
        }

    void testConfigure()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        XmlElement::Handle hConfig = SimpleElement::create();
        hService->configure(hConfig);
        TS_ASSERT(hConfig == hService->getConfig());
        }

    void testDrainEvents()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        TS_ASSERT(!hService->isRunning());
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hMockWrappedService->setStrict(true);
        hService->setService(hMockWrappedService);

        //set expectations
        hMockWrappedService->drainEvents();

        //replay
        hMockWrappedService->replay();
        hService->drainEvents();

        //verify
        hMockWrappedService->verify();
        }






    void testStart()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        TS_ASSERT(!hService->isRunning());
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hMockWrappedService->setStrict(true);
        Object::Handle context = String::create("context");
        hService->setUserContext(context);
        XmlElement::Handle config = SimpleElement::create();
        hService->setConfig(config);

        // add these prior to start so that listeners are not added to
        // wrapped service
        MemberListener::Handle  hMemberListener  = DummyMemberListener::create();
        ServiceListener::Handle hServiceListener = DummyServiceListener::create();
        hService->addMemberListener(hMemberListener);
        hService->addServiceListener(hServiceListener);

        hService->setService(hMockWrappedService);

        //set expectations
        hMockWrappedService->isRunning();
        hMockWrappedService->lastExpectation()->setBoolReturn(false);
        hMockWrappedService->setUserContext(context);
        hMockWrappedService->configure(config);
        hMockWrappedService->addMemberListener(hMemberListener);
        hMockWrappedService->lastExpectation()->ignoreArguments();
        hMockWrappedService->addServiceListener(hServiceListener);
        hMockWrappedService->lastExpectation()->ignoreArguments();
        hMockWrappedService->start();

        //replay
        hMockWrappedService->replay();
        hService->start();

        //verify
        hMockWrappedService->verify();
        }



    void testStartWrappedServiceThrowsException()
        {
        SafeInvocationService::Handle hService = SafeInvocationService::create();
        TS_ASSERT(!hService->isRunning());
        MockRemoteInvocationService::Handle hMockWrappedService = MockRemoteInvocationService::create();
        hMockWrappedService->setStrict(true);
        Object::Handle context = String::create("context");
        hService->setUserContext(context);
        XmlElement::Handle config = SimpleElement::create();
        hService->setConfig(config);

        // add these prior to start so that listeners are not added to
        // wrapped service
        MemberListener::Handle  hMemberListener  = DummyMemberListener::create();
        ServiceListener::Handle hServiceListener = DummyServiceListener::create();
        hService->addMemberListener(hMemberListener);
        hService->addServiceListener(hServiceListener);

        hService->setService(hMockWrappedService);

        //set expectations
        hMockWrappedService->isRunning();
        hMockWrappedService->lastExpectation()->setBoolReturn(false);
        hMockWrappedService->setUserContext(context);
        hMockWrappedService->configure(config);
        hMockWrappedService->addMemberListener(hMemberListener);
        hMockWrappedService->lastExpectation()->ignoreArguments();
        hMockWrappedService->addServiceListener(hServiceListener);
        hMockWrappedService->lastExpectation()->ignoreArguments();
        hMockWrappedService->start();
        hMockWrappedService->lastExpectation()->setException(IllegalStateException::create("expected"));
        hMockWrappedService->stop();

        //replay
        hMockWrappedService->replay();
        int32_t nLevel = Logger::getLogger()->getLevel();
        Logger::getLogger()->setLevel(0);
        //TODO : Should be IllegalStateException...
        TS_ASSERT_THROWS(hService->start(), const Exception::View);
        Logger::getLogger()->setLevel(nLevel);

        //verify
        hMockWrappedService->verify();

        }
    };
