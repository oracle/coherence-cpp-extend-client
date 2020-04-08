/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"

#include "coherence/net/Member.hpp"
#include "coherence/util/HashMap.hpp"

#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/invocation/InvocationRequest.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::protocol::invocation::InvocationServiceProtocol;
using coherence::component::net::extend::protocol::invocation::InvocationRequest;
using coherence::net::Member;
using coherence::net::messaging::ConnectionInitiator;
using coherence::net::messaging::Protocol;
using coherence::util::HashMap;
using coherence::security::SecurityHelper;


// ----- constructors -------------------------------------------------------

RemoteInvocationService::RemoteInvocationService()
    {
    }


// ----- InvocationService interface ----------------------------------------

Map::View RemoteInvocationService::query(Invocable::Handle hTask, Set::View vSetMembers)
    {
    if (NULL == hTask)
        {
        COH_THROW (IllegalArgumentException::create("task cannot be NULL"));
        }

    if (NULL != vSetMembers)
        {
        COH_THROW (IllegalArgumentException::create("directed query not supported; "
                "the specified Member set must be NULL"));
        }

    Channel::Handle                 hChannel = ensureChannel();
    Protocol::MessageFactory::View  vFactory = hChannel->getMessageFactory();
    InvocationRequest::Handle       hRequest = cast<InvocationRequest::Handle>(
            vFactory->createMessage(InvocationRequest::type_id));

    hRequest->setTask(hTask);

    Object::Holder ohResult = hChannel->request(hRequest);
    Member::View   vMember  = getOperationalContext()->getLocalMember();
    Map::Handle    hMap     = HashMap::create();

    hMap->put(vMember, ohResult);

    return hMap;
    }


// ----- ServiceInfo interface ----------------------------------------------

ServiceInfo::ServiceType RemoteInvocationService::getServiceType() const
    {
    return ServiceInfo::remote_invocation;
    }


// ----- internal methods ---------------------------------------------------

void RemoteInvocationService::doConfigure(XmlElement::View vXml)
    {
    RemoteService::doConfigure(vXml);

    // register all Protocols
    ConnectionInitiator::Handle hInitiator = getInitiator();
    hInitiator->registerProtocol(InvocationServiceProtocol::getInstance());
    }

Channel::Handle RemoteInvocationService::openChannel()
    {
    lookupProxyServiceAddress();
    
    Connection::Handle hConnection = getInitiator()->ensureConnection();
    return hConnection->openChannel(InvocationServiceProtocol::getInstance(),
            "InvocationServiceProxy",
            NULL,
            SecurityHelper::getCurrentSubject());
    }

COH_CLOSE_NAMESPACE4
