/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TCP_POF_CONNECTION_HPP
#define COH_TCP_POF_CONNECTION_HPP

#include "coherence/lang.ns"

#include "private/coherence/component/net/extend/PofConnection.hpp"
#include "private/coherence/net/Socket.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::net::Socket;


/**
* Connection implementation that wraps a TCP/IP Socket.
*
* @author nsa 2008.04.15
*/
class COH_EXPORT TcpPofConnection
    : public class_spec<TcpPofConnection,
        extends<PofConnection> >
    {
    friend class factory<TcpPofConnection>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TcpPofConnection.
        */
        TcpPofConnection();


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- accessor methods -----------------------------------------------

    public:
        /**
        * Return the Socket wrapped by this Connection.
        *
        * @return the wrapped Socket
        */
        virtual Socket::View getSocket() const;

        /**
        * Return the Socket wrapped by this Connection.
        *
        * @return the wrapped Socket
        */
        virtual Socket::Handle getSocket();

        /**
        * Configure the Socket wrapped by this Connection.
        *
        * @param hSocket  the wrapped Socket
        */
        virtual void setSocket(Socket::Handle hSocket);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Socket wrapped by this Connection.
        */
        FinalHandle<Socket> f_hSocket;
    };


COH_CLOSE_NAMESPACE4

#endif // COH_TCP_POF_CONNECTION_HPP
