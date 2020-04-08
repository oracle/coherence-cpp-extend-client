/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/AcceptChannelRequest.hpp"

#include "private/coherence/component/net/extend/protocol/AcceptChannelResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

AcceptChannelRequest::AcceptChannelRequest()
    :  m_iChannelId(0), f_vIdentityToken(self()), f_vMessageFactory(self()),
       f_vProtocolName(self()), f_hReceiver(self()), f_vSerializer(self()),
       f_vSubject(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle AcceptChannelRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(
            vFactory->createMessage(AcceptChannelResponse::type_id));
    }

void AcceptChannelRequest::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    PofConnection::Handle hConnection = cast<PofConnection::Handle>(hChannel0->getConnection());
    COH_ENSURE(NULL != hConnection);

    Peer::Handle hModule = cast<Peer::Handle>(getChannel()->getReceiver());
    int32_t      nId     = getChannelId();

    hConnection->acceptChannelRequest
        (
        nId,
        hModule->assertIdentityToken(hModule->deserializeIdentityToken(getIdentityToken()))
        );

    // Cannot use getImplVersion() here as channel0 is established prior to protocol
    // version negotiation.  Instead check the negotiated version directly on the connection's
    // MessageFactoryMap.
    Protocol::MessageFactory::View vClientFactory = cast<Protocol::MessageFactory::View>(hConnection->getMessageFactoryMap()
        ->get(hChannel0->getMessageFactory()->getProtocol()->getName()));
    if (vClientFactory != NULL && vClientFactory->getVersion() >= 3)
        {
        hResponse->setResult(Integer32::valueOf(nId));
        }
    }


// ----- Message interface --------------------------------------------------

int32_t AcceptChannelRequest::getTypeId() const
    {
    return type_id;
    }


// ---- PortableObject interface --------------------------------------------

void AcceptChannelRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setChannelId(hIn->readInt32(1));
    setIdentityToken(hIn->readOctetArray(2));
    }

void AcceptChannelRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeInt32(1, getChannelId());
    hOut->writeOctetArray(2, getIdentityToken());
    }


// ----- Describable interface ----------------------------------------------

String::View AcceptChannelRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", ChannelId=" << getChannelId());
    }


// ----- accessor methods ---------------------------------------------------

int32_t AcceptChannelRequest::getChannelId() const
    {
    return m_iChannelId;
    }

Array<octet_t>::View AcceptChannelRequest::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

Protocol::MessageFactory::View AcceptChannelRequest::getMessageFactory() const
    {
    return f_vMessageFactory;
    }

String::View AcceptChannelRequest::getProtocolName() const
    {
    return f_vProtocolName;
    }

Channel::Receiver::View AcceptChannelRequest::getReceiver() const
    {
    return f_hReceiver;
    }

Channel::Receiver::Handle AcceptChannelRequest::getReceiver()
    {
    return f_hReceiver;
    }

Serializer::View AcceptChannelRequest::getSerializer() const
    {
    return f_vSerializer;
    }

Subject::View AcceptChannelRequest::getSubject() const
    {
    return f_vSubject;
    }

void AcceptChannelRequest::setChannelId(int32_t iId)
    {
    m_iChannelId = iId;
    }

void AcceptChannelRequest::setIdentityToken(Array<octet_t>::View vIdentity)
    {
    initialize(f_vIdentityToken, vIdentity);
    }

void AcceptChannelRequest::setMessageFactory(Protocol::MessageFactory::View vMessageFactory)
    {
    initialize(f_vMessageFactory, vMessageFactory);
    }

void AcceptChannelRequest::setProtocolName(String::View vProtocolName)
    {
    initialize(f_vProtocolName, vProtocolName);
    }

void AcceptChannelRequest::setReceiver(Channel::Receiver::Handle vReceiver)
    {
    initialize(f_hReceiver, vReceiver);
    }

void AcceptChannelRequest::setSerializer(Serializer::View vSerializer)
    {
    initialize(f_vSerializer, vSerializer);
    }

void AcceptChannelRequest::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

COH_CLOSE_NAMESPACE5
