/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/PingRequest.hpp"

#include "private/coherence/component/net/extend/protocol/PingResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

PingRequest::PingRequest()
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle PingRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(
            vFactory->createMessage(PingResponse::type_id));
    }

void PingRequest::onRun(AbstractPofResponse::Handle)
    {
    COH_ENSURE_EQUALITY(getChannel()->getId(), 0);
    }


// ----- Message interface --------------------------------------------------

int32_t PingRequest::getTypeId() const
    {
    return type_id;
    }

COH_CLOSE_NAMESPACE5
