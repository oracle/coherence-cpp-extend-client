/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITIATOR_OPEN_CONNECTION_RESPONSE_HPP
#define COH_INITIATOR_OPEN_CONNECTION_RESPONSE_HPP

#include "private/coherence/component/net/extend/protocol/OpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @authore nsa 2008.04.10
*/
class COH_EXPORT InitiatorOpenConnectionResponse
    : public class_spec<InitiatorOpenConnectionResponse,
        extends<OpenConnectionResponse> >
    {
    friend class factory<InitiatorOpenConnectionResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InitiatorOpenConnectionResponse instance.
        */
        InitiatorOpenConnectionResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        InitiatorOpenConnectionResponse(const InitiatorOpenConnectionResponse&);


    // ----- Runnable interface

    public:
        /**
        * @internal
        */
        virtual void run();


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = OpenConnectionResponse::type_id;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_RESPONSE_HPP
