/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceFactory.hpp"

#include "private/coherence/component/net/extend/protocol/nameservice/LookupRequest.hpp"
#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)


// ----- constructors -------------------------------------------------------

NameServiceFactory::NameServiceFactory()
    {
    }

// ----- MessageFactory interface -------------------------------------------

int32_t NameServiceFactory::getVersion() const
    {
    return 1;
    }

// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle NameServiceFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case LookupRequest::type_id:
            return LookupRequest::create();

        case NameServiceResponse::type_id:
            return NameServiceResponse::create();

        default:
            COH_THROW_STREAM (IllegalArgumentException,
                    "illegal message type: " << nType);
        }
    }

COH_CLOSE_NAMESPACE6
