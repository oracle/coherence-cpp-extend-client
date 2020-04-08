/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_RAW_SOCKET_ADDRESS_HPP
#define COH_POSIX_RAW_SOCKET_ADDRESS_HPP

#include "coherence/lang.ns"

#include "coherence/io/IOException.hpp"

#include "private/coherence/native/posix/PosixSocketAddress.hpp"

#ifdef COH_OS_WINDOWS
    // Windows supports the POSIX API for socket structs
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
    #include <unistd.h>
#endif



COH_OPEN_NAMESPACE3(coherence,native,posix)

using coherence::io::IOException;
using coherence::net::SocketAddress;

/**
* PosixRawSocketAddress is a managed wrapper for an opaque posix socket address
* (sockaddr).
*
* The size of the address is protocol dependent and is not the same as
* sizeof(sockaddr).
*
* @author mf  2008.03.21
*/
class COH_EXPORT PosixRawSocketAddress
    : public class_spec<PosixRawSocketAddress,
        extends<Object>,
        implements<PosixSocketAddress> >
    {
    friend class factory<PosixRawSocketAddress>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PosixRawSocketAddress, copying the value from the supplied
        * sockaddr.
        *
        * @param vabAddr the sockaddr encoded in a byte array
        */
        PosixRawSocketAddress(Array<octet_t>::View vabAddr)
            : f_vSockAddr(self(), vabAddr)
            {
            }

        /**
        * Copy constructor.
        */
        PosixRawSocketAddress(const PosixRawSocketAddress& that)
            : super(that), f_vSockAddr(self(), that.f_vSockAddr)
            {
            }


    // ----- PosixSocketAddress interface -----------------------------------

    public:
        /**
        * Return a const pointer to the posix socket address.
        *
        * The memory associated with the return value continues to be managed
        * by this class, and will only remain valid for the lifetime of the
        * PosixRawSocketAddress from which is was obtained.
        *
        * @return a const pointer to the posix socket address
        */
        virtual const sockaddr* getSockAddr() const
            {
            return (const sockaddr*) (const octet_t*) f_vSockAddr->raw;
            }

        /**
        * Return the size in bytes of the socket address.
        *
        * @return the size in bytes of the posix socket address
        */
        virtual socklen_t getSize() const
            {
            return (socklen_t) f_vSockAddr->length;
            }


    // ----- SocketAddress interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isUnresolved() const
            {
            return f_vSockAddr == NULL;
            }

        /**
        * {@inheritDoc}
        */
        virtual SocketAddress::View resolve() const
            {
            if (isUnresolved())
                {
                COH_THROW (IOException::create(
                        "insufficient information to resolve address"));
                }
            return this;
            }


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("PosixRawSocketAddress[family=" << getSockAddr()->sa_family << ']');
            }

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View that) const
            {
            PosixRawSocketAddress::View vThat =
                cast<PosixRawSocketAddress::View>(that, false);
            return this == vThat ||
                (vThat != NULL && f_vSockAddr->equals(vThat->f_vSockAddr));
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Byte copy of a posix sockaddr.
        */
        FinalView<Array<octet_t> > f_vSockAddr;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POSIX_RAW_SOCKET_ADDRESS_HPP
