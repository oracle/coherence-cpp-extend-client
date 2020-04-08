/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLOSE_CONNECTION_HPP
#define COH_CLOSE_CONNECTION_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;


/**
* Internal Message used to close a Connection.
*
* @author nsa 2008.03.18
*/
class COH_EXPORT CloseConnection
    : public class_spec<CloseConnection,
        extends<AbstractPofRequest> >
    {
    friend class factory<CloseConnection>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        CloseConnection();


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


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the optional reason why the Connection is being closed.
        *
        * @return the optional reason why the Connection is being closed
        */
        virtual Exception::Holder getCause() const;

        /**
        * Return the Connection to close.
        *
        * @return the Connection to close
        */
        virtual PofConnection::View getConnectionClose() const;

        /**
        * Return the Connection to close.
        *
        * @return the Connection to close
        */
        virtual PofConnection::Handle getConnectionClose();

        /**
        * Return whether to notify the peer that the Channel is being closed.
        *
        * @return true if the Peer should be notified that the Channel is
        *         being closed
        */
        virtual bool isNotify() const;

        /**
        * Set the optional reason why the Channel is being closed.
        *
        * @param oheCause  the optional reason why the Channel is being closed
        */
        virtual void setCause(Exception::Holder oheCause);

        /**
        * Set the Connection to close.
        *
        * @param hConnection  the Connection to close
        */
        virtual void setConnectionClose(PofConnection::Handle hConnection);

        /**
        * Set whether to notify the peer that the Channel is being closed.
        *
        * @param fNotify  true if the Peer should be notified that the
        *                 Channel is being closed
        */
        virtual void setNotify(bool fNotify);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -3;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The optional reason why the Channel is being closed.
        */
        FinalHolder<Exception> f_oheCause;

        /**
        * The Connection to close.
        */
        FinalHandle<PofConnection> f_hConnectionClose;

        /**
        * If true, notify the peer that the Channel is being closed.
        */
        bool m_fNotify;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_CLOSE_CONNECTION_HPP
