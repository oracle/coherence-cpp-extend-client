/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceResponse.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)


// ----- constructors -------------------------------------------------------

CacheServiceResponse::CacheServiceResponse()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t CacheServiceResponse::getTypeId() const
    {
    return type_id;
    }

void CacheServiceResponse::run()
    {
    // no-op
    }

COH_CLOSE_NAMESPACE7
