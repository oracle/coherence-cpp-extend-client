/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

PeerResponse::PeerResponse()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t PeerResponse::getTypeId() const
    {
    return type_id;
    }

void PeerResponse::run()
    {
    // no-op
    }

COH_CLOSE_NAMESPACE5
