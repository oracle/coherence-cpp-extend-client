/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_EVENT_HPP
#define COH_CACHE_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,net,cache)

using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::ObservableMap;


/**
* An extension of the MapEvent which allows to differentiate between client
* driven (natural) events and cache internal (synthetic) events.
*
* Consider a client code calling a remove() method for a cache. Quite
* naturally it causes a corresponding entry_deleted event. However, the same
* event could be as well caused by the client code calling put() forcing an
* entry eviction.  Alternatively, the put() method called by client code
* naturally causes either entry_inserted or entry_updated event. However, the
* same event could be as well caused by a client call to a get() method that
* in turn forces an entry insertion by a cache loader.
*
* @author js  2008.06.06
*/
class COH_EXPORT CacheEvent
    : public class_spec<CacheEvent,
        extends<MapEvent> >
    {
    friend class factory<CacheEvent>;


    // ----- TransformationState enum ---------------------------------------

    public:
        /**
        * Transformation state constants.
        * TransformationState describes how a CacheEvent has been or should be
        * transformed.
        */
        enum TransformationState
            {
            /**
            * Value used to indicate that an event is non-transformable and should
            * not be passed to any transformer-based listeners.
            */
            non_transformable,

            /**
            * Value used to indicate that an event is transformable and could be
            * passed to transformer-based listeners.
            */
            transformable,

            /**
            * Value used to indicate that an event has been transformed, and should
            * only be passed to transformer-based listeners.
            */
            transformed
            };


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new CacheEvent.
        *
        * @param hMap             the map on which the Event initially
        * @param nId              occurred the events id (entry_inserted |
        *                         entry_updated | entry_deleted)
        * @param voKey            the key into the map
        * @param voValueOld       the old value (for update and delete events)
        * @param voValueNew       the new value (for insert and update events)
        * @param fSynthetic       true iff the event is caused by internal
        *                         cache processing such as eviction or loading
        */
        CacheEvent(ObservableMap::Handle hMap, int32_t nId,
                Object::View voKey, Object::View voValueOld,
                Object::View voValueNew, bool fSynthetic);

        /**
        * Create a new CacheEvent.
        *
        * @param hMap             the map on which the Event initially
        * @param nId              occurred the events id (entry_inserted |
        *                         entry_updated | entry_deleted)
        * @param voKey            the key into the map
        * @param voValueOld       the old value (for update and delete events)
        * @param voValueNew       the new value (for insert and update events)
        * @param fSynthetic       true iff the event is caused by internal
        *                         cache processing such as eviction or loading
        * @param fPriming         a flag indicating whether or not the event
        *                         is a priming event
        */
        CacheEvent(ObservableMap::Handle hMap, int32_t nId,
                Object::View voKey, Object::View voValueOld,
                Object::View voValueNew, bool fSynthetic, bool fPriming);

        /**
        * Create a new CacheEvent.
        *
        * @param hMap             the map on which the Event initially
        * @param nId              occurred the events id (entry_inserted |
        *                         entry_updated | entry_deleted)
        * @param voKey            the key into the map
        * @param voValueOld       the old value (for update and delete events)
        * @param voValueNew       the new value (for insert and update events)
        * @param fSynthetic       true iff the event is caused by internal
        *                         cache processing such as eviction or loading
        * @param nTransformState  the TransformationState describing how
        *                         this event has been or should be transformed
        * @since Coherence 3.7.1.9
        */
        CacheEvent(ObservableMap::Handle hMap, int32_t nId,
                Object::View voKey, Object::View voValueOld,
                Object::View voValueNew, bool fSynthetic, 
                TransformationState nTransformState);

        /**
        * Create a new CacheEvent.
        *
        * @param hMap             the map on which the Event initially
        * @param nId              occurred the events id (entry_inserted |
        *                         entry_updated | entry_deleted)
        * @param voKey            the key into the map
        * @param voValueOld       the old value (for update and delete events)
        * @param voValueNew       the new value (for insert and update events)
        * @param fSynthetic       true iff the event is caused by internal
        *                         cache processing such as eviction or loading
        * @param nTransformState  the TransformationState describing how
        *                         this event has been or should be transformed
        * @param fPriming         a flag indicating whether or not the event
        *                         is a priming event
        * @since 12.2.1.3.2
        */
        CacheEvent(ObservableMap::Handle hMap, int32_t nId,
                Object::View voKey, Object::View voValueOld,
                Object::View voValueNew, bool fSynthetic,
                TransformationState nTransformState, bool fPriming);

        /**
        * Constructs a new CacheEvent.
        *
        * @param map             the ObservableMap object that fired the event
        * @param nId             this event's id, one of (entry_inserted |
           *                         entry_updated | entry_deleted)
        * @param oKey            the key into the map
        * @param oValueOld       the old value (for update and delete events)
        * @param oValueNew       the new value (for insert and update events)
        * @param fSynthetic      true iff the event is caused by the cache
        *                        internal processing such as eviction or loading
        * @param transformState  the TransformationState describing how
        *                        this event has been or should be transformed
        * @param fPriming        a flag indicating whether or not the event
        *                        is a priming event
        * @param fExpired        true iff the event results from an eviction
        *                        due to time
        * @since 22.06
        */
        CacheEvent(ObservableMap::Handle hMap, int32_t nId,
                Object::View voKey, Object::View voValueOld,
                Object::View voValueNew, bool fSynthetic,
                TransformationState nTransformState, bool fPriming,
                bool fExpired);

    private:
        /**
        * Blocked copy constructor.
        */
        CacheEvent(const CacheEvent&);

		
    // ----- CacheEvent interface -------------------------------------------

    public:
        /**
        * Return true iff this event is caused by internal cache processing
        * such as eviction or loading.
        *
        * @return true iff this event is caused by internal cache processing
        */
        virtual bool isSynthetic() const;

        /**
        * Return transformation state of this event.
        * Non-transformable events will not be delivered to MapEventTransformer
        * MapEventTransformer listeners.
        *
        * @return the TransformationState for this event
        */
        virtual TransformationState getTransformState() const;

        /**
        * Return true iff this event is caused by a priming listener registration.
        *
        * @return true iff this event is caused by a priming listener registration
        * @since 12.2.1.3.2
        */
        virtual bool isPriming() const;

        /**
        * Return true iff this event is caused by an entry eviction due to time limit reached.
        * In this case the event will also be synthetic.
        *
        * @return true iff this event results from a timed eviction
        * @since 22.06
        */
        virtual bool isExpired() const;

    protected:
        /**
        * {@inheritDoc}
        */
        virtual bool shouldDispatch(TypedHandle<const MapListener> vListener) const;


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Event cause flag.
        */
        bool m_fSynthetic;

        /**
        * The TransformationState for this event.
        */
        TransformationState m_nTransformState;

        /**
        * ﻿Flag indicating whether or not the event is a priming event (NearCache).
        * @since 12.2.1.3.2
        */
        bool m_fPriming;

        /**
        * Flag indicating whether the deletion event is a result of time expiration.
        * @since 22.06
        */
       bool m_fExpired;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_EVENT_HPP

