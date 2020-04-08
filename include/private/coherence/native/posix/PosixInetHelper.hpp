/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_INET_HELPER_HPP
#define COH_POSIX_INET_HELPER_HPP

#include "private/coherence/native/NativeInetHelper.hpp"

#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/IllegalStateException.hpp"

#include "coherence/net/InetAddress.hpp"
#include "coherence/net/InetSocketAddress.hpp"

#include "coherence/util/HashSet.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/native/posix/PosixInetSocketAddress.hpp"
#include "private/coherence/native/posix/PosixSocketAddress.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#ifdef COH_OS_WINDOWS
    // Windows supports the POSIX API for socket structs
    #include "private/coherence/native/windows/WinSockInitializer.hpp"
    #define COH_ENSURE_SOCKET_INIT() \
        static const bool init_once = (coherence::native::windows::WinSockInitializer::ensure(), true)
#else
    #include <limits.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define COH_ENSURE_SOCKET_INIT() while (false)
#endif

#ifdef _POSIX_HOST_NAME_MAX
    #define COH_HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#else
    #define COH_HOST_NAME_MAX 255
#endif

#include <algorithm>
#include <ctype.h>
#include <sstream>
#include <string.h>


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Helper class which will properly free a addrinfo chain upon destruction.
    */
    struct AddrInfoFinally
        {
        AddrInfoFinally()
                : pAddrInfo(NULL)
            {
            }

        ~AddrInfoFinally()
            {
            if (pAddrInfo)
                {
                freeaddrinfo(pAddrInfo);
                }
            }

        struct addrinfo* pAddrInfo;
        };
    }


COH_OPEN_NAMESPACE3(coherence,native,posix)

using coherence::net::InetAddress;
using coherence::net::InetSocketAddress;

