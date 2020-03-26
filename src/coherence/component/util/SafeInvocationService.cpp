/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
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
    return ensureRunningInvocationServiceInternal(true)->query(
            hTask, vSetMembers);
    }


// ----- SafeInvocationService interface ------------------------------------

InvocationService::Handle
        SafeInvocationService::ensureRunningInvocationService(bool fDrain)
    {
    return ensureRunningInvocationServiceInternal(fDrain);
    }

InvocationService::View SafeInvocationService::ensureRunningInvocationService(
        bool fDrain) const
    {
    return ensureRunningInvocationServiceInternal(fDrain);
    }


// ----- property getters/settters ------------------------------------------

InvocationService::View
        SafeInvocationService::getRunningInvocationService() const
    {
    return ensureRunningInvocationServiceInternal(true);
    }


// ----- helper methods -----------------------------------------------------

InvocationService::Handle
        SafeInvocationService::ensureRunningInvocationServiceInternal(
        bool fDrain) const
    {
    return cast<InvocationService::Handle>(ensureRunningServiceInternal(fDrain));
    }

COH_CLOSE_NAMESPACE3
