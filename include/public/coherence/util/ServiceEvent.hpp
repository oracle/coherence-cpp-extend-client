/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERVICE_EVENT_HPP
#define COH_SERVICE_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/util/Event.hpp"
#include "coherence/util/Listeners.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

class Service;


/**
* An event which indicates that a Service state has changed:
* <ul>
* <li>a service is starting</li>
* <li>a service has started</li>
* <li>a service is stopping</li>
* <li>a service has stopped</li>
* </ul>
* A ServiceEvent object is sent as an argument to the ServiceListener
* interface methods.
*
* @see Service
* @see ServiceListener
*
* @author jh  2007.12.13
*/
class COH_EXPORT ServiceEvent
    : public class_spec<ServiceEvent,
        extends<Event> >
    {
    friend class factory<ServiceEvent>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Service Handle definition.
        */
        typedef TypedHandle<Service> ServiceHandle;

        /**
        * Service View definition.
        */
        typedef TypedHandle<const Service> ServiceView;


    // ----- Id definition --------------------------------------------------

    public:
        typedef enum
            {
            service_starting = 1, // the service is starting
            service_started  = 2, // the service has started
            service_stopping = 3, // the service is stopping
            service_stopped  = 4  // the service has stopped
            } Id;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ServiceEvent instance.
        *
        * @param vService  the Service that fired the event
        * @param identity  the event's ID, one of the Id enum values
        */
        ServiceEvent(ServiceView vService, Id identity);

    private:
        /**
        * Blocked copy constructor.
        */
        ServiceEvent(const ServiceEvent&);
		

    // ----- ServiceEvent interface -----------------------------------------

    public:
        /**
        * Dispatch this event to the specified listeners collection.
        *
        * @param vListeners the listeners collection
        *
        * @throws ClassCastException if any of the targets is not an instance of
        *         the ServiceListener interface
        */
        virtual void dispatch(Listeners::View vListeners) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return this event's ID.
        *
        * @return the event ID, one of the Id enum values
        */
        virtual Id getId() const;

        /**
        * Return the Service that fired the event.
        *
        * @return the Service
        */
        virtual ServiceView getService() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The event's ID.
        */
        Id m_id;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERVICE_EVENT_HPP
