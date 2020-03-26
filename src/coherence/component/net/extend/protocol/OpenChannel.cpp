/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenChannel.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

OpenChannel::OpenChannel()
    : f_hConnection(self()), f_vProtocol(self()),
      f_hReceiver(self()), f_vsReceiverName(self()), f_vSubject(self()),
      f_vIdentityToken(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle OpenChannel::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }

void OpenChannel::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofConnection::Handle hConnection = getConnection();
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Peer::Handle hModule = cast<Peer::Handle>(hChannel0->getReceiver());

    hResponse->setResult
        (
        hConnection->openChannelInternal
            (
            getProtocol(),
            getReceiverName(),
            hModule->ensureSerializer(),
            getReceiver(),
            getSubject(),
            getIdentityToken()
            )
        );
    }


// ----- Message interface --------------------------------------------------

int32_t OpenChannel::getTypeId() const
    {
    return type_id;
    }


// ----- accessor methods ---------------------------------------------------

PofConnection::View OpenChannel::getConnection() const
    {
    return f_hConnection;
    }

PofConnection::Handle OpenChannel::getConnection()
    {
    return f_hConnection;
    }

Protocol::View OpenChannel::getProtocol() const
    {
    return f_vProtocol;
    }

Protocol::View OpenChannel::getProtocol()
    {
    return f_vProtocol;
    }

Channel::Receiver::View OpenChannel::getReceiver() const
    {
    return f_hReceiver;
    }

Channel::Receiver::Handle OpenChannel::getReceiver()
    {
    return f_hReceiver;
    }

String::View OpenChannel::getReceiverName() const
    {
    return f_vsReceiverName;
    }

Subject::View OpenChannel::getSubject() const
    {
    return f_vSubject;
    }

Array<octet_t>::View OpenChannel::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

void OpenChannel::setConnection(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

void OpenChannel::setProtocol(Protocol::View vProtocol)
    {
    initialize(f_vProtocol, vProtocol);
    }

void OpenChannel::setReceiver(Channel::Receiver::Handle hReceiver)
    {
    initialize(f_hReceiver, hReceiver);
    }

void OpenChannel::setReceiverName(String::View vsReceiverName)
    {
    initialize(f_vsReceiverName, vsReceiverName);
    }

void OpenChannel::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

void OpenChannel::setIdentityToken(Array<octet_t>::View vIdentityToken)
    {
    initialize(f_vIdentityToken, vIdentityToken);
    }

COH_CLOSE_NAMESPACE5
