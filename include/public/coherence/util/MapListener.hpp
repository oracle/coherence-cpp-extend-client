/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_LISTENER_HPP
#define COH_MAP_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/EventListener.hpp"
#include "coherence/util/MapEvent.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The listener interface for receiving MapEvents.
*
* @author js 2008.06.03
*/
class COH_EXPORT MapListener
    : public interface_spec<MapListener,
        implements<EventListener> >
    {
    // ----- MapListener interface ------------------------------------------

    public:
        /**
        * Invoked when a map entry has been inserted.
        *
        * @param vEvent  the MapEvent carrying the insert information
        */
        virtual void entryInserted(MapEvent::View vEvent) = 0;

        /**
        * Invoked when a map entry has been updated.
        *
        * @param vEvent  the MapEvent carrying the update information
        */
        virtual void entryUpdated(MapEvent::View vEvent) = 0;

        /**
        * Invoked when a map entry has been removed.
        *
        * @param vEvent  the MapEvent carrying the delete information
        */
        virtual void entryDeleted(MapEvent::View vEvent) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_LISTENER_HPP

