/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenChannelResponse.hpp"

#include "private/coherence/component/net/extend/protocol/OpenChannelRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofConnection;


// ----- constructors -------------------------------------------------------

OpenChannelResponse::OpenChannelResponse()
    : f_hChannelResult(self(), NULL, /*fMutable*/ true)
    {
    }

// ----- OpenChannelResponse interface --------------------------------------

Channel::Handle OpenChannelResponse::getResultChannel() const
    {
    return f_hChannelResult;
    }

// ----- Message interface --------------------------------------------------

int32_t OpenChannelResponse::getTypeId() const
    {
    return type_id;
    }

void OpenChannelResponse::run()
    {
    PofChannel::Channel::Handle hChannel0 = cast<Channel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    if (isFailure())
        {
        return;
        }

    // extract the new Channel configuration from the AcceptChannelRequest
    OpenChannelRequest::Handle hRequest =
            cast<OpenChannelRequest::Handle>(
                    hChannel0->getRequest(getRequestId()));
    COH_ENSURE(NULL != hRequest);

    PofConnection::Handle hConnection =
        cast<PofConnection::Handle>(hChannel0->getConnection());

    Protocol::MessageFactory::View vFactory =
            hRequest->getMessageFactory();

    Channel::Receiver::Handle hReceiver   = hRequest->getReceiver();
    Serializer::View          vSerializer = hRequest->getSerializer();
    Subject::View             vSubject    = hRequest->getSubject();
    int32_t                   nId         = cast<Integer32::View>(getResult())->getValue();

    Channel::Handle hChan = hConnection->openChannelResponse(
            nId, vFactory, vSerializer, hReceiver, vSubject);

    initialize(f_hChannelResult, hChan);
    }

COH_CLOSE_NAMESPACE5
