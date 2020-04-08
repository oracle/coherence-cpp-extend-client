/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_INET_SOCKET_ADDRESS_HPP
#define COH_POSIX_INET_SOCKET_ADDRESS_HPP

#include "coherence/lang.ns"
#include "coherence/net/InetAddress.hpp"
#include "coherence/net/InetSocketAddress.hpp"

#include "private/coherence/native/posix/PosixSocketAddress.hpp"


#ifdef COH_OS_WINDOWS
    // Windows supports the POSIX API for socket structs
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#include <algorithm>
#include <sstream>

COH_OPEN_NAMESPACE3(coherence,native,posix)

using coherence::net::InetAddress;
using coherence::net::InetSocketAddress;


/**
* PosixInetSocketAddress is a managed object wrapper around a posix
* sockaddr structure for the AF_INET or AF_INET6 family.
*
* @author mf  2008.03.21
*/
class COH_EXPORT PosixInetSocketAddress
    : public class_spec<PosixInetSocketAddress,
        extends<InetSocketAddress>,
        implements<PosixSocketAddress> >
    {
    friend class factory<PosixInetSocketAddress>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PosixInetSocketAddress.
        *
        * @param vAddr    the InetAddress portion of the SocketAddress
        * @param vabAddr  the sockaddr encoded in a byte array (may not be NULL)
        */
        PosixInetSocketAddress(InetAddress::View vAddr, Array<octet_t>::View vabAddr)
            : f_vSockAddr(self(), vabAddr),
              f_vAddr(self(), vAddr == NULL
                      ? (InetAddress::View) InetAddress::create(
                              String::null_string, getRawIP(getSockAddr()))
                      : vAddr)
            {
            COH_ENSURE_PARAM(vabAddr);

            // ensure that an INET address was supplied
            int nFamily = getSockAddr()->sa_family;
            if (nFamily != AF_INET && nFamily != AF_INET6)
                {
                COH_THROW (IllegalArgumentException::create
                            ("supplied address it not an inet address"));
                }

            // if vAddr was specified, ensure it has the same IP address as
            // is encoded in vabAddr
            COH_ENSURE(vAddr == NULL ||
                vAddr->getAddress()->equals(getRawIP(getSockAddr())));
            }

        /**
        * Copy constructor
        */
        PosixInetSocketAddress(const PosixInetSocketAddress& that)
            : super(), f_vSockAddr(self(), that.f_vSockAddr),
              f_vAddr(self(), that.f_vAddr)
            {
            }

    // ----- InetSocketAddress interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InetAddress::View getAddress() const
            {
            return f_vAddr;
            }

        /**
        * {@inheritDoc}
        */
        virtual uint16_t getPort() const
            {
            const sockaddr* pAddr = getSockAddr();
            switch (pAddr->sa_family)
                {
                case AF_INET:
                    return ntohs(((sockaddr_in*) pAddr)->sin_port);

                case AF_INET6:
                    return ntohs(((sockaddr_in6*) pAddr)->sin6_port);

                default:
                    // should not be possible, see constructor
                    COH_THROW (IllegalStateException::create());
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual String::View getHostName() const
            {
            return getAddress()->getHostName();
            }


    // ----- SocketAddress interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isUnresolved() const
            {
            // PosixInetSocketAddress is always resolved, i.e. we always have
            // the IP in some form
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual SocketAddress::View resolve() const
            {
            return this;
            }


    // ----- PosixRawSocketAddress interface --------------------------------

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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View that) const
            {
            PosixInetSocketAddress::View vThat =
                    cast<PosixInetSocketAddress::View>(that, false);
            return this == vThat || (NULL != vThat &&
                   f_vAddr->equals(vThat->f_vAddr));
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return f_vAddr->hashCode();
            }


    // ---- helper methods --------------------------------------------------

    public:
        /**
        * Return the ip address from the supplied sockaddr INET/INET6 address
        *
        * @param pAddr  the socket address
        *
        * @return the corresponding byte array
        */
        static Array<octet_t>::View getRawIP(const sockaddr* pAddr)
            {
            if (pAddr != NULL)
                {
                switch (pAddr->sa_family)
                    {
                    case AF_INET:
                        {
                        // IPv4 address is packed in 32b int, stored in
                        // network byte order unsafe to assume we could
                        // treat it as a byte array
                        Array<octet_t>::Handle haIP = Array<octet_t>::create(4);
                        uint32_t               nIP  = ntohl(
                                ((const sockaddr_in*) pAddr)->sin_addr.s_addr);

                        // store in byte array in network byte order
                        haIP[0] = (octet_t) (0xFF & (nIP >> 24));
                        haIP[1] = (octet_t) (0xFF & (nIP >> 16));
                        haIP[2] = (octet_t) (0xFF & (nIP >> 8));
                        haIP[3] = (octet_t) (0xFF & nIP);

                        return haIP;
                        }

                    case AF_INET6:
                        {
                        // IPv6 is directly accessable as a uint8_t array
                        Array<octet_t>::Handle habAddr = Array<octet_t>::
                                create(16);
                        const octet_t* pabSrc = ((const sockaddr_in6*) pAddr)
                                ->sin6_addr.s6_addr;
                        std::copy(pabSrc, pabSrc + habAddr->length,
                                (octet_t*) habAddr->raw);
                        return habAddr;
                        }

                    default:
                        COH_THROW (IllegalArgumentException::create(
                                "not an inet address"));
                    }
                }
            return NULL;
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Byte copy of a posix sockaddr.
        */
        FinalView<Array<octet_t> > f_vSockAddr;

        /**
        * The InetAddress associated with this address.
        */
        FinalView<InetAddress> f_vAddr;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POSIX_INET_SOCKET_ADDRESS
