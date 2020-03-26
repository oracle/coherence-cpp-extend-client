/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAMED_CACHE_REQUEST_HPP
#define COH_NAMED_CACHE_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofRequest;


/**
* Base class for all NamedCache Protocol Request messages.
*
* @author jh  2008.02.18
*/
class COH_EXPORT NamedCacheRequest
    : public abstract_spec<NamedCacheRequest,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        NamedCacheRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        NamedCacheRequest(const NamedCacheRequest&);
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAMED_CACHE_REQUEST_HPP
