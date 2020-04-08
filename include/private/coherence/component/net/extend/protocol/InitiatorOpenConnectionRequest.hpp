/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITIATOR_OPEN_CONNECTION_REQUEST_HPP
#define COH_INITIATOR_OPEN_CONNECTION_REQUEST_HPP

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnectionRequest.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofConnection;


/**
* This Request is used to accept a Channel that was spawned by a peer.
*
* @author nsa 2008.03.17
*/
class COH_EXPORT InitiatorOpenConnectionRequest
    : public class_spec<InitiatorOpenConnectionRequest,
        extends<OpenConnectionRequest> >
    {
    friend class factory<InitiatorOpenConnectionRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InitiatorOpenConnectionRequest instance.
        */
        InitiatorOpenConnectionRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        InitiatorOpenConnectionRequest(const InitiatorOpenConnectionRequest&);


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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = OpenConnectionRequest::type_id;

    
    // ----- accessors ------------------------------------------------------
    
    public:
        /**
        * Return the PofConnection to open.
        */
        virtual PofConnection::View getConnectionOpen() const;

        /**
        * Return the PofConnection to open.
        */
        virtual PofConnection::Handle getConnectionOpen();

        /**
        * Set the PofConnection to open.
        *
        * @param hConnection  the PofConnection to be opened
        */
        virtual void setConnectionOpen(PofConnection::Handle hConnection);

    	
    // ----- data members ---------------------------------------------------
    	
    protected:
        /**
        * The PofConnection to open.
        */
        FinalHandle<PofConnection> f_hConnection;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_HPP
