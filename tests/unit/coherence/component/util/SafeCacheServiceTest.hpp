/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "cxxtest/TestSuite.h"

#include "coherence/net/DefaultOperationalContext.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ServiceEvent.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "private/coherence/component/net/extend/RemoteCacheService.hpp"
#include "private/coherence/component/util/SafeCacheService.hpp"
#include "private/coherence/component/util/SafeNamedCache.hpp"
#include "private/coherence/component/util/SafeService.hpp"
#include "private/coherence/net/cache/LocalNamedCache.hpp"
#include "private/coherence/net/LocalMember.hpp"
#include "private/coherence/run/xml/SimpleElement.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace std;

using coherence::component::net::extend::RemoteCacheService;
using coherence::component::util::SafeCacheService;
using coherence::component::util::SafeNamedCache;
using coherence::component::util::SafeService;
using coherence::net::cache::LocalNamedCache;
using coherence::net::DefaultOperationalContext;
using coherence::net::LocalMember;
using coherence::net::Member;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::NamedCache;
using coherence::net::ServiceInfo;
using coherence::net::internal::ScopedReferenceStore;
using coherence::run::xml::SimpleElement;
using coherence::run::xml::XmlElement;
using coherence::util::ArrayList;
using coherence::util::Collection;
using coherence::util::Map;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;
using coherence::util::HashMap;
using coherence::util::logging::Logger;

COH_OPEN_NAMESPACE_ANON(SafeCacheServiceTest)

COH_PRAGMA_PUSH

class TestRemoteCacheService
    : public class_spec<TestRemoteCacheService,
        extends<RemoteCacheService> >
    {
    friend class factory<TestRemoteCacheService>;

    private:
        bool m_fStartCalled;
        bool m_fStopCalled;
        bool m_fShutdownCalled;
        bool m_fRunning;
        bool m_fThrowExceptionFromReleaseCache;
        bool m_fThrowExceptionFromDestroyCache;
        bool m_fMemberListenerAdded;
        bool m_fMemberListenerRemoved;
        bool m_fServiceListenerAdded;
        bool m_fServiceListenerRemoved;
        bool m_fThrowExceptionFromStart;
        int32_t   m_nPushedLogLevel;

        MemberHandle<ArrayList> m_hCacheNames;
        MemberHandle<NamedCache> m_hEnsureCacheReturnValue;
        MemberView<String> m_vsEnsureCacheName;
        MemberHandle<NamedCache> m_hReleasedCache;
        MemberHandle<NamedCache> m_hDestroyedCache;
        MemberView<XmlElement> m_vXmlConfig;

        TestRemoteCacheService()
                : super(),
                  m_fStartCalled(false),
                  m_fStopCalled(false),
                  m_fShutdownCalled(false),
                  m_fRunning(false),
                  m_fThrowExceptionFromReleaseCache(false),
                  m_fThrowExceptionFromDestroyCache(false),
                  m_fMemberListenerAdded(false),
                  m_fMemberListenerRemoved(false),
                  m_fServiceListenerAdded(false),
                  m_fServiceListenerRemoved(false),
                  m_fThrowExceptionFromStart(false),
                  m_nPushedLogLevel(Logger::getLogger()->getLevel()),
                  m_hCacheNames(self(), ArrayList::create()),
                  m_hEnsureCacheReturnValue(self(), NamedCache::Handle(NULL)),
                  m_vsEnsureCacheName(self(), String::null_string),
                  m_hReleasedCache(self(), NamedCache::Handle(NULL)),
                  m_hDestroyedCache(self(), NamedCache::Handle(NULL)),
                  m_vXmlConfig(self(), XmlElement::Handle(NULL))
            {
            }

    public:
        static TestRemoteCacheService::Handle createRemoteTestService()
            {
            return TestRemoteCacheService::create();
            }

        void start()
            {
            m_fStartCalled = true;
            if (m_fThrowExceptionFromStart)
                {
                Logger::getLogger()->setLevel(0);
                COH_THROW(IllegalStateException::create("TEST"));
                }

            m_fRunning = true;
            }

        bool wasStartCalled() const
            {
            return m_fStartCalled;
            }

        void stop()
            {
            m_fStopCalled = true;
            m_fRunning    = false;
            }

        bool wasStopCalled() const
            {
            return m_fStopCalled;
            }

        void shutdown()
            {
            m_fShutdownCalled = true;
            m_fRunning        = false;
            }

        bool wasShutdownCalled()
            {
            return m_fShutdownCalled;
            }

        bool isRunning() const
            {
            return m_fRunning;
            }

        void setRunning(bool running)
            {
            m_fRunning = running;
            }

        Collection::View getCacheNames() const
            {
            return m_hCacheNames;
            }

        void configure(XmlElement::View vXml)
            {
            m_vXmlConfig = vXml;
            }

        bool wasConfigureCalled(XmlElement::View vXml) const
            {
            return Object::equals(vXml, m_vXmlConfig);
            }

        void returnCacheFromEnsureCache(String::View vsCacheName,
                NamedCache::Handle hCache)
            {
            m_vsEnsureCacheName       = vsCacheName;
            m_hEnsureCacheReturnValue = hCache;
            }

        void throwExceptionFromStart(bool fThrowException)
            {
            m_fThrowExceptionFromStart = fThrowException;
            }

        void throwExceptionFromReleaseCache(bool fThrowException)
            {
            m_fThrowExceptionFromReleaseCache = fThrowException;
            }

        void throwExceptionFromDestroyCache(bool fThrowException)
            {
            m_fThrowExceptionFromDestroyCache = fThrowException;
            }

        NamedCache::Handle ensureCache(String::View vsName)
            {
            if (Object::equals(m_vsEnsureCacheName, vsName))
                {
                return m_hEnsureCacheReturnValue;
                }
            return NULL;
            }

        void releaseCache(NamedCache::Handle hCache)
            {
            m_hReleasedCache = hCache;
            if (m_fThrowExceptionFromReleaseCache)
                {
                COH_THROW(IllegalArgumentException::create("test"));
                }
            }

        bool wasCacheReleased(NamedCache::Handle hCache) const
            {
            return Object::equals(hCache, m_hReleasedCache);
            }

        void destroyCache(NamedCache::Handle hDestroyedCache)
            {
            m_hDestroyedCache = hDestroyedCache;
            if (m_fThrowExceptionFromDestroyCache)
                {
                COH_THROW(IllegalArgumentException::create("test"));
                }
            }

        bool wasCacheDestroyed(NamedCache::Handle hCache) const
            {
            return Object::equals(hCache, m_hDestroyedCache);
            }

        void addServiceListener(ServiceListener::Handle hListener)
            {
            if (NULL != hListener)
                {
                m_fServiceListenerAdded = true;
                }
            }

        void removeServiceListener(ServiceListener::Handle hListener)
            {
            if (NULL != hListener)
                {
                m_fServiceListenerRemoved = true;
                }
            }

        bool wasServiceListenerAdded() const
            {
            return m_fServiceListenerAdded;
            }

        bool wasServiceListenerRemoved() const
            {
            return m_fServiceListenerRemoved;
            }

        void addMemberListener(MemberListener::Handle hListener)
            {
            if (NULL != hListener)
                {
                m_fMemberListenerAdded = true;
                }
            }

        void removeMemberListener(MemberListener::Handle hListener)
            {
            if (NULL != hListener)
                {
                m_fMemberListenerRemoved = true;
                }
            }

        bool wasMemberListenerAdded() const
            {
            return m_fMemberListenerAdded;
            }

        bool wasMemberListenerRemoved() const
            {
            return m_fMemberListenerRemoved;
            }

        void resetTestState()
            {
            m_fStartCalled                    = false;
            m_fStopCalled                     = false;
            m_fShutdownCalled                 = false;
            m_fRunning                        = false;
            m_fThrowExceptionFromReleaseCache = false;
            m_fThrowExceptionFromDestroyCache = false;
            m_fMemberListenerAdded            = false;
            m_fServiceListenerAdded           = false;
            m_fMemberListenerRemoved          = false;
            m_fServiceListenerRemoved         = false;
            m_fThrowExceptionFromStart        = false;

            m_hReleasedCache  = (NamedCache::Handle) NULL;
            m_hDestroyedCache = (NamedCache::Handle) NULL;
            m_vXmlConfig      = (XmlElement::Handle) NULL;

            Logger::getLogger()->setLevel(m_nPushedLogLevel);
            }
    };


