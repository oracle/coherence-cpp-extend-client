/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/AcceptChannelResponse.hpp"

#include "private/coherence/component/net/extend/protocol/AcceptChannelRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofConnection;


// ----- constructors -------------------------------------------------------

AcceptChannelResponse::AcceptChannelResponse()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t AcceptChannelResponse::getTypeId() const
    {
    return type_id;
    }

void AcceptChannelResponse::run()
    {
    PofChannel::Channel::Handle hChannel0 = cast<Channel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    if (isFailure())
        {
        return;
        }

    // extract the new Channel configuration from the AcceptChannelRequest
    AcceptChannelRequest::Handle hRequest =
            cast<AcceptChannelRequest::Handle>(
                    hChannel0->getRequest(getRequestId()));
    COH_ENSURE(NULL != hRequest);

    PofConnection::Handle hConnection =
        cast<PofConnection::Handle>(hChannel0->getConnection());

    Protocol::MessageFactory::View vFactory =
            hRequest->getMessageFactory();

    int32_t                   nId         = hRequest->getChannelId();
    Channel::Receiver::Handle hReceiver   = hRequest->getReceiver();
    Serializer::View          vSerializer = hRequest->getSerializer();
    Subject::View             vSubject    = hRequest->getSubject();

    setResult
        (
        hConnection->acceptChannelResponse
            (
            nId, vFactory, vSerializer, hReceiver, vSubject
            )
        );
    }

COH_CLOSE_NAMESPACE5
