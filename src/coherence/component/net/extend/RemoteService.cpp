/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/RemoteService.hpp"

#include "coherence/net/InetSocketAddress.hpp"
#include "coherence/net/NameService.hpp"
#include "coherence/net/SingleAddressProvider.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "private/coherence/component/net/extend/RemoteNameService.hpp"

#include "private/coherence/component/net/extend/util/TcpUtil.hpp"

#include "private/coherence/component/util/Initiator.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/TcpInitiator.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::util::TcpUtil;
using coherence::component::util::Initiator;
using coherence::component::util::Peer;
using coherence::component::util::TcpInitiator;
using coherence::net::InetSocketAddress;
using coherence::net::NameService;
using coherence::net::SingleAddressProvider;
using coherence::run::xml::XmlHelper;
using coherence::net::messaging::ConnectionException;


// ----- constructors -------------------------------------------------------

RemoteService::RemoteService()
    : m_hChannel(self()),
      f_hInitiator(self()),
      f_hListenersMember(self(),Listeners::create()),
      f_hListenersService(self(), Listeners::create()),
      f_vsServiceName(self()),
      m_vsRemoteClusterName(self()),
      m_vsRemoteServiceName(self()),
      f_vXml(self()),
      m_ohUserContext(self()),
      f_hThreadGroup(self()),
      f_vContext(self())
    {
    }


// ----- RemoteService interface --------------------------------------------

bool RemoteService::isServiceThread(bool /*fStrict*/) const
    {
    Initiator::View vInitiator = cast<Initiator::View>(getInitiator(), false);
    if (NULL != vInitiator)
        {
        return vInitiator->isServiceThread();
        }
    return false;
    }

// ----- Service interface --------------------------------------------------

ServiceInfo::View RemoteService::getInfo() const
    {
    return this;
    }

void RemoteService::addServiceListener(ServiceListener::Handle hListener)
    {
    f_hListenersService->add(hListener);
    }

void RemoteService::removeServiceListener(ServiceListener::Handle hListener)
    {
    f_hListenersService->remove(hListener);
    }

void RemoteService::addMemberListener(MemberListener::Handle hListener)
    {
    f_hListenersMember->add(hListener);
    }

void RemoteService::removeMemberListener(MemberListener::Handle hListener)
    {
    f_hListenersMember->remove(hListener);
    }

Object::Holder RemoteService::getUserContext() const
    {
    return m_ohUserContext;
    }

void RemoteService::setUserContext(Object::Holder ohCtx)
    {
    m_ohUserContext = ohCtx;
    }

Serializer::View RemoteService::getSerializer() const
    {
    return m_hChannel->getSerializer();
    }

void RemoteService::setThreadGroup(ThreadGroup::Handle hGroup)
    {
    ConnectionInitiator::Handle hInitiator = getInitiator();
    if (instanceof<Peer::Handle>(hInitiator))
        {
        cast<Peer::Handle>(hInitiator)->setThreadGroup(hGroup);
        }
    initialize(f_hThreadGroup, hGroup);
    }

ThreadGroup::Handle RemoteService::getThreadGroup()
    {
    return f_hThreadGroup;
    }

ThreadGroup::View RemoteService::getThreadGroup() const
    {
    return f_hThreadGroup;
    }


// ----- Controllable interface ---------------------------------------------

void RemoteService::configure(XmlElement::View vXml)
    {
    COH_SYNCHRONIZED (this)
        {
        COH_ENSURE(!isRunning());

        doConfigure(vXml);
        }
    }

bool RemoteService::isRunning() const
    {
    ConnectionInitiator::View vInitiator = getInitiator();
    return NULL == vInitiator ? false : vInitiator->isRunning();
    }

void RemoteService::start()
    {
    COH_SYNCHRONIZED (this)
        {
        if (!isRunning())
            {
            try
                {
                doStart();
                }
            catch (...)
                {
                doStop();
                throw;
                }
            }
        }
    }

void RemoteService::shutdown()
    {
    COH_SYNCHRONIZED (this)
        {
        if (isRunning())
            {
            doShutdown();
            }
        }
    }

void RemoteService::stop()
    {
    COH_SYNCHRONIZED (this)
        {
        if (isRunning())
            {
            doStop();
            }
        }
    }


// ----- ServiceInfo interface ----------------------------------------------

String::View RemoteService::getServiceName() const
    {
    return f_vsServiceName;
    }


// ----- ServiceListener interface ------------------------------------------

void RemoteService::serviceStarting(ServiceEvent::View vEvt)
    {
    dispatchServiceEvent(vEvt->getId());
    }

void RemoteService::serviceStarted(ServiceEvent::View vEvt)
    {
    dispatchServiceEvent(vEvt->getId());
    }

void RemoteService::serviceStopping(ServiceEvent::View vEvt)
    {
    dispatchServiceEvent(vEvt->getId());
    }

void RemoteService::serviceStopped(ServiceEvent::View vEvt)
    {
    dispatchServiceEvent(vEvt->getId());
    m_hChannel = NULL;
    }


// ----- ConnectionListener interface ---------------------------------------

