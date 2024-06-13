/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPONENT_SERVICE_HPP
#define COH_COMPONENT_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/SerializerFactory.hpp"
#include "coherence/net/OperationalContext.hpp"
#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/util/Describable.hpp"
#include "coherence/util/DualQueue.hpp"
#include "coherence/util/Event.hpp"
#include "coherence/util/EventListener.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/Service.hpp"
#include "coherence/util/ServiceEvent.hpp"
#include "coherence/util/ServiceListener.hpp"

#include "private/coherence/component/util/QueueProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::util::QueueProcessor;
using coherence::io::Serializer;
using coherence::io::SerializerFactory;
using coherence::net::OperationalContext;
using coherence::run::xml::XmlElement;
using coherence::util::Describable;
using coherence::util::Event;
using coherence::util::EventListener;
using coherence::util::Listeners;
using coherence::util::ServiceEvent;
using coherence::util::ServiceListener;


/**
* Base definition of a Service component.
*
* A Service has a service thread, an optional execute thread pool, and an
* event dispatcher thread.
*
* @author nsa 2008.02.25
*/
class COH_EXPORT Service
    : public class_spec<Service,
        extends<QueueProcessor>,
        implements<coherence::util::Service> >
    {
    friend class factory<Service>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- enum: State ----------------------------------------------------

    public:
        /**
        * State identifiers
        */
        typedef enum
            {
            service_initial  = 0,
            service_starting = 1,
            service_started  = 2,
            service_stopping = 3,
            service_stopped  = 4
            } State;


    // ----- Nested class: DispatchEvent --------------------------------

    public:
        /**
        * Runnable event.
        */
        class COH_EXPORT DispatchEvent
            : public class_spec<DispatchEvent,
                extends<Describable>,
                implements<Runnable> >
            {
            friend class factory<DispatchEvent>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new DispatchEvent instance
                */
                DispatchEvent();

            private:
                /**
                * Blocked copy constructor.
                */
                DispatchEvent(const DispatchEvent&);


            // ----- DispatchEvent interface ----------------------------

            public:
                /**
                * Get the event to dispatch.
                *
                * @return Event::View
                */
                virtual Event::View getEvent() const;

                /**
                * Set the event to dispatch.
                *
                * @return Event::View
                */
                virtual void setEvent(Event::View vEvent);

                /**
                * Get the event listeners.
                *
                * @return the event listeners
                */
                virtual Listeners::View getListeners() const;

                /**
                * Get the event listeners.
                *
                * @return the event listeners
                */
                virtual Listeners::Handle getListeners();

                /**
                * Get the name of the dispatch event object. The default
                * implementation uses Class::getSimpleClassName().
                *
                * @return the name of the dispatch event
                */
                virtual String::View getName() const;

                /**
                * Set the event listeners.
                */
                virtual void setListeners(Listeners::Handle hListeners);


            // ----- Runnable interface ---------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual void run();

            // ----- Describable interface -------------------------------

            public:

                /**
                * {@inheritDoc}
                */
                virtual String::View getDescription() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The event to dispatch.
                */
                FinalView<Event> f_vEvent;

                /**
                * The list of EventListeners.
                */
                FinalHandle<Listeners> f_hListeners;
            };


    // ----- Nested class: EventDispatcher ------------------------------

    public:
        /**
        * This is a Daemon component that waits for items to process from a
        * Queue. Whenever the Queue contains an item, the onNotify event
        * occurs. It is expected that sub-classes will process onNotify as
        * follows:
        *
        * <pre><code>
        * Object o;
        * while ((o = getQueue().removeNoWait()) != null)
        * {
        *     // process the item
        *     // ...
        * }
        * </code></pre>
        *
        * The Queue is used as the synchronization point for the daemon.
        */
        class COH_EXPORT EventDispatcher
            : public class_spec<EventDispatcher,
                extends<QueueProcessor> >
            {
            friend class factory<EventDispatcher>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new EventDispatcher
                */
                EventDispatcher(Service::View vService);

            private:
            /**
            * Blocked copy constructor.
            */
            EventDispatcher(const EventDispatcher&);


            // ----- EventDispatcher interface --------------------------

            public:
                /**
                * Return the CloggedCount.
                *
                * @return the CloggedCount
                */
                virtual size32_t getCloggedCount() const;

                /**
                * Return the CloggedDelay.
                *
                * @return the CloggedDelay
                */
                virtual int32_t getCloggedDelay() const;

                /**
                * Return whether the EventDispatcher is dispatching or not
                *
                * @return true iff the EventDispatcher is dispatching
                */
                virtual bool isDispatching() const;

                /**
                * Set the number of events in the queue that signal the
                * EventDispatcher is full.
                *
                * @param cMaxEvents  the number of events that signal the
                *                    EventDispatcher is full
                */
                virtual void setCloggedCount(size32_t cMaxEvents);

                /**
                * Set the number of milliseconds to pause client threads
                * when a clog occurs. The pause is repeated until the clog
                * is gone. Anything less than one (e.g. zero) is treated as
                * one.
                *
                * @param cMillis  the number of milliseconds to pause client
                * threads when a clog occurs.
                */
                virtual void setCloggedDelay(int32_t cMillis);

                /**
                * Set whether the EventDispatcher is dispatching events.
                *
                * @param fDispatching  Whether events are being dispatched
                *                      or not
                */
                virtual void setDispatching(bool fDispatching);

                /**
                * Slow additions to the queueprocessor down to ensure that it
                * doesn't consume too much memory. This method is typically
                * called on client threads.
                */
                virtual void drainOverflow();

                /**
                * Block the calling thread until the Queue is empty and all
                * outstanding tasks have been executed. This method is
                * mostly called on client threads.
                */
                virtual void drainQueue();


            // ----- QueueProcessor interface ---------------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual Queue::Handle instantiateQueue() const;


            // ----- Daemon interface -----------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View getThreadName() const;

            protected:
                /**
                * {@inheritDoc}
                */
                virtual void onException(Exception::Holder ohe);

                /**
                * {@inheritDoc}
                */
                virtual void onExit();

                /**
                * {@inheritDoc}
                */
                virtual void onNotify();


            // ----- Object interface -------------------------------

            protected:
                /**
                * {@inheritDoc}
                */
                virtual void onInit();


            // ----- nested class EventQueue ------------------------

            public:
                /**
                * This is the Queue to which items that need to be processed
                * are added, and from which the daemon pulls items to
                * process.
                */
                class COH_EXPORT EventQueue
                    : public class_spec<EventQueue,
                        extends<coherence::util::DualQueue> >
                    {
                    friend class factory<EventQueue>;

                    // ----- constructor ----------------------------

                    protected:
                        /**
                        * Create a new DualQueue instance
                        */
                        EventQueue(Service::View vService);

                    private:
                    /**
                    * Blocked copy constructor.
                    */
                    EventQueue(const EventQueue&);


                    // ----- Queue interface ------------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual bool add(Object::Holder oh);


                    // ----- data members ---------------------------

                    protected:
                        /**
                        * Reference back to the service component
                        */
                        WeakView<Service> m_wvService;
                    };

            // ----- data members ---------------------------------------

            protected:
                /**
                * The maximum number of events in the queue before
                * determining that the dispatcher is clogged. Zero means no
                * limit.
                */
                size32_t m_cCloggedCount;

                /**
                * The number of milliseconds to pause client threads when a
                * clog occurs, to wait for the clog to dissipate. (The pause
                * is repeated until the clog is gone.) Anything less than
                * one (e.g. zero) is treated as one.
                */
                int32_t m_cCloggedDelay;

                /**
                * Set to true while the EventDispatcher daemon thread is in
                * the process of dispatching events.
                */
                Volatile<bool> m_fDispatching;

                /**
                * Reference back to the parent service component
                */
                WeakView<Service> m_wvService;
            };


    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new Service.
        */
        Service();

    private:
        /**
        * Blocked copy constructor.
        */
        Service(const Service&);


    // ----- Service interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void addServiceListener(ServiceListener::Handle hListener);

        /**
        * Adjust the default timeout value using the passed context-specific
        * timeout value.
        *
        * @param lDefaultValue  the default timeout value (must be
        *                       non-negative)
        * @param lContextValue  positive, or zero for "default timeout", or
        *                       -1 for "no timeout"
        *
        * @return the adjusted timeout value
        */
        virtual int64_t adjustTimeout(int64_t lDefaultTimeout, int64_t lContextTimeout);

        /**
        * Configure the service based on the provided xml configuration
        * element.
        *
        * Note: This method is synchronized
        *
        * @param vConfig  the service configuration element
        */
        virtual void configure(XmlElement::View vXml);

        /**
        * Dispatch a ServiceEvent to the EventDispatcher.
        *
        * @param nEvent  the ID of the ServiceEvent
        */
        virtual void dispatchServiceEvent(ServiceEvent::Id nEvent);

        /**
        * Return a running EventDispatcher.
        *
        * @return a running EventDispatcher
        */
        virtual EventDispatcher::Handle ensureEventDispatcher();

        /**
        * Return an instance of the configured Serializer.
        *
        * @return a Serializer
        */
        virtual Serializer::View ensureSerializer();

        /**
        * Return the string value of a named child element of the given XML
        * configuration.
        *
        * @param xmlConfig  the parent XML configuration element
        * @param sName      the name of the the child XML configuration
        *                   element
        * @param sDefault   the default value to return if the child element
        *                   is missing or empty
        *
        * @throws IllegalArgumentException if the child element and default
        *         value is missing or empty
        */
        virtual String::View ensureStringValue(XmlElement::View vXmlConfig,
                String::View vsName, String::View vsDefault);

        /**
        * Return a human-readable description of the given Service state.
        *
        * @param nState  the state to return as human-readable
        *
        * @return a human-readable description of the given Service state
        */
        virtual String::View formatServiceStateName(State nState) const;

        /**
        * Return a human-readible description of the Service statistics
        *
        * @return a human-readible description of the Service statistics
        */
        virtual String::View formatStats() const;

        /**
        * Returns whether the service is currently running
        *
        * @return true iff the service is running
        */
        virtual bool isRunning() const;

        /**
        * Return true if the current thread is one of the Service threads.
        *
        * Note: only the service thread and event dispatcher thread are
        *                 considered to be service threads.
        */
        virtual bool isServiceThread() const;

        /**
        * The default implementation of this method sets AcceptingClients to
        * true. If the Service has not completed preparing at this point,
        * then the Service must override this implementation and only set
        * AcceptingClients to true when the Service has actually "finished
        * starting".
        */
        virtual void onServiceStarted();

        /**
        * {@inheritDoc}
        */
        virtual void removeServiceListener(ServiceListener::Handle hListener);

        /**
        * Reset the Service statistics.
        */
        virtual void resetStats();

        /**
        * Stop the Service. The default implemenation of this method simply
        * calls stop().
        *
        * Note: This method is synchronized.
        */
        virtual void shutdown();

        /**
        * Starts the daemon thread associated with this component. If the
        * thread is already starting or has started, invoking this method
        * has no effect.
        *
        * Note: This method is synchronized.
        *
        * Synchronization is used here to verify that the start of the
        * thread occurs; the lock is obtained before the thread is started,
        * and the daemon thread notifies back that it has started from the
        * run() method.
        */
        virtual void start();

        /**
        * Hard-stop the Service. Use shutdown() for normal termination.
        *
        * Note: This method is synchronized.
        */
        virtual void stop();

        /**
        * Block the calling thread until the Service has advanced to the
        * state at which it can accept requests from client threads.
        */
        virtual void waitAcceptingClients();

        /**
        * Set the Serializer used by this Service.
        */
        virtual void setSerializer(Serializer::View vSerializer);

        /**
        * Get the Serializer used by this Service.
        *
        * @return the Serializer object
        */
        virtual Serializer::View getSerializer() const;

    protected:
        /**
        * The default implementation of this method does nothing.
        */
        virtual void onServiceStarting();

        /**
        * Called when the Service has transitioned to the specified state.
        *
        * @param nState  the new Service state
        */
        virtual void onServiceState(State nState);

        /**
        * The default implementation of this method sets AcceptingClients to
        * false.
        */
        virtual void onServiceStopped();

        /**
        * The default implementation of this method sets AcceptingClients to
        * false.
        */
        virtual void onServiceStopping();

        /**
        * Parse the String value of the child XmlElement with the given name
        * as a time in milliseconds. If the specified child XmlElement does
        * not exist or is empty, the specified default value is returned.
        *
        * @param xml the parent XmlElement
        * @param sName the name of the child XmlElement
        * @param cDefault the default value
        * @return the time (in milliseconds) represented by the specified
        *         child XmlElement
        */
        static int64_t parseTime(XmlElement::View vXml, String::View vsName,
                int64_t cDefault);


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the EventDispatcher for this service.
        *
        * @return the EventDispatcher for this service.
        */
        virtual EventDispatcher::Handle getEventDispatcher();

        /**
        * Return the EventDispatcher for this service.
        *
        * @return the EventDispatcher for this service.
        */
        virtual EventDispatcher::View getEventDispatcher() const;

        /**
        * Return the event listeners for this Service
        *
        * @return the event listeners for this Service
        */
        virtual Listeners::Handle getEventListeners();

        /**
        * Return the name of the Service.
        *
        * @return the name of the Service
        */
        virtual String::View getServiceName() const;

        /**
        * Return the human readable name of the current service state.
        *
        * @return the human readable name of the current service state
        */
        virtual String::View getServiceStateName() const;

        /**
        * Return the state of the Service.
        *
        * @return the state of the service
        */
        virtual State getServiceState() const;

        /**
        * Return the cpu usage statistics
        *
        * @return the cpu usage statistics
        */
        virtual int64_t getStatsCpu() const;

        /**
        * Return the last time the statistics were reset.
        *
        * @return the last time the statistics were reset.
        */
        virtual int64_t getStatsReset() const;

        /**
        * Return the number of messages received.
        *
        * @return the number of messages received
        */
        virtual int64_t getStatsReceived() const;

        /**
        * Return whether the Service is accepting clients.
        *
        * @return true if the Service is accepting clients.
        */
        virtual bool isAcceptingClients() const;

        /**
        * Set whether the Service is accepting clients or not.
        *
        * Note: This method is synchronized.
        *
        * @param fAccepting  whether the Service is accepting clients or not
        */
        virtual void setAcceptingClients(bool fAccepting);

        /**
        * Set the Service configuration.
        *
        * @param vElement  the service configuration element
        */
        virtual void setServiceConfig(XmlElement::View vXml);

        /**
        * Set the Service name.
        *
        * @param vsName  the name of the service
        */
        virtual void setServiceName(String::View vsName);

        /**
        * Set the Service state.
        *
        * Note: This method is synchronized.
        *
        * @param nState  the state of the service
        */
        virtual void setServiceState(State nState);

        /**
        * Set the Cpu usage statistics
        *
        * @param cCpu  the cpu usage
        */
        virtual void setStatsCpu(int64_t cCpu);

        /**
        * Set the number of messages received.
        *
        * @param cMessage  the number of messages received
        */
        virtual void setStatsReceived(int64_t cMessage);

        /**
        * Set the time of the last stats reset.
        *
        * @param lStatsReset  the time of the last stats reset
        */
        virtual void setStatsReset(int64_t lStatsReset);

        /**
        * Return the OperationalContext for this Service.
        *
        * @return the OperationalContext for this Service
        *
        * @since Coherence 12.1.2
        */
        virtual OperationalContext::View getOperationalContext() const;

        /**
        * Set the OperationalContext for this Service.
        *
        * @param vContext  the OperationalContext for this Service
        *
        * @throws IllegalStateException if an OperationalContext was already
        *         set
        *
        * @since Coherence 12.1.2
        */
        virtual void setOperationalContext(OperationalContext::View vContext);

        /**
        * Set the SerializerFactory used by this Service.
        *
        * @param hFactory  the SerializerFactory for this Service
        *
        * @since Coherence 12.1.2
        */
        virtual void setSerializerFactory(SerializerFactory::View hFactory);

        /**
        * Get the SerializerFactory used by this Service.
        *
        * @return the SerializerFactory object
        *
        * @since Coherence 12.1.2
        */
        virtual SerializerFactory::View getSerializerFactory() const;

    protected:
        /**
        * Set the EventDispatcher
        *
        * @param hDispatcher  the EventDispatcher
        */
        virtual void setEventDispatcher(EventDispatcher::Handle hDispatcher);

    private:
        /**
        * Set the EventListeners for this service.
        *
        * @param hListeners  the Listeners to set
        */
        virtual void setEventListeners(Listeners::Handle hListeners);


    // ----- Daemon interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getThreadName() const;

        /**
        * {@inheritDoc}
        */
        virtual int64_t getWaitMillis() const;

        /**
        * {@inheritDoc}
        */
        virtual void onException(Exception::Holder ohe);

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onEnter();

        /**
        * {@inheritDoc}
        */
        virtual void onExit();

        /**
        * {@inheritDoc}
        */
        virtual void onNotify();


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void onInit();

        /**
        * {@inheritDoc}
        *
        * Describable override is necessary as for services, the wrapper
        * characters are curly braces instead of parentheses.
        *
        * @since 12.2.1.3
        */
        virtual TypedHandle<const String> toString() const;


    // ----- internal helper methods ----------------------------------------

    protected:
        /**
        * Register the given EventListener with this Service.
        *
        * @param vEventListener  the EventListener to register
        *
        * @see #removeEventListener
        */
        virtual void addEventListener(EventListener::Handle hEventListener);

        /**
        * Dispatch the given event to the EventDispatcher.
        *
        * @param vEventt     the event to dispatch
        * @param hListeners  the set of Listeners that should be notified
        */
        virtual void dispatchEvent(Event::View vEvent, Listeners::Handle hListeners);

        /**
        * Factory pattern: create a new DispatchEvent instance.
        *
        * @return the new DispatchEvent
        */
        virtual DispatchEvent::Handle instantiateDispatchEvent() const;

        /**
        * Create a Serializer from the Service configuration
        *
        * @param vLoader Classloader
        *
        * @return the new Serializer
        */
        virtual Serializer::View instantiateSerializer(ClassLoader::View vLoader) const;

        /**
        * Unregister the given EventListener from this Service.
        *
        * @param l the EventListener to unregister
        *
        * @see addEventListener
        */
        virtual void removeEventListener(EventListener::Handle hListener);


    // ----- Service child component factory --------------------------------

    protected:
        /**
        * Factory pattern: create a new DispatchEvent instance.
        *
        * @return the new DispatchEvent
        */
        virtual EventDispatcher::Handle instantiateEventDispatcher() const;

        /**
        * Ensure that an EventDispatcher object has been initialized for this
        * Component.
        */
        virtual EventDispatcher::Handle ensureInitializedEventDispatcher();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Set to true when the Service has advanced to the state at which it
        * can accept requests from client threads.
        */
        bool m_fAcceptingClients;

        /**
        * The event dispatcher daemon.
        */
        FinalHandle<EventDispatcher> f_hEventDispatcher;

        /**
        * The list of registered EventListeners.
        */
        FinalHandle<Listeners> f_hEventListeners;

        /**
        * The cached Serializer instance for this Service.
        */
        FinalView<Serializer> f_vSerializer;

        /**
        * The SerializerFactory used by this Service.
        */
        FinalView<SerializerFactory> f_vSerializerFactory;

        /**
        * Original XML configuration that was supplied to the Service; may
        * be null.
        */
        FinalView<XmlElement> f_vServiceConfig;

        /**
        * The name of this Service.
        */
        FinalView<String> f_vsServiceName;

        /**
        * The state of the Service; one of the SERVICE State enums.
        */
        State m_nServiceState;

        /**
        * Statistics: total time spent while processing messages.
        */
        int64_t m_lStatsCpu;

        /**
        * Statistics: total number of received messages.
        */
        int64_t m_lStatsReceived;

        /**
        * Statistics: Date/time value that the stats have been reset.
        */
        int64_t m_lStatsReset;

        /**
        * The OperationalContext for this Service.
        */
        FinalView<OperationalContext> f_vContext;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPONENT_SERVICE_HPP
