/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/Initiator.hpp"

#include "coherence/net/RequestTimeoutException.hpp"
#include "coherence/security/auth/Subject.hpp"

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/component/net/extend/protocol/InitiatorProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnection.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/TcpInitiator.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Request.hpp"
#include "private/coherence/security/SecurityHelper.hpp"
#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::PofConnection;
using coherence::component::net::extend::protocol::InitiatorProtocol;
using coherence::component::net::extend::protocol::OpenConnection;
using coherence::net::RequestTimeoutException;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::security::SecurityHelper;
using coherence::security::auth::Subject;


// ----- constructor --------------------------------------------------------

Initiator::Initiator()
    : m_hConnection(self()), m_ldtConnectTimeout(0)
    {
    }


// ----- factory method -----------------------------------------------------

ConnectionInitiator::Handle Initiator::createInitiator(
        XmlElement::View vXml,
        OperationalContext::View vContext)
    {
    ConnectionInitiator::Handle hInitiator = cast<ConnectionInitiator::Handle>(TcpInitiator::create());

    hInitiator->setOperationalContext(vContext);
    hInitiator->configure(vXml);
    return hInitiator;
    }

XmlElement::Handle Initiator::findInitiatorConfig(XmlElement::View vXml)
    {
    XmlElement::View vXmlConfig;
    if (vXml->getName()->equals("initiator-config"))
        {
        vXmlConfig = vXml;
        }
    else
        {
        vXmlConfig = vXml->getSafeElement("initiator-config");
        }

    return cast<XmlElement::Handle>(vXmlConfig->clone());
    }


// ----- Initiator interface ------------------------------------------------

PofConnection::Handle Initiator::openConnection()
    {
    PofConnection::Handle hConnection = instantiateConnection();
    hConnection->open();

    return hConnection;
    }


// ----- Peer interface -----------------------------------------------------

void Initiator::checkPingTimeouts()
    {
    PofConnection::Handle hConnection = getConnection();
    if (NULL != hConnection)
        {
        checkPingTimeout(hConnection);
        }
    }

void Initiator::onConnectionClosed(PofConnection::Handle hConnection)
    {
    if (getConnection() == hConnection)
        {
        setConnection(NULL);
        super::onConnectionClosed(hConnection);
        }
    }

void Initiator::onConnectionError(PofConnection::Handle hConnection,
        Exception::Holder ohe)
    {
    if (getConnection() == hConnection)
        {
        setConnection(NULL);
        super::onConnectionError(hConnection, ohe);
        }
    }

void Initiator::onConnectionOpened(PofConnection::Handle hConnection)
    {
    if (get_Connection() == hConnection)
        {
        return;
        }

    if (NULL == getConnection())
        {
        setConnection(hConnection);
        super::onConnectionOpened(hConnection);
        }
    }

void Initiator::openConnection(PofConnection::Handle hConnection)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    OpenConnection::Handle hRequest = cast<OpenConnection::Handle>(
            vFactory0->createMessage(OpenConnection::type_id));

    Subject::View vSubject = SecurityHelper::getCurrentSubject();

    hRequest->setConnectionOpen(hConnection);
    hRequest->setIdentityToken(serializeIdentityToken(generateIdentityToken(vSubject)));
    hRequest->setSubject(vSubject);

    Request::Status::Handle hStatus = cast<Request::Status::Handle>(
            hChannel0->request(hRequest));
    if (NULL != hStatus)
        {
        try
            {
            hStatus->waitForResponse(getConnectTimeout());
            }
        catch (RequestTimeoutException::View e)
            {
            hConnection->close(false, e);
            COH_THROW (e);
            }
        }
    }

void Initiator::onServiceStopped()
    {
    PofConnection::Handle hConnection = getConnection();
    if (NULL != hConnection)
        {
        hConnection->closeInternal(true, NULL, 100);
        if (hConnection->isOpen())
            {
            hConnection->closeInternal(true, NULL, 1000);
            if (hConnection->isOpen())
                {
                COH_LOG("Unable to close \"" << hConnection
                        << "\"; this Connection will be abandoned", 1);
                }
            }
        }

    super::onServiceStopped();
    }

void Initiator::ping()
    {
    PofConnection::Handle hConnection = getConnection();
    if (NULL != hConnection)
        {
        hConnection->ping();
        }
    }


// ----- Service interface --------------------------------------------------

void Initiator::configure(XmlElement::View vXml)
    {
    COH_SYNCHRONIZED (this)
        {
        super::configure(vXml);

        setConnectTimeout(parseTime(vXml, "connect-timeout", getRequestTimeout()));
        }
    }

void Initiator::onServiceStopping()
    {
    PofConnection::Handle hConnection = getConnection();
    if (NULL != hConnection)
        {
        hConnection->closeInternal(true, NULL, 0);
        }

    super::onServiceStopping();
    }


// ----- ConnectionInitiator interface --------------------------------------

Connection::Handle Initiator::ensureConnection()
    {
    COH_SYNCHRONIZED(this)
        {
        if (!isRunning())
            {
            COH_THROW_STREAM(IllegalStateException, getServiceName() << " is not running");
            }

        PofConnection::Handle hConnection = getConnection();
        if (NULL == hConnection || !hConnection->isOpen())
            {
            hConnection = openConnection();
            }

        return hConnection;
        }
    }


// ----- Peer child component factory -----------------------------------

Protocol::View Initiator::instantiateProtocol() const
    {
    return InitiatorProtocol::getInstance();
    }


// ----- accessor methods ---------------------------------------------------

PofConnection::View Initiator::getConnection() const
    {
    return m_hConnection;
    }

PofConnection::Handle Initiator::getConnection()
    {
    return m_hConnection;
    }

void Initiator::setConnection(PofConnection::Handle hConnection)
    {
    m_hConnection = hConnection;
    }

int64_t Initiator::getConnectTimeout() const
    {
    return m_ldtConnectTimeout;
    }

void Initiator::setConnectTimeout(int64_t ldtTimeOut)
    {
    m_ldtConnectTimeout = ldtTimeOut;
    }


// ----- Describable interface ----------------------------------------------

String::View Initiator::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", ConnectTimeout=" << getConnectTimeout());
    }


COH_CLOSE_NAMESPACE3
