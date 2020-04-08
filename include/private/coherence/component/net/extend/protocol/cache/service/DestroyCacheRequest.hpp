/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DESTROY_CACHE_REQUEST_HPP
#define COH_DESTROY_CACHE_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceRequest.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)

using coherence::component::net::extend::AbstractPofResponse;


/**
* CacheService::destroyCache(NamedCache::View vMap) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT DestroyCacheRequest
    : public class_spec<DestroyCacheRequest,
        extends<CacheServiceRequest> >
    {
    friend class factory<DestroyCacheRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new DestroyCacheRequest instance.
        */
        DestroyCacheRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        DestroyCacheRequest(const DestroyCacheRequest&);


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 2;
    };

COH_CLOSE_NAMESPACE7

#endif // COH_DESTROY_CACHE_REQUEST_HPP
