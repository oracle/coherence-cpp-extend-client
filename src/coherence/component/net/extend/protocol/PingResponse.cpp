/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/PingResponse.hpp"

#include "private/coherence/component/net/extend/protocol/PingRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofConnection;


// ----- constructors -------------------------------------------------------

PingResponse::PingResponse()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t PingResponse::getTypeId() const
    {
    return type_id;
    }

void PingResponse::run()
    {
    PofChannel::Channel::Handle hChannel0 = cast<Channel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    PofConnection::Handle hConnection =
        cast<PofConnection::Handle>(hChannel0->getConnection());

    hConnection->setPingLastMillis(0L);
    }

COH_CLOSE_NAMESPACE5
