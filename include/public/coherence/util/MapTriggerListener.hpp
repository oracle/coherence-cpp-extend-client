/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_TRIGGER_LISTENER_HPP
#define COH_MAP_TRIGGER_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapTrigger.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* MapTriggerListener is a special purpose MapListener implementation that is
* used to register a MapTrigger on a corresponding ObservableMap.
*
* <b>Note:</b> Currently, the MapTriggerListener can only be registered
* with partitioned caches and only "globally" (without specifying any filter
* or key), using the ObservableMap::addMapListener() method.
*
* @author djl  2008.06.23
*/
class COH_EXPORT MapTriggerListener
    : public class_spec<MapTriggerListener,
        extends<MultiplexingMapListener> >
    {
    friend class factory<MapTriggerListener>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a MapTriggerListener that can be used to register the
        * specified MapTrigger.
        *
        * @param hTrigger  the MapTrigger
        */
        MapTriggerListener(MapTrigger::Handle hTrigger);


    // ----- MultiplexingMapListener interface ------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onMapEvent(MapEvent::View vEvent);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the MapTrigger agent represented by this TriggerListener.
        *
        * @return the MapTrigger agent represented by this TriggerListener
        */
        virtual MapTrigger::View getTrigger() const;

        /**
        * Obtain the MapTrigger agent represented by this TriggerListener.
        *
        * @return the MapTrigger agent represented by this TriggerListener
        */
        virtual MapTrigger::Handle getTrigger();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying MapTrigger.
        */
        FinalHandle<MapTrigger> f_hTrigger;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_TRIGGER_LISTENER_HPP
