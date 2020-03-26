/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenChannelRequest.hpp"

#include "private/coherence/component/net/extend/protocol/OpenChannelResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

OpenChannelRequest::OpenChannelRequest()
    :  f_vIdentityToken(self()), f_vMessageFactory(self()),
       f_vProtocolName(self()), f_hReceiver(self()), f_vsReceiverName(self()),
       f_vSerializer(self()), f_vSubject(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle OpenChannelRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(
            vFactory->createMessage(OpenChannelResponse::type_id));
    }

void OpenChannelRequest::onRun(AbstractPofResponse::Handle hResponse)
    {
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    PofConnection::Handle hConnection =
        cast<PofConnection::Handle>(hChannel0->getConnection());

    COH_ENSURE(NULL != hConnection);

    Peer::Handle hModule = cast<Peer::Handle>(hChannel0->getReceiver());

    Channel::Receiver::Handle hReceiver =
        hModule->getReceiver(getReceiverName());

    if (NULL == hReceiver)
        {
        COH_THROW_STREAM(IllegalArgumentException, "unknown receiver: "
                << getReceiverName());
        }

    hResponse->setResult
        (
        Integer32::create
            (
            hConnection->openChannelRequest
                (
                getProtocolName(),
                hChannel0->getSerializer(),
                hReceiver,
                hModule->assertIdentityToken(hModule->deserializeIdentityToken(
                        getIdentityToken()))
                )
            )
        );
    }


// ----- Message interface --------------------------------------------------

int32_t OpenChannelRequest::getTypeId() const
    {
    return type_id;
    }


// ---- PortableObject interface --------------------------------------------

void OpenChannelRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setProtocolName(hIn->readString(1));
    setReceiverName(hIn->readString(2));
    setIdentityToken(hIn->readOctetArray(3));
    }

void OpenChannelRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeString(1, getProtocolName());
    hOut->writeString(2, getReceiverName());
    hOut->writeOctetArray(3, getIdentityToken());
    }


// ----- Describable interface ----------------------------------------------

String::View OpenChannelRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Protocol=" << getProtocolName()
        << ", Receiver=" << getReceiverName());
    }


// ----- accessor methods ---------------------------------------------------

Array<octet_t>::View OpenChannelRequest::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

Protocol::MessageFactory::View OpenChannelRequest::getMessageFactory() const
    {
    return f_vMessageFactory;
    }

String::View OpenChannelRequest::getProtocolName() const
    {
    return f_vProtocolName;
    }

Channel::Receiver::View OpenChannelRequest::getReceiver() const
    {
    return f_hReceiver;
    }

Channel::Receiver::Handle OpenChannelRequest::getReceiver()
    {
    return f_hReceiver;
    }

String::View OpenChannelRequest::getReceiverName() const
    {
    return f_vsReceiverName;
    }

Serializer::View OpenChannelRequest::getSerializer() const
    {
    return f_vSerializer;
    }

Subject::View OpenChannelRequest::getSubject() const
    {
    return f_vSubject;
    }

void OpenChannelRequest::setIdentityToken(Array<octet_t>::View vIdentity)
    {
    initialize(f_vIdentityToken, vIdentity);
    }

void OpenChannelRequest::setMessageFactory(Protocol::MessageFactory::View vMessageFactory)
    {
    initialize(f_vMessageFactory, vMessageFactory);
    }

void OpenChannelRequest::setProtocolName(String::View vProtocolName)
    {
    initialize(f_vProtocolName, vProtocolName);
    }

void OpenChannelRequest::setReceiver(Channel::Receiver::Handle vReceiver)
    {
    initialize(f_hReceiver, vReceiver);
    }

void OpenChannelRequest::setReceiverName(String::View vsReceiverName)
    {
    initialize(f_vsReceiverName, vsReceiverName);
    }

void OpenChannelRequest::setSerializer(Serializer::View vSerializer)
    {
    initialize(f_vSerializer, vSerializer);
    }

void OpenChannelRequest::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

COH_CLOSE_NAMESPACE5
