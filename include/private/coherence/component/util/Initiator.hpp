/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INITIATOR_HPP
#define COH_INITIATOR_HPP

#include "coherence/lang.ns"

#include "coherence/net/OperationalContext.hpp"
#include "coherence/run/xml/XmlElement.hpp"

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::PofConnection;
using coherence::net::OperationalContext;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionInitiator;
using coherence::run::xml::XmlElement;


/**
* Base definition of a ConnectionAcceptor.
*
* @author nsa  2008.04.11
*/
class COH_EXPORT Initiator
    : public abstract_spec<Initiator,
        extends<Peer>,
        implements<ConnectionInitiator> >
    {
    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * @internal
        */
        Initiator();


    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Factory method: create and configure a new ConnectionInitiator for
        * the given configuration.
        *
        * @param vXml      the XmlElement used to create and configure a new
        *                  ConnectionInitiator
        * @param vContext  the OperationalContext for the newly created
        *                  ConnectionInitiator
        *
        * @return a new ConnectionInitiator
        *
        * @throws IllegalArgumentException if the given XmlElement is not a
        *         valid ConnectionInitiator configuration element
        *
        * @see findInitiatorConfig
        */
        static ConnectionInitiator::Handle createInitiator(
                XmlElement::View vXml,
                OperationalContext::View vContext);

        /**
        * Return a child XmlElement of the given XmlElement that can be used
        * to create and configure a new ConnectionInitiator. The given
        * XmlElement must have a child element with one of the following
        * names:
        *
        * (1) tcp-initiator: used to create and configure a new TcpInitiator
        *
        * @param vXml  the parent XmlElement of the XmlElement used to create
        *              and configure a new ConnectionInitiator
        *
        * @return a child XmlElement that can be used to create and
        *         configure a new ConnectionInitiator or null if no such
        *         XmlElement exists
        *
        * @throws IllegalArgumentException if the given XmlElement does not
        *         have a valid ConnectionInitiator configuration child
        *         element
        */
        static XmlElement::Handle findInitiatorConfig(XmlElement::View vXml);


    // ----- Initiator interface --------------------------------------------

    protected:
        /**
        * Open and return a new Connection.
        *
        * @return a newly opened Connection
        */
        virtual PofConnection::Handle openConnection();


    // ----- Peer interface -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void checkPingTimeouts();

        /**
        * {@inheritDoc}
        */
        virtual void onConnectionClosed(PofConnection::Handle hConnection);

        /**
        * {@inheritDoc}
        */
        virtual void onConnectionError(PofConnection::Handle hConnection, Exception::Holder ohe);

        /**
        * {@inheritDoc}
        */
        virtual void onConnectionOpened(PofConnection::Handle hConnection);

        /**
        * {@inheritDoc}
        */
        virtual void openConnection(PofConnection::Handle hConnection);

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onServiceStopped();

        /**
        * {@inheritDoc}
        */
        virtual void ping();


    // ----- Service interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void configure(XmlElement::View vXml);

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onServiceStopping();


    // ----- ConnectionInitiator interface ----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Connection::Handle ensureConnection();


    // ----- Peer child component factory -----------------------------------

    protected:
        /**
        * Factory pattern: create a Protocol instance.
        *
        * @return the new Protocol
        */
        virtual Protocol::View instantiateProtocol() const;


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the Connection managed by this Initiator.
        *
        * @return the Connection managed by this Initiator
        */
        virtual PofConnection::View getConnection() const;

        /**
        * Return the Connection managed by this Initiator.
        *
        * @return the Connection managed by this Initiator
        */
        virtual PofConnection::Handle getConnection();

        /**
        * Set the Connection managed by this Initiator.
        *
        * @param hConnection  the Connection managed by this Initiator
        */
        virtual void setConnection(PofConnection::Handle hConnection);

        /**
        * Return the maximum amount of time (in milliseconds) that the
        * Initiator will wait for a new Connection to be established.
        *
        * @return the maximum amount of time (in milliseconds) that the
        * Initiator will wait for a new Connection to be established
        */
        virtual int64_t getConnectTimeout() const;

        /**
        * Set the maximum amount of time (in milliseconds) that the
        * Initiator will wait for a new Connection to be established.
        *
        * @param ldtTimeout  the maximum amount of time (in milliseconds) that
        *                    the Initiator will wait for a new Connection to
        *                    be established
        */
        virtual void setConnectTimeout(int64_t ldtTimeout);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Connection managed by this Initiator.
        *
        * @see coherence::net::messaging::ConnectionInitiator::ensureConnection
        */
        MemberHandle<PofConnection> m_hConnection;

        /**
        * The maximum amount of time (in milliseconds) that the Initiator will
        * wait for a new Connection to be established. If 0, the Initiator will
        * wait indefinitely. This property defaults to the value of the
        * RequestTimeout property.
        */
        int64_t m_ldtConnectTimeout;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_INITIATOR_HPP
