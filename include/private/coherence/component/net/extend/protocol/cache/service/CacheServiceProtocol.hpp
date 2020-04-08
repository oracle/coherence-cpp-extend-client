/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_SERVICE_PROTOCOL_HPP
#define COH_CACHE_SERVICE_PROTOCOL_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)

using coherence::component::net::extend::AbstractPofMessageFactory;
using coherence::component::net::extend::AbstractPofProtocol;


/**
* The CacheService Protocol is used to obtain, release, and destroy remote
* references to a NamedCache running within a Coherence cluster.
*
* @author jh  2008.02.18
*/
class COH_EXPORT CacheServiceProtocol
    : public class_spec<CacheServiceProtocol,
        extends<AbstractPofProtocol> >
    {
    friend class factory<CacheServiceProtocol>;
    
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new CacheServiceProtocol instance.
        */
        CacheServiceProtocol();

    private:
        /**
        * Blocked copy constructor.
        */
        CacheServiceProtocol(const CacheServiceProtocol&);


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
        * Return the singleton CacheServiceProtocol instance.
        *
        * @return the singleton
        */
        static Handle getInstance();
    };

COH_CLOSE_NAMESPACE7

#endif // COH_CACHE_SERVICE_PROTOCOL_HPP
