/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheFactory.hpp"

#include "private/coherence/component/net/extend/protocol/cache/AggregateAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AggregateFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ClearRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsKeyRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ContainsValueRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/GetRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/IndexRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/InvokeRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerFilterRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/ListenerKeyRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/LockRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/MapEventMessage.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PartialResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/QueryRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveAllRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/RemoveRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/SizeRequest.hpp"
#include "private/coherence/component/net/extend/protocol/cache/UnlockRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

NamedCacheFactory::NamedCacheFactory()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle NamedCacheFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case AggregateAllRequest::type_id:
            return AggregateAllRequest::create();

        case AggregateFilterRequest::type_id:
            return AggregateFilterRequest::create();

        case ClearRequest::type_id:
            return ClearRequest::create();

        case ContainsAllRequest::type_id:
            return ContainsAllRequest::create();

        case ContainsKeyRequest::type_id:
            return ContainsKeyRequest::create();

        case ContainsValueRequest::type_id:
            return ContainsValueRequest::create();

        case GetAllRequest::type_id:
            return GetAllRequest::create();

        case GetRequest::type_id:
            return GetRequest::create();

        case IndexRequest::type_id:
            return IndexRequest::create();

        case InvokeAllRequest::type_id:
            return InvokeAllRequest::create();

        case InvokeFilterRequest::type_id:
            return InvokeFilterRequest::create();

        case InvokeRequest::type_id:
            return InvokeRequest::create();

        case ListenerFilterRequest::type_id:
            return ListenerFilterRequest::create();

        case ListenerKeyRequest::type_id:
            return ListenerKeyRequest::create();

        case LockRequest::type_id:
            return LockRequest::create();

        case MapEventMessage::type_id:
            return MapEventMessage::create();

        case PartialResponse::type_id:
            return PartialResponse::create();

        case PutAllRequest::type_id:
            return PutAllRequest::create();

        case PutRequest::type_id:
            return PutRequest::create();

        case QueryRequest::type_id:
            return QueryRequest::create();

        case RemoveAllRequest::type_id:
            return RemoveAllRequest::create();

        case RemoveRequest::type_id:
            return RemoveRequest::create();

        case SizeRequest::type_id:
            return SizeRequest::create();

        case NamedCacheResponse::type_id:
            return NamedCacheResponse::create();

        case UnlockRequest::type_id:
            return UnlockRequest::create();

        default:
            COH_THROW_STREAM (IllegalArgumentException,
                    "illegal message type: " << nType);
        }
    }

COH_CLOSE_NAMESPACE6
