/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

NamedCacheResponse::NamedCacheResponse()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t NamedCacheResponse::getTypeId() const
    {
    return type_id;
    }

void NamedCacheResponse::run()
    {
    // no-op
    }

COH_CLOSE_NAMESPACE6
