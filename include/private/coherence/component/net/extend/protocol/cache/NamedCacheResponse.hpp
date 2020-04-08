/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMED_CACHE_RESPONSE_HPP
#define COH_NAMED_CACHE_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Generic Response class used for basic NamedCache Protocol Response
* messages.
*
* @author jh  2008.02.18
*/
class COH_EXPORT NamedCacheResponse
    : public class_spec<NamedCacheResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<NamedCacheResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new NamedCacheResponse instance.
        */
        NamedCacheResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        NamedCacheResponse(const NamedCacheResponse&);


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


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 0;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAMED_CACHE_RESPONSE_HPP
