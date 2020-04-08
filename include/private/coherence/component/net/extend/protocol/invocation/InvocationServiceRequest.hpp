/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INVOCATION_SERVICE_REQUEST_HPP
#define COH_INVOCATION_SERVICE_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)

using coherence::component::net::extend::AbstractPofRequest;


/**
* Base class for all InvocationService Protocol Request messages.
*
* @author jh  2008.02.15
*/
class COH_EXPORT InvocationServiceRequest
    : public abstract_spec<InvocationServiceRequest,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        InvocationServiceRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        InvocationServiceRequest(const InvocationServiceRequest&);
    };

COH_CLOSE_NAMESPACE6

#endif // COH_INVOCATION_SERVICE_REQUEST_HPP
