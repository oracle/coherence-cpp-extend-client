/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBSERVABLE_MAP_HPP
#define COH_OBSERVABLE_MAP_HPP

#include "coherence/lang.ns"

#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* ObservableMap interface represents an object with a model being
* a Map that allows for pluggable notifications for occuring changes.
* 
* This is primarily intended for maps that have automatic pruning and
* purging strategies or maps that are asynchronously modified by different
* threads.
* 
* Starting from Coherence 2.3 it supports optimizations that optionally
* do not require the map values to be included in the map events,
* allowing a "lite" event to be delivered and saving memory, processing
* and bandwidth for distributed applications.
*
* @see NamedCache
* @see LocalCache
* @see ObservableHashMap
*
* @author js  2008.06.04
*/
class COH_EXPORT ObservableMap
    : public interface_spec<ObservableMap,
        implements<Map> >
    {
    // ----- ObservableMap interface ----------------------------------------

    public:
        /**
        * Add a map listener for a specific key.
        * 
        * The listeners will receive MapEvent objects, but if fLite is passed
        * as true, they <i>might</i> not contain the OldValue and NewValue
        * properties.
        * 
        * To unregister the MapListener, use the 
        * removeKeyListener(MapListener, Object) method.
        *
        * @param hListener  the MapEvent listener to add
        * @param vKey       the key that identifies the entry for which to
        *                   raise events
        * @param fLite      true to indicate that the MapEvent objects do not
        *                   have to include the OldValue and NewValue
        *                   property values in order to allow optimizations
        */
        virtual void addKeyListener(MapListener::Handle hListener,
                Object::View vKey, bool fLite) = 0;

        /**
        * Remove a map listener that previously signed up for events about a
        * specific key.
        *
        * @param hListener  the listener to remove
        * @param vKey       the key that identifies the entry for which to
        *                   raise events
        */
        virtual void removeKeyListener(MapListener::Handle hListener,
                Object::View vKey) = 0;

        /**
        * Add a standard map listener that will receive all events (inserts,
        * updates, deletes) that occur against the map, with the key, old-value 
        * and new-value included. This has the same result as the following call:
        * @code
        *   addFilterListener(hListener, (Filter::View) NULL, false);
        * @endcode
        *
        * @param hListener the MapEvent listener to add
        *
        * @since Coherence 12.1.3
        */
        virtual void addMapListener(MapListener::Handle hListener) = 0;

        /**
        * Remove a standard map listener that previously signed up for all 
        * events. This has the same result as the following call:
        * @code
        *   removeFilterListener(hListener, (Filter::View) NULL);
        * @endcode
        *
        * @param hListener the listener to remove
        *
        * @since Coherence 12.1.3
        */
        virtual void removeMapListener(MapListener::Handle hListener) = 0;

        /**
        * Add a map listener that receives events based on a filter
        * evaluation.
        * 
        * The listeners will receive MapEvent objects, but if fLite is passed
        * as true, they <i>might</i> not contain the OldValue and NewValue
        * properties.
        * 
        * To unregister the MapListener, use the
        * removeFilterListener(MapListener, Filter) method.
        *
        * @param hListener  the MapEvent listener to add
        * @param vFilter    an optional filter that will be passed MapEvent
        *                   objects to select from; a MapEvent will be
        *                   delivered to the listener only if the filter
        *                   evaluates to true for that MapEvent (see
        *                   {com.tangosol.util.filter.MapEventFilter}); NULL
        *                   is equivalent to a filter that alway returns true
        * @param fLite      an optional parameter where true indicates that
        *                   the {MapEvent} objects do not have to include the
        *                   OldValue and NewValue property values in order to
        *                   allow optimizations; default value is false
        *
        */
        virtual void addFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL, bool fLite = false) = 0;

        /**
        * Remove a map listener that previously signed up for events based on
        * a filter evaluation.
        *
        * @param hListener the listener to remove
        * @param vFilter   the optional filter that was passed
        *                  into the corresponding addFilterListener() call;
        *                  defaults to NULL
        */
        virtual void removeFilterListener(MapListener::Handle hListener,
                Filter::View vFilter = NULL) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OBSERVABLE_MAP_HPP

