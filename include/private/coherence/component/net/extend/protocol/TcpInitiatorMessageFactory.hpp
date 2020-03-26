/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_INITATOR_MESSAGE_FACTORY_HPP
#define COH_TCP_INITATOR_MESSAGE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/InitiatorMessageFactory.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* MessageFactory implementation for version 2 of the MessagingProtocol. This
* MessageFactory contains Message classes necessary to manage the lifecycle of
* Connections and Channels; it supports load balancing through connection
* redirection.
*
* @author lsho 2010.12.30
*/
class COH_EXPORT TcpInitiatorMessageFactory
    : public class_spec<TcpInitiatorMessageFactory,
        extends<InitiatorMessageFactory> >
    {
    friend class factory<TcpInitiatorMessageFactory>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TcpInitiatorMessageFactory instance.
        */
        TcpInitiatorMessageFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        TcpInitiatorMessageFactory(const TcpInitiatorMessageFactory&);


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessage::Handle instantiateMessage(int32_t nType) const;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_TCP_INITATOR_MESSAGE_FACTORY_HPP
