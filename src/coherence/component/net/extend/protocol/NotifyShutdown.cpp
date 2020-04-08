/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/NotifyShutdown.hpp"

#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/Service.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;
using coherence::component::util::Service;


// ----- constructors -------------------------------------------------------

NotifyShutdown::NotifyShutdown()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t NotifyShutdown::getTypeId() const
    {
    return type_id;
    }

void NotifyShutdown::run()
    {
    Peer::Handle hModule = cast<Peer::Handle>(getChannel()->getReceiver());

    hModule->setServiceState(Service::service_stopping);
    hModule->stop();
    }


COH_CLOSE_NAMESPACE5
