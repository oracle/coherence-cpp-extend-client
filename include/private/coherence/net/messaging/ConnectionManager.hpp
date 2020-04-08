/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONNECTION_MANAGER_HPP
#define COH_CONNECTION_MANAGER_HPP

#include "coherence/lang.ns"

#include "coherence/net/OperationalContext.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Service.hpp"

#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Codec.hpp"
#include "private/coherence/net/messaging/ConnectionListener.hpp"
#include "private/coherence/net/messaging/Protocol.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::net::OperationalContext;
using coherence::util::Map;
using coherence::util::Service;


/**
* The ConnectionManager is the base SPI (Service Provider Interface) for both
* {@link ConnectionAcceptor} and {@link ConnectionInitiator} implementations.
* <p/>
* Implementations of this interface use a provider-specific mechanism to
* establish a bi-directional communication channel between two endpoints,
* represented by a {@link Connection}. Some implementations restrict data
* transfer between endpoints within a single JVM, whereas others enable two
* processes to exchange data. Advanced implementations allow communication
* between processes on different machines, for example using TCP sockets or
* JMS.
* <p/>
* Before a Connection can be established between a ConnectionInitiator
* (client) and ConnectionAcceptor (server), one or more Protocol instances
* must be registered with the ConnectionManager on each. During Connection
* establishment, the ConnectionInitiator sends information about each
* registered Protocol. A compatable set of Protocol instances (or superset)
* must be registered with the acceptor's ConnectionManager in order for the
* Connection to be established.
* <p/>
* Establishing a Connection is assumed to be a heavyweight operation that may
* allocate significant resources within and outside the JVM. For example, a
* TCP-based implementation of this interface may implement a Connection using
* a persistent Socket connection with a remote server. However, once
* established, successive uses of the same Connection should be relatively
* lightweight. In other words, a Connection object, once opened, should
* appear to be persistent from the perspective of the user until closed.
* Additionally, underlying transports used by implementations must be both
* reliable and ordered.
* <p/>
* Once a Connection is established, either client or server may open a
* {@link Channel} to a {@link Channel.Receiver} registered by its peer and
* use it to send and receive {@link Message} objects to/from the peer.
*
* @author jh  2007.12.18
*
* @see Channel
* @see Channel::Receiver
* @see Connection
* @see ConnectionAcceptor
* @see ConnectionInitiator
* @see Protocol
*/
class COH_EXPORT ConnectionManager
    : public interface_spec<ConnectionManager,
        implements<Service> >
    {
    // ----- ConnectionManager interface ------------------------------------

    public:
        /**
        * Return the {@link OperationalContext} used by this ConnectionManager.
        *
        * @return the {@link OperationalContext} used by this ConnectionManager
        */
        virtual OperationalContext::View getOperationalContext() const = 0;

        /**
        * Configure the {@link OperationalContext} used by this
        * ConnectionManager.
        *
        * @param vCtx  the {@link OperationalContext} used by this
        *              ConnectionManager
        */
        virtual void setOperationalContext(OperationalContext::View vCtx) = 0;

        /**
        * Return a Protocol that was registered with this ConnectionManager.
        *
        * @param vsName  the name of the registered Protocol
        *
        * @return the registered Protocol or null if a Protocol with the
        *         given name is not registered with this ConnectionManager
        */
        virtual Protocol::View getProtocol(String::View vsName) const = 0;

        /**
        * Return a map of Protocol names to Protocol objects.
        * <p/>
        * The client should assume that the returned map is an immutable
        * snapshot of the actual map of Protocol objects maintained by this
        * ConnectionManager.
        *
        * @return an map of all registered Protocol objects, keyed by the
        *         Protocol name
        */
        virtual Map::View getProtocols() const = 0;

        /**
        * Register a Protocol with this ConnectionManager.
        * <p/>
        * This method may only be called before the ConnectionManager is
        * started.
        *
        * @param vProtocol  the new Protocol to register; if the Protocol has
        *                   already been registered, this method has no effect
        *
        * @throws IllegalStateException if the ConnectionManager is running
        */
        virtual void registerProtocol(Protocol::View vProtocol) = 0;

        /**
        * Return a Receiver that was registered with this ConnectionManager.
        * <p/>
        * The client should assume that the returned map is an immutable
        * snapshot of the actual map of Receiver objects maintained by this
        * ConnectionManager.
        *
        * @param vsName  the name of the registered Receiver
        *
        * @return the registered Protocol or null if a Protocol with the given
        *         name is not registered with this ConnectionManager
        */
        virtual Channel::Receiver::View getReceiver(String::View vsName) const = 0;

        /**
        * Return a Receiver that was registered with this ConnectionManager.
        * <p/>
        * The client should assume that the returned map is an immutable
        * snapshot of the actual map of Receiver objects maintained by this
        * ConnectionManager.
        *
        * @param vsName  the name of the registered Receiver
        *
        * @return the registered Protocol or null if a Protocol with the given
        *         name is not registered with this ConnectionManager
        */
        virtual Channel::Receiver::Handle getReceiver(String::View vsName) = 0;

        /**
        * Return a map of Receiver names to Receiver objects.
        * <p/>
        * The client should assume that the returned map is an immutable
        * snapshot of the actual map of Receiver objects maintained by this
        * ConnectionManager.
        *
        * @return an map of all registered Receiver objects, keyed by the
        *         Receiver name
        */
        virtual Map::View getReceivers() const = 0;

        /**
        * Register a Receiver that will received unsolicited Message objects
        * sent through Channel objects associated with the Receiver name and
        * Protocol. <p/>
        * This method may only be called before the ConnectionManager is
        * started.
        *
        * @param hReceiver  the new Receiver to register; if the Receiver has
        *                  already been registered, this method has no effect
        *
        * @throws IllegalStateException if the ConnectionManager is running
        */
        virtual void registerReceiver(Channel::Receiver::Handle hReceiver) = 0;

        /**
        * Return the Codec that will be used to encode and decode Messages
        * sent through Connections managed by this ConnectionManager.
        *
        * @return the configured Codec
        */
        virtual Codec::Handle getCodec() = 0;

        /**
        * Return the Codec that will be used to encode and decode Messages
        * sent through Connections managed by this ConnectionManager.
        *
        * @return the configured Codec
        */
        virtual Codec::View getCodec() const = 0;

        /**
        * Configure the Codec that will be used to encode and decode Messages
        * sent through Connections managed by this ConnectionManager.
        *
        * @param hCodec  the Codec associated with Connection objects
        *
        * @throws IllegalStateException if the ConnectionManager is running
        */
        virtual void setCodec(Codec::Handle hCodec) = 0;

        /**
        * Register a ConnectionListener that will receive events pertaining
        * to the Connection objects managed by this ConnectionManager.
        *
        * @param hListener  the new ConnectionListener to register; if the
        *                   listener has already been registered, this method
        *                   has no effect
        */
        virtual void addConnectionListener(ConnectionListener::Handle hListener) = 0;

        /**
        * Unregister a ConnectionListener from this ConnectionManager.
        * <p/>
        * After a ConnectionListener is removed, it will no longer receive
        * events pertaining to the Connection objects managed by this
        * ConnectionManager.
        *
        * @param listener  the ConnectionListener to deregister; if the
        *                  listener has not previously been registered, this
        *                  method has no effect
        */
        virtual void removeConnectionListener(ConnectionListener::Handle hListener) = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONNECTION_MANAGER_HPP
