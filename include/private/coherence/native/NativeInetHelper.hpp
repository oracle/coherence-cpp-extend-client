/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NATIVE_INET_HELPER_HPP
#define COH_NATIVE_INET_HELPER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Array.hpp"
#include "coherence/lang/String.hpp"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/InetSocketAddress.hpp"

#include "coherence/util/Collection.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

using namespace coherence::lang; // required since lang.hpp was not included
using coherence::net::InetAddress;
using coherence::net::InetSocketAddress;
using coherence::util::Collection;


/**
* NativeInetHelper provides a common interface to native InetAddress related
* operations.
*
* @author mf  2008.03.17
*/
class COH_EXPORT NativeInetHelper
    {
    // ----- NativeInetHelper interface -------------------------------------

    public:
        /**
        * Return the hostname or textual IP for the local machine.
        *
        * @return the name, or NULL if none could be found
        */
        static String::View getLocalHostName();

        /**
        * Return the raw address as an Array<octet_t> for the specified machine name.
        *
        * @param vsName  the name or IP of the host to resolve
        *
        * @return the raw address or NULL if none could be found
        */
        static Array<octet_t>::View getRawAddress(String::View vsName);

        /**
         * Given the name of a host, returns a collection of raw addresses in
         * Array<octet_t> form, based on the configured name service on the system.
         *
         * @param vsHost  the name or IP of the host to resolve
         *
         * @return a collection of all NativeInetAddresses or NULL if none
         *         could be found
         *
         * @since 12.2.1
         */
        static Collection::View getAllRawAddresses(String::View vsHost);

        /**
        * Return the NativeInetSocketAddress for the specified machine name and port.
        *
        * @param vAddr  the InetAddress
        * @param nPort  the port
        *
        * @return the InetSocketAddress or NULL if none could be found
        */
        static InetSocketAddress::Handle getSocketAddress(InetAddress::View vAddr, uint16_t nPort);
    };

COH_CLOSE_NAMESPACE2

#endif // COH_NATIVE_INET_HELPER_HPP
