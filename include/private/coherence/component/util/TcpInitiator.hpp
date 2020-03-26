/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_INITIATOR_HPP
#define COH_TCP_INITIATOR_HPP

#include "coherence/lang.ns"

#include "coherence/run/xml/XmlElement.hpp"
#include "coherence/native/NativeAtomic32.hpp"
#include "coherence/net/AddressProvider.hpp"
#include "coherence/net/ConfigurableAddressProvider.hpp"
#include "coherence/net/InetSocketAddress.hpp"

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/component/net/extend/TcpPofConnection.hpp"
#include "private/coherence/component/util/Initiator.hpp"
#include "private/coherence/io/InputStream.hpp"
#include "private/coherence/io/OutputStream.hpp"
#include "private/coherence/net/Socket.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::io::InputStream;
using coherence::io::OutputStream;
using coherence::component::net::extend::PofConnection;
using coherence::native::NativeAtomic32;
using coherence::net::AddressProvider;
using coherence::net::InetSocketAddress;
using coherence::net::Socket;
using coherence::run::xml::XmlElement;


/**
* A ConnectionInitiator implementation that initiates Connections over
* TCP/IP.
*
* @author nsa  2008.04.16
*/
class COH_EXPORT TcpInitiator
    : public class_spec<TcpInitiator,
        extends<Initiator> >
    {
    friend class factory<TcpInitiator>;

    // ----- handle definitions (needed for nested classes) -----------------

    public:
        typedef this_spec::Handle Handle;
        typedef this_spec::View   View;
        typedef this_spec::Holder Holder;


    // ----- constructor ----------------------------------------------------

    protected:
        /**
        * Create a new TcpInitiator instance.
        */
        TcpInitiator();


    // ----- TcpInitiator interface -----------------------------------------

    protected:
        /**
        * Configure the given Socket.
        *
        * @param hSocket  the Socket to configure
        */
        virtual void configureSocket(Socket::Handle hSocket) const;

        /**
        * Factory method: create and configure a new Socket.
        *
        * @return a new Socket
        */
        virtual Socket::Handle instantiateSocket() const;

        /**
        * Parse the given XmlElement as a local InetSocketAddress. If the
        * specified XmlElement contains an empty address, null is returned.
        *
        * @param vXml            the XmlElement to parse
        *
        * @return a new InetSocketAddress representing the contents of the
        *         given XmlElement
        */
        static InetSocketAddress::Handle parseLocalSocketAddress(
                XmlElement::View vXml);


    // ----- Initiator interface --------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual PofConnection::Handle openConnection();


    // ----- Peer interface -------------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual PofConnection::Handle instantiateConnection();

        /**
        * {@inheritDoc}
        */
        using Peer::openConnection;


    // ----- Service interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void configure(XmlElement::View vXml);


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
        * Return whether or not keep-alive is enabled on Socket objects
        * created by this TcpInitiator.
        *
        * @return whether or not keep-alive is enabled on Socket objects
        *         created by this TcpInitiator
        */
        virtual bool isKeepAliveEnabled() const;

        /**
        * Return the linger timeout of a Socket created by this TcpInitiator.
        *
        * @return the linger timeout of a Socket created by this TcpInitiator
        */
        virtual int64_t getLingerTimeout() const;

        /**
        * Return the local InetSocketAddress that all Socket objects created
        * by this TcpInitiator will be bound to.
        *
        * @return the local InetSocketAddress that all Socket objects created
        *         by this TcpInitiator will be bound to.
        */
        virtual InetSocketAddress::Handle getLocalAddress();

        /**
        * Return the local InetSocketAddress that all Socket objects created
        * by this TcpInitiator will be bound to.
        *
        * @return the local InetSocketAddress that all Socket objects created
        *         by this TcpInitiator will be bound to.
        */
        virtual InetSocketAddress::View getLocalAddress() const;

        /**
        * Return whether Socket objects created by this TcpInitiator that are
        * explicitly bound to a local InetSocketAddress will be bound even if
        * a previously bound Socket is in a timeout state.
        *
        * @return whether Socket objects created by this TcpInitiator that are
        *         explicitly bound to a local InetSocketAddress will be bound
        *         even if a previously bound Socket is in a timeout state.
        */
        virtual bool isLocalAddressReusable() const;

        /**
        * Return the size of the receive buffer (in bytes) of all Socket
        * objects created by this TcpInitiator.
        *
        * @return the size of the receive buffer (in bytes) of all Socket
        *         objects created by this TcpInitiator.
        */
        virtual int64_t getReceiveBufferSize() const;

        /**
        * Obtain the AddressProvider used by the TcpInitiator to obtain the
        * address(es) of the remote TcpAcceptor(s) that it will connect to.
        *
        * @return the AddressProvider used by the TcpInitiator to obtain the
        *         address(es) of the remote TcpAcceptor(s) that it will
        *         connect to.
        */
        virtual AddressProvider::View getRemoteAddressProvider() const;

       /**
        * Obtain the AddressProvider used by the TcpInitiator to obtain the
        * address(es) of the remote TcpAcceptor(s) that it will connect to.
        *
        * @return the AddressProvider used by the TcpInitiator to obtain the
        *         address(es) of the remote TcpAcceptor(s) that it will
        *         connect to.
        */
        virtual AddressProvider::Handle getRemoteAddressProvider();

        /**
        * Return The size of the send buffer (in bytes) of all Socket objects
        * created by this TcpInitiator.
        *
        * @return the size of the send buffer (in bytes) of all Socket
        *         objects created by this TcpInitiator.
        */
        virtual int64_t getSendBufferSize() const;

        /**
        * Return whether or not TCP delay (Nagle's algorithm) is enabled on
        * Socket objects created by this TcpInitiator.
        *
        * @return whether or not TCP delay (Nagle's algorithm) is enabled on
        *         Socket objects created by this TcpInitiator.
        */
        virtual bool isTcpDelayEnabled() const;

        /**
        * Set the property for the RemoteAddressProvider used to obtain the
        * address(es) of the remote TcpAcceptor(s) that it will connect to
        *
        * @param hAddr  the AddressProvider used by the TcpInitiator to
        *               obtain the address(es) of the remote TcpAcceptor(s)
        *               that it will connect to.
        */
        virtual void setRemoteAddressProvider(AddressProvider::Handle hAddr);

        /** 
        * Return whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        *
        * @return true if the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService
        *
        * @since Coherence 12.1.2
        */
        virtual bool isNameServiceAddressProvider() const;
        
        /**
        * Configure whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        * 
        * @param fNameServiceAddressProvider  true if the remote AddressProvider 
        *                                     address are to be use to look up 
        *                                     the remote address of the ProxyService
        *
        * @since Coherence 12.1.2
        */
        virtual void setNameServiceAddressProvider(bool fNameServiceAddressProvider);

        /** 
        * Return the subport this TcpInitiator connects to.
        *
        * @return the subport
        *
        * @since Coherence 12.1.2
        */
        virtual int32_t getSubport() const;
        
        /**
        * Configure the subport this TcpInitiator connects to.
        * 
        * @param nSubport  the subport
        *
        * @since Coherence 12.1.2
        */
        virtual void setSubport(int32_t nSubport);

    protected:
        /**
        * Set whether or not keep-alive is enabled on Socket objects
        * created by this TcpInitiator.
        *
        * @param fEnabled  whether or not keep-alive is enabled on Socket
        *                  objects created by this TcpInitiator
        */
        virtual void setKeepAliveEnabled(bool fEnabled);

        /**
        * Return the linger timeout of a Socket created by this TcpInitiator.
        *
        * @return the linger timeout of a Socket created by this TcpInitiator
        */
        virtual void setLingerTimeout(int64_t cMillis);

        /**
        * Set the local InetSocketAddress that all Socket objects created
        * by this TcpInitiator will be bound to.
        *
        * @param hAddr  the local InetSocketAddress that all Socket objects
        *               created by this TcpInitiator will be bound to.
        */
        virtual void setLocalAddress(InetSocketAddress::Handle hAddr);

        /**
        * Set whether Socket objects created by this TcpInitiator that are
        * explicitly bound to a local InetSocketAddress will be bound even if
        * a previously bound Socket is in a timeout state.
        *
        * @param fReusable  whether Socket objects created by this
        *                   TcpInitiator that are explicitly bound to a local
        *                   InetSocketAddress will be bound even if a
        *                   previously bound Socket is in a timeout state.
        */
        virtual void setLocalAddressReusable(bool fReusable);

        /**
        * Set the size of the receive buffer (in bytes) of all Socket
        * objects created by this TcpInitiator.
        *
        * @param cb  the size of the receive buffer (in bytes) of all Socket
        *            objects created by this TcpInitiator.
        */
        virtual void setReceiveBufferSize(int64_t cb);

        /**
        * Set the size of the send buffer (in bytes) of all Socket objects
        * created by this TcpInitiator.
        *
        * @param lSendBufferSize  the size of the send buffer (in bytes) of
        *                         all Socket objects created by this
        *                         TcpInitiator.
        */
        virtual void setSendBufferSize(int64_t lSendBufferSize);

        /**
        * Set whether or not TCP delay (Nagle's algorithm) is enabled on
        * Socket objects created by this TcpInitiator.
        *
        * @param fEnabled  whether or not TCP delay (Nagle's algorithm) is
        *                  enabled on Socket objects created by this
        *                  TcpInitiator.
        */
        virtual void setTcpDelayEnabled(bool fEnabled);


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- helper methods -------------------------------------------------

    public:
        /**
        * Maximum size32_t value.
        */
        static const size32_t npos = size32_t(-1);

        /**
        * Read a message length from the given Socket as a packed integer
        * value.
        *
        * @param hIn  the SocketInput to read from
        *
        * @return the message length read from the SocketInput or npos
        *         if the ByteBuffer does not contain a complete
        *         packed integer value
        *
        * @throws IllegalArgumentException if the ByteBuffer contains a
        *         non-positive packed integer value
        */
        static int32_t readMessageLength(InputStream::Handle hIn);

        /**
        * Write a message length to the given Socket as a packed integer
        * value.
        *
        * @param hOut  the SocketOutput to write to
        * @param cb    the size of the message in bytes
        */
        static void writeMessageLength(OutputStream::Handle hOut,
                size32_t cb);


    // ----- nested class: TcpConnection --------------------------------

    public:
        /**
        * Connection implementation that wraps a TCP/IP Socket.
        */
        class COH_EXPORT TcpConnection
            : public class_spec<TcpConnection,
                extends<coherence::component::net::extend::TcpPofConnection> >
            {
            friend class factory<TcpConnection>;

            // ----- handle definitions (needed for nested classes) -----

            public:
                typedef this_spec::Handle Handle;
                typedef this_spec::View   View;
                typedef this_spec::Holder Holder;


            // ----- constructor ----------------------------------------

            protected:
                /**
                * Create a new TcpConnection instance.
                */
                TcpConnection();


            // ----- nested class: TcpReader ------------------------

            public:
                class COH_EXPORT TcpReader
                    : public class_spec<TcpReader,
                        extends<Daemon> >
                    {
                    friend class factory<TcpReader>;

                    // ----- constructor ----------------------------

                    protected:
                        /**
                        * Create a new TcpInitiator instance.
                        */
                        TcpReader(TcpConnection::Handle vConnection);


                    // ----- Daemon interface -----------------------

                    public:
                        /**
                        * {@inheritDoc}
                        */
                        virtual String::View getThreadName() const;

                    protected:
                        /**
                        * {@inheritDoc}
                        */
                        virtual void onException(Exception::Holder ohe);

                        /**
                        * {@inheritDoc}
                        */
                        virtual void onNotify();

                        /**
                        * {@inheritDoc}
                        */
                        virtual void onWait();


                    // ----- data members ---------------------------

                    protected:
                        /**
                        * Reference back to the TcpConnection component.
                        */
                        WeakHandle<TcpConnection> m_whTcpConnection;
                    };


            // ----- PofConnection methods ------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool closeInternal(bool fNotify,
                         Exception::Holder ohe, int64_t cMillis);

                /**
                * {@inheritDoc}
                */
                virtual void openInternal();

                /**
                * {@inheritDoc}
                */
                virtual void send(WriteBuffer::View vwb);


            // ----- accessor methods -----------------------------------

            public:
                /**
                * Get the socket input stream.
                *
                * @return an InputStream
                */
                virtual InputStream::Handle getInputStream();

                /**
                * Get the socket output stream.
                *
                * @return an OutputStream
                */
                virtual OutputStream::Handle getOutputStream();

                /**
                * Return the Reader Daemon.
                *
                * @return the Reader Daemon
                */
                virtual TcpReader::Handle getReader();

                /**
                * Return whether or not this TcpConnection has been or 
                * should be redirected.
                *
                * @return true if this TcpConnection has been or should
                *         be redirected
                */
                virtual bool isRedirect() const;

                /**
                * Configure whether or not this TcpConnection has been or
                * should be redirected.
                * 
                * @param fRedirect  true if this TcpConnection has been
                *                   or should be redirected
                */
                void setRedirect(bool fRedirect);

                /**
                * Return a list of TCP/IP addresses that the 
                * TcpConnection should be redirected to. Each element of
                * the list is a two element array, with the first element
                * being the IP address in string format and the second 
                * being theport number. 
                *
                * @return a list of TCP/IP addresses that the 
                *         TcpConnection should be redirected to
                */
                virtual List::View getRedirectList() const;

                /**
                * Configure the list of TCP/IP addresses that the
                * TcpConnection should be redirected to. Each element
                * of the list is a two element array, with the first
                * element being the IP address in string format and the
                * second being the port number. 
                *
                * @param vList  a list of TCP/IP addresses that the 
                *               TcpConnection should be redirected to
                */
                void setRedirectList(List::View vList);

                /**
                * Set the socket output stream.
                *
                * @param hIn  the OutputStream
                */
                virtual void setOutputStream(OutputStream::Handle hOut);

            protected:
                /**
                * Set the socket input stream.
                *
                * @param hIn  the InputStream
                */
                virtual void setInputStream(InputStream::Handle hIn);

                /**
                * Set the Reader Daemon.
                *
                * @param hReader  the Reader Daemon
                */
                virtual void setReader(TcpReader::Handle hReader);


            // ----- TcpConnection child component factory --------------

            protected:
                /**
                * Factory pattern: create a TcpReader instance.
                *
                * @return the new TcpReader
                */
                virtual TcpReader::Handle instantiateTcpReader();


            // ----- data members ---------------------------------------

            protected:
                /**
                * The socket input stream
                */
                FinalHandle<InputStream> f_hInput;

                /**
                * The socket output stream
                */
                FinalHandle<OutputStream> f_hOutput;

                /**
                * Reader Daemon responsible for reading data off the socket
                */
                MemberHandle<TcpReader> m_hReader;

                /**
                * Tracks the number of threads concurrently sending
                * messages.
                */
                NativeAtomic32 m_cConcurrentWriters;

                /**
                * Flag that indicates redirect; true if the TcpConnection 
                * has been or should be redirected.
                */
                bool m_fRedirect;

                /**
                * A list of TCP/IP addresses that the TcpConnection 
                * should be redirected to. Each element of the list is a
                * two element array, with the first element being the IP 
                * address in string format and the second being the port
                * number.
                */
                FinalView<List> f_vListRedirect;
            };


    // ----- data members ---------------------------------------------------

    protected:

        /**
        * Flag that indicates whether or not keep-alive is enabled on Socket
        * objects created by this TcpInitiator.
        */
        bool m_fKeepAliveEnabled;

        /**
        * The linger timeout of a Socket created by this TcpInitiator. If
        * negative, linger will be disabled. If 0, the default value will be
        * used.
        */
        int64_t m_lLingerTimeout;

        /**
        * The local InetSocketAddress that all Socket objects created by this
        * TcpInitiator will be bound to. If null, an InetSocketAddress created
        * from an ephemeral port and a valid local address will be used.
        */
        FinalHandle<InetSocketAddress> f_hLocalAddress;

        /**
        * If true, Socket objects created by this TcpInitiator that are
        * explicitly bound to a local InetSocketAddress will be bound even if
        * a previously bound Socket is in a timeout state.
        */
        bool m_fLocalAddressReusable;

        /**
        * The size of the receive buffer (in bytes) of all Socket objects
        * created by this TcpInitiator. If 0 or negative, the default receive
        * buffer size will be used.
        */
        int64_t m_lReceiveBufferSize;

        /**
        * The AddressProvider used by the TcpInitiator to obtain the
        * address(es) of the remote TcpAcceptor(s) that it will connect to.
        */
        MemberHandle<AddressProvider> m_hRemoteAddressProvider;

        /**
        * The index of the server InetSocketAddress that the next Socket will
        * be connected to.
        */
        size32_t m_iRemoteAddressIndex;

        /**
        * The size of the send buffer (in bytes) of all Socket objects
        * created by this TcpInitiator.If 0 or negative, the default send
        * buffer size will be used.
        */
        int64_t m_lSendBufferSize;

        /**
        * Flag that indicates whether or not TCP delay (Nagle's algorithm) is
        * enabled on Socket objects created by this TcpInitiator.
        */
        bool m_fTcpDelayEnabled;
        
        /**
        * Whether the remote AddressProvider address are to be used
        * to look up the remote address of the ProxyService.
        */
        bool m_fNameServiceAddressProvider;
        
        /**
        * The subport to connect to.
        */
        int32_t m_nSubport;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_TCP_INITIATOR_HPP
