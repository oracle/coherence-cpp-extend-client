/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_SERVICE_FACTORY_HPP
#define COH_CACHE_SERVICE_FACTORY_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)

using coherence::component::net::extend::AbstractPofMessageFactory;


/**
* MessageFactory for version 1 of the CacheService Protocol.
*
* @author jh  2008.02.18
*/
class COH_EXPORT CacheServiceFactory
    : public class_spec<CacheServiceFactory,
        extends<AbstractPofMessageFactory> >
    {
    friend class factory<CacheServiceFactory>;
    
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new CacheServiceFactory instance.
        */
        CacheServiceFactory();

    private:
        /**
        * Blocked copy constructor.
        */
        CacheServiceFactory(const CacheServiceFactory&);


    // ----- MessageFactory -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getVersion() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual AbstractPofMessage::Handle instantiateMessage(int32_t nType) const;
    };

COH_CLOSE_NAMESPACE7

#endif // COH_CACHE_SERVICE_FACTORY_HPP
