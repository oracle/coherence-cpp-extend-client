/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REMOTE_SERVICE_HPP
#define COH_REMOTE_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/net/MemberEvent.hpp"
#include "coherence/net/MemberListener.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/net/Service.hpp"
#include "coherence/net/ServiceInfo.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "private/coherence/component/util/QueueProcessor.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/ConnectionEvent.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/ConnectionListener.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::util::QueueProcessor;
using coherence::io::Serializer;
using coherence::net::MemberEvent;
using coherence::net::MemberListener;
using coherence::net::OperationalContext;
using coherence::net::Service;
using coherence::net::ServiceInfo;
using coherence::net::messaging::Channel;
using coherence::net::messaging::ConnectionEvent;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::ConnectionListener;
using coherence::run::xml::XmlElement;
using coherence::util::Describable;
using coherence::util::Listeners;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;


/**
* Service implementation that allows a process to use a remote clustered
* Service without having to join the Cluster.
*
* @author jh  2008.02.20
*/
class COH_EXPORT RemoteService
    : public abstract_spec<RemoteService,
        extends<Describable>,
        implements<Service, ServiceInfo, ServiceListener, ConnectionListener> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RemoteService.
        */
        RemoteService();

    private:
        /**
        * Blocked copy constructor.
        */
        RemoteService(const RemoteService&);


    // ----- RemoteService interface ----------------------------------------

    public:
        /**
        * Return true if the current thread is one of the Service threads.
        *
        * @param fStrict  if true then only the service thread and event
        *                 dispatcher thread are considered to be service
        *                 threads, if false, then DaemonPool threads are
        *                 also considered to be service threads.
        *
        * @return true if the current thread is one of the Service threads
        */
        virtual bool isServiceThread(bool fStrict) const;

    // ----- Service interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual ServiceInfo::View getInfo() const;

        /**
        * {@inheritDoc}
        */
        virtual void addServiceListener(ServiceListener::Handle hListener);

        /**
        * {@inheritDoc}
        */
        virtual void removeServiceListener(ServiceListener::Handle hListener);

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


    // ----- ServiceInfo interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getServiceName() const;


    // ----- ServiceListener interface --------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serviceStarting(ServiceEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void serviceStarted(ServiceEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void serviceStopping(ServiceEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void serviceStopped(ServiceEvent::View vEvt);


    // ----- ConnectionListener interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void connectionOpened(ConnectionEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void connectionClosed(ConnectionEvent::View vEvt);

        /**
        * {@inheritDoc}
        */
        virtual void connectionError(ConnectionEvent::View vEvt);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Create and dispatch a new local MemberEvent with the given
        * identifier to the collection of register MemberListener.
        *
        * @param nId  the type of MemberEvent to create and dispatch
        */
        virtual void dispatchMemberEvent(MemberEvent::Id nId);

        /**
        * Create and dispatch a new ServiceEvent with the given identifier
        * to the collection of register ServiceListener.
        *
        * @param nId  the type of ServiceEvent to create and dispatch
        */
        virtual void dispatchServiceEvent(ServiceEvent::Id nId);

        /**
        * The configure() implementation method. This method must only be
        * called by a thread that has synchronized on this RemoteService.
        *
        * @param hXml  the XmlElement containing the new configuration for
        * this RemoteService
        */
        virtual void doConfigure(XmlElement::View vXml);

        /**
        * The shutdown() implementation method. This method must only be
        * called by a thread that has synchronized on this RemoteService.
        */
        virtual void doShutdown();

        /**
        * The start() implementation method. This method must only be called
        * by a thread that has synchronized on this RemoteService.
        */
        virtual void doStart();

        /**
        * The stop() implementation method. This method must only be called
        * by a thread that has synchronized on this RemoteService.
        */
        virtual void doStop();

        /**
        * Return the Channel used by this Service. If the Channel is null or
        * is not open, a new Channel is opened.
        *
        * @return a Channel that can be used to exchange Messages with the
        *         remote ProxyService
        */
        virtual Channel::Handle ensureChannel();

        /**
        * Return a running QueueProcessor used to dispatch events to
        * registered listeners.
        *
        * @return a running QueueProcessor
        */
        virtual QueueProcessor::Handle ensureEventDispatcher();

        /**
        * Open a Channel to the remote ProxyService.
        *
        * @return an open Channel to the ProxyService
        */
        virtual Channel::Handle openChannel() = 0;

        /**
        * Obtains the connect address of the ProxyService from a remote
        * NameService.
        *
        * @since Coherence 12.1.2
        */
        virtual void lookupProxyServiceAddress();


    // ----- accessors ------------------------------------------------------

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
        * Return the ConnectionInitiator used to connect to a ProxyService.
        *
        * @return the ConnectionInitiator
        */
        virtual ConnectionInitiator::Handle getInitiator();

        /**
        * Return the ConnectionInitiator used to connect to a ProxyService.
        *
        * @return the ConnectionInitiator
        */
        virtual ConnectionInitiator::View getInitiator() const;

        /**
        * Configure the name of this Service.
        *
        * @param vsName  the new Service name
        */
        virtual void setServiceName(String::View vsName);

        /**
         * Return the name of the remote Cluster to which this RemoteService will connect.
         *
         * @return the cluster name, or null if unknown
         *
         * @since Coherence 12.2.1
         */
        virtual String::View getRemoteClusterName() const;

        /**
         * Set the cluster name.
         *
         * @param vsName  the name of the remote cluster
         *
         * @since Coherence 12.2.1
         */
        virtual void setRemoteClusterName(String::View vsName);

        /**
        * Return the remote proxy service name.
        *
        * @return the remote proxy service name
        *
        * @since Coherence 12.2.1
        */
        virtual String::View getRemoteServiceName() const;

        /**
        * Set the remote proxy service name.
        *
        * @param vsName  the remote proxy service name
        *
        * @since Coherence 12.2.1
        */
        virtual void setRemoteServiceName(String::View vsName);

        /**
        * Return whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        *
        * @return true if the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService
        *
        * @since Coherence 12.1.2
        */
        virtual bool isNameServiceAddressProvider() const;

        /**
        * Configure whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        *
        * @param fNameServiceAddressProvider  true if the remote AddressProvider
        *                                     address are to be use to look up
        *                                     the remote address of the ProxyService
        *
        * @since Coherence 12.1.2
        */
        virtual void setNameServiceAddressProvider(bool fNameServiceAddressProvider);

        /**
        * Return the XML element used to configure the Initiator.
        *
        * @return the XML element used to configure the Initiator
        *
        * @since Coherence 12.1.2
        */
        virtual XmlElement::View getXml() const;

        /**
        * Configure the XML element used to configure the Initiator.
        *
        * @param vXml  the XML element used to configure the Initiator
        *
        * @since Coherence 12.1.2
        */
        void setXml(XmlElement::View vXml);


    protected:
        /**
        * Configure the ConnectionInitiator used to connect to a
        * ProxyService.
        *
        * @param hInitiator  the new ConnectionInitiator
        */
        virtual void setInitiator(ConnectionInitiator::Handle hInitiator);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Channel used to send and receive Messages to/from the remote
        * ProxyService.
        */
        MemberHandle<Channel> m_hChannel;

        /**
        * The ConnectionInitiator used to connect to a ProxyService.
        */
        FinalHandle<ConnectionInitiator> f_hInitiator;

        /**
        * The collection of registered MemberListener objects.
        */
        FinalHandle<Listeners> f_hListenersMember;

        /**
        * The collection of registered ServiceListener objects.
        */
        FinalHandle<Listeners> f_hListenersService;

        /**
        * The name of this Service.
        */
        FinalView<String> f_vsServiceName;

        /**
         * The remote cluster name.
         */
        MemberView<String> m_vsRemoteClusterName;

        /**
        * The name of the remote ProxyService.
        */
        MemberView<String> m_vsRemoteServiceName;

        /**
        * Whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        */
        bool m_fNameServiceAddressProvider;

        /**
        * The XML element used to configure the Initiator.
        */
        FinalView<XmlElement> f_vXml;

        /**
        * The optional user context.
        */
        MemberHolder<Object> m_ohUserContext;

        /**
        * The Service's ThreadGroup
        */
        FinalHandle<ThreadGroup> f_hThreadGroup;

        /**
        * The OperationalContext for this Service.
        */
        FinalView<OperationalContext> f_vContext;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_REMOTE_SERVICE_HPP
