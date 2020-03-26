/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONNECTION_EVENT_HPP
#define COH_CONNECTION_EVENT_HPP

#include "coherence/lang.ns"

#include "coherence/util/Event.hpp"
#include "coherence/util/Listeners.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::util::Event;
using coherence::util::Listeners;

class Connection;


/**
* An event which indicates that a {@link Connection} was:
* <ul>
*   <li>opened</li>
*   <li>closed</li>
*   <li>determined to be unusable</li>
* </ul>
* A ConnectionEvent object is passed as an argument to the
* {@link ConnectionListener} interface methods.
*
* @author jh  2007.12.19
*
* @see ConnectionListener
*/
class COH_EXPORT ConnectionEvent
    : public class_spec<ConnectionEvent,
        extends<Event> >
    {
    friend class factory<ConnectionEvent>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Connection Handle definition.
        */
        typedef TypedHandle<Connection> ConnectionHandle;

        /**
        * Connection View definition.
        */
        typedef TypedHandle<const Connection> ConnectionView;


    // ----- Id definition --------------------------------------------------

    public:
        typedef enum
            {
            connection_opened = 1, // the Connection has been established
            connection_closed = 2, // the Connection has closed
            connection_error  = 3  // the Connection has failed
            } Id;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a new ConnectionEvent.
        *
        * @param connection the Connection for which the event applies
        * @param nId        the event's ID, one of the Id enum values
        * @param e          an optional Throwable associated with the event
        */
        ConnectionEvent(ConnectionView vConnection, Id identity,
                Exception::Holder ohe = NULL);


    // ----- ConnectionEvent interface --------------------------------------

    public:
        /**
        * Dispatch this event to the specified listeners collection.
        *
        * @param vListeners the listeners collection
        *
        * @throws ClassCastException if any of the targets is not an instance of
        *         the ConnectionListener interface
        */
        virtual void dispatch(Listeners::View vListeners) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * @inheritDoc
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
        * Return the Connection associated with this event.
        *
        * @return the Connection
        */
        virtual ConnectionView getConnection() const;

        /**
        * Return the optional Exception associated with this event.
        * <p/>
        * This method will usually return a null value if the event identifier
        * is anything but {@link Id#connection_error}.
        *
        * @return the Exception
        */
        virtual Exception::Holder getException() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The event's ID.
        */
        Id m_id;

        /**
        * An optional Exception associated with the event.
        */
        FinalHolder<Exception> f_ohe;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONNECTION_EVENT_HPP
