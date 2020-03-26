/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/Service.hpp"
#include "coherence/util/ServiceEvent.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::run::xml::XmlElement;
using coherence::util::Listeners;
using coherence::util::Service;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;

COH_OPEN_NAMESPACE_ANON(ServiceEventTest)

class TestService
    : public class_spec<TestService,
        extends<Object>,
        implements<Service> >
    {
    friend class factory<TestService>;

    public:
        void addServiceListener(ServiceListener::Handle /*hListener*/)
            {
            }

        void removeServiceListener(ServiceListener::Handle /*hListener*/)
            {
            }

        void configure(XmlElement::View /*hXml*/)
            {
            }

        bool isRunning() const
            {
            return true;
            }

        void start()
            {
            }

        void shutdown()
            {
            }

        void stop()
            {
            }

    protected:
        TestService()
            {
            }
    };

class TestServiceListener
        : public class_spec<TestServiceListener,
            extends<Object>,
            implements<ServiceListener> >
    {
    friend class factory<TestServiceListener>;

    public:
        void serviceStarting(ServiceEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("serviceStarting");
            }

        void serviceStarted(ServiceEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("serviceStarted");
            }

        void serviceStopping(ServiceEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("serviceStopping");
            }

        void serviceStopped(ServiceEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("serviceStopped");
            }

    protected:
        TestServiceListener()
                : COH_NO_WARN(m_vEvt(self())),
                  COH_NO_WARN(m_vsMethod(self(), NULL))
            {
            }

    public:
        MemberView<ServiceEvent> m_vEvt;
        MemberView<String> m_vsMethod;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test suite for the ServiceEvent class.
*/
class ServiceEventTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test ServiceEvent creation.
        */
        void testCreate()
            {
            TestService::View    vService = TestService::create();
            ServiceEvent::Handle hEvt     = ServiceEvent::create(vService,
                    ServiceEvent::service_started);

            TS_ASSERT(hEvt->getSource() == vService);
            TS_ASSERT(hEvt->getService() == vService);
            TS_ASSERT(hEvt->getId() == ServiceEvent::service_started);
            }

        /**
        * Test dispatching of a ServiceEvent::service_starting event.
        */
        void testServiceStarting()
            {
            TestService::View           vService   = TestService::create();
            Listeners::Handle           hListeners = Listeners::create();
            TestServiceListener::Handle hListener  = TestServiceListener::create();
            ServiceEvent::Handle        hEvt       = ServiceEvent::create(
                    vService, ServiceEvent::service_starting);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("serviceStarting"));
            }

        /**
        * Test dispatching of a ServiceEvent::service_started event.
        */
        void testServiceStarted()
            {
            TestService::View           vService   = TestService::create();
            Listeners::Handle           hListeners = Listeners::create();
            TestServiceListener::Handle hListener  = TestServiceListener::create();
            ServiceEvent::Handle        hEvt       = ServiceEvent::create(
                    vService, ServiceEvent::service_started);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("serviceStarted"));
            }

        /**
        * Test dispatching of a ServiceEvent::service_stopping event.
        */
        void testServiceStopping()
            {
            TestService::View           vService   = TestService::create();
            Listeners::Handle           hListeners = Listeners::create();
            TestServiceListener::Handle hListener  = TestServiceListener::create();
            ServiceEvent::Handle        hEvt       = ServiceEvent::create(
                    vService, ServiceEvent::service_stopping);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("serviceStopping"));
            }

        /**
        * Test dispatching of a ServiceEvent::service_stopped event.
        */
        void testServiceStopped()
            {
            TestService::View           vService   = TestService::create();
            Listeners::Handle           hListeners = Listeners::create();
            TestServiceListener::Handle hListener  = TestServiceListener::create();
            ServiceEvent::Handle        hEvt       = ServiceEvent::create(
                    vService, ServiceEvent::service_stopped);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("serviceStopped"));
            }
    };

