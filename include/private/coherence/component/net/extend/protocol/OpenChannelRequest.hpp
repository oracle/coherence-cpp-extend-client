/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OPEN_CHANNEL_REQUEST_HPP
#define COH_OPEN_CHANNEL_REQUEST_HPP

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
class COH_EXPORT OpenChannelRequest
    : public class_spec<OpenChannelRequest,
        extends<AbstractPofRequest> >
    {
    friend class factory<OpenChannelRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        OpenChannelRequest();


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


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return an optional token representing a Subject to associate with
        * the Channel. Operations performed on receipt of Messages sent via
        * the newly established Channel will be performed on behalf of this
        * Subject.
        *
        * @return an optional token representing a Subject to associate with
        *         the Channel
        */
        virtual Array<octet_t>::View getIdentityToken() const;

        /**
        * Return The MessageFactory used by the new Channel.
        */
        virtual Protocol::MessageFactory::View getMessageFactory() const;

        /**
        * Return The name of the Protocol used by the new Channel. This
        * Protocol name must have been previously returned in a Response sent
        * by the peer.
        *
        * @return The name of the Protocol used by the new Channel
        */
        virtual String::View getProtocolName() const;

        /**
        * Return the optional Receiver that the Channel will register with.
        *
        * @return the optional Receiver that the Channel will register with
        */
        virtual Channel::Receiver::View getReceiver() const;

        /**
        * Return the optional Receiver that the Channel will register with.
        *
        * @return the optional Receiver that the Channel will register with
        */
        virtual Channel::Receiver::Handle getReceiver();

        /**
        * Return the name of the peer Receiver to which the Channel should be
        * bound.
        *
        * @return  the name of the peer Receiver to which the Channel should be
        *          bound
        */
        virtual String::View getReceiverName() const;

        /**
        * Return the Serializer used by the new Channel.
        *
        * @return the Serializer used by the new Channel
        */
        virtual Serializer::View getSerializer() const;

        /**
        * Return the optional Receiver that the Channel will register with.
        *
        * @return the optional Receiver that the Channel will register with.
        */
        virtual Subject::View getSubject() const;

        /**
        * set an optional token representing a Subject to associate with
        * the Channel. Operations performed on receipt of Messages sent via
        * the newly established Channel will be performed on behalf of this
        * Subject.
        *
        * @param hIdentity  an optional token representing a Subject to
        *                   associate with the Channel
        */
        virtual void setIdentityToken(Array<octet_t>::View vIdentity);

        /**
        * Set the MessageFactory used by the new Channel.
        *
        * @param vMessageFactory  the MessageFactory used by the new Channel
        */
        virtual void setMessageFactory(Protocol::MessageFactory::View vMessageFactory);

        /**
        * Set the name of the Protocol used by the new Channel. This
        * Protocol name must have been previously returned in a Response sent
        * by the peer.
        *
        * @param vProtocolName  the name of the Protocol used by the new
        *                       Channel
        */
        virtual void setProtocolName(String::View vProtocolName);

        /**
        * Set the optional Receiver that the Channel will register with.
        *
        * @param vReceiver  the optional Receiver that the Channel will
        *                   register with
        */
        virtual void setReceiver(Channel::Receiver::Handle vReceiver);

        /**
        * Set the name of the peer Receiver to which the Channel should be
        * bound.
        *
        * @param vsReceiverName  the name of the peer Receiver to which the
        *                        Channel should be bound
        */
        virtual void setReceiverName(String::View vsReceiverName);

        /**
        * Set the Serializer used by the new Channel.
        *
        * @param hSerializer  the Serializer used by the new Channel
        */
        virtual void setSerializer(Serializer::View vSerializer);

        /**
        * Set the optional Receiver that the Channel will register with.
        *
        * @param vSubject  the optional Receiver that the Channel will
        *                  register with.
        */
        virtual void setSubject(Subject::View vSubject);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 11;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * An optional token representing a Subject to associate with the
        * Channel. Operations performed on receipt of Messages sent via the
        * newly established Channel will be performed on behalf of this
        * Subject.
        */
        FinalView<Array<octet_t> > f_vIdentityToken;

        /**
        * The MessageFactory used by the new Channel.
        */
        FinalView<Protocol::MessageFactory> f_vMessageFactory;

        /**
        * The name of the Protocol used by the new Channel. This Protocol
        * name must have been previously returned in a Response sent by the
        * peer.
        */
        FinalView<String> f_vProtocolName;

        /**
        * The optional Receiver that the Channel will register with.
        */
        FinalHandle<Channel::Receiver> f_hReceiver;

        /**
        * The Channel through which this Message was sent or received.
        *
        * @see coherence::net::messaging::Message::getChannel
        */
        FinalView<String> f_vsReceiverName;

        /**
        * The Serializer used by the new Channel.
        */
        FinalView<Serializer> f_vSerializer;

        /**
        * The optional Receiver that the Channel will register with.
        */
        FinalView<Subject> f_vSubject;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_OPEN_CHANNEL_REQUEST_HPP
