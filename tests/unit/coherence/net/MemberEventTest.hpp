/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::io::Serializer;
using coherence::net::Member;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::ServiceInfo;
using coherence::run::xml::XmlElement;
using coherence::util::Listeners;
using coherence::util::ServiceListener;

COH_OPEN_NAMESPACE_ANON(MemberEventTest)

class TestNetService
    : public class_spec<TestNetService,
        extends<Object>,
        implements<coherence::net::Service, ServiceInfo> >
    {
    friend class factory<TestNetService>;

    public:
        ServiceInfo::View getInfo() const
            {
            return this;
            }

        void addMemberListener(MemberListener::Handle /*hListener*/)
            {
            }

        void removeMemberListener(MemberListener::Handle /*hListener*/)
            {
            }

        Object::Holder getUserContext() const
            {
            return m_ohCtx;
            }

        void setUserContext(Object::Holder ohCtx)
            {
            m_ohCtx = ohCtx;
            }

        String::View getServiceName() const
            {
            return NULL;
            }

        ServiceInfo::ServiceType getServiceType() const
            {
            return ServiceInfo::remote_cache;
            }

        void addServiceListener(coherence::util::ServiceListener::Handle /*hListener*/)
            {
            }

        void removeServiceListener(coherence::util::ServiceListener::Handle /*hListener*/)
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

        Serializer::View getSerializer() const
            {
            return NULL;
            }

        void setThreadGroup(ThreadGroup::Handle)
            {
            }

        ThreadGroup::Handle getThreadGroup()
            {
            return NULL;
            }

        ThreadGroup::View getThreadGroup() const
            {
            return NULL;
            }


    protected:
        TestNetService()
                : COH_NO_WARN(m_ohCtx(self()))
            {
            }

    protected:
        MemberHolder<Object> m_ohCtx;
    };

class TestMember
        : public class_spec<TestMember,
            extends<Object>,
            implements<Member> >
    {
    friend class factory<TestMember>;

    protected:
        TestMember()
            {
            }

    public:
        String::View getClusterName() const
            {
            return NULL;
            }

        String::View getSiteName() const
            {
            return NULL;
            }

        String::View getRackName() const
            {
            return NULL;
            }

        String::View getMachineName() const
            {
            return NULL;
            }

        String::View getProcessName() const
            {
            return NULL;
            }

        String::View getMemberName() const
            {
            return NULL;
            }

        String::View getRoleName() const
            {
            return NULL;
            }
    };

class TestMemberListener
    : public class_spec<TestMemberListener,
        extends<Object>,
        implements<MemberListener> >
    {
    friend class factory<TestMemberListener>;

    protected:
        TestMemberListener()
                : COH_NO_WARN(m_vEvt(self())),
                  COH_NO_WARN(m_vsMethod(self(), NULL))
            {
            }

    public:
        void memberJoined(MemberEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("memberJoined");
            }

        void memberLeaving(MemberEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("memberLeaving");
            }

        void memberLeft(MemberEvent::View vEvt)
            {
            m_vEvt     = vEvt;
            m_vsMethod = String::create("memberLeft");
            }

    public:
        MemberView<MemberEvent> m_vEvt;
        MemberView<String> m_vsMethod;
    };

COH_CLOSE_NAMESPACE_ANON

/**
* Test suite for the MemberEvent class.
*/
class MemberEventTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test MemberEvent creation.
        */
        void testCreate()
            {
            TestNetService::View vService = TestNetService::create();
            TestMember::View     vMember  = TestMember::create();
            MemberEvent::Handle  hEvt     = MemberEvent::create(vService,
                    MemberEvent::member_joined, vMember);

            TS_ASSERT(hEvt->getSource() == vService);
            TS_ASSERT(hEvt->getService() == vService);
            TS_ASSERT(hEvt->getId() == MemberEvent::member_joined);
            TS_ASSERT(hEvt->getMember() == vMember);
            TS_ASSERT(hEvt->isLocal());
            }

        /**
        * Test dispatching of a MemberEvent::member_joined event.
        */
        void testMemberJoined()
            {
            TestNetService::View       vService   = TestNetService::create();
            Listeners::Handle          hListeners = Listeners::create();
            TestMemberListener::Handle hListener  = TestMemberListener::create();
            TestMember::View           vMember    = TestMember::create();
            MemberEvent::Handle        hEvt       = MemberEvent::create(
                    vService, MemberEvent::member_joined, vMember);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("memberJoined"));
            }

        /**
        * Test dispatching of a MemberEvent::member_leaving event.
        */
        void testMemberLeaving()
            {
            TestNetService::View       vService   = TestNetService::create();
            Listeners::Handle          hListeners = Listeners::create();
            TestMemberListener::Handle hListener  = TestMemberListener::create();
            TestMember::View           vMember    = TestMember::create();
            MemberEvent::Handle        hEvt       = MemberEvent::create(
                    vService, MemberEvent::member_leaving, vMember);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("memberLeaving"));
            }

        /**
        * Test dispatching of a MemberEvent::member_left event.
        */
        void testMemberLeft()
            {
            TestNetService::View       vService   = TestNetService::create();
            Listeners::Handle          hListeners = Listeners::create();
            TestMemberListener::Handle hListener  = TestMemberListener::create();
            TestMember::View           vMember    = TestMember::create();
            MemberEvent::Handle        hEvt       = MemberEvent::create(
                    vService, MemberEvent::member_left, vMember);

            hListeners->add(hListener);
            hEvt->dispatch(hListeners);

            TS_ASSERT(hListener->m_vEvt == hEvt);
            TS_ASSERT(hListener->m_vsMethod->equals("memberLeft"));
            }
    };

