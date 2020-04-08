/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/RemoteNameService.hpp"

#include "coherence/net/Member.hpp"
#include "coherence/util/HashMap.hpp"

#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/nameservice/LookupRequest.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::protocol::nameservice::NameServiceProtocol;
using coherence::component::net::extend::protocol::nameservice::LookupRequest;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::Protocol;
using coherence::security::SecurityHelper;

// ----- constructors -------------------------------------------------------

RemoteNameService::RemoteNameService()
    {
    }

// ----- NameService interface ----------------------------------------------

void RemoteNameService::bind(String::View /*vsName*/, Object::View /*o*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Object::View RemoteNameService::lookup(String::View vsName)
    {
    Channel::Handle                hChannel = ensureChannel();
    Protocol::MessageFactory::View vFactory = hChannel->getMessageFactory();
    LookupRequest::Handle          hRequest = cast<LookupRequest::Handle>(
        vFactory->createMessage(LookupRequest::type_id));

    hRequest->setLookupName(vsName);

    return hChannel->request(hRequest);
    }

void RemoteNameService::unbind(String::View /*vsName*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

// ----- ServiceInfo interface ----------------------------------------------

ServiceInfo::ServiceType RemoteNameService::getServiceType() const
    {
    return ServiceInfo::remote_nameservice;
    }

// ----- internal methods ---------------------------------------------------

void RemoteNameService::doConfigure(XmlElement::View vXml)
    {
    RemoteService::doConfigure(vXml);

    // register all Protocols
    ConnectionInitiator::Handle hInitiator = getInitiator();
    hInitiator->registerProtocol(NameServiceProtocol::getInstance());
    }

Channel::Handle RemoteNameService::openChannel()
    {
    Connection::Handle hConnection = getInitiator()->ensureConnection();
    return hConnection->openChannel(NameServiceProtocol::getInstance(),
        "NameService",
        NULL,
        SecurityHelper::getCurrentSubject());
    }

COH_CLOSE_NAMESPACE4
