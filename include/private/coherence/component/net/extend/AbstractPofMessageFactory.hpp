/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_POF_MESSAGE_FACTORY_HPP
#define COH_ABSTRACT_POF_MESSAGE_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/util/Describable.hpp"

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"
#include "private/coherence/net/messaging/Message.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"



COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::messaging::Message;
using coherence::net::messaging::Protocol;
using coherence::util::Describable;


/**
* Abstract MessageFactory implementation that creates POFable
* Message instances.
*
* @author jh  2008.02.14
*/
class COH_EXPORT AbstractPofMessageFactory
    : public abstract_spec<AbstractPofMessageFactory,
        extends<Describable>,
        implements<Protocol::MessageFactory> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractPofMessageFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractPofMessageFactory(const AbstractPofMessageFactory&);
		

    // ----- MessageFactory interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Protocol::View getProtocol() const;

        /**
        * {@inheritDoc}
        */
        virtual Message::Handle createMessage(int32_t nType) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getVersion() const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Create and return a new instance of the specified type of
        * AbstractPofMessage.
        *
        * @param nType  the type of Message to return
        *
        * @return a new AbstractPofMessage of the specified type
        */
        virtual AbstractPofMessage::Handle instantiateMessage(
                int32_t nType) const = 0;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Configure the Protocol for this MessageFactory.
        *
        * @param vProtocol  the Protocol
        */
        virtual void setProtocol(Protocol::View vProtocol);

        /**
        * Set message version
        *
        * @param nVersion  the message version
        */
        virtual void setVersion(int32_t nVersion);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Protocol for which this MessageFactory creates Message
        * objects.
        */
        FinalView<Protocol> f_vProtocol;

        /**
        * The Version of the Message objects created by this MessageFactory.
        */
        int32_t m_nVersion;
    };

COH_CLOSE_NAMESPACE4

#endif // COH_ABSTRACT_POF_MESSAGE_FACTORY_HPP
