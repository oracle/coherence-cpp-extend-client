/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/ContainsKeyRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

ContainsKeyRequest::ContainsKeyRequest()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t ContainsKeyRequest::getTypeId() const
    {
    return type_id;
    }


// ----- internal methods ---------------------------------------------------

void ContainsKeyRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

COH_CLOSE_NAMESPACE6
