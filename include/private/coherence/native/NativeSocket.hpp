/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_SOCKET_HPP
#define COH_NATIVE_SOCKET_HPP

#include "coherence/lang/compatibility.hpp"
#include "coherence/net/SocketAddress.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::net::SocketAddress;

/**
* NativeSocket interface.
*
* NativeSockets are non-blocking, that is any method which could block will
* accept a timeout parameer, and wait for at most that time before returnning
* control to the caller.  While this is the maximum blocking time, these
* methods may also return earlier due to insufficient data on the socket.  It
* is therefor up to the caller (coherence.net.Socket) to monitor the system
* time and issue any timeout expcetion.
*
* @author mf  2008.3.20
*/
class COH_EXPORT NativeSocket
    {
    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new NativeSocket.
        *
        * The caller is responsible for deleting the returned NativeSocket.
        *
        * @return a new NativeSocket
        */
        static NativeSocket* create();


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * @internal
        */
        virtual ~NativeSocket()
            {
            }


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The largest possible value of type size32_t.
        */
        static const size32_t npos = size32_t(-1);


    // ----- inner class Options --------------------------------------------

    public:
        /**
        * Standard socket options.
        */
        class Options
            {
            public:
                typedef enum
                    {
                    keep_alive,
                    linger,
                    receive_buffer,
                    send_buffer,
                    reuse_addr,
                    tcp_no_delay
                    } so_option_t;
            };


    // ----- NativeSocket interface -----------------------------------------

    public:
        /**
        * Set a socket option to a specific value.
        *
        * @param nOption  the socket option to set
        * @param nValue   the value to set for the option
        */
        virtual void setSocketOption(Options::so_option_t nOption,
                int32_t nValue) = 0;

        /**
        * Return the value of the specified socket option.
        *
        * @return the value of the socket option.
        */
        virtual int32_t getSocketOption(Options::so_option_t nOption) const = 0;

        /**
        * Return the locally bound address.
        *
        * @param cBytes  the assumed size of the address
        *
        * @return the locally bound address, or NULL if not bound
        */
        virtual SocketAddress::View getLocalAddress(size32_t cBytes = 128) const = 0;

        /**
        * Return the remote address.
        *
        * @param cBytes  the assumed size of the address
        *
        * @return the remote address, or NULL if not connected
        */
        virtual SocketAddress::View getRemoteAddress(size32_t cBytes = 128) const = 0;

        /**
        * Bind the socket to the specified address.
        *
        * @param vAddrLocal  the address to bind to
        */
        virtual void bind(SocketAddress::View vAddrLocal) = 0;

        /**
        * Mark the socket as accepting connections.
        *
        * @param cMaxBacklog  a hint as to how many outstanding listen requests
        *                     to queue up.
        *
        * @throws IOException if an error occurs while connecting
        */
        virtual void listen(int32_t cMaxBacklog) = 0;

        /**
        * Shutdown the socket for input, output, or both.
        *
        * @param fIn            true if input is to be shutdown
        * @param fOut           true if output is to be shutdown
        * @param fConnectError  true to raise an error if the socket is not connected
        */
        virtual void shutdown(bool fIn = true, bool fOut = true,
                              bool fConnectError = false) = 0;

        /**
        * Close the socket.
        *
        * @throws IOException if an errors occurs while closing.
        */
        virtual void close() = 0;

        /**
        * Return true if the socket has been closed.
        *
        * @return true if the socket has been closed
        */
        virtual bool isClosed() const = 0;

        /**
        * Connect the socket to a remote address.
        *
        * @param vAddrRemote     the remote address to bind to
        * @param cMillisTimeout  the maximum amount of time to wait to connect
        *
        * @return true iff connection established
        *
        * @throws IOException if an error occurs while connecting
        */
        virtual bool connect(SocketAddress::View vAddrRemote, int64_t cMillisTimeout) = 0;

        /**
        * Accept an incomming connection request.
        *
        * @param cMillisTimeout  the maximum amount of time to wait for a connection
        *
        * @return the NativeSocket associated with the new connection, or NULL
        */
        virtual NativeSocket* accept(int64_t cMillisTimeout) = 0;

        /**
         * Tries to read a cbPreferred number of bytes or being time outed
         * before exiting.
         *
         * @param ab              output byte array
         * @param cb              number of bytes to read
         * @param cMillisTimeout  the maximum amount of time to wait
         *
         * @return the number of bytes read, or npos on disconnect
         *
         * @throws IOException on IO error
         * @throws EOFException on peer disconnect
         */
         virtual size32_t read(octet_t ab[], size32_t cb,
                 int64_t cMillisTimeout) = 0;

        /**
        * Write a series of bytes to the socket.
        *
        * @param ab              intput byte array
        * @param cb              number of bytes to write
        * @param cMillisTimeout  the maximum amount of time to wait
        *
        * @return the number of bytes written, or npos on disconnect
        *
        * @throws IOException on IO error
        */
        virtual size32_t write(const octet_t ab[], size32_t cb,
                int64_t cMillisTimeout) = 0;

        /**
        * Get the number of bytes in the socket.
        *
        * @return  greater than 0 if invocation was successful otherwise an
        *          error code specific to the underlying platform.
        */
        virtual size32_t available() = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_SOCKET_HPP
