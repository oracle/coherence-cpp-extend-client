/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionRequest.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

InitiatorOpenConnectionRequest::InitiatorOpenConnectionRequest()
    : f_hConnection(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle InitiatorOpenConnectionRequest::instantiateResponse(
                Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(
                vFactory->createMessage(InitiatorOpenConnectionResponse::type_id));
    }

void InitiatorOpenConnectionRequest::onRun(AbstractPofResponse::Handle)
    {
    COH_ENSURE_EQUALITY(getChannel()->getId(), 0);
    }

PofConnection::View InitiatorOpenConnectionRequest::getConnectionOpen() const
    {
    return f_hConnection;
    }

PofConnection::Handle InitiatorOpenConnectionRequest::getConnectionOpen()
    {
    return f_hConnection;
    }

void InitiatorOpenConnectionRequest::setConnectionOpen(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

COH_CLOSE_NAMESPACE5
