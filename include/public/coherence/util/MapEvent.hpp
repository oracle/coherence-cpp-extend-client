/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_EVENT_HPP
#define COH_MAP_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/util/EventObject.hpp"
#include "coherence/util/Listeners.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

class MapListener;
class ObservableMap;

/**
* An event which indicates that the content of a map has changed:
* <ul>
* <li>an entry has been added</li>
* <li>an entry has been removed</li>
* <li>an entry has been changed</li>
* </ul>
* A MapEvent object is sent as an argument to the MapListener interface
* methods.  NULL values may be provided for the old and the new values.
*
* @author js  2008.06.03
*/
class COH_EXPORT MapEvent
    : public class_spec<MapEvent,
        extends<EventObject> >
    {
    friend class factory<MapEvent>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a MapEvent.
        *
        * @param hMap       the map on which the Event initially occurred
        * @param nId        the events id (entry_inserted | entry_updated |
        *                   entry_deleted)
        * @param vKey       the key into the map
        * @param vValueOld  the old value (for update and delete events)
        * @param vValueNew  the new value (for insert and update events)
        */
        MapEvent(TypedHandle<ObservableMap> hMap, int32_t nId, Object::View vKey,
                Object::View vValueOld, Object::View vValueNew);

    private:
        /**
        * Blocked copy constructor.
        */
        MapEvent(const MapEvent&);

		
    // ----- MapEvent interface ---------------------------------------------

    public:
        /**
        * Return an ObservableMap object on which this event has actually
        * occured.
        *
        * @return an ObservableMap object
        */
        virtual TypedHandle<ObservableMap> getMap() const;

        /**
        * Return this event's id. The event id is one of the entry_*
        * enumerated constants.
        *
        * @return an id
        */
        virtual int32_t getId() const;

        /**
        * Return a key associated with this event.
        *
        * @return a key
        */
        virtual Object::View getKey() const;

        /**
        * Return an old value associated with this event.
        * <p>
        * The old value represents a value deleted from or updated in a map.
        * It is always NULL for "insert" notifications.
        *
        * @return an old value
        */
        virtual Object::View getOldValue() const;

        /**
        * Return a new value associated with this event.
        * <p>
        * The new value represents a new value inserted into or updated in
        * a map. It is always NULL for "delete" notifications.
        *
        * @return a new value
        */
        virtual Object::View getNewValue() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Dispatch this event to the specified listeners collection.
        * <p>
        * This call is equivalent to
        * <pre>
        *   dispatch(listeners, true);
        * </pre>
        *
        * @param vListeners the listeners collection
        *
        * @throws ClassCastException if any of the targets is not
        *         an instance of MapListener interface
        */
        virtual void dispatch(Listeners::View vListeners) const;

        /**
        * Dispatch this event to the specified listeners collection.
        *
        * @param vListeners the listeners collection
        * @param fStrict    if true then any RuntimeException thrown by event
        *                   handlers stops all further event processing and
        *                   the exception is re-thrown; if false then all
        *                   exceptions are logged and the process continues
        *
        * @throws ClassCastException if any of the targets is not
        *         an instance of MapListener interface
        */
        virtual void dispatch(Listeners::View vListeners,
                bool fStrict) const;

        /**
        * Dispatch this event to the specified MapListener.
        *
        * @param hListener  the listener
        */
        virtual void dispatch(TypedHandle<MapListener> hListener) const;

        /**
        * Convert an event ID into a human-readable string.
        *
        * @param nId  an event ID, one of the entry_* enumerated values
        *
        * @return a corresponding human-readable string, for example
        *         "inserted"
        */
        static String::View getDescription(int32_t nId);

        using Describable::getDescription;

    protected:
        /**
        * Return true if the provided MapListener should receive this
        * event.
        *
        * @param vListener  the MapListener to dispatch this event to
        *
        * @return true if the provided MapListener should receive the event
        * @since 12.2.1.3.2
        */
        virtual bool shouldDispatch(TypedHandle<const MapListener> vListener) const;

    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * This event indicates that an entry has been added to the map.
        */
        static const int32_t entry_inserted = 1;

        /**
        * This event indicates that an entry has been updated in the map.
        */
        static const int32_t entry_updated  = 2;

        /**
        * This event indicates that an entry has been removed from the map.
        */
        static const int32_t entry_deleted  = 3;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The event's id.
        */
        int32_t m_nId;

        /**
        * A key.  This is mutable because subclasses may lazily initialize
        * this value from an accessor.
        */
        mutable MemberView<Object> m_voKey;

        /**
        * A previous value.  May be NULL if not known.  This is mutable
        * because subclasses may lazily initialize this value from an
        * accessor.
        */
        mutable MemberView<Object> m_voValueOld;

        /**
        * A new value.  May be NULL if not known.  This is mutable because
        * subclasses may lazily initialize this value from an accessor.
        */
        mutable MemberView<Object> m_voValueNew;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_EVENT_HPP

