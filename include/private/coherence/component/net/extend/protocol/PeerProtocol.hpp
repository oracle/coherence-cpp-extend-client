/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PEER_PROTOCOL_HPP
#define COH_PEER_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::AbstractPofMessageFactory;
using coherence::component::net::extend::AbstractPofProtocol;


/**
* The Extend Protocol is used to manipulate a remote Cache Component running
* within a Coherence cluster.
*
* @author nsa  2008.03.17
*/
class COH_EXPORT PeerProtocol
    : public class_spec<PeerProtocol,
        extends<AbstractPofProtocol> >
    {
    friend class factory<PeerProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        PeerProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        PeerProtocol(const PeerProtocol&);


    // ----- Protocol interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getCurrentVersion() const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getSupportedVersion() const;


    // ----- internal methods -----------------------------------------------

    public:

        static String::View getProtocolName();

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessageFactory::Handle instantiateMessageFactory(
                int32_t nVersion) const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return the singleton PeerProtocol instance.
        *
        * @return the singleton
        */
        static Handle getInstance();
    };

COH_CLOSE_NAMESPACE5

#endif // COH_PEER_PROTOCOL_HPP
