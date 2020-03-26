/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITIATOR_PROTOCOL_HPP
#define COH_INITIATOR_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/protocol/PeerProtocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


/**
* The InitiatorProtocol is used by the Initiator to manage the lifecycle of
* the Connection and Channel objects.
*
* @author nsa  2008.03.17
*/
class COH_EXPORT InitiatorProtocol
    : public class_spec<InitiatorProtocol,
        extends<PeerProtocol> >
    {
    friend class factory<InitiatorProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new InitiatorProtocol instance.
        */
        InitiatorProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        InitiatorProtocol(const InitiatorProtocol&);


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

#endif // COH_NAMED_CACHE_PROTOCOL_HPP
