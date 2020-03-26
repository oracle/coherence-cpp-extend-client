/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPEN_CHANNEL_HPP
#define COH_OPEN_CHANNEL_HPP

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Protocol;


/**
* Internal Request used to create a Channel.
*
* @authore nsa 2008.03.19
*/
class COH_EXPORT OpenChannel
    : public class_spec<OpenChannel,
        extends<AbstractPofRequest> >
    {
    friend class factory<OpenChannel>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        OpenChannel();


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
        * Return the name of the remote Receiver.
        */
        virtual String::View getReceiverName() const;

        /**
        * Return the identity under which Messages received by the new
        * Channel will be executed.
        *
        * @return the identity under which Messages received by the new
        *         Channel will be executed.
        */
        virtual Subject::View getSubject() const;

        /**
        * Return the token representing a user's identity.
        *
        * @return the token representing a user's identity.
        */
        virtual Array<octet_t>::View getIdentityToken() const;

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

        /**
        * Set the name of the remote Receiver.
        *
        * @param vsReceiverName  the name of the remote Receiver
        */
        virtual void setReceiverName(String::View vsReceiverName);

        /**
        * Set the identity under which Messages received by the new
        * Channel will be executed.
        *
        * @param vSubject  the identity under which Messages received by the
        *                  new Channel will be executed.
        */
        virtual void setSubject(Subject::View vSubject);

        /**
        * Set the token representing a user's identity.
        *
        * @param vIdentityToken  the token representing a user's identity.
        */
        virtual void setIdentityToken(Array<octet_t>::View vIdentityToken);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -7;


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

        /**
        * The name of the remote Receiver.
        */
        FinalView<String> f_vsReceiverName;

        /**
        * The identity under which Messages received by the new Channel will
        * be executed.
        */
        FinalView<Subject> f_vSubject;

        /**
        * The token representing a user's identity.
        */
        FinalView<Array<octet_t> > f_vIdentityToken;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CHANNEL_HPP
