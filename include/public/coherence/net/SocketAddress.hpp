/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOCKET_ADDRESS_HPP
#define COH_SOCKET_ADDRESS_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,net)


/**
* SocketAddress interface.
*
* @author mf  2008.03.21
*/
class COH_EXPORT SocketAddress
    : public interface_spec<SocketAddress>
    {
    // ----- SocketAddress interface ----------------------------------------

    public:
        /**
        * Identify if the SocketAddress has been resolved.
        */
        virtual bool isUnresolved() const = 0;

        /**
        * Resolve the SocketAddress.
        *
        * If the address was already in a resolved state the returned address
        * may be the original address.
        *
        * @return a resolved SocketAddress
        *
        * @throws IOException upon resolution failure.
        */
        virtual SocketAddress::View resolve() const = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SOCKET_ADDRESS_HPP