class TestSafeCacheService
    : public class_spec<TestSafeCacheService,
        extends<SafeCacheService> >
    {
    friend class factory<TestSafeCacheService>;

    private:
        bool m_fThrowExceptionFromServiceStart;
        bool m_fCleanupCalled;
        mutable MemberHandle<TestRemoteCacheService> m_hWrappedService;

    private:
        TestSafeCacheService()
                : super(),
                  m_fThrowExceptionFromServiceStart(false),
                  m_fCleanupCalled(false),
                  m_hWrappedService(self(), TestRemoteCacheService::Handle(NULL), true)
            {
            }
    public:
        static TestSafeCacheService::Handle createTestService()
            {
            return TestSafeCacheService::create();
            }

        RemoteCacheService::Handle createRemoteCacheService() const
            {
            m_hWrappedService = TestRemoteCacheService::createRemoteTestService();
            m_hWrappedService->throwExceptionFromStart(m_fThrowExceptionFromServiceStart);
            return m_hWrappedService;
            }

        TestRemoteCacheService::Handle getWrappedService()
            {
            return m_hWrappedService;
            }

        void throwExceptionFromServiceStart(bool fThrow)
            {
            m_fThrowExceptionFromServiceStart = fThrow;
            }

        void cleanup()
            {
            m_fCleanupCalled = true;
            SafeCacheService::cleanup();
            }

        bool wasCleanupCalled()
            {
            return m_fCleanupCalled;
            }

        MemberListener::Handle getMemberCallback() const
            {
            return f_hMemberCallback;
            }

        ServiceListener::Handle getServiceCallback() const
            {
            return f_hServiceCallback;
            }

        void resetTestState()
            {
            m_fCleanupCalled = false;
            m_fThrowExceptionFromServiceStart = false;
            if (NULL != m_hWrappedService)
                {
                m_hWrappedService->resetTestState();
                }
            }
    };


