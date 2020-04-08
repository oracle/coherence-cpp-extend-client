/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INET_SOCKET_ADDRESS_HPP
#define COH_INET_SOCKET_ADDRESS_HPP

#include "coherence/lang.ns"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/SocketAddress.hpp"



COH_OPEN_NAMESPACE2(coherence,net)


/**
* InetSocketAddress represents an endpoint of a IP socket connection.
*
* @author mf  2008.03.13
*/
class COH_EXPORT InetSocketAddress
    : public abstract_spec<InetSocketAddress,
        extends<Object>,
        implements<SocketAddress> >
    {
    friend class factory<InetSocketAddress>;

    // ----- factory methods ------------------------------------------------

    public:
        /**
        * Create a new InetSocketAddress.
        *
        * @param vAddr  the IP address of the endpoint
        * @param nPort  the port of the endpoint
        *
        * @return a new InetSocketAddress
        */
        static InetSocketAddress::Handle create(InetAddress::View vAddr,
                uint16_t nPort);

        /**
        * Create a new InetSocketAddress.
        *
        * @param vsHost    the host name of the endpoint
        * @param nPort     the port of the endpoint
        * @param fResolve  specifies if the hostname should be resolved
        *
        * @return a new InetSocketAddress
        */
        static InetSocketAddress::Handle create(String::View vsHost,
                uint16_t nPort, bool fResolve = true);


    // ----- InetSocketAddress interface ------------------------------------

    public:
        /**
        * Return the associated InetAddress.
        *
        * @return the associated InetAddress.
        */
        virtual InetAddress::View getAddress() const = 0;

        /**
        * Return the associated port.
        *
        * @return the associated port
        */
        virtual uint16_t getPort() const = 0;

        /**
        * Return the hostname associated with the InetAddress.
        *
        * @return the associated hostname
        */
        virtual String::View getHostName() const = 0;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
        };

COH_CLOSE_NAMESPACE2

#endif // COH_INET_SOCKET_ADDRESS_HPP
