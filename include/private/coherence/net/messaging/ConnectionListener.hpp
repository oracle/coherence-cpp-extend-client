/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONNECTION_LISTENER_HPP
#define COH_CONNECTION_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/EventListener.hpp"

#include "private/coherence/net/messaging/ConnectionEvent.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::util::EventListener;


/**
* A ConnectionListener receives events pertaining to {@link Connection}
* objects managed by a {@link ConnectionManager}.
* <p/>
* Before a ConnectionListener can start to receive events, it must be
* registered with a ConnectionManager. Once registered, the
* ConnectionListener will receive events pertaining to all Connection objects
* managed by the ConnectionManager until it is unregistered or the
* ConnectionManager is terminated.
* <p/>
* Implementations are not required to be thread-safe.
*
* @author jh  2007.12.19
*
* @see Connection
* @see ConnectionManager
* @see ConnectionEvent
*/
class COH_EXPORT ConnectionListener
    : public interface_spec<ConnectionListener,
        implements<EventListener> >
    {
    // ----- ConnectionListener interface -----------------------------------

    public:
        /**
        * Invoked after a Connection has been successfully established.
        *
        * @param vEvt  the {@link ConnectionEvent#connection_opened} event
        */
        virtual void connectionOpened(ConnectionEvent::View vEvt) = 0;

        /**
        * Invoked after a Connection is closed.
        *
        * @param vEvt  the {@link ConnectionEvent#connection_closed} event
        * <p/>
        * After this event is raised, any attempt to use the Connection (or
        * any Channel created by the Connection) may result in an exception.
        */
        virtual void connectionClosed(ConnectionEvent::View vEvt) = 0;

        /**
        * Invoked when the ConnectionManager detects that the underlying
        * communication channel has been closed by the peer, severed, or
        * become unusable.
        * <p/>
        * After this event is raised, any attempt to use the Connection (or
        * any Channel created by the Connection) may result in an exception.
        *
        * @param vEvt  the {@link ConnectionEvent#connection_error} event
        */
        virtual void connectionError(ConnectionEvent::View vEvt) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONNECTION_LISTENER_HPP
