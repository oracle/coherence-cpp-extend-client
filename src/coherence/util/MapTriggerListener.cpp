/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapTriggerListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

MapTriggerListener::MapTriggerListener(MapTrigger::Handle hTrigger)
        : f_hTrigger(self(), hTrigger)
    {
    COH_ENSURE_PARAM(hTrigger);
    }


// ----- MultiplexingMapListener interface ----------------------------------

void MapTriggerListener::onMapEvent(MapEvent::View /* vEvent */)
    {
    COH_THROW (IllegalStateException::create(
            "MapTriggerListener may not be used as a generic MapListener"));
    }


// ----- accessors ----------------------------------------------------------

MapTrigger::View MapTriggerListener::getTrigger() const
    {
    return f_hTrigger;
    }

MapTrigger::Handle MapTriggerListener::getTrigger()
    {
    return f_hTrigger;
    }

COH_CLOSE_NAMESPACE2
