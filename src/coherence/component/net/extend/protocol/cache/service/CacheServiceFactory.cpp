/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceFactory.hpp"

#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/DestroyCacheRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/EnsureCacheRequest.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)


// ----- constructors -------------------------------------------------------

CacheServiceFactory::CacheServiceFactory()
    {
    }


// ----- MessageFactory interface -------------------------------------------

int32_t CacheServiceFactory::getVersion() const
    {
    return 1;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle CacheServiceFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case EnsureCacheRequest::type_id:
            return EnsureCacheRequest::create();

        case DestroyCacheRequest::type_id:
            return DestroyCacheRequest::create();

        case CacheServiceResponse::type_id:
            return CacheServiceResponse::create();

        default:
            COH_THROW_STREAM (IllegalArgumentException,
                    "illegal message type: " << nType);
        }
    }

COH_CLOSE_NAMESPACE7
