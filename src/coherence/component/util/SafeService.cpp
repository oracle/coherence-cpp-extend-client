/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/SafeService.hpp"

#include "coherence/net/CacheFactory.hpp"
#include "coherence/util/Service.hpp"

#include "private/coherence/component/net/extend/RemoteCacheService.hpp"
#include "private/coherence/component/net/extend/RemoteService.hpp"
#include "private/coherence/component/util/Service.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::RemoteCacheService;
using coherence::component::net::extend::RemoteService;
using coherence::net::CacheFactory;
using coherence::security::auth::Subject;
using coherence::util::logging::Logger;


// ----- typedefs -----------------------------------------------------------

typedef SafeService::ServiceType ServiceType;


// ----- local class: ListenerCallback --------------------------------------

/**
* This class is registered with the wrapped service as a
* MemberListener and ServiceListener callback.
*/
class ListenerCallback
    : public class_spec<ListenerCallback,
        extends<Object>,
        implements<MemberListener, ServiceListener> >
    {
    friend class factory<ListenerCallback>;

    // ----- constructors -----------------------------------------------

    protected:
        /**
        * Create a new ListenerCallback instance.
        *
        * @param vSafeService  the enclosing SafeService
        */
        ListenerCallback(SafeService::View vSafeService)
            : m_wvSafeService(self(), vSafeService)
            {
            }


    // ----- MemberListener interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void memberJoined(MemberEvent::View vEvt)
            {
            translateMemberEvent(vEvt);
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeaving(MemberEvent::View vEvt)
            {
            translateMemberEvent(vEvt);
            }

        /**
        * {@inheritDoc}
        */
        virtual void memberLeft(MemberEvent::View vEvt)
            {
            translateMemberEvent(vEvt);
            }

    // -----ServiceListener interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serviceStarted(ServiceEvent::View vEvt)
            {
            translateServiceEvent(vEvt);
            }

        /**
        * {@inheritDoc}
        */
        virtual void serviceStarting(ServiceEvent::View vEvt)
            {
            translateServiceEvent(vEvt);
            }

        /**
        * {@inheritDoc}
        */
        virtual void serviceStopped(ServiceEvent::View vEvt)
            {
            translateServiceEvent(vEvt);
            }

        /**
        * {@inheritDoc}
        */
        virtual void serviceStopping(ServiceEvent::View vEvt)
            {
            translateServiceEvent(vEvt);
            }

    // ----- helper methods ---------------------------------------------

    protected:
        /**
        * Handle the provided member event.
        *
        * @param vEvt  the member event to act on
        */
        virtual void translateMemberEvent(MemberEvent::View vEvt)
            {
            SafeService::View vSafeService = m_wvSafeService;
            ServiceType::View vService     = vSafeService->getService();

            if (NULL == vService)
                {
                // for the JOIN events, the Service property may not be set
                // until after the "real" service is started (see synchronized
                // block at the ensureRunningServiceInternal method) just wait
                // till we are out of there
                COH_SYNCHRONIZED (vSafeService)
                    {
                    vService = vSafeService->getService();
                    }
                }

            // allow for post-mortem events
            Listeners::View vListeners = vSafeService->getMemberListeners();
            if (!vListeners->isEmpty())
                {
                ServiceType::View vServiceSource = vEvt->getService();

                MemberEvent::Handle hEvtSafe = MemberEvent::create(
                        vService == vServiceSource
                            ? (ServiceType::View) vSafeService
                            : vServiceSource,
                        vEvt->getId(),
                        vEvt->getMember());
                hEvtSafe->dispatch(vListeners);
                }
            }

        /**
        * Handle the provided service event.
        *
        * @param vEvt  the service event to act on
        */
        virtual void translateServiceEvent(ServiceEvent::View vEvt)
            {
            SafeService::View vSafeService = m_wvSafeService;
            ServiceType::View vService     = vSafeService->getService();

            if (NULL == vService)
                {
                // for the JOIN events, the Service property may not be set
                // until after the "real" service is started (see synchronized
                // block at the ensureRunningServiceInternal method) just wait
                // till we are out of there
                COH_SYNCHRONIZED (vSafeService)
                    {
                    vService = vSafeService->getService();
                    }
                }

            // allow for post-mortem events
            Listeners::View vListeners = vSafeService->getServiceListeners();
            if (!vListeners->isEmpty())
                {
                coherence::util::Service::View vServiceSource = vEvt->getService();

                ServiceEvent::Handle hEvtSafe = ServiceEvent::create(
                        vService == vServiceSource
                            ? (coherence::util::Service::View) vSafeService
                            : vServiceSource,
                        vEvt->getId());
                hEvtSafe->dispatch(vListeners);
                }
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * The enclosing SafeService.
        */
        WeakView<SafeService> m_wvSafeService;
    };



// ----- constructors/destructor  -------------------------------------------

SafeService::SafeService()
    : f_vsServiceName(self()),
      m_safeServiceState(self(), initial),
      m_serviceType(self(), ServiceInfo::local_cache),
      m_hService(self(), ServiceType::Handle(NULL), true),
      f_hMemberListeners(self(), Listeners::create()),
      f_hServiceListeners(self(), Listeners::create()),
      f_vConfig(self(), NULL, /*fMutable*/ true),
      m_ohUserContext(self(), Object::Holder(NULL)),
      f_hMemberCallback(self(), (MemberListener::Handle) NULL, true),
      f_hServiceCallback(self(), (ServiceListener::Handle) NULL, true),
      f_hThreadGroup(self(), NULL, /*fMuttable*/ true),
      f_vContext(self()),
      f_vSubject(self())
    {
    }


// ----- net/Service interface ----------------------------------------------

ServiceInfo::View SafeService::getInfo() const
    {
    return ensureRunningServiceInternal(true)->getInfo();
    }

void SafeService::addMemberListener(MemberListener::Handle hListener)
    {
    COH_SYNCHRONIZED(this)
        {
        bool fWasEmpty = f_hMemberListeners->isEmpty();

        f_hMemberListeners->add(hListener);

        if (fWasEmpty && !f_hMemberListeners->isEmpty())
            {
            ServiceType::Handle hService = m_hService;
            if (NULL != hService && hService->isRunning())
                {
                hService->addMemberListener(f_hMemberCallback);
                }
            }
        }
    }

void SafeService::removeMemberListener(MemberListener::Handle hListener)
    {
    COH_SYNCHRONIZED(this)
        {
        if (!f_hMemberListeners->isEmpty())
            {
            f_hMemberListeners->remove(hListener);
            if (f_hMemberListeners->isEmpty())
                {
                ServiceType::Handle hService = m_hService;
                if (NULL != hService && hService->isRunning())
                    {
                    hService->removeMemberListener(f_hMemberCallback);
                    }
                }
            }
        }
    }

Object::Holder SafeService::getUserContext() const
    {
    return m_ohUserContext;
    }

void SafeService::setUserContext(Object::Holder ohCtx)
    {
    m_ohUserContext = ohCtx;
    ServiceType::Handle hService = m_hService;

    if (NULL != hService)
        {
        hService->setUserContext(ohCtx);
        }
    }

Serializer::View SafeService::getSerializer() const
    {
    return getRunningService()->getSerializer();
    }


// ----- util/Service interface ---------------------------------------------

void SafeService::addServiceListener(ServiceListener::Handle hListener)
    {
    COH_SYNCHRONIZED(this)
        {
        bool fWasEmpty = f_hServiceListeners->isEmpty();

        f_hServiceListeners->add(hListener);

        if (fWasEmpty && !f_hServiceListeners->isEmpty())
            {
            ServiceType::Handle hService = m_hService;
            if (NULL != hService && hService->isRunning())
                {
                hService->addServiceListener(f_hServiceCallback);
                }
            }
        }
    }

void SafeService::removeServiceListener(ServiceListener::Handle hListener)
    {
    COH_SYNCHRONIZED(this)
        {
        if (!f_hServiceListeners->isEmpty())
            {
            f_hServiceListeners->remove(hListener);
            if (f_hServiceListeners->isEmpty())
                {
                ServiceType::Handle hService = m_hService;
                if (NULL != hService && hService->isRunning())
                    {
                    hService->removeServiceListener(f_hServiceCallback);
                    }
                }
            }
        }
    }


// ----- Controllable interface ---------------------------------------------

void SafeService::configure(XmlElement::View vXml)
    {
    setConfig(vXml);
    }

bool SafeService::isRunning() const
    {
    ServiceType::View vService = getService();
    return NULL != vService && vService->isRunning();
    }

void SafeService::start()
    {
    COH_SYNCHRONIZED(this)
        {
        if (getSafeServiceState() == stopped)
            {
            m_safeServiceState = initial;
            }

        try
            {
            ensureRunningServiceInternal(false);
            }
        catch (...)
            {
            // Started indicates that "start" was called
            m_safeServiceState = started;
            throw;
            }

        m_safeServiceState = started;
        }
    drainEvents();
    }

void SafeService::shutdown()
    {
    COH_SYNCHRONIZED(this)
        {
        if (getSafeServiceState() != stopped)
            {
            ServiceType::Handle hService = m_hService;
            if (NULL != hService)
                {
                COH_SYNCHRONIZED(hService)
                    {
                    if (hService->isRunning())
                        {
                        hService->shutdown();
                        }
                    }
                }
            cleanup();
            setSafeServiceState(stopped);
            }
        }
    }

void SafeService::stop()
    {
    COH_SYNCHRONIZED(this)
        {
        if (getSafeServiceState() != stopped)
            {
            ServiceType::Handle hService = m_hService;
            if (NULL != hService)
                {
                COH_SYNCHRONIZED(hService)
                    {
                    if (hService->isRunning())
                        {
                        hService->stop();
                        }
                    }
                }
            cleanup();
            setSafeServiceState(stopped);
            }
        }
    }


// ----- SafeService interface ----------------------------------------------

ServiceType::View SafeService::ensureRunningService(bool fDrain) const
    {
    return ensureRunningServiceInternal(fDrain);
    }

ServiceType::Handle SafeService::ensureRunningService(bool fDrain)
    {
    return ensureRunningServiceInternal(fDrain);
    }

void SafeService::drainEvents() const
    {
    ServiceType::View     vService       = getService();
    RemoteService::Handle hRemoteService =
            cast<RemoteService::Handle>(m_hService);

    if (NULL != hRemoteService)
        {
        hRemoteService->drainEvents();
        }
    else
        {
        coherence::component::util::Service::Handle hService =
            cast<coherence::component::util::Service::Handle>(m_hService);

        if (NULL != hService)
            {
            hService->drainEvents();
            }
        }
    }

void SafeService::setThreadGroup(ThreadGroup::Handle hGroup)
    {
    Service::Handle hService = m_hService;
    if (hService != NULL)
        {
        hService->setThreadGroup(hGroup);
        }
    initialize(f_hThreadGroup, hGroup);
    }

ThreadGroup::Handle SafeService::getThreadGroup()
    {
    return f_hThreadGroup;
    }

ThreadGroup::View SafeService::getThreadGroup() const
    {
    return f_hThreadGroup;
    }


// ----- Object inerface ----------------------------------------------------

void SafeService::onInit()
    {
    super::onInit();

    ListenerCallback::Handle hCallback = ListenerCallback::create(this);

    initialize(f_hMemberCallback, hCallback);
    initialize(f_hServiceCallback, hCallback);
    }


// ----- property getters/setters -------------------------------------------

OperationalContext::View SafeService::getOperationalContext() const
    {
    return f_vContext;
    }

void SafeService::setOperationalContext(OperationalContext::View vContext)
    {
    if (f_vContext != NULL)
        {
        COH_THROW (IllegalStateException::
                create("OperationalContext has already been set"));
        }
    initialize(f_vContext, vContext);
    }

void SafeService::setServiceName(String::View vsServiceName)
    {
    initialize(f_vsServiceName, vsServiceName);
    }

String::View SafeService::getServiceName() const
    {
    return f_vsServiceName;
    }

void SafeService::setService(ServiceType::Handle hService)
    {
    m_hService = hService;
    }

ServiceType::View SafeService::getService() const
    {
    return cast<ServiceType::View>(m_hService);
    }

Listeners::View SafeService::getMemberListeners() const
    {
    return cast<Listeners::View>(f_hMemberListeners);
    }

Listeners::View SafeService::getServiceListeners() const
    {
    return cast<Listeners::View>(f_hServiceListeners);
    }

void SafeService::setConfig(XmlElement::View vXml)
    {
    initialize(f_vConfig, vXml);
    }

XmlElement::View SafeService::getConfig() const
    {
    return f_vConfig;
    }

void SafeService::setSafeServiceState(ServiceState state)
    {
    m_safeServiceState = state;
    }

SafeService::ServiceState SafeService::getSafeServiceState() const
    {
    return static_cast<SafeService::ServiceState>((int32_t) m_safeServiceState);
    }

void SafeService::setServiceType(ServiceInfo::ServiceType serviceType)
    {
    m_serviceType = serviceType;
    }

ServiceInfo::ServiceType SafeService::getServiceType() const
    {
    return static_cast<ServiceInfo::ServiceType>((int32_t) m_serviceType);
    }

ServiceType::View SafeService::getRunningService() const
    {
    return ensureRunningServiceInternal(true);
    }

Subject::View SafeService::getSubject() const
    {
    return f_vSubject;
    }

void SafeService::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }


// ----- helper methods -----------------------------------------------------

ServiceType::Handle SafeService::restartService() const
    {
    ServiceType::Handle hService;
    switch (getServiceType())
        {
        case ServiceInfo::remote_cache:
            {
            // In case the underlying cache is scoped by Subject, use the
            // original Subject
            COH_RUN_AS (getSubject())
                {
                RemoteCacheService::Handle hRemoteCacheService =
                        createRemoteCacheService();
                hRemoteCacheService->setOperationalContext(getOperationalContext());
                hRemoteCacheService->setServiceName(getServiceName());
                hRemoteCacheService->setThreadGroup(f_hThreadGroup);
                hService = hRemoteCacheService;
                }
            }
            break;

        case ServiceInfo::remote_invocation:
            {
            // In case the underlying cache is scoped by Subject, use the
            // original Subject
            COH_RUN_AS (getSubject())
                {
                RemoteInvocationService::Handle hRemoteInvocationService =
                        createRemoteInvocationService();
                hRemoteInvocationService->setOperationalContext(getOperationalContext());
                hRemoteInvocationService->setServiceName(getServiceName());
                hRemoteInvocationService->setThreadGroup(f_hThreadGroup);
                hService = hRemoteInvocationService;
                }
            }
            break;

        default:
            COH_THROW_STREAM(UnsupportedOperationException,
                        "Invalid Service Type: " << getServiceType());
        }

    startService(hService);
    return hService;
    }

void SafeService::startService(ServiceType::Handle hService) const
    {
    hService->setUserContext(getUserContext());
    hService->configure(f_vConfig);

    if (!getMemberListeners()->isEmpty())
        {
        hService->addMemberListener(f_hMemberCallback);
        }

    if (!getServiceListeners()->isEmpty())
        {
        hService->addServiceListener(f_hServiceCallback);
        }

    try
        {
        hService->start();
        }
    catch (Exception::View e)
        {
        COH_LOG("Unable to start service "
                << getServiceName() << ": " << e, Logger::level_error);
        stopAfterFailedStart(hService);
        COH_THROW (e);
        }
    }

RemoteCacheService::Handle SafeService::createRemoteCacheService() const
    {
    return RemoteCacheService::create();
    }

RemoteInvocationService::Handle
        SafeService::createRemoteInvocationService() const
    {
    return RemoteInvocationService::create();
    }

void SafeService::cleanup()
    {
    setService(NULL);
    f_hMemberListeners->removeAll();
    f_hServiceListeners->removeAll();
    }

ServiceType::Handle SafeService::ensureRunningServiceInternal(
        bool fDrain) const
    {
    ServiceType::Handle hService = m_hService;
    if (NULL == hService || !hService->isRunning())
        {
        COH_SYNCHRONIZED(this)
            {
            hService = m_hService;
            switch(getSafeServiceState())
                {
                case initial:
                    if (NULL == hService)
                        {
                        m_hService = hService = restartService();
                        }
                    else
                        {
                        startService(hService);
                        }
                    break;
                case started:
                    if (NULL == hService || !hService->isRunning())
                        {
                        m_hService = NULL;
                        COH_LOG("Restarting Service: "
                                << getServiceName(), Logger::level_info);
                        m_hService = hService = restartService();
                        }
                    break;
                case stopped:
                    COH_THROW (IllegalStateException::create(
                            "SafeService was explicitly stopped."));
                }
            }

        if (fDrain)
            {
            drainEvents();
            }
        }
    return hService;
    }

void SafeService::stopAfterFailedStart(ServiceType::Handle hService) const
    {
    try
        {
        hService->stop();
        }
    catch (...)
        {
        // eat the exception
        COH_LOG("Unable to stop service "
                << getServiceName() << " after failing to start",
                    Logger::level_warning);
        }
    }

COH_CLOSE_NAMESPACE3
