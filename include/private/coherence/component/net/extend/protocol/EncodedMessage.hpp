/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENCODED_MESSAGE_HPP
#define COH_ENCODED_MESSAGE_HPP

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessage;
using coherence::component::net::extend::PofConnection;


/**
* A Message with a ReadBuffer that contains an encoded Message. The service
* thread will decode the Message using the configured Codec before dispatching
* it for execution.
*
* @author nsa 2008.03.19
*/
class COH_EXPORT EncodedMessage
    : public class_spec<EncodedMessage,
        extends<AbstractPofMessage> >
    {
    friend class factory<EncodedMessage>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        EncodedMessage();


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


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the Connection that received the encoded Message.
        *
        * @return the Connection that received the encoded Message
        */
        virtual PofConnection::View getConnection() const;

        /**
        * Return the Connection that received the encoded Message.
        *
        * @return the Connection that received the encoded Message
        */
        virtual PofConnection::Handle getConnection();

        /**
        * Return the encoded Message.
        *
        * @return the encoded Message
        */
        virtual ReadBuffer::View getReadBuffer() const;

        /**
        * Return the encoded Message.
        *
        * @return the encoded Message
        */
        virtual ReadBuffer::View getReadBuffer();

        /**
        * Set the Connection that received the encoded Message.
        *
        * @param hConnection  the Connection that received the encoded
        *                     Message
        */
        virtual void setConnection(PofConnection::Handle hConnection);

        /**
        * Set the encoded Message.
        *
        * @param hMessage  the encoded Message
        */
        virtual void setReadBuffer(ReadBuffer::View vMessage);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = -10;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Connection that received the encoded Message.
        */
        FinalHandle<PofConnection> f_hConnection;

        /**
        * The encoded Message.
        */
        FinalView<ReadBuffer> f_vReadBuffer;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_ENCODED_MESSAGE_HPP
