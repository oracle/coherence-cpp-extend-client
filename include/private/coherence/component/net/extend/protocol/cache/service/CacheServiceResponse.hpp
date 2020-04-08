/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_SERVICE_RESPONSE_HPP
#define COH_CACHE_SERVICE_RESPONSE_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)

using coherence::component::net::extend::AbstractPofResponse;


/**
* Generic Response class used for basic InvocationService Protocol Response
* messages.
*
* @author jh  2008.02.18
*/
class COH_EXPORT CacheServiceResponse
    : public class_spec<CacheServiceResponse,
        extends<AbstractPofResponse> >
    {
    friend class factory<CacheServiceResponse>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new CacheServiceResponse instance.
        *
        * @return the new CacheServiceResponse
        */
        CacheServiceResponse();

    private:
        /**
        * Blocked copy constructor.
        */
        CacheServiceResponse(const CacheServiceResponse&);


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

COH_CLOSE_NAMESPACE7

#endif // COH_CACHE_SERVICE_RESPONSE_HPP
