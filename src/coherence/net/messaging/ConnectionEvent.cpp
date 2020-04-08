/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/messaging/ConnectionEvent.hpp"

#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionListener.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::util::Listeners;


// ----- constructors -------------------------------------------------------

ConnectionEvent::ConnectionEvent(ConnectionEvent::ConnectionView vConnection,
        Id identity, Exception::Holder ohe)
    : super(vConnection), m_id(identity), f_ohe(self(), ohe)
    {
    }


// ----- ConnectionEvent interface ------------------------------------------

void ConnectionEvent::dispatch(Listeners::View vListeners) const
    {
    if (NULL != vListeners)
        {
        ObjectArray::View vaListener = vListeners->listeners();

        for (size32_t i = vaListener->length; i > 0; )
            {
            ConnectionListener::Handle hListener =
                    cast<ConnectionListener::Handle>(vaListener[--i]);

            switch (getId())
                {
                case ConnectionEvent::connection_opened:
                    hListener->connectionOpened(this);
                    break;

                case ConnectionEvent::connection_closed:
                    hListener->connectionClosed(this);
                    break;

                case ConnectionEvent::connection_error:
                    hListener->connectionError(this);
                    break;
                }
            }
        }
    }


// ----- Object interface ---------------------------------------------------

static const size32_t COH_ID_TEXT_LENGTH = 4;
static const char*  COH_ID_TEXT[COH_ID_TEXT_LENGTH] =
    {
    "<unknown>",
    "OPENED",
    "CLOSED",
    "ERROR"
    };

TypedHandle<const String> ConnectionEvent::toString() const
    {
    String::View vs = COH_TO_STRING("ConnectionEvent{"
        << COH_ID_TEXT[getId()]
        << ' '
        << getConnection());

    Exception::View ve = getException();
    if (NULL != ve)
        {
        vs = COH_TO_STRING(vs << ' ' << ve);
        }

    return COH_TO_STRING(vs << '}');
    }


// ----- accessors ----------------------------------------------------------

ConnectionEvent::Id ConnectionEvent::getId() const
    {
    return m_id;
    }

ConnectionEvent::ConnectionView ConnectionEvent::getConnection() const
    {
    return cast<Connection::View>(getSource());
    }

Exception::Holder ConnectionEvent::getException() const
    {
    return f_ohe;
    }

COH_CLOSE_NAMESPACE3
