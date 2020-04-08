/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ACCEPT_CHANNEL_RESPONSE_HPP
#define COH_ACCEPT_CHANNEL_RESPONSE_HPP

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofResponse;


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @authore nsa 2008.03.17
*/
class COH_EXPORT AcceptChannelResponse
    : public class_spec<AcceptChannelResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<AcceptChannelResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        AcceptChannelResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        AcceptChannelResponse(const AcceptChannelResponse&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;

        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 14;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_ACCEPT_CHANNEL_RESPONSE_HPP
