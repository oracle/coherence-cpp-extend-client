/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
 #include "private/coherence/component/util/SafeInvocationService.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)


// ----- constructors/destructor  -------------------------------------------

SafeInvocationService::SafeInvocationService()
    {
    }


// ----- InvocationService interface ----------------------------------------

Map::View SafeInvocationService::query(Invocable::Handle hTask,
        Set::View vSetMembers)
    {
    return ensureRunningInvocationServiceInternal()->query(
            hTask, vSetMembers);
    }


// ----- SafeInvocationService interface ------------------------------------

InvocationService::Handle
        SafeInvocationService::ensureRunningInvocationService()
    {
    return ensureRunningInvocationServiceInternal();
    }

InvocationService::View SafeInvocationService::ensureRunningInvocationService() const
    {
    return ensureRunningInvocationServiceInternal();
    }


// ----- property getters/settters ------------------------------------------

InvocationService::View
        SafeInvocationService::getRunningInvocationService() const
    {
    return ensureRunningInvocationServiceInternal();
    }


// ----- helper methods -----------------------------------------------------

InvocationService::Handle
        SafeInvocationService::ensureRunningInvocationServiceInternal() const
    {
    return cast<InvocationService::Handle>(ensureRunningServiceInternal());
    }

COH_CLOSE_NAMESPACE3