class TestListener
    : public class_spec<TestListener,
        extends<Object>,
        implements<MemberListener, ServiceListener> >
    {
    friend class factory<TestListener>;

    protected:
        TestListener()
                : m_vMemberJoinedEvent(self(), MemberEvent::View(NULL)),
                  m_vMemberLeavingEvent(self(), MemberEvent::View(NULL)),
                  m_vMemberLeftEvent(self(), MemberEvent::View(NULL)),
                  m_vServiceStartedEvent(self(), ServiceEvent::View(NULL)),
                  m_vServiceStartingEvent(self(), ServiceEvent::View(NULL)),
                  m_vServiceStoppedEvent(self(), ServiceEvent::View(NULL)),
                  m_vServiceStoppingEvent(self(), ServiceEvent::View(NULL))
            {
            }

    public:
        virtual void memberJoined(MemberEvent::View vEvt)
            {
            m_vMemberJoinedEvent = vEvt;
            }

        bool wasMemberJoinedCalled(SafeCacheService::Handle hSafeService,
                Member::Handle hMember)
            {
            return NULL != m_vMemberJoinedEvent &&
                   m_vMemberJoinedEvent->getId() == MemberEvent::member_joined &&
                   Object::equals(hSafeService, m_vMemberJoinedEvent->getSource()) &&
                   Object::equals(hMember, m_vMemberJoinedEvent->getMember());
            }

        virtual void memberLeaving(MemberEvent::View vEvt)
            {
            m_vMemberLeavingEvent = vEvt;
            }

        bool wasMemberLeavingCalled(SafeCacheService::Handle hSafeService,
                Member::Handle hMember)
            {
            return NULL != m_vMemberLeavingEvent &&
                   m_vMemberLeavingEvent->getId() == MemberEvent::member_leaving &&
                   Object::equals(hSafeService, m_vMemberLeavingEvent->getSource()) &&
                   Object::equals(hMember, m_vMemberLeavingEvent->getMember());
            }

        virtual void memberLeft(MemberEvent::View vEvt)
            {
            m_vMemberLeftEvent = vEvt;
            }

        bool wasMemberLeftCalled(SafeCacheService::Handle hSafeService,
                Member::Handle hMember)
            {
            return NULL != m_vMemberLeftEvent &&
                   m_vMemberLeftEvent->getId() == MemberEvent::member_left &&
                   Object::equals(hSafeService, m_vMemberLeftEvent->getSource()) &&
                   Object::equals(hMember, m_vMemberLeftEvent->getMember());
            }

    public:
        virtual void serviceStarted(ServiceEvent::View vEvt)
            {
            m_vServiceStartedEvent = vEvt;
            }

        bool wasServiceStartedCalled(SafeCacheService::Handle hSafeService)
            {
            return NULL != m_vServiceStartedEvent &&
                   m_vServiceStartedEvent->getId() == ServiceEvent::service_started &&
                   Object::equals(hSafeService, m_vServiceStartedEvent->getSource());
            }

        virtual void serviceStarting(ServiceEvent::View vEvt)
            {
            m_vServiceStartingEvent = vEvt;
            }

        bool wasServiceStartingCalled(SafeCacheService::Handle hSafeService)
            {
            return NULL != m_vServiceStartingEvent &&
                   m_vServiceStartingEvent->getId() == ServiceEvent::service_starting &&
                   Object::equals(hSafeService, m_vServiceStartingEvent->getSource());
            }

        virtual void serviceStopped(ServiceEvent::View vEvt)
            {
            m_vServiceStoppedEvent = vEvt;
            }

        bool wasServiceStoppedCalled(SafeCacheService::Handle hSafeService)
            {
            return NULL != m_vServiceStoppedEvent &&
                   m_vServiceStoppedEvent->getId() == ServiceEvent::service_stopped &&
                   Object::equals(hSafeService, m_vServiceStoppedEvent->getSource());
            }

        virtual void serviceStopping(ServiceEvent::View vEvt)
            {
            m_vServiceStoppingEvent = vEvt;
            }

        bool wasServiceStoppingCalled(SafeCacheService::Handle hSafeService)
            {
            return NULL != m_vServiceStoppingEvent &&
                   m_vServiceStoppingEvent->getId() == ServiceEvent::service_stopping &&
                   Object::equals(hSafeService, m_vServiceStoppingEvent->getSource());
            }

    protected:
        MemberView<MemberEvent> m_vMemberJoinedEvent;
        MemberView<MemberEvent> m_vMemberLeavingEvent;
        MemberView<MemberEvent> m_vMemberLeftEvent;
        MemberView<ServiceEvent> m_vServiceStartedEvent;
        MemberView<ServiceEvent> m_vServiceStartingEvent;
        MemberView<ServiceEvent> m_vServiceStoppedEvent;
        MemberView<ServiceEvent> m_vServiceStoppingEvent;
    };

COH_CLOSE_NAMESPACE_ANON

