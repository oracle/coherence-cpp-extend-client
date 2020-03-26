/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_INET_ADDRESS_HELPER_HPP
#define COH_INET_ADDRESS_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/net/InetAddress.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


/**
* Helper class that encapsulates common InetAddress functionality.
*
* @author nsa 04.14.2008
*/
class COH_EXPORT InetAddressHelper
    : public abstract_spec<InetAddressHelper>
    {
    // ----- InetAddressHelper interface ------------------------------------

    public:
        /**
        * Obtain the local host address. If at all possible, ensure that the
        * returned address is not a loopback, wildcard or a link local address.
        *
        * @return the InetAddress that is the best fit for the local host
        *         address
        */
        static InetAddress::View getLocalHost();

        /**
        * Format an IP address string representing the specified InetAddress
        * object. The main difference if this method over the
        * <tt>addr.toString()</tt> call is that this implementation avoids a call
        * to the <tt>addr.getHostName()</tt> method, which could be very expensive
        * due to the reverse DNS lookup.
        * <br>
        * For IPv6 addresses this method produces an alternative form of
        * "x:x:x:x:x:x:d.d.d.d" and a compressed form of "::d.d.d.d" for IPv4
        * compatible addresses (instead of the default form "x:x:x:x:x:x:x:x").
        *
        * @param addr  the address for which to format the IP address string
        *
        * @return the IP address string
        *
        * @see InetAddress::getHostAddress()
        */
        static String::View toString(InetAddress::View vAddr);

        /**
        * {@inheritDoc}
        */
        using Object::toString;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_INET_ADDRESS_HELPER_HPP
