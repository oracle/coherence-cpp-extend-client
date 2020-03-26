/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOTIFY_CHANNEL_CLOSED_HPP
#define COH_NOTIFY_CHANNEL_CLOSED_HPP

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessage;


/**
* This Message is sent to the peer when a Channel has been closed. This allows
* the peer to collect any resources held by the Channel.
*
* @author nsa 2008.03.19
*/
class COH_EXPORT NotifyChannelClosed
    : public class_spec<NotifyChannelClosed,
        extends<AbstractPofMessage> >
    {
    friend class factory<NotifyChannelClosed>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        NotifyChannelClosed();


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


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the optional reason the Channel was closed.
        *
        * @return the optional reason the Channel was closed
        */
        virtual Exception::Holder getCause() const;

        /**
        * Return the identifier of the Channel that was closed.
        *
        * @return the identifier of the Channel that was closed
        */
        virtual int32_t getChannelId() const;

        /**
        * Set the optional reason the Channel was closed.
        *
        * @param ohCause  the reason the Channel was closed
        */
        virtual void setCause(Exception::Holder ohCause);

        /**
        * Set the identifier of the Channel that was closed.
        *
        * @param iId  the identifier of the Channel that was closed
        */
        virtual void setChannelId(int32_t iId);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 20;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The optional reason the Channel was closed.
        */
        FinalHolder<Exception> f_ohCause;

        /**
        * The identifier of the Channel that was closed.
        */
        int32_t m_iChannelId;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_NOTIFY_CHANNEL_CLOSED_HPP
