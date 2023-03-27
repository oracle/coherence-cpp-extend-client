/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOCKET_HPP
#define COH_SOCKET_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/InputStream.hpp"
#include "private/coherence/io/OutputStream.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/net/SocketAddress.hpp"
#include "coherence/io/WriteBuffer.hpp"



COH_OPEN_NAMESPACE2(coherence,native)
class NativeSocket;
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::InputStream;
using coherence::io::OutputStream;
using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::native::NativeSocket;


/**
* Socket provides a managed Object interface to client side network sockets.
*
* Note: Sockets must be explicitly closed in order to free all associated
*       resources.
*
* @author mf  2008.03.13
*/
class COH_EXPORT Socket
    : public class_spec<Socket>
    {
    friend class factory<Socket>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a Socket from a NativeSocket.  This is used by ServerSocket
        * as it accepts new connections.
        *
        * @return a new Socket
        */
        Socket(NativeSocket* pNative = NULL);

        /**
        * @internal
        */
        virtual ~Socket();

    private:
        /**
        * Blocked copy constructor.
        */
        Socket(const Socket&);


    // ----- Socket interface -----------------------------------------------

    public:
        /**
        * Bind the socket to a specific local address.
        *
        * @param vAddrLocal  the local address to bind to, or NULL to delay
        *                    binding allowing the system to select the address.
        *
        * @throws IOException is the socket is already bound
        */
        virtual void bind(SocketAddress::View vAddrLocal);

        /**
        * Return true if the socket has been bound.
        *
        * @return true if the socket has been bound
        */
        virtual bool isBound() const;

        /**
        * Return the local address to which the Socket is bound, or NULL if
        * the Socket is not bound.
        *
        * @return the local address to which the socket is bound
        */
        virtual SocketAddress::View getLocalSocketAddress() const;

        /**
        * Connect the socket to the specified endpoint address.
        *
        * @param vAddrRemote     the endpoint to connect to
        * @param cMillisTimeout  the maximum number of milliseconds to wait
        *                        to establish the connection, or zero for
        *                        indefinite
        *
        * @throws IOException if an error or timeout occurs while connecting
        */
        virtual void connect(SocketAddress::View vAddrRemote,
                             int64_t cMillisTimeout = 0);

        /**
        * Return true if the socket is currently in the connected state.
        *
        * @return true if the socket is connected
        */
        virtual bool isConnected() const;

        /**
        * Return the remote address to which the Socket is connected, or NULL
        * if the Socket is not connected.
        *
        * @return the remote address to which the Socket is connected.
        */
        virtual SocketAddress::View getRemoteSocketAddress() const;

        /**
        * Close the socket.
        *
        * Once closed a socket is no longer useable for networking, and cannot
        * be reconnected.
        */
        virtual void close();

        /**
        * Return true if the socket is in the closed state.
        *
        * @return true if the socket is closed
        */
        virtual bool isClosed() const;

        /**
        * Set the Socket's keep alive flag
        *
        * @param fKeepAlive  true if keep alive is to be enabled
        */
        virtual void setKeepAlive(bool fKeepAlive);

        /**
        * Return the Socket's keep alive setting.
        *
        * @return true iff keep alive is enabled
        */
        virtual bool getKeepAlive() const;

        /**
        * Set the Socket's linger time.
        *
        * @param fLinger     true iff linger is to be enabled
        * @param cLingerSec  the number of seconds to linger
        */
        virtual void setSoLinger(bool fLinger, int32_t cLingerSec);

        /**
        * Return the Socket's linger time.
        *
        * @return the linger time in seconds, or a negative value if disabled
        */
        virtual int32_t getSoLinger() const;

        /**
        * Set the Socket's reuse address flag
        *
        * @param fReuse  true if reuse address is to be enabled
        */
        virtual void setReuseAddress(bool fReuse);

        /**
        * Return the Socket's reuse address setting.
        *
        * @return true iff reuse is enabled
        */
        virtual bool getReuseAddress() const;

        /**
        * Set the Socket's no delay flag
        *
        * @param fNoDelay  true if no delay is to be enabled
        */
        virtual void setTcpNoDelay(bool fNoDelay);

        /**
        * Return the Socket's no delay setting.
        *
        * @return true iff no delay is enabled
        */
        virtual bool getTcpNoDelay() const;

        /**
        * Set the Socket's receive buffer size.
        *
        * @param cBytes  the buffer size in bytes
        */
        virtual void setReceiveBufferSize(int32_t cBytes);

        /**
        * Return the Socket's receive buffer size.
        *
        * @return the buffer size in bytes
        */
        virtual int32_t getReceiveBufferSize() const;

        /**
        * Set the Socket's send buffer size.
        *
        * @param cb  the buffer size in bytes
        */
        virtual void setSendBufferSize(int32_t cb);

        /**
        * Return the Socket's send buffer size.
        *
        * @return the buffer size in bytes
        */
        virtual int32_t getSendBufferSize() const;

        /**
        * Set the Socket's send and receive timeout.
        *
        * @param cMillis  the timeout, or 0 to disable the timeout
        */
        virtual void setSoTimeout(int32_t cMillis);

        /**
        * Return the Socket's send and receive timeout.
        *
        * @return the send and receive timeout
        */
        virtual int32_t getSoTimeout() const;

        /**
        * Set the amount of time to wait in a write call before throwing a
        * SocketTimeoutException.
        *
        * @param cMillis  the timeout, or 0 to disable the timeout
        *
        * @since 14.1.1.0.13
        */
        virtual void setSendTimeout(int32_t cMillis);

        /**
        * Get the amount of time to wait in a write call before throwing a
        * SocketTimeoutException.
        *
        * @since 14.1.1.0.13
        */
        virtual int32_t getSendTimeout() const;

        // ----- inner class: SocketInput -----------------------------------

        public:
            /**
            * SocketInput provides an interface for reading data from a socket.
            */
            class COH_EXPORT SocketInput
                : public class_spec<SocketInput,
                    extends<InputStream> >
                {
                friend class factory<SocketInput>;

                // ----- constructors -----------------------------------

                protected:
                    /**
                    * Create a new SocketInput.
                    *
                    * @param hSocket the socket to read from
                    */
                    SocketInput(Socket::Handle hSocket);


                // ----- InputStream interface --------------------------

                public:
                    /**
                    * {@inheritDoc}
                    */
                    virtual octet_t read();

                    /**
                    * {@inheritDoc}
                    */
                    virtual size32_t read(Array<octet_t>::Handle hab,
                                        size32_t i = 0, size32_t cb = npos);

                    /**
                    * {@inheritDoc}
                    */
                    virtual void close();

                    /**
                    * {@inheritDoc}
                    */
                    virtual size32_t available();

                // ----- Object interface -------------------------------

                public:
                    /**
                    * {@inheritDoc}
                    */
                    virtual TypedHandle<const String> toString() const;

                // ----- data members -----------------------------------

                protected:
                    /**
                    * The associated socket.
                    */
                    FinalHandle<Socket> f_hSocket;

                // ----- friends ----------------------------------------

                friend class Socket;
                };

            /**
            * Return the SocketInput for this socket.
            *
            * @return the SocketInput.
            */
            virtual InputStream::Handle getInputStream();


        // ----- inner class: SocketOutput ----------------------------------

        public:
            /**
            * SocketOutput provides an interface for writting data to a socket.
            */
            class COH_EXPORT SocketOutput
                : public class_spec<SocketOutput,
                    extends<OutputStream> >
                {
                friend class factory<SocketOutput>;

                // ----- constructors -----------------------------------

                protected:
                    /**
                    * Create a new SocketOutput.
                    *
                    * @param hSocket the socket to write to
                    */
                    SocketOutput(Socket::Handle hSocket);

                // ----- SocketOutput interface -------------------------

                public:
                    /**
                    * {@inheritDoc}
                    */
                    virtual void write(octet_t b);

                    /**
                    * {@inheritDoc}
                    */
                    virtual void write(Array<octet_t>::View vab, size32_t i = 0,
                               size32_t cb = npos);

                    /**
                    * {@inheritDoc}
                    */
                    virtual void close();

                // ----- Object interface -------------------------------

                public:
                    /**
                    * {@inheritDoc}
                    */
                    virtual TypedHandle<const String> toString() const;

                // ----- data members -----------------------------------

                protected:
                    /**
                    * The associated socket.
                    */
                    FinalHandle<Socket> f_hSocket;

                // ----- friends ----------------------------------------

                friend class Socket;
                };

        /**
        * Return the SocketOutput for this socket.
        *
        * @return the SocketOutput.
        */
        virtual OutputStream::Handle getOutputStream();

    protected:
        /**
        * Return the NativeSocket.
        *
        * @param fEnsureConnected  true if the socket must also be connected
        *
        * @return the NativeSocket
        *
        * @throw IOException if the socket has been closed.
        */
        virtual NativeSocket* ensureNativeSocket(bool fEnsureConnected = false);

        /**
        * Return the NativeSocket.
        *
        * @return the NativeSocket
        *
        * @throw IOException if the socket has been closed.
        */
        virtual const NativeSocket* ensureNativeSocket() const;

        /**
        * Read up to <code>cb</code> octets and store them in
        * <code>ab</code>.
        *
        * @param ab      the array to store the octets which are read
        *                from the stream
        * @param cb      the number of bytes to read
        *
        * @return  the number of actual octets read.
        *
        * @throws EOFException if the stream is exhausted before
        *         the number of octets indicated by the array
        *         length could be read
        * @throws IOException if an I/O error occurs
        */
        virtual size32_t readInternal(octet_t* ab, size32_t cb);

        /**
        * Writes cb octets from the array <tt>ab</tt>.
        *
        * @param ab  the octet array to write
        * @param cb  the number of octets to write
        *
        * @throws IOException if an I/O error occurs
        */
        virtual void writeInternal(const octet_t* ab, size32_t cb);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * @{inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The native Socket which backs this socket.
        */
        NativeSocket* m_pNative;

        /**
        * The local address to which the Socket is bound.
        */
        mutable FinalView<SocketAddress> f_vAddrLocal;

        /**
        * The remote address to which the Socket is connected.
        */
        mutable FinalView<SocketAddress> f_vAddrRemote;

        /**
        * The configured socket timeout.
        */
        int32_t m_cMillisTimeout;

        /**
        * The send timeout.
        *
        * @since 14.1.1.0.13
        */
        int32_t m_cMillisSendTimeout;

        /**
        * The current SocketInput for this Socket.
        */
        MemberHandle<SocketInput> m_hInput;

        /**
        * The current SocketOutput for this Socket.
        */
        MemberHandle<SocketOutput> m_hOutput;

        /**
        * True if the socket has been closed.
        */
        bool m_fClosed;


    // ----- friends --------------------------------------------------------

    friend class SocketInput;
    friend class SocketOutput;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SOCKET_HPP
