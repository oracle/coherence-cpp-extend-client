/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PEER_RESPONSE_HPP
#define COH_PEER_RESPONSE_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/URI.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;
using coherence::net::URI;
using coherence::net::messaging::Channel;


/**
* Generic Response used for all internal Requests.
*
* @author nsa 2008.03.17
*/
class COH_EXPORT PeerResponse
    : public class_spec<PeerResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<PeerResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        PeerResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        PeerResponse(const PeerResponse&);


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
        static const int32_t type_id = -9;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_PEER_RESPONSE_HPP