class SafeCacheServiceTest : public CxxTest::TestSuite
  {

    public:
        /**
        * getCachenames() should ensure wrapped cahce is started, if not, it
        * should be started and getCacheNames() should be invoked on running
        * wrapped service.
        *
        */
        void testCacheNames()
            {
            TestSafeCacheService::Handle hService = newTestService();
            Collection::View vNames = hService->getCacheNames();

            TestRemoteCacheService::View hWrappedService =
                    cast<TestRemoteCacheService::View>(hService->getService());

            TS_ASSERT(hWrappedService->wasStartCalled());

            // ensure that the the collection was retrieved from the wrapped service
            TS_ASSERT(vNames == hService->getCacheService()->getCacheNames());
            }

        void testGetCacheService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TestRemoteCacheService::Handle hRemoteService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hRemoteService);
            TS_ASSERT(hRemoteService == hService->getCacheService());
            }

        void testgetNamedCacheStore()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(hService->getOperationalContext());
            hService->setNamedCacheStore(hNamedCacheStore);
            TS_ASSERT(hNamedCacheStore == hService->getNamedCacheStore());
            }

        void testGetConfig()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            SimpleElement::Handle hConfig = SimpleElement::create();
            hService->setConfig(hConfig);
            TS_ASSERT(hConfig == hService->getConfig());
            }

        void testGetSafeServiceState()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->setSafeServiceState(SafeService::started);
            TS_ASSERT(SafeService::started == hService->getSafeServiceState());
            }

        void testGetService()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);
            TS_ASSERT(hWrappedService == hService->getService());
            }

        void testGetServiceName()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            String::Handle hName = "YoYoYo";
            hService->setServiceName(hName);
            TS_ASSERT(hName == hService->getServiceName());
            }

        void testGetServiceType()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->setServiceType(ServiceInfo::remote_cache);
            TS_ASSERT(ServiceInfo::remote_cache == hService->getServiceType());
            }

        void testGetInfo()
            {
            TestSafeCacheService::Handle hService = newTestService();
            coherence::net::Service::Handle hRemoteService =
                    hService->ensureRunningService();
            TS_ASSERT(hRemoteService->getInfo() == hService->getInfo());
            }

        void testGetUserContext()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);
            // Test that it is also set on non-null wrapped service
            String::Handle hUserCtx = String::create("user-context");
            hService->setUserContext(hUserCtx);
            TS_ASSERT(hUserCtx == hService->getUserContext());
            TS_ASSERT(hUserCtx == hWrappedService->getUserContext());
            }

        void testIsRunning()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TS_ASSERT(!hService->isRunning());
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);
            TS_ASSERT(!hService->isRunning());

            // calling start() on the test remote service changes it's state to running
            hWrappedService->start();
            TS_ASSERT(hService->isRunning());
            }

        void testConfigure()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            XmlElement::Handle hConfig = SimpleElement::create();
            hService->configure(hConfig);
            TS_ASSERT(hConfig == hService->getConfig());
            }

        void testEnsureCache()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hService->setNamedCacheStore(hNamedCacheStore);

            SafeNamedCache::Handle hDefaultCache = SafeNamedCache::create();
            hDefaultCache->setCacheName("Default");
            SafeNamedCache::Handle hNamedCache = SafeNamedCache::create();
            hNamedCache->setCacheName("test-name");
            hNamedCacheStore->putCache(hDefaultCache);
            hNamedCacheStore->putCache(hNamedCache);

            TS_ASSERT(hNamedCache == hService->ensureCache("test-name"));
            TS_ASSERT(hDefaultCache == hService->ensureCache("Default"));
            }

        void testEnsureCacheNullName()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hService->setNamedCacheStore(hNamedCacheStore);
            SafeNamedCache::Handle hNamedCache = SafeNamedCache::create();
            hNamedCache->setCacheName("Default");
            // map named cache to "Default"
            hNamedCacheStore->putCache(hNamedCache);
            // should return "Default" named cache when NULL is passed in
            TS_ASSERT(hNamedCache == hService->ensureCache(String::null_string));
            // should return "Default" named cache when "" is passed in
            TS_ASSERT(hNamedCache == hService->ensureCache(""));
            }

        void testEnsureCacheRetrieveFromWrappedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            NamedCache::Handle hCache = LocalNamedCache::create("test");
            // tell the test remote cache to return hCache when ensureCache
            // is called with "test"
            hWrappedService->returnCacheFromEnsureCache("test", hCache);

            SafeNamedCache::Handle hSafeCache =
                    cast<SafeNamedCache::Handle>(hService->ensureCache("test"));
            TS_ASSERT(hService == hSafeCache->getCacheService());
            TS_ASSERT(hSafeCache->getCacheName()->equals("test"));
            TS_ASSERT(hCache == hSafeCache->getNamedCache());
            // ensure that safeCache was put into map
            TS_ASSERT(hSafeCache ==
                    hService->getNamedCacheStore()->getCache("test"));
            }

        void testEnsureCacheRetrieveFromNotStartedWrappedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);

            NamedCache::Handle hCache = LocalNamedCache::create("test");
            // tell the test remote cache to return hCache when ensureCache
            // is called with "test"
            hWrappedService->returnCacheFromEnsureCache("test", hCache);

            SafeNamedCache::Handle hSafeCache =
                    cast<SafeNamedCache::Handle>(hService->ensureCache("test"));
            TS_ASSERT(hWrappedService->wasStartCalled());
            TS_ASSERT(hService == hSafeCache->getCacheService());
            TS_ASSERT(hSafeCache->getCacheName()->equals("test"));
            TS_ASSERT(hCache == hSafeCache->getNamedCache());
            // ensure that safeCache was put into map
            TS_ASSERT(hSafeCache ==
                    hService->getNamedCacheStore()->getCache("test"));
            }

        void testReleaseCache()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            hService->releaseCache(hSafeCache);
            TS_ASSERT(hService->getNamedCacheStore()->getAllCaches()->isEmpty());
            }

        void testReleaseCacheReleasesWrappedCache()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            NamedCache::Handle hNamedCache = LocalNamedCache::create("remote-cache");

            hWrappedService->returnCacheFromEnsureCache("remote-cache", hNamedCache);
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hSafeCache->setNamedCache(hNamedCache);

            hService->releaseCache(hSafeCache);
            TS_ASSERT(hWrappedService->wasCacheReleased(hNamedCache));
            }

        void testReleaseRemoteCacheThrowsException()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            NamedCache::Handle hNamedCache = LocalNamedCache::create("remote-cache");

            hWrappedService->returnCacheFromEnsureCache("remote-cache", hNamedCache);
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hSafeCache->setNamedCache(hNamedCache);

            hWrappedService->throwExceptionFromReleaseCache(true);
            // SafeCacheService should re-throw the exception if started
            TS_ASSERT_THROWS(hService->releaseCache(hSafeCache),
                             IllegalArgumentException::View);
            }

        void testReleaseNotRunningRemoteCacheThrowsException()
            {
            TestSafeCacheService::Handle hService = newTestService();
            // service not started
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);

            NamedCache::Handle hNamedCache = LocalNamedCache::create("remote-cache");
            hWrappedService->returnCacheFromEnsureCache("remote-cache", hNamedCache);
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hSafeCache->setNamedCache(hNamedCache);

            hWrappedService->throwExceptionFromReleaseCache(true);
            // SafeCacheService should not re-throw the exception if not started
            hService->releaseCache(hSafeCache);
            TS_ASSERT(hWrappedService->wasCacheReleased(hNamedCache));
            }

        void testDestroyCache()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            hService->setServiceType(ServiceInfo::remote_cache);
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            NamedCache::Handle hNamedCache = LocalNamedCache::create("remote-cache");

            hWrappedService->returnCacheFromEnsureCache("remote-cache", hNamedCache);
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hSafeCache->setNamedCache(hNamedCache);

            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            hService->destroyCache(hSafeCache);
            // assert that cache was removed from named cache map
            TS_ASSERT(hNamedCacheStore->getAllCaches()->isEmpty());
            // assert that destroyCache was called on the wrapped service
            TS_ASSERT(hWrappedService->wasCacheDestroyed(hNamedCache));
            }

        void testDestroyCacheNullWrappedCache()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            hService->setServiceType(ServiceInfo::remote_cache);
            hService->start();

            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");

            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            // exception expected because cache was already released
            TS_ASSERT_THROWS(hService->destroyCache(hSafeCache),
                             IllegalStateException::View);

            // assert that cache was removed from named cache map
            TS_ASSERT(hNamedCacheStore->getAllCaches()->isEmpty());
            }

        void testDestroyCacheNullWrappedCacheServiceIsNull()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            hService->setServiceType(ServiceInfo::remote_cache);

            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");

            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            // no exception expected because wrapped service is NULL
            hService->destroyCache(hSafeCache);
            // assert that cache was removed from named cache map
            TS_ASSERT(hNamedCacheStore->getAllCaches()->isEmpty());
            }

        void testDestroyCacheNullWrappedCacheServiceNotRunning()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            hService->setServiceType(ServiceInfo::remote_cache);
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);

            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");

            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            // no exception expected because wrapped service is not running
            hService->destroyCache(hSafeCache);
            // assert that cache was removed from named cache map
            TS_ASSERT(hNamedCacheStore->getAllCaches()->isEmpty());
            }

        void testDestroyCacheRemoteServiceThrowsException()
            {
            TestSafeCacheService::Handle hService = TestSafeCacheService::createTestService();
            hService->setServiceType(ServiceInfo::remote_cache);
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            NamedCache::Handle hNamedCache = LocalNamedCache::create("remote-cache");

            hWrappedService->returnCacheFromEnsureCache("remote-cache", hNamedCache);
            SafeNamedCache::Handle hSafeCache = SafeNamedCache::create();
            hSafeCache->setCacheName("test");
            hSafeCache->setNamedCache(hNamedCache);

            ScopedReferenceStore::Handle hNamedCacheStore =
                    ScopedReferenceStore::create(
                            hService->getOperationalContext());
            hNamedCacheStore->putCache(hSafeCache);
            hService->setNamedCacheStore(hNamedCacheStore);

            hWrappedService->throwExceptionFromDestroyCache(true);
            // Assert that exception is re-thrown
            TS_ASSERT_THROWS(hService->destroyCache(hSafeCache),
                             IllegalArgumentException::View);

            // assert that cache was removed from named cache map
            TS_ASSERT(hNamedCacheStore->getAllCaches()->isEmpty());
            }

        void testStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TS_ASSERT(!hService->isRunning());

            // add these prior to start so that listeners are not added to
            // wrapped service
            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            hService->start();

            TestRemoteCacheService::View vWrappedService =
                    cast<TestRemoteCacheService::View>(hService->getService());
            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    vWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    vWrappedService->getUserContext()));
            TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(vWrappedService->wasStartCalled());
            }

        void testStartWrappedServiceThrowsException()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->throwExceptionFromServiceStart(true);

            TS_ASSERT_THROWS(hService->start(), IllegalStateException::View);
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::started == hService->getSafeServiceState());
            TS_ASSERT(!hService->isRunning());
            }

        void testStartNoListenersRegistered()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::View vWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(!vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(!vWrappedService->wasServiceListenerAdded());
            }

        void testStartStoppedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();

            TS_ASSERT(!hService->isRunning());
            hService->start();

            TestRemoteCacheService::View vWrappedService =
                    cast<TestRemoteCacheService::View>(hService->getService());
            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    vWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    vWrappedService->getUserContext()));
            TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(vWrappedService->wasStartCalled());
            }

        void testStartWhenWrappedServiceStopped()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            TestRemoteCacheService::Handle hOrigWrappedService =
                    hService->getWrappedService();

            hOrigWrappedService->stop();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(hWrappedService != hOrigWrappedService);

            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    hWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    hWrappedService->getUserContext()));
            TS_ASSERT(hWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(hWrappedService->wasStartCalled());
            }

        void testStartWithInvalidServiceType()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->setServiceType(ServiceInfo::local_cache);

            TS_ASSERT_THROWS(hService->start(),
                    UnsupportedOperationException::View);

            TS_ASSERT(SafeService::started == hService->getSafeServiceState());
            TS_ASSERT(!hService->isRunning());
            }

        void testStartAlreadyStartedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    hWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    hWrappedService->getUserContext()));
            TS_ASSERT(hWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(hWrappedService->wasStartCalled());
            }

        void testStartAfterFailedStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->throwExceptionFromServiceStart(true);

            try
                {
                hService->start();
                }
            catch (IllegalStateException::View)
                {
                // expected
                }

            hService->throwExceptionFromServiceStart(false);
            hService->getWrappedService()->resetTestState();

            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    hWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    hWrappedService->getUserContext()));
            TS_ASSERT(hWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(hWrappedService->wasStartCalled());
            }

        void testStartInitialStateAndWrappedServiceNotRunning()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            hService->setSafeServiceState(SafeService::initial);
            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            hWrappedService->stop();
            hWrappedService->resetTestState();

            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            hService->start();

            TestRemoteCacheService::View vWrappedService =
                    cast<TestRemoteCacheService::View>(hService->getService());
            TS_ASSERT(hService->getServiceType() == ServiceInfo::remote_cache);
            TS_ASSERT(vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::started);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    vWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    vWrappedService->getUserContext()));
            TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(vWrappedService->wasStartCalled());
            }

        void testEnsureRunningService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            TestRemoteCacheService::Handle hWrappedService =
                    cast<TestRemoteCacheService::Handle>(hService->ensureRunningService());

            TS_ASSERT(hService->getService() == hWrappedService);
            TS_ASSERT(ServiceInfo::remote_cache == hService->getInfo()->getServiceType());
            TS_ASSERT(hWrappedService->wasMemberListenerAdded());
            TS_ASSERT(hWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::initial);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    hWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    hWrappedService->getUserContext()));
            TS_ASSERT(hWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(hWrappedService->wasStartCalled());
            }

        void testEnsureRunningServiceWrappedServiceThrowsException()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->throwExceptionFromServiceStart(true);

            TS_ASSERT_THROWS(hService->ensureRunningService(),
                    IllegalStateException::View);
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::initial == hService->getSafeServiceState());
            TS_ASSERT(!hService->isRunning());
            }

        void testEnsureRunningServiceNoListenersRegistered()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->ensureRunningService();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(!hWrappedService->wasMemberListenerAdded());
            TS_ASSERT(!hWrappedService->wasServiceListenerAdded());
            }

        void testEnsureRunningServiceOnStoppedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();

            TS_ASSERT(!hService->isRunning());
            hService->getWrappedService()->resetTestState();

            TS_ASSERT_THROWS(hService->ensureRunningService(),
                    IllegalStateException::View);

            TS_ASSERT(!hService->isRunning());
            TS_ASSERT(!hService->getWrappedService()->wasStartCalled());
            }

        void testEnsureRunningServiceOnRunningService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            hService->getWrappedService()->resetTestState();
            hService->getWrappedService()->setRunning(true);
            TestRemoteCacheService::View vWrappedService =
                cast<TestRemoteCacheService::View>(hService->getService());

            TS_ASSERT(vWrappedService == hService->ensureRunningService());
            TS_ASSERT(!vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(!vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(!vWrappedService->wasStartCalled());
            }

        void testEnsureRunningCacheService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            TestRemoteCacheService::Handle hWrappedService =
                    cast<TestRemoteCacheService::Handle>(hService->ensureRunningCacheService());

            TS_ASSERT(hService->getService() == hWrappedService);
            TS_ASSERT(ServiceInfo::remote_cache == hService->getInfo()->getServiceType());
            TS_ASSERT(hWrappedService->wasMemberListenerAdded());
            TS_ASSERT(hWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::initial);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    hWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    hWrappedService->getUserContext()));
            TS_ASSERT(hWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(hWrappedService->wasStartCalled());
            }

        void testEnsureRunningCacheServiceWrappedServiceThrowsException()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->throwExceptionFromServiceStart(true);

            TS_ASSERT_THROWS(hService->ensureRunningCacheService(),
                    IllegalStateException::View);
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::initial == hService->getSafeServiceState());
            TS_ASSERT(!hService->isRunning());
            }

        void testEnsureRunningCacheServiceNoListenersRegistered()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->ensureRunningCacheService();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(!hWrappedService->wasMemberListenerAdded());
            TS_ASSERT(!hWrappedService->wasServiceListenerAdded());
            }

        void testEnsureRunningCacheServiceOnStoppedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();

            TS_ASSERT(!hService->isRunning());
            hService->getWrappedService()->resetTestState();

            TS_ASSERT_THROWS(hService->ensureRunningCacheService(),
                    IllegalStateException::View);

            TS_ASSERT(!hService->isRunning());
            TS_ASSERT(!hService->getWrappedService()->wasStartCalled());
            }

        void testEnsureRunningCacheServiceOnRunningService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            hService->getWrappedService()->resetTestState();
            hService->getWrappedService()->setRunning(true);
            TestRemoteCacheService::View vWrappedService =
                cast<TestRemoteCacheService::View>(hService->getService());

            TS_ASSERT(vWrappedService == hService->ensureRunningCacheService());
            TS_ASSERT(!vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(!vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(!vWrappedService->wasStartCalled());
            }

        // test the const version
        void testEnsureRunningServiceConst()
        {
            TestSafeCacheService::Handle hService = newTestService();
            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            TestRemoteCacheService::View vWrappedService = cast<TestRemoteCacheService::View>(
                    cast<TestSafeCacheService::View>(hService)->ensureRunningService());

            TS_ASSERT(hService->getService() == vWrappedService);
            TS_ASSERT(ServiceInfo::remote_cache == hService->getInfo()->getServiceType());
            TS_ASSERT(vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::initial);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    vWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    vWrappedService->getUserContext()));
            TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(vWrappedService->wasStartCalled());
        }

        // Test the const version of ensureRunningCacheService
        void testEnsureRunningCacheServiceConst()
        {
        TestSafeCacheService::Handle hService = newTestService();
        MemberListener::Handle  hMemberListener  = TestListener::create();
        ServiceListener::Handle hServiceListener = TestListener::create();
        hService->addMemberListener(hMemberListener);
        hService->addServiceListener(hServiceListener);

        TestRemoteCacheService::View vWrappedService = cast<TestRemoteCacheService::View>(
                    cast<TestSafeCacheService::View>(hService)->ensureRunningCacheService());

        TS_ASSERT(hService->getService() == vWrappedService);
        TS_ASSERT(ServiceInfo::remote_cache == hService->getInfo()->getServiceType());
        TS_ASSERT(vWrappedService->wasMemberListenerAdded());
        TS_ASSERT(vWrappedService->wasServiceListenerAdded());
        TS_ASSERT(hService->getSafeServiceState() == SafeService::initial);
        TS_ASSERT(hService->isRunning());
        TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
        TS_ASSERT(Object::equals(hService->getServiceName(),
                vWrappedService->getServiceName()));
        TS_ASSERT(Object::equals(hService->getUserContext(),
                vWrappedService->getUserContext()));
        TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
        TS_ASSERT(vWrappedService->wasStartCalled());
        }

        // simply calls through to ensureRunningService()
        void testGetRunningService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            MemberListener::Handle  hMemberListener  = TestListener::create();
            ServiceListener::Handle hServiceListener = TestListener::create();
            hService->addMemberListener(hMemberListener);
            hService->addServiceListener(hServiceListener);

            TestRemoteCacheService::View vWrappedService =
                    cast<TestRemoteCacheService::View>(hService->getRunningService());

            TS_ASSERT(hService->getService() == vWrappedService);
            TS_ASSERT(ServiceInfo::remote_cache == hService->getInfo()->getServiceType());
            TS_ASSERT(vWrappedService->wasMemberListenerAdded());
            TS_ASSERT(vWrappedService->wasServiceListenerAdded());
            TS_ASSERT(hService->getSafeServiceState() == SafeService::initial);
            TS_ASSERT(hService->isRunning());
            TS_ASSERT(hService->getServiceName()->equals("Test-Safe-Cache-Service"));
            TS_ASSERT(Object::equals(hService->getServiceName(),
                    vWrappedService->getServiceName()));
            TS_ASSERT(Object::equals(hService->getUserContext(),
                    vWrappedService->getUserContext()));
            TS_ASSERT(vWrappedService->wasConfigureCalled(hService->getConfig()));
            TS_ASSERT(vWrappedService->wasStartCalled());
            }

        void testStop()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();

            TS_ASSERT(hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testStopAlreadyStoppedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();
            //reset mock state
            hService->resetTestState();

            hService->stop();

            TS_ASSERT(!hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(!hService->wasCleanupCalled());
            }

        void testStopWithoutStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->stop();

            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testStopWhenWrappedServiceAlreadyStopped()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->getWrappedService()->stop();
            // reset mock state
            hService->resetTestState();

            hService->stop();

            TS_ASSERT(!hService->getWrappedService()->wasStopCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testShutdown()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->shutdown();

            TS_ASSERT(hService->getWrappedService()->wasShutdownCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testShutdownAlreadyStoppedService()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->stop();
            // reset mock state
            hService->resetTestState();

            hService->shutdown();
            TS_ASSERT(!hService->getWrappedService()->wasShutdownCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(!hService->wasCleanupCalled());
            }

        void testShutdownWithoutStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->shutdown();
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testShutdownWhenWrappedServiceAlreadyStopped()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();
            hService->getWrappedService()->stop();
            // reset mock state
            hService->resetTestState();

            hService->shutdown();
            TS_ASSERT(!hService->getWrappedService()->wasShutdownCalled());
            TS_ASSERT(SafeService::stopped == hService->getSafeServiceState());
            TS_ASSERT(NULL == hService->getService());
            TS_ASSERT(hService->wasCleanupCalled());
            }

        void testAddMemberListener()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(!hWrappedService->wasMemberListenerAdded());
            hService->addMemberListener(TestListener::create());
            TS_ASSERT(hWrappedService->wasMemberListenerAdded());

            hWrappedService->resetTestState();
            hService->addMemberListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasMemberListenerAdded());
            }

        void testAddMemberListenerWrappedServiceNotStarted()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);
            hService->addMemberListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasMemberListenerAdded());
            }

        void testAddMemberListenerPriorToStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->addMemberListener(TestListener::create());
            hService->start();
            TS_ASSERT(hService->getWrappedService()->wasMemberListenerAdded());
            hService->getWrappedService()->resetTestState();
            hService->addMemberListener(TestListener::create());
            // should not add since listener was added during start
            TS_ASSERT(!hService->getWrappedService()->wasMemberListenerAdded());
            }

        void testAddServiceListener()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            TS_ASSERT(!hWrappedService->wasServiceListenerAdded());
            hService->addServiceListener(TestListener::create());
            TS_ASSERT(hWrappedService->wasServiceListenerAdded());

            hWrappedService->resetTestState();
            hService->addServiceListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasServiceListenerAdded());
            }

        void testAddServiceListenerWrappedServiceNotStarted()
            {
            TestSafeCacheService::Handle hService = newTestService();
            TestRemoteCacheService::Handle hWrappedService =
                    TestRemoteCacheService::createRemoteTestService();
            hService->setService(hWrappedService);
            hService->addServiceListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasServiceListenerAdded());
            }

        void testAddServiceListenerPriorToStart()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->addServiceListener(TestListener::create());
            hService->start();
            TS_ASSERT(hService->getWrappedService()->wasServiceListenerAdded());
            hService->getWrappedService()->resetTestState();
            hService->addServiceListener(TestListener::create());
            // should not add since listener was added during start
            TS_ASSERT(!hService->getWrappedService()->wasServiceListenerAdded());
            }

        void testRemoveMemberListener()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestListener::Handle hListener1 = TestListener::create();
            TestListener::Handle hListener2 = TestListener::create();
            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();

            hService->removeMemberListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasMemberListenerRemoved());

            hService->addMemberListener(hListener1);
            hService->addMemberListener(hListener2);

            hService->removeMemberListener(hListener1);
            // remove should only be called on wrapped service when listeners
            // is empty
            TS_ASSERT(!hWrappedService->wasMemberListenerRemoved());
            // remove an entry that was not added
            hService->removeMemberListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasMemberListenerRemoved());

            hService->removeMemberListener(hListener2);
            TS_ASSERT(hWrappedService->wasMemberListenerRemoved());
            hWrappedService->resetTestState();
            hService->removeMemberListener(hListener2);
            TS_ASSERT(!hWrappedService->wasMemberListenerRemoved());
            }

        void testRemoveServiceListener()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            TestRemoteCacheService::Handle hWrappedService =
                    hService->getWrappedService();
            TestListener::Handle hListener1 = TestListener::create();
            TestListener::Handle hListener2 = TestListener::create();

            hService->removeServiceListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasServiceListenerRemoved());

            hService->addServiceListener(hListener1);
            hService->addServiceListener(hListener2);

            hService->removeServiceListener(hListener1);
            // remove should only be called on wrapped service when listeners
            // is empty
            TS_ASSERT(!hWrappedService->wasServiceListenerRemoved());
            // remove an entry that was not added
            hService->removeServiceListener(TestListener::create());
            TS_ASSERT(!hWrappedService->wasServiceListenerRemoved());

            hService->removeServiceListener(hListener2);
            TS_ASSERT(hWrappedService->wasServiceListenerRemoved());
            hWrappedService->resetTestState();
            hService->removeServiceListener(hListener2);
            TS_ASSERT(!hWrappedService->wasServiceListenerRemoved());
            }

        void testMemberJoinedCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_joined, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberJoined(hEvent);
            TS_ASSERT(hListener->wasMemberJoinedCalled(hService, hMember));
            }

        void testMemberJoinedCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_joined, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberJoined(hEvent);
            TS_ASSERT(hListener->wasMemberJoinedCalled(hService, hMember));
            }

        void testMemberJoinedCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    MemberEvent::member_joined, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberJoined(hEvent);
            TS_ASSERT(!hListener->wasMemberJoinedCalled(hService, hMember));
            }

        void testMemberLeavingCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_leaving, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeaving(hEvent);
            TS_ASSERT(hListener->wasMemberLeavingCalled(hService, hMember));
            }

        void testMemberLeavingCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_leaving, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeaving(hEvent);
            TS_ASSERT(hListener->wasMemberLeavingCalled(hService, hMember));
            }

        void testMemberLeavingCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    MemberEvent::member_leaving, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeaving(hEvent);
            TS_ASSERT(!hListener->wasMemberLeavingCalled(hService, hMember));
            }

        void testMemberLeftCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_left, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeft(hEvent);
            TS_ASSERT(hListener->wasMemberLeftCalled(hService, hMember));
            }

        void testMemberLeftCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(hService->getService(),
                    MemberEvent::member_left, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeft(hEvent);
            TS_ASSERT(hListener->wasMemberLeftCalled(hService, hMember));
            }

        void testMemberLeftCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            Member::Handle hMember = LocalMember::create();
            MemberListener::Handle hCallback = hService->getMemberCallback();
            MemberEvent::Handle hEvent = MemberEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    MemberEvent::member_left, hMember);

            TestListener::Handle hListener = TestListener::create();
            hService->addMemberListener(hListener);

            hCallback->memberLeft(hEvent);
            TS_ASSERT(!hListener->wasMemberLeftCalled(hService, hMember));
            }

        void testServiceStartedCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_started);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarted(hEvent);
            TS_ASSERT(hListener->wasServiceStartedCalled(hService));
            }

        void testServiceStartedCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_started);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarted(hEvent);
            TS_ASSERT(hListener->wasServiceStartedCalled(hService));
            }

        void testServiceStartedCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    ServiceEvent::service_started);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarted(hEvent);
            TS_ASSERT(!hListener->wasServiceStartedCalled(hService));
            }

        void testServiceStartingCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_starting);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarting(hEvent);
            TS_ASSERT(hListener->wasServiceStartingCalled(hService));
            }

        void testServiceStartingCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_starting);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarting(hEvent);
            TS_ASSERT(hListener->wasServiceStartingCalled(hService));
            }

        void testServiceStartingCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    ServiceEvent::service_starting);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStarting(hEvent);
            TS_ASSERT(!hListener->wasServiceStartingCalled(hService));
            }

        void testServiceStoppedCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_stopped);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopped(hEvent);
            TS_ASSERT(hListener->wasServiceStoppedCalled(hService));
            }

        void testServiceStoppedCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_stopped);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopped(hEvent);
            TS_ASSERT(hListener->wasServiceStoppedCalled(hService));
            }

        void testServiceStoppedCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    ServiceEvent::service_stopped);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopped(hEvent);
            TS_ASSERT(!hListener->wasServiceStoppedCalled(hService));
            }

        void testServiceStoppingCallback()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_stopping);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopping(hEvent);
            TS_ASSERT(hListener->wasServiceStoppingCalled(hService));
            }

        void testServiceStoppingCallbackNullService()
            {
            TestSafeCacheService::Handle hService = newTestService();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(hService->getService(),
                    ServiceEvent::service_stopping);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopping(hEvent);
            TS_ASSERT(hListener->wasServiceStoppingCalled(hService));
            }

        void testServiceStoppingCallbackDifferentSource()
            {
            TestSafeCacheService::Handle hService = newTestService();
            hService->start();

            ServiceListener::Handle hCallback = hService->getServiceCallback();
            ServiceEvent::Handle hEvent = ServiceEvent::create(
                    TestRemoteCacheService::createRemoteTestService(),
                    ServiceEvent::service_stopping);

            TestListener::Handle hListener = TestListener::create();
            hService->addServiceListener(hListener);

            hCallback->serviceStopping(hEvent);
            TS_ASSERT(!hListener->wasServiceStoppingCalled(hService));
            }


    private:
        TestSafeCacheService::Handle newTestService()
            {
            TestSafeCacheService::Handle hService =
                TestSafeCacheService::createTestService();

            hService->setOperationalContext(
                    DefaultOperationalContext::
                    create(SimpleElement::create("coherence")));
            hService->setServiceType(ServiceInfo::remote_cache);
            hService->setServiceName("Test-Safe-Cache-Service");
            hService->setUserContext(String::create("user-context"));
            hService->setConfig(SimpleElement::create());

            return hService;
            }
    };



