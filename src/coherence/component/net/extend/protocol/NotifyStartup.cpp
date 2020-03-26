/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/NotifyStartup.hpp"

#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/component/util/Service.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;
using coherence::component::util::Service;


// ----- constructors -------------------------------------------------------

NotifyStartup::NotifyStartup()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t NotifyStartup::getTypeId() const
    {
    return type_id;
    }

void NotifyStartup::run()
    {
    Peer::Handle hModule = cast<Peer::Handle>(getChannel()->getReceiver());

    hModule->setServiceState(Service::service_started);
    }


COH_CLOSE_NAMESPACE5
