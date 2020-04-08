/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceFactory.hpp"

#include "private/coherence/component/net/extend/protocol/invocation/InvocationRequest.hpp"
#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)


// ----- constructors -------------------------------------------------------

InvocationServiceFactory::InvocationServiceFactory()
    {
    }


// ----- MessageFactory interface -------------------------------------------

int32_t InvocationServiceFactory::getVersion() const
    {
    return 1;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle InvocationServiceFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case InvocationRequest::type_id:
            return InvocationRequest::create();

        case InvocationServiceResponse::type_id:
            return InvocationServiceResponse::create();

        default:
            COH_THROW_STREAM (IllegalArgumentException,
                    "illegal message type: " << nType);
        }
    }

COH_CLOSE_NAMESPACE6
