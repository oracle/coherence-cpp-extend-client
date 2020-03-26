/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ADDRESS_PROVIDER_HPP
#define COH_ADDRESS_PROVIDER_HPP

#include "coherence/lang.ns"

#include "coherence/net/InetSocketAddress.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::net::InetSocketAddress;

/**
* The AddressProvider is an interface that serves as a means to provide
* addresses to a consumer. Simple implementations could be backed by a
* static list; more complex ones could use dynamic discovery protocols.
*
* @author gg,jh  2008-08-14
* @author gm 2008-08-25
* @since Coherence 3.4
*/
class COH_EXPORT AddressProvider
    : public interface_spec<AddressProvider >
    {
    public:
        /**
        * Obtain a next available address to use. If the caller can
        * successfully use the returned address (e.g. a connection was
        * established), it should call the AddressProvider's
        * {@link #accept()}
        *
        * @return the next available address or NULL if the list of available
        *         addresses was exhausted
        */
        virtual InetSocketAddress::View getNextAddress() = 0;

        /**
        * This method should be called by the client immediately after it
        * determines that it can successfully use an address returned by the
        * {@link #getNextAddress} method.
        */
        virtual void accept() = 0;

        /**
        * This method should be called by the client immediately after it
        * determines that an attempt to use an address returned by the
        * {@link #getNextAddress} method has failed.
        *
        * @param oheCause (optional) an exception that carries the reason why
        *                 the the caller rejected the previously returned
        *                 address
        */
        virtual void reject(Exception::Holder oheCause) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_ADDRESS_PROVIDER_HPP
