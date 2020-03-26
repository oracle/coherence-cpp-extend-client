/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NAME_SERVICE_REQUEST_HPP
#define COH_NAME_SERVICE_REQUEST_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

using coherence::component::net::extend::AbstractPofRequest;

/**
* Base class for all NameService Protocol Request messages.
*
* @author Wei Lin  2012.06.15
*
* @since Coherence 12.1.2
*/
class COH_EXPORT NameServiceRequest
    : public abstract_spec<NameServiceRequest,
        extends<AbstractPofRequest> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        NameServiceRequest();

    private:
        /**
        * Blocked copy constructor.
        */
        NameServiceRequest(const NameServiceRequest&);
    };

COH_CLOSE_NAMESPACE6

#endif // COH_NAME_SERVICE_REQUEST_HPP
