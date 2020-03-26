/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERVICE_HPP
#define COH_SERVICE_HPP

#include "coherence/lang.ns"

#include "coherence/util/Controllable.hpp"
#include "coherence/util/ServiceListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* A Service is a Controllable that emits service lifecycle events.
*
* @see ServiceListener
*
* @author jh 2007.12.12
*/
class COH_EXPORT Service
    : public interface_spec<Service,
        implements<Controllable> >
    {
    // ----- Service interface ----------------------------------------------

    public:
        /**
        * Register a ServiceListener that will receive events pertaining to
        * the lifecycle of this Service.
        *
        * @param hListener  the new ServiceListener to register; if the
        *                   listener has already been registered, this
        *                   method has no effect
        */
        virtual void addServiceListener(ServiceListener::Handle hListener) = 0;

        /**
        * Unregister a ServiceListener from this ConnectionManager.
        *
        * After a ServiceListener is removed, it will no longer receive
        * events pertaining to the lifecycle of this Service.
        *
        * @param hListener  the ServiceListener to deregister; if the
        *                   listener has not previously been registered,
        *                   this method has no effect
        */
        virtual void removeServiceListener(ServiceListener::Handle hListener) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERVICE_HPP
