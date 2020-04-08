/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOTIFY_CONNECTION_CLOSED_HPP
#define COH_NOTIFY_CONNECTION_CLOSED_HPP

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessage;


/**
* This Message is sent to the peer when a Connection has been closed.
* This allows the peer to collect any resources held by the Connection.
*
* @author nsa 2008.03.19
*/
class COH_EXPORT NotifyConnectionClosed
    : public class_spec<NotifyConnectionClosed,
        extends<AbstractPofMessage> >
    {
    friend class factory<NotifyConnectionClosed>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        NotifyConnectionClosed();


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


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the optional reason the Connection was closed.
        *
        * @return  the reason the Connection was closed
        */
        virtual Exception::Holder getCause() const;

        /**
        * Set the optional reason the Connection was closed.
        *
        * @param ohCause  the reason the Connection was closed
        */
        virtual void setCause(Exception::Holder ohCause);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 10;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The optional reason the Connection was closed.
        */
        FinalHolder<Exception> f_ohCause;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_NOTIFY_CONNECTION_CLOSED_HPP