class COH_EXPORT PosixInetHelper
        : public NativeInetHelper
    {
    // ---- PosixInetHelper statics -----------------------------------------

    public:
        /**
        * Return the Socket address family to use.
        *
        * @return the address family.
        */
        static int getFamily()
            {
            static FinalView<String> vsNet(System::common(),
                    System::getProperty("coherence.net", "ipv4"));
            static const int nFam =
                  vsNet->equals("ipv4") ? AF_INET :
                  vsNet->equals("ipv6") ? AF_INET6 : AF_UNSPEC;
            return nFam;
            }

        /**
        * Lookup address info based on hostname and optional port.
        *
        * @param result  the search result
        * @param vsName  the address to seach for
        * @param nPort   the port name on the host
        */
        static void getAddrInfo(AddrInfoFinally& result, String::View vsName, uint16_t nPort = 0)
            {
            COH_ENSURE_SOCKET_INIT();
            std::stringstream sService;
            addrinfo hints;

            memset(&hints, 0, sizeof(hints));

            hints.ai_family = getFamily();
#if defined(AI_ADDRCONFIG)
            // not defined on Windows, good to have if available
            hints.ai_flags |= AI_ADDRCONFIG; // only return compatible AFs
#endif
#if defined(AI_V4MAPPED)
            // not defined on Windows, good to have if available
            hints.ai_flags |= AI_V4MAPPED;
#endif

            if (nPort == 0)
                {
                // name resolution
                hints.ai_flags |= AI_CANONNAME;  // full name resolution
                }
            else
                {
                // service resolution
                sService << nPort << std::ends;

                hints.ai_protocol  = IPPROTO_TCP;
                hints.ai_socktype  = SOCK_STREAM;
#if defined(AI_NUMERICSERV)
                // not defined (or necessary) on OS X, needed on Solaris
                hints.ai_flags    |= AI_NUMERICSERV;
#endif
                }

            while (true)
                {
                int nResult = getaddrinfo(vsName->getCString(),
                        (nPort ? sService.str().c_str() : NULL), &hints,
                        &result.pAddrInfo);
                switch (nResult)
                    {
                    case 0: // success
                        return;

                    case EAI_NONAME:
#if defined(EAI_NODATA) && EAI_NODATA != EAI_NONAME
                    // non-POSIX, needed on OS X 10.4, not defined on Windows
                    case EAI_NODATA:
#endif
                    case EAI_AGAIN:
                    case EAI_FAIL:
                    case EAI_SERVICE:
                        // unknown host, or service level failure, not a hard error
                        // see if we can refine the search
                        if (NULL != vsName && (vsName->equals("localhost") ||
                            vsName->equals(getLocalHostName())))
                            {
                            COH_LOG("Unable to resolve local address \""
                                    << vsName << "\"; error[" << nResult
                                    << "] " << gai_strerror(nResult)
                                    << "; using loopback", 3);
                            hints.ai_flags |= AI_NUMERICHOST;
                            vsName          = "127.0.0.1";
                            }
                        else if ((hints.ai_flags & AI_NUMERICHOST) == 0)
                            {
                            // try as a numeric host, though we could scan the
                            // string and check, getaddrinfo will do this anyway
                            hints.ai_flags |= AI_NUMERICHOST;
                            }
                        else
                            {
                            // no refinements available
                            COH_LOG("Unable to resolve address \""
                                    << vsName << "\"; error[" << nResult << "] "
                                    << gai_strerror(nResult), 6);
                            return;
                            }
                        break; // try again

                    default:
                        // unexpected error
                        COH_THROW_STREAM (IllegalStateException,
                                "unexpected result from getaddrinfo(\"" <<
                                vsName << "\", ...); error[" << nResult <<
                                "] " << gai_strerror(nResult));
                    }
                }
            }

        /**
        * Return the "best" address from a list of addresses.
        *
        * @param pAddrInfo a pointer the first address in the linked list
        *
        * @return a pointer to the best address, or NULL is none found
        */
        static struct addrinfo* getBestAddress(struct addrinfo* pAddrInfo)
            {
            // find best IPv4/6 address, prefer IPv4 over IPv6
            addrinfo* pAddrBest = NULL;
            for (struct addrinfo* pAddrNext = pAddrInfo; pAddrNext != NULL;
                    pAddrNext = pAddrNext->ai_next)
                {
                if (!isRoutable(pAddrNext))
                    {
                    continue;
                    }

                if (NULL == pAddrBest)
                    {
                    pAddrBest = pAddrNext;
                    }
                else
                    {
                    if (compare(pAddrBest, pAddrNext) < 0)
                        {
                        pAddrBest = pAddrNext;
                        }
                    }
                }

            return pAddrBest;
            }

    private:
        /**
        * Compare two addresses.
        *
        * @param pA  first address
        * @param pB  second address
        *
        * @return  results of comparison: -1 if A < B, 0 if A = B, and 1 if A > B
        */
        static int compare(struct addrinfo * pA, struct addrinfo *pB)
            {
            if (pA == pB)
                {
                return 0;
                }
            else if (pA == NULL)
                {
                return -1;
                }
            else if (pB == NULL)
                {
                return 1;
                }

            // to allow ipv4 and ipv6 address to be "correctly" compared we
            // will do the comparison backwards, discarding any leading zeros
            Array<octet_t>::View vabA = PosixInetSocketAddress::getRawIP(pA->ai_addr);
            Array<octet_t>::View vabB = PosixInetSocketAddress::getRawIP(pB->ai_addr);

            int ofA;
            int cbA;
            for (ofA = 0, cbA = vabA->length; ofA < cbA && vabA[ofA] == 0; ++ofA);
            int cbsA = cbA - ofA;

            int ofB;
            int cbB;
            for (ofB = 0, cbB = vabB->length; ofB < cbB && vabB[ofB] == 0; ++ofB);
            int cbsB = cbB - ofB;

            // if the significant parts are of different lengths, then the address
            // cannot be equal, so just base the decision upon length
            if (cbsA < cbsB)
                {
                return -1;
                }
            else if (cbsB < cbsA)
                {
                return 1;
                }
            else
                {
                // significant parts are of comparable length
                for (int i = 0; i < cbsA; ++i)
                    {
                    int n = (0xFF & (int) vabA[ofA + i]) - (0xFF & (int) vabB[ofB + i]); // unsigned comparison
                    if (n != 0)
                      {
                      return n < 0 ? -1 : 1;
                      }
                    }
                return 0;
                }
            }

        /**
        * Is a given address routable. An address is routable if it's not
        * a loopback or Link local
        *
        * @param pAddr  address to evaluate
        *
        * @return  true if address is routable, false if not
        */
        static bool isRoutable(struct addrinfo* pAddr)
            {
            return !isLoopbackAddress(pAddr) &&
                   !isLinkLocalAddress(pAddr);
            }

        /**
        * Is a given address a link local address. An address is LinkLocal if
        * it falls in the specified range (RFC3927, RFC4291).
        *
        * @param pAddr  address to evaluate
        *
        * @return  true if the address is LinkLocal, false if not
        */
        static bool isLinkLocalAddress(struct addrinfo* pAddr)
            {
            bool isLinkLocal = false;
            Array<octet_t>::View vabAddr = PosixInetSocketAddress::getRawIP(pAddr->ai_addr);

            switch (pAddr->ai_family)
                {
                case AF_INET:
                    if ((vabAddr[0] & 0xff) == 0xa9 &&
                        (vabAddr[1] & 0xff) == 0xfe)
                        {
                        // falls into 169.254.*.*
                        isLinkLocal = true;
                        }
                    break;

                case AF_INET6:
                    if ((vabAddr[0] & 0xFF) == 0xFE &&
                        (vabAddr[1] & 0xC0) == 0x80)
                        {
                        // falls into fe80:*:*:*:*:*:*:*
                        isLinkLocal = true;
                        }
                    break;

                default:
                    break;
                }
            return isLinkLocal;
            }

        /**
        * Is a given address a loopback address.
        *
        * @param pAddr  address to evaluate
        *
        * @return  true if the address is LinkLocal, false if not
        */
        static bool isLoopbackAddress(struct addrinfo* pAddr)
            {
            // checks for both loopback and anylocal address
            Array<octet_t>::View vabAddr = PosixInetSocketAddress::getRawIP(pAddr->ai_addr);
            bool isLoopback = false;

            switch (pAddr->ai_family)
                {
                case AF_INET:
                    {
                    int32_t nIP = (vabAddr[0] & 0xFF) << 24
                                | (vabAddr[1] & 0xFF) << 16
                                | (vabAddr[2] & 0xFF) <<  8
                                | (vabAddr[3] & 0xFF);

                    if (nIP == 0x00000000 && nIP == 0x7F000001)
                        {
                        isLoopback = true;
                        }
                    }
                    break;

                case AF_INET6:
                    {
                    int32_t nIP1 = (vabAddr[ 0] & 0xFF) << 24
                                 | (vabAddr[ 1] & 0xFF) << 16
                                 | (vabAddr[ 2] & 0xFF) <<  8
                                 | (vabAddr[ 3] & 0xFF);
                    int32_t nIP2 = (vabAddr[ 4] & 0xFF) << 24
                                 | (vabAddr[ 5] & 0xFF) << 16
                                 | (vabAddr[ 6] & 0xFF) <<  8
                                 | (vabAddr[ 7] & 0xFF);
                    int32_t nIP3 = (vabAddr[ 8] & 0xFF) << 24
                                 | (vabAddr[ 9] & 0xFF) << 16
                                 | (vabAddr[10] & 0xFF) <<  8
                                 | (vabAddr[11] & 0xFF);
                    int32_t nIP4 = (vabAddr[12] & 0xFF) << 24
                                 | (vabAddr[13] & 0xFF) << 16
                                 | (vabAddr[14] & 0xFF) <<  8
                                 | (vabAddr[15] & 0xFF);
                    if (nIP1 == 0x00000000 &&  nIP2 == 0x00000000   &&
                        nIP3 == 0x00000000 && (nIP4 == 0x00000000   ||
                                           nIP4 == 0x00000001))
                        {
                        isLoopback = true;
                        }
                    }
                    break;

                default:
                    break;
                }
            return isLoopback;
            }
    };

