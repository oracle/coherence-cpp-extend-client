/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/RunnableCacheEvent.hpp"

#include "coherence/net/NamedCache.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::net::NamedCache;
using coherence::util::logging::Logger;


// ----- constructors -------------------------------------------------------

RunnableCacheEvent::RunnableCacheEvent(MapEvent::Handle hMapEvent,
        Listeners::View vListeners)
        : f_vListeners(self(), vListeners),
          f_hListenerSupport(self()),
          f_hMapEvent(self(), hMapEvent),
          f_hMapListener(self())
    {
    COH_ENSURE_PARAM(hMapEvent);
    COH_ENSURE_PARAM(vListeners);
    }

RunnableCacheEvent::RunnableCacheEvent(MapEvent::Handle hMapEvent,
        MapListener::Handle hListener)
        : f_vListeners(self()),
          f_hListenerSupport(self()),
          f_hMapEvent(self(), hMapEvent),
          f_hMapListener(self(), hListener)
    {
    COH_ENSURE_PARAM(hMapEvent);
    COH_ENSURE_PARAM(hListener);
    }


RunnableCacheEvent::RunnableCacheEvent(MapEvent::Handle hMapEvent,
        MapListenerSupport::Handle hListenerSupport)
        : f_vListeners(self()),
          f_hListenerSupport(self(), hListenerSupport),
          f_hMapEvent(self(), hMapEvent),
          f_hMapListener(self())
    {
    COH_ENSURE_PARAM(hMapEvent);
    COH_ENSURE_PARAM(hListenerSupport);
    }


// ----- Runnable interface -------------------------------------------------

void RunnableCacheEvent::run()
    {
    MapEvent::Handle hEvent = getMapEvent();

    if (cast<NamedCache::View>(hEvent->getSource())->isActive())
        {
        MapListenerSupport::Handle hSupport = getListenerSupport();
        if (NULL == hSupport)
            {
            Listeners::View vListeners = getListeners();
            if (NULL == vListeners)
                {
                hEvent->dispatch(getMapListener());
                }
            else
                {
                hEvent->dispatch(vListeners, true);
                }
            }
        else
            {
            hSupport->fireEvent(hEvent, true);
            }
        }
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> RunnableCacheEvent::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this) << ": " << getMapEvent());
    }


// ----- static functions ---------------------------------------------------

void RunnableCacheEvent::dispatchSafe(MapEvent::Handle hEvent,
        Listeners::View vListeners, Queue::Handle hQueue)
    {
    if (vListeners != NULL)
        {
        ObjectArray::View vaListener = vListeners->listeners();
        for (size32_t i = 0, c = vaListener->length; i < c; i++)
            {
            MapListener::Handle hListener = cast<MapListener::Handle>
                    (vaListener[i]);
            if (instanceof<MapListenerSupport::SynchronousListener::Handle>(hListener))
                {
                try
                    {
                    hEvent->dispatch(hListener);
                    }
                catch (Exception::View vEx)
                    {
                    COH_LOG(COH_TO_STRING(
                            "An exception occured while dispatching synchronous event:" <<
                            hEvent), Logger::level_error);
                    COH_LOGEX(vEx, Logger::level_error);
                    COH_LOG("(The exception has been logged and execution is continuing.)",
                            Logger::level_error);
                    }
                }
            else
                {
                hQueue->add(RunnableCacheEvent::create(hEvent, hListener));
                }
            }
        }
    }


// ----- accessors ----------------------------------------------------------

Listeners::View RunnableCacheEvent::getListeners() const
    {
    return f_vListeners;
    }

MapListenerSupport::Handle RunnableCacheEvent::getListenerSupport()
    {
    return f_hListenerSupport;
    }

MapListenerSupport::View RunnableCacheEvent::getListenerSupport() const
    {
    return f_hListenerSupport;
    }

MapEvent::Handle RunnableCacheEvent::getMapEvent()
    {
    return f_hMapEvent;
    }

MapEvent::View RunnableCacheEvent::getMapEvent() const
    {
    return f_hMapEvent;
    }

MapListener::Handle RunnableCacheEvent::getMapListener()
    {
    return f_hMapListener;
    }

MapListener::View RunnableCacheEvent::getMapListener() const
    {
    return f_hMapListener;
    }

COH_CLOSE_NAMESPACE3

