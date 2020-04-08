/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/AcceptChannel.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

AcceptChannel::AcceptChannel()
    : f_vChannelUri(self()), f_hConnection(self()), f_hReceiver(self()),
      f_vSubject(self()), f_vIdentityToken(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle AcceptChannel::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }

void AcceptChannel::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofConnection::Handle hConnection = getConnection();
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Subject::View   vSubject  = getSubject();
    Peer::Handle    hModule   = cast<Peer::Handle>(hChannel0->getReceiver());

    hResponse->setResult
        (
        hConnection->acceptChannelInternal
            (
            getChannelUri(),
            hModule->ensureSerializer(),
            getReceiver(),
            vSubject,
            getIdentityToken()
            )
        );
    }


// ----- Message interface --------------------------------------------------

int32_t AcceptChannel::getTypeId() const
    {
    return type_id;
    }


// ----- accessor methods ---------------------------------------------------

URI::View AcceptChannel::getChannelUri() const
    {
    return f_vChannelUri;
    }

PofConnection::View AcceptChannel::getConnection() const
    {
    return f_hConnection;
    }

PofConnection::Handle AcceptChannel::getConnection()
    {
    return f_hConnection;
    }

Channel::Receiver::View AcceptChannel::getReceiver() const
    {
    return f_hReceiver;
    }

Channel::Receiver::Handle AcceptChannel::getReceiver()
    {
    return f_hReceiver;
    }

Subject::View AcceptChannel::getSubject() const
    {
    return f_vSubject;
    }

Array<octet_t>::View AcceptChannel::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

void AcceptChannel::setChannelUri(URI::View vUri)
    {
    initialize(f_vChannelUri, vUri);
    }

void AcceptChannel::setConnection(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

void AcceptChannel::setReceiver(Channel::Receiver::Handle hReceiver)
    {
    initialize(f_hReceiver, hReceiver);
    }

void AcceptChannel::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

void AcceptChannel::setIdentityToken(Array<octet_t>::View vIdentityToken)
    {
    initialize(f_vIdentityToken, vIdentityToken);
    }

COH_CLOSE_NAMESPACE5
