/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/TcpInitiatorMessageFactory.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorOpenConnection.hpp"
#include "private/coherence/component/net/extend/protocol/TcpInitiatorOpenConnectionRequest.hpp"
#include "private/coherence/component/net/extend/protocol/TcpInitiatorOpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

TcpInitiatorMessageFactory::TcpInitiatorMessageFactory()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle TcpInitiatorMessageFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case InitiatorOpenConnectionRequest::type_id:
            return TcpInitiatorOpenConnectionRequest::create();

        case InitiatorOpenConnectionResponse::type_id:
            return TcpInitiatorOpenConnectionResponse::create();

        default:
            return super::instantiateMessage(nType);
        }
    }

COH_CLOSE_NAMESPACE5
