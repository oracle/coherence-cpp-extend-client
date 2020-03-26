/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITATOR_MESSAGE_FACTORY_HPP
#define COH_INITATOR_MESSAGE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/PeerMessageFactory.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* MessageFactory implementation for version 2 of the MessagingProtocol. This
* MessageFactory contains Message classes necessary to manage the lifecycle of
* Connections and Channels.
*
* The type identifiers of the Message classes instantiated by this
* MessageFactory are organized as follows:
*
* Internal (<0):
* (-1)  AcceptChannel
* (-2)  CloseChannel
* (-3)  CloseConnection
* (-4)  CreateChannel
* (-5)  NotifyShutdown
* (-6)  NotifyStartup
* (-7)  OpenChannel
* (-8)  OpenConnection
* (-9)  Response
* (-10) EncodedMessage
*
* Connection Lifecycle (0 - 10):
*
* (0)  OpenConnectionResponse (*)
* (1)  OpenConnectionRequest
* (3)  PingRequest
* (4)  PingResponse
* (10) NotifyConnectionClosed
*
* The OpenConnectionResponse has type identifier 0 for historical reasons.
* Prior to version 2 of the Messaging Protocol, all Request messages used a
* common Response type with type identifier 0. Since the first Response that a
* client expects to receive is an OpenConnectionResponse, this allows version
* 2 and newer servers to rejects connection attempts from version 1 clients.
*
* Channel Lifecycle (11-20):

* (11) OpenChannelRequest
* (12) OpenChannelResponse
* (13) AcceptChannelRequest
* (14) AcceptChannelResponse
* (20) NotifyChannelClosed
*
* @author nsa 2008.04.10
*/
class COH_EXPORT InitiatorMessageFactory
    : public class_spec<InitiatorMessageFactory,
        extends<PeerMessageFactory> >
    {
    friend class factory<InitiatorMessageFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PeerMessageFactory instance.
        */
        InitiatorMessageFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        InitiatorMessageFactory(const InitiatorMessageFactory&);


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessage::Handle instantiateMessage(int32_t nType) const;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_INITATOR_MESSAGE_FACTORY_HPP
