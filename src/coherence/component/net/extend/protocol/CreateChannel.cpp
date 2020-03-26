/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/CreateChannel.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

CreateChannel::CreateChannel()
    : f_hConnection(self()), f_vProtocol(self()), f_hReceiver(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle CreateChannel::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }

void CreateChannel::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofConnection::Handle hConnection = getConnection();
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Peer::Handle hModule = cast<Peer::Handle>(hChannel0->getReceiver());

    hResponse->setResult
        (
        hConnection->createChannelInternal
            (
            getProtocol(),
            hModule->ensureSerializer(),
            getReceiver()
            )
        );
    }


// ----- Message interface --------------------------------------------------

int32_t CreateChannel::getTypeId() const
    {
    return type_id;
    }


// ----- accessor methods ---------------------------------------------------

PofConnection::View CreateChannel::getConnection() const
    {
    return f_hConnection;
    }

PofConnection::Handle CreateChannel::getConnection()
    {
    return f_hConnection;
    }

Protocol::View CreateChannel::getProtocol() const
    {
    return f_vProtocol;
    }

Protocol::View CreateChannel::getProtocol()
    {
    return f_vProtocol;
    }

Channel::Receiver::View CreateChannel::getReceiver() const
    {
    return f_hReceiver;
    }

Channel::Receiver::Handle CreateChannel::getReceiver()
    {
    return f_hReceiver;
    }

void CreateChannel::setConnection(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

void CreateChannel::setProtocol(Protocol::View vProtocol)
    {
    initialize(f_vProtocol, vProtocol);
    }

void CreateChannel::setReceiver(Channel::Receiver::Handle hReceiver)
    {
    initialize(f_hReceiver, hReceiver);
    }

COH_CLOSE_NAMESPACE5
