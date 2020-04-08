/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ServiceEvent.hpp"

#include "coherence/util/Service.hpp"
#include "coherence/util/ServiceListener.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

ServiceEvent::ServiceEvent(ServiceEvent::ServiceView vService, Id identity)
    : super(vService), m_id(identity)
    {
    }


// ----- ServiceEvent interface ---------------------------------------------

void ServiceEvent::dispatch(Listeners::View vListeners) const
    {
    if (NULL != vListeners)
        {
        ObjectArray::View vaListener = vListeners->listeners();

        for (size32_t i = vaListener->length; i-- > 0; )
            {
            ServiceListener::Handle hListener =
                    cast<ServiceListener::Handle>(vaListener[i]);

            switch (getId())
                {
                case ServiceEvent::service_starting:
                    hListener->serviceStarting(this);
                    break;

                case ServiceEvent::service_started:
                    hListener->serviceStarted(this);
                    break;

                case ServiceEvent::service_stopping:
                    hListener->serviceStopping(this);
                    break;

                case ServiceEvent::service_stopped:
                    hListener->serviceStopped(this);
                    break;
                }
            }
        }
    }


// ----- Object interface ---------------------------------------------------

static const size32_t COH_ID_TEXT_LENGTH = 5;
static const char*  COH_ID_TEXT[COH_ID_TEXT_LENGTH] =
    {
    "<unknown>",
    "starting",
    "started",
    "stopping",
    "stopped"
    };

TypedHandle<const String> ServiceEvent::toString() const
    {
    return COH_TO_STRING("ServiceEvent{"
        << COH_ID_TEXT[getId()]
        << ' '
        << getService()
        << '}');
    }


// ----- accessors ----------------------------------------------------------

ServiceEvent::Id ServiceEvent::getId() const
    {
    return m_id;
    }

ServiceEvent::ServiceView ServiceEvent::getService() const
    {
    return cast<Service::View>(getSource());
    }

COH_CLOSE_NAMESPACE2
