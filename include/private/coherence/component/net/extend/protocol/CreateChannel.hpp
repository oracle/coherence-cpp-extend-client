/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CREATE_CHANNEL_HPP
#define COH_CREATE_CHANNEL_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"
#include "private/coherence/net/messaging/Channel.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Channel;


/**
* Internal Request used to create a Channel.
*
* @authore nsa 2008.03.19
*/
class COH_EXPORT CreateChannel
    : public class_spec<CreateChannel,
        extends<AbstractPofRequest> >
    {
    friend class factory<CreateChannel>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        CreateChannel();


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
        * Return the Connection used to accept the Channel.
        *
        * @return the connection used to accept the channel.
        */
        virtual PofConnection::View getConnection() const;

        /**
        * Return the Connection used to accept the Channel.
        *
        * @return the connection used to accept the channel.
        */
        virtual PofConnection::Handle getConnection();

        /**
        * Return the Protocol used by the new Channel.
        *
        * @return the Protocol used by the new Channel
        */
        virtual Protocol::View getProtocol() const;

        /**
        * Return the Protocol used by the new Channel.
        *
        * @return the Protocol used by the new Channel
        */
        virtual Protocol::View getProtocol();

        /**
        * Return the optional Receiver that the Channel will register with.
        *
        * @retun the optional Receiver that the Channel will register with
        */
        virtual Channel::Receiver::View getReceiver() const;

        /**
        * Return the optional Receiver that the Channel will register with.
        *
        * @retun the optional Receiver that the Channel will register with
        */
        virtual Channel::Receiver::Handle getReceiver();

        /**
        * Set the Connection used to accept the Channel.
        *
        * @param hConnection  the connection used to accept the channel.
        */
        virtual void setConnection(PofConnection::Handle hConnection);

        /**
        * Set the Protocol used by the new Channel.
        *
        * @param hProtocol  the Protocol used by the new Channel
        */
        virtual void setProtocol(Protocol::View hProtocol);

        /**
        * Set the optional Receiver that the Channel will register with.
        *
        * @param hReceiver  the optional Receiver that the Channel will
        *                   register with
        */
        virtual void setReceiver(Channel::Receiver::Handle hReceiver);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -4;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Connection used to accept the Channel.
        */
        FinalHandle<PofConnection> f_hConnection;

        /**
        * The Protocol used by the new Channel.
        */
        FinalView<Protocol> f_vProtocol;

        /**
        * The optional Receiver that the Channel will register with.
        */
        FinalHandle<Channel::Receiver> f_hReceiver;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_CREATE_CHANNEL_HPP
