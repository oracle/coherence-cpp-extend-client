/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RUNNABLE_CACHE_EVENT_HPP
#define COH_RUNNABLE_CACHE_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/util/Listeners.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/Queue.hpp"

#include "coherence/util/MapListenerSupport.hpp"



COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::util::Listeners;
using coherence::util::MapEvent;
using coherence::util::MapListener;
using coherence::util::MapListenerSupport;
using coherence::util::Queue;


/**
* A runnable CacheEvent.
*
* @author js  2008.07.20
*/
class COH_EXPORT RunnableCacheEvent
    : public class_spec<RunnableCacheEvent,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<RunnableCacheEvent>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new RunnableCacheEvent instance.
        *
        * @param hMapEvent   event being wrapped
        * @param vListeners  listeners to which the event will be dispatched
        */
        RunnableCacheEvent(MapEvent::Handle hMapEvent,
                Listeners::View vListeners);

        /**
        * Create a new RunnableCacheEvent instance.
        *
        * @param hMapEvent  event being wrapped
        * @param hListener  the listener which will receive the event
        */
        RunnableCacheEvent(MapEvent::Handle hMapEvent,
                MapListener::Handle hListener);

        /**
        * Create a new RunnableCacheEvent instance.
        *
        * @param hMapEvent         event being wrapped
        * @param hListenerSupport  used to fire the event
        */
        RunnableCacheEvent(MapEvent::Handle hMapEvent,
                MapListenerSupport::Handle hListenerSupport);


    // ----- Runnable interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- static functions -----------------------------------------------

    public:
        /**
        * Dispatch the specified MapEvent to all Synchronous listeners and
        * add to the specified Queue for deferred execution for standard
        * ones.
        */
        static void dispatchSafe(MapEvent::Handle hEvent,
                Listeners::View vListeners, Queue::Handle hQueue);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Get the optional Listeners object containing MapListener objects.
        *
        * @return the optional Listeners
        */
        virtual Listeners::View getListeners() const;

        /**
        * Get the optional MapListenerSupport object which is used to fire
        * the event.
        *
        * @return the optional MapListenerSupport object
        */
        virtual MapListenerSupport::Handle getListenerSupport();

        /**
        * Get the optional MapListenerSupport object which is used to fire
        * the event.
        *
        * @return the optional MapListenerSupport object
        */
        virtual MapListenerSupport::View getListenerSupport() const;

        /**
        * Get the actual MapEvent to fire.
        *
        * @return the wrapped event
        */
        virtual MapEvent::Handle getMapEvent();

        /**
        * Get the actual MapEvent to fire.
        *
        * @return the wrapped event
        */
        virtual MapEvent::View getMapEvent() const;

        /**
        * Get the optional MapListener object which the event is dispatched
        * to.
        *
        * @return the optional MapListener
        */
        virtual MapListener::Handle getMapListener();

        /**
        * Get the optional MapListener object which the event is dispatched
        * to.
        *
        * @return the optional MapListener
        */
        virtual MapListener::View getMapListener() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Optional Listeners object containing MapListener objects.
        */
        FinalView<Listeners> f_vListeners;

        /**
        * Optional MapListenerSupport object which is used to fire the event.
        */
        FinalHandle<MapListenerSupport> f_hListenerSupport;

        /**
        * Actual MapEvent to fire.
        */
        FinalHandle<MapEvent> f_hMapEvent;

        /**
        * Optional MapListener object which the event is dispatched to.
        */
        FinalHandle<MapListener> f_hMapListener;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_RUNNABLE_CACHE_EVENT_HPP

