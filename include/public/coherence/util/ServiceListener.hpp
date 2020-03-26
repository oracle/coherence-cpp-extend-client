/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERVICE_LISTENER_HPP
#define COH_SERVICE_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/EventListener.hpp"
#include "coherence/util/ServiceEvent.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* The listener interface for receiving ServiceEvents.
*
* @see Service
* @see ServiceEvent
*
* @author jh  2007.12.12
*/
class COH_EXPORT ServiceListener
    : public interface_spec<ServiceListener,
        implements<EventListener> >
    {
    // ----- ServiceListener interface --------------------------------------

    public:
        /**
        * Invoked when a service is starting.
        *
        * @param vEvt  the ServiceEvent::service_starting event
        */
        virtual void serviceStarting(ServiceEvent::View vEvt) = 0;

        /**
        * Invoked when a service has started.
        *
        * @param vEvt  the ServiceEvent::service_started event
        */
        virtual void serviceStarted(ServiceEvent::View vEvt) = 0;

        /**
        * Invoked when a service is stopping.
        *
        * @param vEvt  the ServiceEvent::service_stopping event
        */
        virtual void serviceStopping(ServiceEvent::View vEvt) = 0;

        /**
        * Invoked when a service has stopped.
        *
        * @param vEvt  the ServiceEvent::service_stopped event
        */
        virtual void serviceStopped(ServiceEvent::View vEvt) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERVICE_LISTENER_HPP
