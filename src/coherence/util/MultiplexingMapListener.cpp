/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MultiplexingMapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- MapListener interface ----------------------------------------------

void MultiplexingMapListener::entryInserted(MapEvent::View vEvent)
    {
    onMapEvent(vEvent);
    }

void MultiplexingMapListener::entryUpdated(MapEvent::View vEvent)
    {
    onMapEvent(vEvent);
    }

void MultiplexingMapListener::entryDeleted(MapEvent::View vEvent)
    {
    onMapEvent(vEvent);
    }

COH_CLOSE_NAMESPACE2
