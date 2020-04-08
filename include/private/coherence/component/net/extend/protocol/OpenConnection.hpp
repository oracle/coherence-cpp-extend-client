/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPEN_CONNECTION_HPP
#define COH_OPEN_CONNECTION_HPP

#include "coherence/lang.ns"

#include "coherence/security/auth/Subject.hpp"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Protocol;
using coherence::security::auth::Subject;


/**
* Internal Request used to create a Channel.
*
* @authore nsa 2008.03.19
*/
class COH_EXPORT OpenConnection
    : public abstract_spec<OpenConnection,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        OpenConnection();

    private:
        /**
        * Blocked copy constructor.
        */
        OpenConnection(const OpenConnection&);


    // ----- AbstractPofRequest interface -----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofResponse::Handle instantiateResponse(
                        Protocol::MessageFactory::View vFactory) const;


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Connection to open.
        *
        * @return the Connection to open
        */
        virtual PofConnection::View getConnectionOpen() const;

        /**
        * Return the Connection to open.
        *
        * @return the Connection to open
        */
        virtual PofConnection::Handle getConnectionOpen();

        /**
        * Set the Connection to open.
        *
        * @param hConnection  the Connection to open
        */
        virtual void setConnectionOpen(PofConnection::Handle hConnection);

        /**
        * Return the identity under which Messages received by "Channel0"
        * will be executed.
        *
        * @return the identity under which Messages received by "Channel0"
        *         will be executed.
        */
        virtual Subject::View getSubject() const;

        /**
        * Return the token representing a user's identity.
        *
        * @return the token representing a user's identity.
        */
        virtual Array<octet_t>::View getIdentityToken() const;

        /**
        * Set the identity under which Messages received by "Channel0" will be
        * executed.
        *
        * @param vSubject  the identity under which Messages received by
        *                  "Channel0" will be executed.
        */
        virtual void setSubject(Subject::View vSubject);

        /**
        * Set the token representing a user's identity.
        *
        * @param vIdentityToken  the token representing a user's identity.
        */
        virtual void setIdentityToken(Array<octet_t>::View vToken);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -8;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Connection to open.
        */
        FinalHandle<PofConnection> f_hConnectionOpen;

        /**
        * The identity under which Messages received by "Channel0" will be
        * executed.
        */
        FinalView<Subject> f_vSubject;

        /**
        * The token representing a user's identity.
        */
        FinalView<Array<octet_t> > f_vIdentityToken;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CONNECTION_HPP
