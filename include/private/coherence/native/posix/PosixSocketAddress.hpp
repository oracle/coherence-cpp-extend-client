/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_SOCKET_ADDRESS_HPP
#define COH_POSIX_SOCKET_ADDRESS_HPP

#include "coherence/lang.ns"

#include "coherence/net/SocketAddress.hpp"

#ifdef COH_OS_WINDOWS
    // Windows supports the POSIX API for socket structs
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
    #include <unistd.h>
#endif



COH_OPEN_NAMESPACE3(coherence,native,posix)

using coherence::net::SocketAddress;


/**
* PosixSocketAddress is a managed wrapper for an opaque posix socket address
* (sockaddr).
*
* The size of the address is protocol dependent and is not the same as
* sizeof(sockaddr).
*
* @author mf  2008.03.21
*/
class COH_EXPORT PosixSocketAddress
    : public interface_spec<PosixSocketAddress,
        implements<SocketAddress> >
    {
    // ----- PosixSocketAddress interface -----------------------------------

    public:
        /**
        * Return a const pointer to the posix socket address.
        *
        * The memory associated with the return value continues to be managed
        * by this class, and will only remain valid for the lifetime of the
        * PosixSocketAddress from which is was obtained.
        *
        * @return a const pointer to the posix socket address
        */
        virtual const sockaddr* getSockAddr() const  = 0;

        /**
        * Return the size in bytes of the socket address.
        *
        * @return the size in bytes of the posix socket address
        */
        virtual socklen_t getSize() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POSIX_SOCKET_ADDRESS_HPP