void RemoteService::connectionOpened(ConnectionEvent::View /*vEvt*/)
    {
    dispatchMemberEvent(MemberEvent::member_joined);
    }

void RemoteService::connectionClosed(ConnectionEvent::View /*vEvt*/)
    {
    m_hChannel = NULL;

    dispatchMemberEvent(MemberEvent::member_leaving);
    dispatchMemberEvent(MemberEvent::member_left);
    }

void RemoteService::connectionError(ConnectionEvent::View /*vEvt*/)
    {
    m_hChannel = NULL;

    dispatchMemberEvent(MemberEvent::member_leaving);
    dispatchMemberEvent(MemberEvent::member_left);
    }


// ----- Describable interface ----------------------------------------------

String::View RemoteService::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "Name=" << getServiceName());
    }


// ----- internal methods ---------------------------------------------------

void RemoteService::dispatchMemberEvent(MemberEvent::Id nId)
    {
    Listeners::Handle hListeners = f_hListenersMember;
    if (!hListeners->isEmpty())
        {
        MemberEvent::create(this, nId, getOperationalContext()->getLocalMember())->dispatch(hListeners);
        }
    }

void RemoteService::dispatchServiceEvent(ServiceEvent::Id nId)
    {
    Listeners::Handle hListeners = f_hListenersService;
    if (!hListeners->isEmpty())
        {
        ServiceEvent::create(this, nId)->dispatch(hListeners);
        }
    }

void RemoteService::doConfigure(XmlElement::View vXml)
    {
    if (vXml == NULL)
        {
        COH_THROW (IllegalArgumentException::create("xml configuration must not be null"));
        }

    setXml(vXml);

    // find the configuration for the Initiator
    XmlElement::Handle hXmlInitiator = Initiator::findInitiatorConfig(vXml);

    // inject service configuration
    XmlElement::Handle hXmlHandler = XmlHelper::ensureElement(hXmlInitiator,
            "incoming-message-handler");

    XmlElement::Handle hXmlSub = XmlHelper::ensureElement(hXmlHandler, "thread-count");
    if (NULL == hXmlSub->getValue())
        {
        hXmlSub->setInt32(vXml->getSafeElement("thread-count")->getInt32());
        }
    hXmlSub = XmlHelper::ensureElement(hXmlHandler, "task-hung-threshold");
    if (NULL == hXmlSub->getValue())
        {
        hXmlSub->setString(vXml->getSafeElement("task-hung-threshold")->getString());
        }
    hXmlSub = XmlHelper::ensureElement(hXmlHandler, "task-timeout");
    if (NULL == hXmlSub->getValue())
        {
        hXmlSub->setString(vXml->getSafeElement("task-timeout")->getString());
        }
    hXmlSub = XmlHelper::ensureElement(hXmlHandler, "request-timeout");
    if (NULL == hXmlSub->getValue())
        {
        hXmlSub->setString(vXml->getSafeElement("request-timeout")->getString());
        }

    // create the Initiator
    ConnectionInitiator::Handle hConInitiator =
            Initiator::createInitiator(hXmlInitiator, getOperationalContext());
    if (instanceof<Initiator::Handle>(hConInitiator))
        {
        Initiator::Handle hInitiator = cast<Initiator::Handle>(hConInitiator);
        hInitiator->setServiceName(COH_TO_STRING(getServiceName() << ':'
                << hInitiator->getServiceName()));
        hInitiator->setThreadGroup(getThreadGroup());
        hInitiator->setParentService(this);
        }

    setInitiator(hConInitiator);

    setRemoteClusterName(vXml->getSafeElement("cluster-name")->getString());

    setRemoteServiceName(vXml->getSafeElement("proxy-service-name")->getString());

    if (instanceof<TcpInitiator::Handle>(hConInitiator))
        {
        setNameServiceAddressProvider(
                (cast<TcpInitiator::Handle>(hConInitiator))
                ->isNameServiceAddressProvider());
        }
    }

void RemoteService::doShutdown()
    {
    ConnectionInitiator::Handle hInitiator = getInitiator();
    COH_ENSURE(NULL != hInitiator);

    hInitiator->shutdown();
    }

void RemoteService::doStart()
    {
    ConnectionInitiator::Handle hInitiator = getInitiator();
    COH_ENSURE(NULL != hInitiator);

    hInitiator->addConnectionListener(this);
    hInitiator->addServiceListener(this);
    hInitiator->start();

    m_hChannel = openChannel();
    }

void RemoteService::doStop()
    {
    ConnectionInitiator::Handle hInitiator = getInitiator();
    COH_ENSURE(NULL != hInitiator);

    hInitiator->stop();

    // required for C++ to break the memory reference cycle
    hInitiator->removeServiceListener(this);
    hInitiator->removeConnectionListener(this);
    }

Channel::Handle RemoteService::ensureChannel()
    {
    Channel::Handle hChannel = m_hChannel;
    if (NULL == hChannel || !hChannel->isOpen())
        {
        hChannel = m_hChannel = openChannel();
        }
    return hChannel;
    }

