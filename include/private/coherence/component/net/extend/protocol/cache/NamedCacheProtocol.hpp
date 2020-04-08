/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMED_CACHE_PROTOCOL_HPP
#define COH_NAMED_CACHE_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofMessageFactory;
using coherence::component::net::extend::AbstractPofProtocol;


/**
* The NamedCache Protocol is used to manipulate a remote NamedCache running
* within a Coherence cluster.
*
* @author jh  2008.02.18
*/
class COH_EXPORT NamedCacheProtocol
    : public class_spec<NamedCacheProtocol,
        extends<AbstractPofProtocol> >
    {
    friend class factory<NamedCacheProtocol>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheProtocol instance.
        */
        NamedCacheProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        NamedCacheProtocol(const NamedCacheProtocol&);


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

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessageFactory::Handle instantiateMessageFactory(
                int32_t nVersion) const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Return the singleton NamedCacheProtocol instance.
        *
        * @return the singleton
        */
        static Handle getInstance();
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAMED_CACHE_PROTOCOL_HPP
