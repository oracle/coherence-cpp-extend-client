/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_UTIL_HPP
#define COH_TCP_UTIL_HPP

#include "coherence/lang.ns"

#include "coherence/net/InetSocketAddress.hpp"
#include "coherence/net/SocketAddress.hpp"

#include "private/coherence/io/OutputStream.hpp"
#include "private/coherence/net/Socket.hpp"


COH_OPEN_NAMESPACE5(coherence,component,net,extend,util)

using coherence::io::OutputStream;
using coherence::net::InetSocketAddress;
using coherence::net::Socket;
using coherence::net::SocketAddress;


/**
* A collection of TCP/IP-releated utility methods.
*
* author nsa 04.14.2008
*/
class COH_EXPORT TcpUtil
    {
    // ----- enums ----------------------------------------------------------

    public:
        /**
        * Well known subport.
        *
        * @since Coherence 12.1.2
        */
        enum WellKnownSubports
            {
            NameService = 3 // The name service subport.
            };
    

    // ----- TcpUtil interface ----------------------------------------------

    public:
        /**
        * Close the given Socket. If the Socket is closed successfully, this
        * method returns true; otherwise, this method returns false.
        *
        * @param hSocket  the Socket to close; may be null
        *
        * @return true if successful; false otherwise
        */
        static bool close(Socket::Handle hSocket);

        /**
        * Enable or disable SO_KEEPALIVE on the given Socket. The value of
        * SO_KEEPALIVE is only changed if the given value differs from the
        * current value.
        *
        * @param socket      the Socket to configure
        * @param fKeepAlive  the new value of SO_KEEPALIVE
        */
        static void setKeepAlive(Socket::Handle hSocket, bool fKeepAlive);

        /**
        * Set the receive buffer size of the given Socket to the specified
        * number of bytes. The maximum value is platform specific. The
        * receive buffer size is only changed if the given value is greater
        * than 0 and differs from the current value.
        *
        * @param hSocket  the Socket to configure
        * @param cb       the new size of the receive buffer (in bytes)
        */
        static void setReceiveBufferSize(Socket::Handle hSocket, int32_t cb);

        /**
        * Enable or disable SO_REUSEADDR on the given Socket. The value of
        * SO_REUSEADDR is only changed if the given value differs from the
        * current value. If the Socket is already bound, this method has
        * no effect.
        *
        * @param hSocket        the Socket to configure.
        * @param fReuseAddress  the value to set SO_REUSEADDR to
        */
        static void setReuseAddress(Socket::Handle hScoket, bool fReuseAddress);

        /**
        * Set the send buffer size of the given Socket to the specified
        * number of bytes. The maximum value is platform specific. The
        * send buffer size is only changed if the given value is greater
        * than 0 and differs from the current value.
        *
        * @param socket  the Socket to configure
        * @param cb      the new size of the send buffer (in bytes)
        */
        static void setSendBufferSize(Socket::Handle hSocket, int cb);

        /**
        * Enable or disable SO_LINGER with the specified linger time in
        * seconds on the given Socket. The maximum value is platform
        * specific. A value of -1 disables SO_LINGER. The value of
        * SO_LINGER is only changed if the given value is not 0 and
        * differs from the current value.
        *
        * @param socket  the Socket to configure
        * @param cSecs  the new value of SO_LINGER (in seconds)
        */
        static void setSoLinger(Socket::Handle hSocket, int32_t cSecs);

        /**
        * Enable or disable TCP_NODELAY on the given Socket. The value of
        * TCP_NODELAY is only changed if the given value differs from the
        * current value.
        *
        * @param hSocket      the Socket to configure
        * @param fKeepAlive  the new value of TCP_NODELAY
        */
        static void setTcpNoDelay(Socket::Handle hSocket, bool fTcpNoDelay);

        /**
        * Return a String representation of the given SocketAddress.
        *
        * @param vAddr  the SocketAddress
        *
        * @return a String representation of the given SocketAddress
        */
        static String::View toString(SocketAddress::View vAddr);

        /**
        * Return a String representation of the given SocketAddress
        * with optional subport.
        *
        * @param vAddr     the SocketAddress
        * @param nSubport  subport, -1 if there is none
        *
        * @return a String representation of the given SocketAddress
        *         in format ADDRESS:BASEPORT.SUBPORT
        *
        * @since Coherenence 12.1.2
        */
        static String::View toString(SocketAddress::View vAddr, 
                int32_t subport);
                
        /**
        * Write out the 8 byte multiplexed protocol header which consists
        * of two 4-byte words: first is the protocol ID 0x5AC1E000, 
        * seconde is the subport number.
        *
        * @since Coherenence 12.1.2
        */
        static void writeSubport(OutputStream::Handle hStream, int32_t nSubport);
        
        /**
        * Calculate the baseport from the 32 bit port.
        *
        * @param nPort  the 32 bit port
        *
        * @return the baseport
        *
        * @since Coherenence 12.1.2
        */
        static int32_t calculateBaseport(int32_t nPort);

        /**
        * Calculate the subport from the 32 bit port.
        *
        * @param nPort  the 32 bit port
        *
        * @return the subport
        *
        * @since Coherenence 12.1.2
        */
        static int32_t calculateSubport(int32_t nPort);

    protected:
        /**
        * Log a warning message if the given Socket or ServerSocket receive
        * or send buffer size is less than the specified number of bytes.
        *
        * @param fReceiveBuffer  if true, the specified buffer sizes are
        *                        assumed to be receive buffer sizes; false
        *                        if they are send buffer sizes
        * @param cbActual        the actual size of the buffer (in bytes)
        * @param cbRequested     the requested size of the buffer (in bytes)
        *
        */
        static void validateBufferSize(bool fReceiveBuffer,
                int32_t cbActual, int32_t cbRequested);

        /**
        * Log a warning message if the receive or send buffer size of the
        * given Socket is less than the specified number of bytes.
        *
        * @param hSocket         the target Socket
        * @param fReceiveBuffer  if true, the specified buffer size is
        *                        assumed to be a receive buffer size; false
        *                        if it is a send buffer size
        * @param cbRequested     the requested size of the buffer (in bytes)
        */
        static void validateBufferSize(Socket::Handle hSocket,
                bool fReceiver, int32_t cbRequested);

    };

COH_CLOSE_NAMESPACE5

#endif // COH_TCP_UTIL_HPP