QueueProcessor::Handle RemoteService::ensureEventDispatcher()
    {
    PofChannel::Handle hChannel = cast<PofChannel::Handle>(ensureChannel());
    return hChannel->getConnectionManager()->ensureEventDispatcher();
    }

void RemoteService::lookupProxyServiceAddress()
    {
    if (isNameServiceAddressProvider())
        {
        ConnectionInitiator::Handle hInitiator = getInitiator();
        if (instanceof<TcpInitiator::Handle>(hInitiator))
            {
            // attempt to lookup the ProxyService address from a NameService
            TcpInitiator::Handle      hTcpInitiator = cast<TcpInitiator::Handle> (hInitiator);
            RemoteNameService::Handle hService      = RemoteNameService::create();
            String::View              vsName        = getRemoteServiceName();

            hService->setOperationalContext(getOperationalContext());
            hService->setServiceName(COH_TO_STRING(getServiceName()
                    << ":RemoteNameService"));
            hService->configure(getXml());

            String::View vsClusterRemote = getRemoteClusterName();

            hService->setRemoteClusterName(vsClusterRemote);
            hService->setRemoteServiceName("NameService");

            hService->start();

            Exception::View e = NULL;
            try
                {
                ObjectArray::View vao = cast<ObjectArray::View>(hService->lookup(vsName));
                if (vao == NULL)
                    {
                    // we got an answer, which means we found the cluster, but not the service
                    e = ConnectionException::create(COH_TO_STRING("Unable to locate ProxyService '" << vsName
                        << "' within cluster '" << vsClusterRemote << "'"));
                    }
                else
                    {
                    String::View vs  = cast<String::View>(vao[0]);
                    int32_t      n   = cast<Integer32::View>(vao[1])->getInt32Value();
                    hTcpInitiator->setRemoteAddressProvider(SingleAddressProvider::create(
                            InetSocketAddress::create(vs, TcpUtil::calculateBaseport(n), true)));
                    hTcpInitiator->setSubport(TcpUtil::calculateSubport(n));
                    }
                }
            catch (Exception::View ex)
                {
                // we failed to connect, thus the cluster was not reachable
                e = ConnectionException::create(COH_TO_STRING("Unable to locate cluster '" << vsClusterRemote << "' while looking for its ProxyService '"
                        << vsName << "'"), ex, (Connection::View) NULL);
                }
            hService->stop();
            if (e != NULL)
                {
                COH_THROW(e);
                }
            }
        }
    }


// ----- accessors ----------------------------------------------------------

OperationalContext::View RemoteService::getOperationalContext() const
    {
    return f_vContext;
    }

void RemoteService::setOperationalContext(OperationalContext::View vContext)
    {
    if (f_vContext != NULL)
        {
        COH_THROW (IllegalStateException::
                create("OperationalContext has already been set"));
        }
    initialize(f_vContext, vContext);
    }

ConnectionInitiator::Handle RemoteService::getInitiator()
    {
    return f_hInitiator;
    }

ConnectionInitiator::View RemoteService::getInitiator() const
    {
    return f_hInitiator;
    }

void RemoteService::setServiceName(String::View vsName)
    {
    initialize(f_vsServiceName, vsName);
    }

void RemoteService::setInitiator(ConnectionInitiator::Handle hInitiator)
    {
    initialize(f_hInitiator, hInitiator);
    }

String::View RemoteService::getRemoteClusterName() const
    {
    String::View vsName = m_vsRemoteClusterName;
    if (vsName == NULL || vsName->isEmpty())
        {
        // NS lookups and corresponding redirects are always done with a cluster name since multiple
        // clusters may effectivley share the cluster port we don't know what cluster we'd land in.
        // remote-address based lookups on the other hand use the cluster name configured in the remote
        // scheme, which is allowed to be null.  This is because a remote-address based lookup is pointing
        // at an explict unsharable port and it is presumed the configuration is correct.
        if (isNameServiceAddressProvider())
            {
            return getOperationalContext()->getLocalMember()->getClusterName();
            }
        else
            {
            return NULL;
            }
        }
    return vsName;
    }

void RemoteService::setRemoteClusterName(String::View vsName)
    {
    m_vsRemoteClusterName = vsName;
    }

String::View RemoteService::getRemoteServiceName() const
    {
    String::View vsName = m_vsRemoteServiceName;
    if (vsName == NULL || vsName->isEmpty())
        {
        if (isNameServiceAddressProvider())
            {
            return getServiceName();
            }
        else
            {
            return NULL;
            }
        }
    return vsName;
    }

void RemoteService::setRemoteServiceName(String::View vsName)
    {
    m_vsRemoteServiceName = vsName;
    }

bool RemoteService::isNameServiceAddressProvider() const
    {
    return m_fNameServiceAddressProvider;
    }

void RemoteService::setNameServiceAddressProvider(bool fNameServiceAddressProvider)
    {
    m_fNameServiceAddressProvider = fNameServiceAddressProvider;
    }

XmlElement::View RemoteService::getXml() const
    {
    return f_vXml;
    }

void RemoteService::setXml(XmlElement::View vXml)
    {
    initialize(f_vXml, vXml);
    }

COH_CLOSE_NAMESPACE4
