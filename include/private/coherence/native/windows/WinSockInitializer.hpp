/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WIN_SOCK_INITIALIZER_HPP
#define COH_WIN_SOCK_INITIALIZER_HPP

#include "coherence/lang/compatibility.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>

COH_OPEN_NAMESPACE3(coherence,native,windows)


/**
* WinSockInitializer initializes the winsock api.
*/
class WinSockInitializer
    {
    private:
        WinSockInitializer()
                : m_fCleanup(false)
            {
            WSADATA wsaData;
            // request version 2.2 or higher of winsock, available on Win2K+
            if (int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData))
                {
                // error starting winsock (not related to version)
                std::cerr << "Coherence: error starting winsock DLL, error = "
                          << nResult << std::endl;
                }

            // validate version
            if (LOBYTE(wsaData.wVersion) != 2 ||
                HIBYTE(wsaData.wVersion) != 2 )
                {
                WSACleanup();
                std::cerr << "Coherence: error negotiating winsock DLL"
                          << " version, 2.2 required, "
                          << LOBYTE(wsaData.wVersion) << '.'
                          << HIBYTE(wsaData.wVersion) << " available."
                          << std::endl;
                }
            else
                {
                // record that cleanup in necessary when coherence.dll is
                // unloaded
                m_fCleanup = true;
                }
            }

        ~WinSockInitializer()
            {
            if (m_fCleanup)
                {
                // match successfull call to WSAStartup
                WSACleanup();
                }
            }

    public:
        /**
        * Ensure that the winsock library has been initialized.
        */
        static void ensure()
            {
            static WinSockInitializer singleton;
            }

    private:
        bool m_fCleanup;
    };
COH_STATIC_INIT(WinSockInitializer::ensure());

COH_CLOSE_NAMESPACE3

#endif // COH_WIN_SOCK_INITIALIZER_HPP
