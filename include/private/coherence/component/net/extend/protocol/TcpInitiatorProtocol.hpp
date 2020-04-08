/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_INITIATOR_PROTOCOL_HPP
#define COH_TCP_INITIATOR_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/InitiatorProtocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* The TcpInitiatorProtocol is used by the Initiator to manage the lifecycle
* of Connection and Channel objects.
*
* @author lsho  2010.12.30
*/
class COH_EXPORT TcpInitiatorProtocol
    : public class_spec<TcpInitiatorProtocol,
        extends<InitiatorProtocol> >
    {
    friend class factory<TcpInitiatorProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TcpInitiatorProtocol instance.
        */
        TcpInitiatorProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        TcpInitiatorProtocol(const TcpInitiatorProtocol&);


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessageFactory::Handle instantiateMessageFactory(
                int32_t nVersion) const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return the singleton InitiatorProtocol instance.
        *
        * @return the singleton
        */
        static Handle getInstance();
    };

COH_CLOSE_NAMESPACE5

#endif // COH_TCP_INITIATOR_PROTOCOL_HPP
