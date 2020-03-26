/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/AbstractInvocable.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

// ----- Invocable interface ------------------------------------------------

void AbstractInvocable::init(InvocationService::Handle /*hService*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }

void AbstractInvocable::run()
    {
    COH_THROW (UnsupportedOperationException::create());
    }

Object::Holder AbstractInvocable::getResult() const
    {
    COH_THROW (UnsupportedOperationException::create());
    }

// ----- PriorityTask interface -----------------------------------------

int32_t AbstractInvocable::getSchedulingPriority() const
    {
    return schedule_standard;
    }

int64_t AbstractInvocable::getExecutionTimeoutMillis() const
    {
    return timeout_default;
    }

void AbstractInvocable::runCanceled(bool /* fAbandoned */)
    {
    }

int64_t AbstractInvocable::getRequestTimeoutMillis() const
    {
    return timeout_default;
    }

COH_CLOSE_NAMESPACE2
