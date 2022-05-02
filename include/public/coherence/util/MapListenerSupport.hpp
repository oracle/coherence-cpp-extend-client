/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_LISTENER_SUPPORT_HPP
#define COH_MAP_LISTENER_SUPPORT_HPP

#include "coherence/lang.ns"

#include "coherence/net/cache/CacheEvent.hpp"
#include "coherence/util/Converter.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"
#include "coherence/util/MultiplexingMapListener.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/SynchronousListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::net::cache::CacheEvent;


/**
* This class provides support for advanced MapListener functionality.
*
* @author js  2008.06.10
*/
class COH_EXPORT MapListenerSupport
    : public class_spec<MapListenerSupport>
    {
    friend class factory<MapListenerSupport>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new MapListenerSupport.
        */
        MapListenerSupport();


    // ----- MapListenerSupport interface -----------------------------------

    public:
        /**
        * Add a map listener that receives events based on a filter
        * evaluation.
        *
        * @param hListener  the listener to add
        * @param vFilter    a filter that will be passed MapEvent objects to
        *                   select from; a MapEvent will be delivered to the
        *                   listener only if the filter evaluates to true for
        *                   that MapEvent; NULL is equivalent to a filter
        *                   that alway returns true
        * @param fLite      true to indicate that the MapEvent objects do not
        *                   have to include the OldValue and NewValue
        *                   property values in order to allow optimizations
        */
        virtual void addListener(MapListener::Handle hListener,
                Filter::View vFilter, bool fLite);

        /**
        * Add a map listener for a specific key.
        *
        * @param hListener  the listener to add
        * @param vKey       the key that identifies the entry for which to
        *                   raise events
        * @param fLite      true to indicate that the MapEvent objects do not
        *                   have to include the OldValue and NewValue
        *                   property values in order to allow optimizations
        *
        */
        virtual void addListener(MapListener::Handle hListener,
                Object::View vKey, bool fLite);

        /**
        * Remove a map listener that previously signed up for events based on
        * a filter evaluation.
        *
        * @param vListener  the listener to remove
        * @param vFilter    a filter used to evaluate events
        */
        virtual void removeListener(MapListener::View vListener,
                Filter::View vFilter);

        /**
        * Remove a map listener that previously signed up for events about a
        * specific key.
        *
        * @param vListener  the listener to remove
        * @param vKey       the key that identifies the entry for which to
        *                   raise events
        */
        virtual void removeListener(MapListener::View vListener,
                Object::View vKey);

        /**
        * Remove all signed up listeners.
        */
        virtual void clear();

        /**
        * Checks whether or not this MapListenerSupport object contains
        * any listeners.
        *
        * @return true iff there are no listeners encapsulated by this
        *         MapListenerSupport object
        */
        virtual bool isEmpty() const;

        /**
        * Checks whether or not this MapListenerSupport object contains
        * any listeners for a given filter.
        *
        * @param vFilter  the filter
        *
        * @return true iff there are no listeners for the specified filter
        *              encapsulated by this MapListenerSupport object
        */
        virtual bool isEmpty(Filter::View vFilter) const;

        /**
        * Checks whether or not this MapListenerSupport object contains
        * any listeners for a given key.
        *
        * @param vKey  the key
        *
        * @return true iff there are no listeners for the specified filter
        *              encapsulated by this MapListenerSupport object
        */
        virtual bool isEmpty(Object::View vKey) const;

        /**
        * Checks whether or not this MapListenerSupport object contains
        * any standard (not lite) listeners for a given filter.
        *
        * @param vFilter  the filter
        *
        * @return true iff there are no standard listeners for the specified
        *              filter encapsulated by this MapListenerSupport object
        */
        virtual bool containsStandardListeners(
                Filter::View vFilter) const;

        /**
        * Checks whether or not this MapListenerSupport object contains
        * any standard (not lite) listeners for a given key.
        *
        * @param vKey  the key
        *
        * @return true iff there are no standard listeners for the specified
        *              filter encapsulated by this MapListenerSupport object
        */
        virtual bool containsStandardListeners(Object::View vKey) const;

        /**
        * Obtain a set of all filters that have assosiated global listeners.
        * <p>
        * <b>Note</b>: The returned value must be treated as an immutable.
        *
        * @return a set of all filters that have assosiated global listeners
        */
        virtual Set::View getFilterSet() const;

        /**
        * Obtain a set of all keys that have assosiated key listeners.
        * <p>
        * <b>Note</b>: The returned value must be treated as an immutable.
        *
        * @return a set of all keys that have assosiated key listeners
        */
        virtual Set::View getKeySet() const;

        /**
        * Obtain the Listeners object for a given filter.
        *
        * @param vFilter  the filter
        *
        * @return the Listeners object for the filter; NULL if none exists
        */
        virtual Listeners::View getListeners(Filter::View vFilter) const;

        /**
        * Obtain the Listeners object for a given key.
        *
        * @param vKey  the key
        *
        * @return the Listeners object for the key; NULL if none exists
        */
        virtual Listeners::View getListeners(Object::View vKey) const;

        /**
        * Collect all Listeners that should be notified for a given event.
        * <p>
        * <b>Note</b>: The returned value must be treated as an immutable.
        *
        * @param vEvent the MapEvent object
        *
        * @return the Listeners object containing the relevant listeners
        */
        virtual Listeners::View collectListeners(MapEvent::View vEvent);

        /**
        * Fire the specified map event.
        *
        * @param hEvent    the map event
        * @param fStrict   if true then any RuntimeException thrown by event
        *                  handlers stops all further event processing and
        *                  the exception is re-thrown; if false then all
        *                  exceptions are logged and the process continues
        */
        virtual void fireEvent(MapEvent::Handle hEvent, bool fStrict);

        /**
        * Convert the specified map event into another MapEvent that ensures
        * the lazy event data conversion using the specified converters.
        *
        * @param vEvent    the map event
        * @param hmapConv  the source for the converted event
        * @param vConvKey  (optional) the key Converter
        * @param vConvVal  (optional) the value converter
        *
        * @return the converted MapEvent object
        */
        static MapEvent::Handle convertEvent(MapEvent::View vEvent,
                ObservableMap::Handle hmapConv,
                Converter::View vConvKey = NULL,
                Converter::View vConvVal = NULL);

        /**
        * Transform the given MapEvent into a FilterEvent if it is not already a
        * FilterEvent and there are matching filters associated with the
        * specified Listeners object.
        *
        * @param hEvent      the MapEvent to transform, if necessary
        * @param vListeners  the Listeners object
        *
        * @return a FilterEvent if the given MapEvent is not a FilterEvent and
        *         the specified Listeners object has associated filters;
        *         otherwise, the given MapEvent
        * @since  Coherence 3.7.1.8
        */
        static MapEvent::Handle enrichEvent(MapEvent::Handle hEvent,
                Listeners::View vListeners);

        /**
        * Unwrap the specified map event and return the underlying source event.
        *
        * @param vEvent  the event to unwrap
        *
        * @return the unwrapped event
        */
        static MapEvent::View unwrapEvent(MapEvent::View vEvent);

        /**
        * Check if the given listener is a PrimingListener or if it wraps one.
        *
        * @param listener  Map listener to check
        *
        * @return true iff the listener is a PrimingListener or wraps one
        *
        * @since 12.2.1
        */
        static bool isPrimingListener(MapListener::View listener);


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Evaluate whether or not the specified event should be delivered to the
        * listener associated with the specified filter.
        *
        * @param vFilter  the filter
        * @param vEvent   the event
        *
        * @return true iff the event should be delivered to the corresponding listener
        */
        static bool evaluateEvent(Filter::View vFilter, MapEvent::View vEvent);

        /**
        * Return true iff the specified event represents a transformed CacheEvent.
        *
        * @param vEvent  the event to test
        *
        * @return true iff the event has been transformed
        */
        static bool isTransformedEvent(MapEvent::View vEvent);

        /**
        * Ensure that the specified map has a Listeners object assosiated
        * with the specified key and add the specified listener to it.
        *
        * @param hmapListeners  map of listeners
        * @param vKey           key of the listener
        * @param hListener      the listener to add
        */
        static void addSafeListener(Map::Handle hmapListeners,
                Object::View vKey, MapListener::Handle hListener);

        /**
        * Ensure that the specified map has a Listeners object associated
        * with the specified Filter and add the specified listener to it.
        *
        * @param hmapListeners  map of listeners
        * @param vFilter        filter of the listener
        * @param hListener      the listener to add
        * @since Coherence 3.7.1.8
        */
        static void addSafeListener(Map::Handle hMapListeners,
                Filter::View vAnyFilter, MapListener::Handle hListener);

        /**
        * Remove the specified listener from the Listeners object assosiated
        * with the specified key.
        *
        * @param hmapListeners  map of listeners
        * @param vKey           key of the listener
        * @param vListener      the listener to remove
        */
        static void removeSafeListener(Map::Handle hmapListeners,
                Object::View vKey, MapListener::View vListener);

        /**
        * Add a state information (lite or standard) associated with
        * specified key and listener.
        *
        * @param hmapStandardListeners  map of standard listeners
        * @param vKey                   key of the listener
        * @param hListener              the listener to add
        * @param fLite                  true to indicate that the MapEvent
        *                               objects do not have to include the
        *                               OldValue and NewValue property values
        */
        static void addListenerState(Map::Handle hmapStandardListeners,
                Object::View vKey, MapListener::Handle hListener,
                bool fLite);

        /**
        * Remove a state information (lite or standard) assosiated with
        * specified key and listener.
        *
        * @param hmapStandardListeners  map of standard listeners
        * @param vKey                   key of the listener
        * @param vListener              the listener to remove
        */
        static void removeListenerState(Map::Handle hmapStandardListeners,
                Object::View vKey, MapListener::View vListener);

        /**
        * Unwrap the specified map listener and return the underlying map listener.
        *
        * @param vListener  the listener to unwrap
        *
        * @return the unwrapped listener
        *
        * @since 12.2.1
        */
        static MapListener::View unwrapListener(MapListener::View vListener);

    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- inner class: FilterEvent ---------------------------------------

    public:
        /**
        * An extension of the CacheEvent which may carry no values (old or
        * new), but instead holds an array of Filter objects being the
        * "cause" of the event.
        */
        class COH_EXPORT FilterEvent
            : public class_spec<FilterEvent,
                extends<CacheEvent> >
            {
            friend class factory<FilterEvent>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new FilterEvent with no old and new values.
                *
                * @param hMap        the map on which the Event initially
                *                    occurred
                * @param nId         the events id (entry_inserted |
                *                    entry_updated | entry_deleted)
                * @param vKey        the key into the map
                * @param fSynthetic  true iff the event is caused by internal
                *                    cache processing such as eviction or
                *                    loading
                * @param vaFilters   an array of Filters that caused this event.
                */
                FilterEvent(ObservableMap::Handle hMap, int32_t nId,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic,
                        ObjectArray::View vaFilters);

                /**
                * Create a new FilterEvent with no old and new values.
                *
                * @param hMap        the map on which the Event initially
                *                    occurred
                * @param nId         the events id (entry_inserted |
                *                    entry_updated | entry_deleted)
                * @param vKey        the key into the map
                * @param fSynthetic  true iff the event is caused by internal
                *                    cache processing such as eviction or
                *                    loading
                * @param fPriming    a flag indicating whether or not the event
                *                    is a priming event
                * @param vaFilters   an array of Filters that caused this event.
                * @since 12.2.1.3.2
                */
                FilterEvent(ObservableMap::Handle hMap, int32_t nId,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic,
                        bool fPriming, ObjectArray::View vaFilters);

                /**
                * Create a new FilterEvent with no old and new values.
                *
                * @param hMap             the map on which the Event initially
                *                         occurred
                * @param nId              the events id (entry_inserted |
                *                         entry_updated | entry_deleted)
                * @param vKey             the key into the map
                * @param fSynthetic       true iff the event is caused by internal
                *                         cache processing such as eviction or
                *                         loading
                * @param nTransformState  the state describing how this event has 
                *                         been or should be transformed
                * @param vaFilters        an array of Filters that caused this event.
                */
                FilterEvent(ObservableMap::Handle hMap, int32_t nId,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic,
                        TransformationState nTransformState,
                        ObjectArray::View vaFilters);

                /**
                * Create a new FilterEvent with no old and new values.
                *
                * @param hMap             the map on which the Event initially
                *                         occurred
                * @param nId              the events id (entry_inserted |
                *                         entry_updated | entry_deleted)
                * @param vKey             the key into the map
                * @param fSynthetic       true iff the event is caused by internal
                *                         cache processing such as eviction or
                *                         loading
                * @param nTransformState  the state describing how this event has
                *                         been or should be transformed
                * @param fPriming         a flag indicating whether or not the event
                *                         is a priming event
                * @param vaFilters        an array of Filters that caused this event.
                * @since 12.2.1.3.2
                */
                FilterEvent(ObservableMap::Handle hMap, int32_t nId,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic,
                        TransformationState nTransformState,
                        bool fPriming, ObjectArray::View vaFilters);

                /**
                * Create a new FilterEvent with no old and new values.
                *
                * @param hMap             the map on which the Event initially
                *                         occurred
                * @param nId              the events id (entry_inserted |
                *                         entry_updated | entry_deleted)
                * @param vKey             the key into the map
                * @param fSynthetic       true iff the event is caused by internal
                *                         cache processing such as eviction or
                *                         loading
                * @param nTransformState  the state describing how this event has
                *                         been or should be transformed
                * @param fPriming         a flag indicating whether or not the event
                *                         is a priming event
                * @param fExpired         true iff the event results from an eviction due to time
                * @param vaFilters        an array of Filters that caused this event.
                * @since 14.1.1.0.10
                */
                FilterEvent(ObservableMap::Handle hMap, int32_t nId,
                        Object::View vKey, Object::View vValueOld,
                        Object::View vValueNew, bool fSynthetic,
                        TransformationState nTransformState,
                        bool fPriming, bool fExpired, ObjectArray::View vaFilters);
    
            // ----- FilterEvent interface ------------------------------

            public:
                /**
                * Return an array of filters that are the cause of this
                * event.
                *
                * @return an array of filters
                */
                virtual ObjectArray::View getFilter() const;

                /**
                * Set the event that this FilterEvent wraps.
                *
                * @param vEvent  the wrapped event
                * @since Coherence 3.7.1.8
                */
                virtual void setEvent(MapEvent::View vEvent);

            // ----- Describable interface ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View getDescription() const;

            // ----- MapEvent methods -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual Object::View getKey() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getOldValue() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::View getNewValue() const;

            // ----- helper methods -------------------------------------

            public:
                /**
                * Determine if this filter event contains a specified filter.
                *
                * @param vFilter  the filter to search for
                *
                * @return true if the event contained the specified filter,
                *         false otherwise
                */
                virtual bool containsFilter(Filter::View vFilter) const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * Filters which caused the event.
                */
                FinalView<ObjectArray> f_vaFilters;

                /**
                * Optional wrapped MapEvent.
                * @since Coherence 3.7.1.8
                */
                FinalView<MapEvent> f_vEvent;
            };


    // ----- inner class: SynchronousListener -------------------------------

    public:
        /**
        * A tag interface indicating that tagged MapListener implementation
        * has to receive the MapEvent notifications in a synchronous manner.
        * <p>
        * Consider a MapListener that subscribes to receive notifications for
        * distributed (partitioned) cache. All events notifications are
        * received by the service thread and immediately queued to be
        * processed by the dedicated event dispatcher thread. This makes it
        * impossible to differentiate between the event caused by the updates
        * made by this thread and any other thread (possibly in a different
        * VM). Forcing the events to be processed on the service thread
        * guarantees that by the time "put" or "remove" requests return to
        * the caller all relevant MapEvent notification have been processed
        * (due to the "in order delivery" rule enforced by the TCMP).
        * <p>
        * This interface should be considered as a very advanced feature, so
        * a MapListener implementation that is tagged as a
        * SynchronousListener must exercise extreme caution during event
        * processing since any delay with return or unhandled exception will
        * cause a delay or complete shutdown of the corresponding cache
        * service.
        * <p>
        * <b>Note:</b> The contract by the event producer in respect to the
        * SynchronousListener is somewhat weaker then the general one.
        * First, the SynchronousListener implementaion should make no
        * assumptions about the event source obtained by {MapEvent#getMap()}.
        * Second, in the event of [automatic] service restart, the listener
        * has to be re-registered manually (for example, in response to the
        * {com::tangosol::net::MemberEvent#member_joined member_joined}
        * event). Third, and the most important, no calls against the
        * NamedCache are allowed during the synchronous event processing (the
        * only exception being a call to remove the listener itself).
        */
        class COH_EXPORT SynchronousListener
            : public interface_spec<SynchronousListener,
                implements<coherence::util::SynchronousListener, MapListener> >
            {
            };

    // ----- inner class: PrimingListener ----------------------------------

    public:
        /**
         * A tag interface indicating that tagged MapListener implementation
         * has to receive the MapEvent notifications in a synchronous manner
         * for lite events (carrying only a key).
         * Additionally a "priming" event is generated when registering this listener.
         *
         * @since 12.2.1
         */
        class COH_EXPORT PrimingListener
            : public interface_spec<PrimingListener,
                implements<SynchronousListener> >
            {
            };


    // ----- inner class: WrapperSynchronousListener ------------------------

    public:
        /**
        * A wrapper class that turns the specified MapListener into
        * a synchronous listener.
        */
        class COH_EXPORT WrapperSynchronousListener
            : public class_spec<WrapperSynchronousListener,
                extends<MultiplexingMapListener>,
                implements<SynchronousListener> >
            {
            friend class factory<WrapperSynchronousListener>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new WrapperSynchronousListener.
                *
                * @param hListener  the listener to be wrapped
                *
                * @return the new WrapperSynchronousListener instance
                */
                WrapperSynchronousListener(MapListener::Handle hListener);

            // ----- accessors ------------------------------------------

            public:
                /**
                * Get the underlying MapListener.
                *
                * @return the underlying listener
                */
                virtual MapListener::View getMapListener() const;

            // ----- MultiplexingMapListener interface ------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void onMapEvent(MapEvent::View vEvent);

            // ----- Object interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * Wrapped MapListener.
                */
                MemberHandle<MapListener> m_hListener;
            };

    // ----- inner class: WrapperPrimingListener ------------------------

    public:
        /**
         * A wrapper class that turns the specified MapListener into
         * a Priminglistener.
         *
         * @since 12.2.1
         */
        class COH_EXPORT WrapperPrimingListener
            : public class_spec<WrapperPrimingListener,
                extends<WrapperSynchronousListener>,
                implements<PrimingListener> >
        {
        friend class factory<WrapperPrimingListener>;

        // ----- constructors ---------------------------------------

        protected:
            /**
             * Create a new WrapperPrimingListener.
             *
             * @param hListener  the listener to be wrapped
             *
             * @return the new WrapperPrimingListener instance
             */
            WrapperPrimingListener(MapListener::Handle hListener);

        private:
            /**
             * Blocked copy constructor.
             */
            WrapperPrimingListener(const WrapperPrimingListener&);
        };

    // ----- enums ----------------------------------------------------------

    protected:
        /**
        * Optimization Plan enum.
        */
        enum Plan
            {
            none,          // A plan has not yet been formed
            no_listeners,  // There are no listeners
            all_listener,  // There is one all-keys non-filtered listener
            key_listener,  // One key listener (even if for multiple keys)
            no_optimize    // No optimized plan, use the default approach
            };


    // ----- data members ---------------------------------------------------

    protected:
         /**
        * The collections of MapListener objects that have signed up for
        * notifications from an ObservableMap implementation keyed by the
        * corresponding Filter objects.
        */
        MemberHandle<Map> m_hmapListeners;

        /**
        * The collections of MapListener objects that have signed up for key
        * based notifications from an ObservableMap implementation keyed by
        * the corresponding key objects.
        */
        MemberHandle<Map> m_hmapKeyListeners;

        // consider adding listener tag support to Listeners class, which
        // would allow getting rid of the following data structures ...
        /**
        * The subset of standard (not lite) global listeners. The keys are
        * the Filter objects, the values are sets of corresponding standard
        * listeners.
        */
        MemberHandle<Map> m_hmapStandardListeners;

        /**
        * The subset of standard (not lite) key listeners. The keys are the
        * key objects, the values are sets of corresponding standard
        * listeners.
        */
        MemberHandle<Map> m_hmapStandardKeyListeners;

        /**
        * The optimization plan which indicates the fastest way to put
        * together a set of listeners.
        */
        Plan m_optimizationPlan;

        /**
        * A cached set of Listeners.
        */
        MemberView<Listeners> m_vListenersCached;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MAP_LISTENER_SUPPORT_HPP

