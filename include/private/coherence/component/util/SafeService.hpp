/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_SERVICE_HPP
#define COH_SAFE_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/ServiceEvent.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "private/coherence/component/net/extend/RemoteCacheService.hpp"
#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::RemoteCacheService;
using coherence::component::net::extend::RemoteInvocationService;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::OperationalContext;
using coherence::net::ServiceInfo;
using coherence::run::xml::XmlElement;
using coherence::security::auth::Subject;
using coherence::util::Listeners;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;


/**
* Base class for remote services safe-wrapping.
* <p/>
* These "Safe" wrappers are responsible for ensuring that the wrapped
* component is always usable. For example, if the connection between the
* Coherence Extend client and proxy service is ever severed, the messaging
* framework alerts the "Remote" service (via a connection listener), and the
* "Remote" service transitions itself to a stopped state. During the next
* use of the "Remote" service (via the "Safe" wrapper), the "Safe" wrapper
* detects that the wrapped service has stopped and attempts to restart it.
*
* @author js  2008.02.12
*/
class COH_EXPORT SafeService
    : public class_spec<SafeService,
        extends<Object>,
        implements<coherence::net::Service> >
    {
    friend class factory<SafeService>;

    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * Typedef for coherence::net::Service to avoid confusion
        * with coherence::component::util::Service.
        */
        typedef coherence::net::Service ServiceType;


    // ----- enums ----------------------------------------------------------

    public:
        /**
        * The state of the service.
        */
        enum ServiceState
            {
            initial,  // initial state of the service
            started,  // indicates that start has been called
            stopped   // the service has been stopped or shutdown
            };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a SafeService.
        */
        SafeService();

    private:
        /**
        * Blocked copy constructor.
        */
        SafeService(const SafeService&);


    // ----- net/Service interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ServiceInfo::View getInfo() const;

        /**
        * {@inheritDoc}
        */
        virtual void addMemberListener(MemberListener::Handle hListener);

        /**
        * {@inheritDoc}
        */
        virtual void removeMemberListener(MemberListener::Handle hListener);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getUserContext() const;

        /**
        * {@inheritDoc}
        */
        virtual void setUserContext(Object::Holder ohCtx);

        /**
        * {@inheritDoc}
        */
        virtual Serializer::View getSerializer() const;

        /**
        * {@inheritDoc}
        */
        virtual void setThreadGroup(ThreadGroup::Handle hGroup);

        /**
        * {@inheritDoc}
        */
        virtual ThreadGroup::Handle getThreadGroup();

        /**
        * {@inheritDoc}
        */
        virtual ThreadGroup::View getThreadGroup() const;


    // ----- util/Service interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void addServiceListener(ServiceListener::Handle hListener);

        /**
        * {@inheritDoc}
        */
        virtual void removeServiceListener(ServiceListener::Handle hListener);


    // ----- Controllable interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void configure(XmlElement::View vXml);

        /**
        * {@inheritDoc}
        */
        virtual bool isRunning() const;

        /**
        * {@inheritDoc}
        */
        virtual void start();

        /**
        * {@inheritDoc}
        */
        virtual void shutdown();

        /**
        * {@inheritDoc}
        */
        virtual void stop();


    // ----- SafeService interface ------------------------------------------

    public:
        /**
        * Ensure that the wrapped service is running before returning it.
        * If the service is not running and has not been explicitly stopped,
        * the service is restarted.
        *
        * @return the running wrapped service
        */
        virtual ServiceType::View ensureRunningService() const;

        /**
        * Ensure that the wrapped service is running before returning it.
        * If the service is not running and has not been explicitly stopped,
        * the service is restarted.
        *
        * @return the running wrapped service
        */
        virtual ServiceType::Handle ensureRunningService();

    // ----- Object interface -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();


    // ----- property getters/setters ---------------------------------------

    public:
        /**
        * Return the OperationalContext for this Service.
        *
        * @return the OperationalContext for this Service
        *
        * @since Coherence 3.7
        */
        virtual OperationalContext::View getOperationalContext() const;

        /**
        * Set the OperationalContext for this Service.
        *
        * @param vContext  the OperationalContext for this Service
        *
        * @throws IllegalStateException if an OperationalContext was already
        *         set
        *
        * @since Coherence 3.7
        */
        virtual void setOperationalContext(OperationalContext::View vContext);

        /**
        * Setter for the serviceName property.
        *
        * @param vsServiceName  the name of the service
        */
        virtual void setServiceName(String::View vsServiceName);

        /**
        * Getter for the serviceName property.
        *
        * @return the name of the service
        */
        virtual String::View getServiceName() const;

        /**
        * Setter for the service property.
        *
        * @param hService  the service to wrap
        */
        virtual void setService(ServiceType::Handle hService);

        /**
        * Getter for the service property.
        *
        * @return the wrapped service
        */
        virtual ServiceType::View getService() const;

        /**
        * Getter for the member listeners property.
        *
        * @return the member listeners
        */
        virtual Listeners::View getMemberListeners() const;

        /**
        * Getter for the service listeners property.
        *
        * @return the service listeners
        */
        virtual Listeners::View getServiceListeners() const;

        /**
        * Setter for the config property.
        *
        * @param hXml  the xml configuration
        */
        virtual void setConfig(XmlElement::View vXml);

        /**
        * Getter for the config property.
        *
        * @return the xml configuration
        */
        virtual XmlElement::View getConfig() const;

        /**
        * Setter for the safe service state property.
        *
        * @param state  the new state of the SafeService
        */
        virtual void setSafeServiceState(ServiceState state);

        /**
        * Getter for the safe service state property.
        *
        * @return the state of the safe service wrapper
        */
        virtual ServiceState getSafeServiceState() const;

        /**
        * Setter for the serviceType property.
        *
        * @param serviceType  the type of service being wrapped
        */
        virtual void setServiceType(ServiceInfo::ServiceType serviceType);

        /**
        * Getter for the service type property.
        *
        * @return the type of service being wrapped
        */
        virtual ServiceInfo::ServiceType getServiceType() const;

        /**
        * Ensure that the wrapped service is running before returning it.
        * If the service is not running and has not been explicitly stopped,
        * the service is restarted.
        *
        * @return the running wrapped service
        */
        virtual ServiceType::View getRunningService() const;

        /**
        * Get the optional identity associated with this cache.
        */
        virtual Subject::View getSubject() const;

        /**
        * Set the optional identity associated with this cache.
        *
        * @param vSubject  the identity
        */
        virtual void setSubject(Subject::View vSubject);


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Restart the wrapped service.
        *
        * @return the running wrapped service
        */
        virtual ServiceType::Handle restartService() const;

        /**
        * Start the provided service.
        *
        * @param hService  the service to start
        */
        virtual void startService(ServiceType::Handle hService) const;

        /**
        * Instantiate a new RemoteCacheService.
        *
        * @return a newly constructed RemoteCacheService
        */
        virtual RemoteCacheService::Handle createRemoteCacheService() const;

        /**
        * Instantiate a new RemoteInvocationService.
        *
        * @return a newly constructed RemoteInvocationService
        */
        virtual RemoteInvocationService::Handle
                createRemoteInvocationService() const;

        /**
        * Cleanup resources.
        */
        virtual void cleanup();

        /**
        * Ensure that the wrapped service is running before returning it.
        * If the service is not running and has not been explicitly stopped,
        * the service is restarted.
        *
        * @return the running wrapped service
        */
        virtual ServiceType::Handle ensureRunningServiceInternal() const;

        /**
        * Stop and catch any exceptions.  Provide a log message if stop
        * fails.  Used only when starting a service has failed.
        */
        virtual void stopAfterFailedStart(ServiceType::Handle hService) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The name of the service.
        */
        FinalView<String> f_vsServiceName;

        /**
        * The state of the safe service wrapper.
        */
        Volatile<int32_t> m_safeServiceState;

        /**
        * The service type.  Either ServiceInfo::remote_cache or
        * ServiceInfo::remote_invocation.
        */
        //TODO: Change to Volatile<ServiceInfo::ServiceType>
        Volatile<int32_t> m_serviceType;

        /**
        * The wrapped service.
        */
        mutable MemberHandle<ServiceType> m_hService;

        /**
        * The Member listeners.
        */
        FinalHandle<Listeners> f_hMemberListeners;

        /**
        * The service listeners.
        */
        FinalHandle<Listeners> f_hServiceListeners;

        /**
        * The xml configuration.
        */
        mutable FinalView<XmlElement> f_vConfig;

        /**
        * User context object associated with this service.
        * <p/>
        * The data type and semantics of this context object are entirely
        * application specific and are opaque to the service.
        */
        MemberHolder<Object> m_ohUserContext;

        /**
        * Used to handle all MemberListener callbacks.
        */
        mutable FinalHandle<MemberListener> f_hMemberCallback;

        /**
        * Used to handle all ServiceListener callbacks.
        */
        mutable FinalHandle<ServiceListener> f_hServiceCallback;

        /**
        * The Service's ThreadGroup.
        */
        mutable FinalHandle<ThreadGroup> f_hThreadGroup;

        /**
        * The Service's OperationalContext.
        */
        FinalView<OperationalContext> f_vContext;

        /**
        * The identity associated with this cache.
        */
        FinalView<Subject> f_vSubject;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SAFE_SERVICE_HPP
