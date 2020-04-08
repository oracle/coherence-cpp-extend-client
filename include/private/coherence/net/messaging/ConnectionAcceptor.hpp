/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONNECTION_ACCEPTOR_HPP
#define COH_CONNECTION_ACCEPTOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/Collection.hpp"

#include "private/coherence/net/messaging/ConnectionManager.hpp"

COH_OPEN_NAMESPACE3(coherence,net,messaging)

using coherence::util::Collection;


/**
* A ConnectionAcceptor represents a {@link ConnectionManager} running on a
* server, and as a server, it is responsible for accepting a connection
* request initiated by a {@link ConnectionInitiator}.
* <p/>
* Before a connection can be accepted, the ConnectionAcceptor must be started
* using the {@link #start()} method. Calling this method allocates any
* necessary resources and transitions the ConnectionAcceptor to the running
* state. The ConnectionAcceptor will then accept new connections, which are
* represented by {@link Connection} object. The ConnectionAcceptor maintains
* references to accepted Connection objects until they are closed or the
* ConnectionAcceptor is {@link #shutdown gracefully} or
* {@link #stop forcibly} terminated. Terminating a ConnectionAcceptor also
* closes all accepted Connection objects.
* <p/>
* All ConnectionAcceptor implementations must be fully thread-safe.
*
* @author jh  2007.12.18
*
* @see Connection
* @see ConnectionInitiator
* @see ConnectionListener
* @see ConnectionManager
*/
class COH_EXPORT ConnectionAcceptor
    : public interface_spec<ConnectionAcceptor,
        implements<ConnectionManager> >
    {
    // ----- ConnectionAcceptor interface -----------------------------------

    public:
        /**
        * Return the collection of open Connection objects accepted by this
        * ConnectionAcceptor.
        * <p/>
        * The client should assume that the returned collection is an immutable
        * snapshot of the actual set of Connection objects maintained by this
        * ConnectionAcceptor.
        *
        * @return the collection of open Connection objects
        */
        virtual Collection::Handle getConnections() = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CONNECTION_ACCEPTOR_HPP
