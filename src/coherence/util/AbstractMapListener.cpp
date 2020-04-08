/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractMapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- MapListener interface ----------------------------------------------

void AbstractMapListener::entryInserted(MapEvent::View /*vEvent*/)
    {
    }

void AbstractMapListener::entryUpdated(MapEvent::View /*vEvent*/)
    {
    }

void AbstractMapListener::entryDeleted(MapEvent::View /*vEvent*/)
    {
    }

COH_CLOSE_NAMESPACE2
