/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MULTIPLEXING_MAP_LISTENER_HPP
#define COH_MULTIPLEXING_MAP_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A base class that simplifies the implementation of a MapListener by
* multiplexing all events into a single listener method.
*
* @author djl 2008.06.23
*/
class COH_EXPORT MultiplexingMapListener
    : public abstract_spec<MultiplexingMapListener,
        extends<Object>,
        implements<MapListener> >
    {
    // ----- MultiplexingMapListener interface ------------------------------

    public:
        /**
        * Invoked when a map entry has been inserted, updated or deleted. To
        * determine what action has occurred, use MapEvent::getId().
        *
        * @param vEvent the MapEvent carrying the insert, update or delete
        *                information
        */
        virtual void onMapEvent(MapEvent::View vEvent) = 0;


    // ----- MapListener interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void entryInserted(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryUpdated(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryDeleted(MapEvent::View vEvent);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MULTIPLEXING_MAP_LISTENER_HPP
