/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/PeerMessageFactory.hpp"

#include "private/coherence/component/net/extend/protocol/AcceptChannel.hpp"
#include "private/coherence/component/net/extend/protocol/AcceptChannelRequest.hpp"
#include "private/coherence/component/net/extend/protocol/AcceptChannelResponse.hpp"
#include "private/coherence/component/net/extend/protocol/CloseChannel.hpp"
#include "private/coherence/component/net/extend/protocol/CloseConnection.hpp"
#include "private/coherence/component/net/extend/protocol/CreateChannel.hpp"
#include "private/coherence/component/net/extend/protocol/EncodedMessage.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyChannelClosed.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyConnectionClosed.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyShutdown.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyStartup.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannel.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannelRequest.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannelResponse.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnection.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnectionRequest.hpp"
#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/net/extend/protocol/PingRequest.hpp"
#include "private/coherence/component/net/extend/protocol/PingResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

PeerMessageFactory::PeerMessageFactory()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessage::Handle PeerMessageFactory::instantiateMessage(
        int32_t nType) const
    {
    switch (nType)
        {
        case AcceptChannel::type_id:
            return AcceptChannel::create();

        case AcceptChannelRequest::type_id:
            return AcceptChannelRequest::create();

        case AcceptChannelResponse::type_id:
            return AcceptChannelResponse::create();

        case CloseChannel::type_id:
            return CloseChannel::create();

        case CloseConnection::type_id:
            return CloseConnection::create();

        case CreateChannel::type_id:
            return CreateChannel::create();

        case EncodedMessage::type_id:
            return EncodedMessage::create();

        case NotifyChannelClosed::type_id:
            return NotifyChannelClosed::create();

        case NotifyConnectionClosed::type_id:
            return NotifyConnectionClosed::create();

        case NotifyShutdown::type_id:
            return NotifyShutdown::create();

        case NotifyStartup::type_id:
            return NotifyStartup::create();

        case OpenChannel::type_id:
            return OpenChannel::create();

        case OpenChannelRequest::type_id:
            return OpenChannelRequest::create();

        case OpenChannelResponse::type_id:
            return OpenChannelResponse::create();

        case PeerResponse::type_id:
            return PeerResponse::create();

        case PingRequest::type_id:
            return PingRequest::create();

        case PingResponse::type_id:
            return PingResponse::create();

        default:
            COH_THROW_STREAM (IllegalArgumentException,
                    "illegal message type: " << nType);
        }
    }

COH_CLOSE_NAMESPACE5
