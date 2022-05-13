/*
 * Copyright (c) 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/NoStorageMembers.hpp"

#include "private/coherence/component/util/RunnableCacheEvent.hpp"

#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"

#include "private/coherence/net/messaging/Channel.hpp"

#include "coherence/util/Event.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/MapEvent.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::util::RunnableCacheEvent;
using coherence::net::messaging::Channel;
using coherence::util::Event;
using coherence::util::Listeners;
using coherence::util::MapEvent;

// ----- constructors -------------------------------------------------------

NoStorageMembers::NoStorageMembers()
    {
    }

// ----- Message interface --------------------------------------------------

int32_t NoStorageMembers::getTypeId() const
    {
    return type_id;
    }

// ----- internal methods ---------------------------------------------------

void NoStorageMembers::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    Channel::Handle          hChannel   = getChannel();
    RemoteNamedCache::Handle hCache     = cast<RemoteNamedCache::Handle>(hChannel->getReceiver());
    Listeners::Handle        hListeners = hCache->getDeactivationListeners();

    if (!hListeners->isEmpty())
        {
        MapEvent::Handle hEvtDeleted = MapEvent::create(hCache, MapEvent::entry_deleted,
                (Object::View) NULL, (Object::View) NULL, (Object::View) NULL);
        // dispatch the event to the listeners, which are all synchronous (hence the null Queue)
        RunnableCacheEvent::dispatchSafe(hEvtDeleted, hListeners, NULL /*Queue*/);
        }
    }

COH_CLOSE_NAMESPACE6
