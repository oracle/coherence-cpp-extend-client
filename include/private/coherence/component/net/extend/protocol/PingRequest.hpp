/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PING_REQUEST_HPP
#define COH_PING_REQUEST_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Protocol;


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @author nsa 2008.03.17
*/
class COH_EXPORT PingRequest
    : public class_spec<PingRequest,
        extends<AbstractPofRequest> >
    {
    friend class factory<PingRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        PingRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        PingRequest(const PingRequest&);


    // ----- AbstractPofRequest interface -----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                        Protocol::MessageFactory::View vFactory) const;

        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 3;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_PING_REQUEST_HPP
