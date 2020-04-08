/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERVER_SOCKET_HPP
#define COH_SERVER_SOCKET_HPP

#include "coherence/lang.ns"

#include "private/coherence/net/Socket.hpp"



COH_OPEN_NAMESPACE2(coherence,net)


/**
* ServerSocket allows for accepting incomming socket connections.
*
* @author mf  2008.03.24
*/
class COH_EXPORT ServerSocket
    : public class_spec<ServerSocket,
        extends<Socket> >
    {
    friend class factory<ServerSocket>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ServerSocket.
        *
        * @return a new ServerSocket
        */
        ServerSocket();

    private:
        /**
        * Blocked copy constructor.
        */
        ServerSocket(const ServerSocket&);


    // ----- ServerSocket interface -----------------------------------------

    public:
        /**
        * Bind the socket to a specific local address, and start listening
        * for connections.
        *
        * @param vAddrLocal  the local address to bind to, or NULL to delay
        *                    binding allowing the system to select the address.
        *
        * @throws IOException is the socket is already bound
        */
        virtual void bind(SocketAddress::View vAddrLocal);

        /**
        * Bind the socket to a specific local address, and specify the listen
        * backlog.
        *
        * @param vAddrLocal   the local address to bind to, or NULL to delay
        *                     binding allowing the system to select the address.
        * @param cMaxBacklog  the listen backlog
        *
        * @throws IOException is the socket is already bound
        */
        virtual void bind(SocketAddress::View vAddrLocal, int32_t cMaxBacklog);

        /**
        * Block waiting for a connection to accept.
        *
        * @return the accepted connection
        *
        * @throws IOException on IO error.
        */
        virtual Socket::Handle accept();
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERVER_SOCKET_HPP