COH_CLOSE_NAMESPACE3


// ----- NativeInetHelper statics -------------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::native::posix::PosixInetHelper;
using coherence::native::posix::PosixInetSocketAddress;
using coherence::util::HashSet;
using coherence::util::Set;

String::View NativeInetHelper::getLocalHostName()
    {
    COH_ENSURE_SOCKET_INIT();
    char achHost[COH_HOST_NAME_MAX + 1];

    achHost[COH_HOST_NAME_MAX] = '\0'; // ensure NUL termination
    COH_ENSURE_EQUALITY(gethostname(achHost, COH_HOST_NAME_MAX + 1), 0);

    return achHost[0] ? achHost : NULL;
    }

Array<octet_t>::View NativeInetHelper::getRawAddress(String::View vsName)
    {
    COH_ENSURE_PARAM(vsName);
    AddrInfoFinally finally;
    PosixInetHelper::getAddrInfo(finally, vsName, 0);

    struct addrinfo* pAddrBest = PosixInetHelper::getBestAddress(finally.pAddrInfo);

    if (NULL == pAddrBest)
        {
        return NULL;
        }

    return PosixInetSocketAddress::getRawIP(pAddrBest->ai_addr);
    }

Collection::View NativeInetHelper::getAllRawAddresses(String::View vsHost)
    {
    COH_ENSURE_PARAM(vsHost);

    AddrInfoFinally finally;
    PosixInetHelper::getAddrInfo(finally, vsHost, 0);

    Set::Handle hSetAddr = HashSet::create(); // use a set to eliminate duplicates
    for (struct addrinfo* pAddrNext = finally.pAddrInfo; pAddrNext != NULL;
            pAddrNext = pAddrNext->ai_next)
        {
        hSetAddr->add(PosixInetSocketAddress::getRawIP(pAddrNext->ai_addr));
        }
    return hSetAddr;
    }

InetSocketAddress::Handle NativeInetHelper::getSocketAddress(
        InetAddress::View vAddr, uint16_t nPort)
    {
    COH_ENSURE_PARAM(vAddr);
    AddrInfoFinally finally;
    PosixInetHelper::getAddrInfo(finally, vAddr->getHostAddress(), nPort);

    struct addrinfo* pAddrBest = PosixInetHelper::getBestAddress(finally.pAddrInfo);

    if (NULL == pAddrBest)
        {
        return NULL;
        }

    Array<octet_t>::Handle habAddr = Array<octet_t>::create
        ((size32_t) pAddrBest->ai_addrlen);

    std::copy((octet_t*) pAddrBest->ai_addr,
              (octet_t*) pAddrBest->ai_addr + pAddrBest->ai_addrlen,
              (octet_t*) habAddr->raw);
    return PosixInetSocketAddress::create(vAddr, habAddr);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_INET_HELPER_HPP
