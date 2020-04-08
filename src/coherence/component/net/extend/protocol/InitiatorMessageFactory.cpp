/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/InitiatorMessageFactory.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnection.hpp"
#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionRequest.hpp"
#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

InitiatorMessageFactory::InitiatorMessageFactory()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle InitiatorMessageFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case InitiatorOpenConnection::type_id:
            return InitiatorOpenConnection::create();

        case InitiatorOpenConnectionRequest::type_id:
            return InitiatorOpenConnectionRequest::create();

        case InitiatorOpenConnectionResponse::type_id:
            return InitiatorOpenConnectionResponse::create();

        default:
            return PeerMessageFactory::instantiateMessage(nType);
        }
    }

COH_CLOSE_NAMESPACE5
