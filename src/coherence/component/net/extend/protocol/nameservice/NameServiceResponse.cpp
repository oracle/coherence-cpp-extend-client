/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

// ----- constructors -------------------------------------------------------

NameServiceResponse::NameServiceResponse()
    {
    }

// ----- Message interface --------------------------------------------------

int32_t NameServiceResponse::getTypeId() const
    {
    return type_id;
    }

void NameServiceResponse::run()
    {
    // no-op
    }

COH_CLOSE_NAMESPACE6
