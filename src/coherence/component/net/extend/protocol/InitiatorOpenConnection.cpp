/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnection.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionRequest.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/net/extend/RemoteService.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

InitiatorOpenConnection::InitiatorOpenConnection()
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

void InitiatorOpenConnection::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofConnection::Handle hConnection = getConnectionOpen();
    COH_ENSURE(!hConnection->isOpen());

    Peer::Handle hModule = cast<Peer::Handle>(getChannel()->getReceiver());
    hConnection->openInternal();

    try
        {
        Channel::Handle                  hChannel0 = hConnection->getChannel(0);
        Protocol::MessageFactory::View   vFactory0 = hChannel0->getMessageFactory();

        // sent a OpenConnectionRequest to the peer via "Channel0"

        InitiatorOpenConnectionRequest::Handle hRequest =
            cast<InitiatorOpenConnectionRequest::Handle>(
                vFactory0->createMessage(InitiatorOpenConnectionRequest::type_id));

        hRequest->setClientId(hModule->getProcessId());
        hRequest->setConnectionOpen(hConnection);
        hRequest->setEdition(hModule->getOperationalContext()->getEdition());
        hRequest->setIdentityToken(getIdentityToken());
        hRequest->setMember(hModule->getOperationalContext()->getLocalMember());
        hRequest->setProtocolVersionMap(hModule->getProtocolVersionMap());
        hRequest->setSubject(getSubject());

        Service::View vSvcParent = hModule->getParentService();
        if (instanceof<RemoteService::View>(vSvcParent))
            {
            RemoteService::View vSvcRemote = cast<RemoteService::View>(vSvcParent);
            hRequest->setClusterName(vSvcRemote->getRemoteClusterName());
            hRequest->setServiceName(vSvcRemote->getRemoteServiceName());
            }

        hResponse->setResult(hChannel0->send((Request::Handle) hRequest));
        }
    catch (Exception::View e)
        {
        hConnection->closeInternal(false, e, -1L);
        COH_THROW (e);
        }
    }

COH_CLOSE_NAMESPACE5
