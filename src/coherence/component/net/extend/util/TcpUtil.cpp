/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/util/TcpUtil.hpp"

#include "coherence/io/IOException.hpp"

#include "private/coherence/net/InetAddressHelper.hpp"
#include "private/coherence/io/OutputStream.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE5(coherence,component,net,extend,util)

using coherence::io::IOException;
using coherence::io::OutputStream;
using coherence::net::InetAddressHelper;


bool TcpUtil::close(Socket::Handle hSocket)
    {
    if (NULL != hSocket)
        {
        try
            {
            hSocket->close();
            return true;
            }
        catch (IOException::View) {}
        }
    return false;
    }

void TcpUtil::setKeepAlive(Socket::Handle hSocket, bool fKeepAlive)
    {
    if (hSocket->getKeepAlive() != fKeepAlive)
        {
        hSocket->setKeepAlive(fKeepAlive);
        }
    }

void TcpUtil::setReceiveBufferSize(Socket::Handle hSocket, int32_t cb)
    {
    if (cb > 0 && hSocket->getReceiveBufferSize() != cb)
        {
        hSocket->setReceiveBufferSize(cb);
        validateBufferSize(hSocket, true, cb);
        }
    }

void TcpUtil::setReuseAddress(Socket::Handle hSocket, bool fReuseAddress)
    {
    if (!hSocket->isBound() && hSocket->getReuseAddress() != fReuseAddress)
        {
        hSocket->setReuseAddress(fReuseAddress);
        }
    }

void TcpUtil::setSendBufferSize(Socket::Handle hSocket, int cb)
    {
    if (cb > 0 && hSocket->getSendBufferSize() != cb)
        {
        hSocket->setSendBufferSize(cb);
        validateBufferSize(hSocket, false, cb);
        }
    }

void TcpUtil::setSoLinger(Socket::Handle hSocket, int32_t cSecs)
    {
    cSecs = std::max(cSecs, int32_t(-1));

    if (cSecs == 0)
        {
        return;
        }

    if (hSocket->getSoLinger() != cSecs)
        {
        if (cSecs > 0)
            {
            hSocket->setSoLinger(true, cSecs);
            }
        else
            {
            hSocket->setSoLinger(false, 0);
            }

        if (hSocket->getSoLinger() != cSecs)
            {
            if (cSecs > 0)
                {
                COH_LOG("Failed to set a TCP Socket linger time to " << cSecs
                        << " seconds; actual value is " << hSocket->getSoLinger()
                        << " seconds", 2);
                }
            else
                {
                COH_LOG("Failed to disabled a TCP Socket linger time", 0);
                }
            }
        }
    }

void TcpUtil::setTcpNoDelay(Socket::Handle hSocket, bool fTcpNoDelay)
    {
    if (hSocket->getTcpNoDelay() != fTcpNoDelay)
        {
        hSocket->setTcpNoDelay(fTcpNoDelay);
        }
    }

String::View TcpUtil::toString(SocketAddress::View vAddr)
    {
    if (instanceof<InetSocketAddress::View>(vAddr))
        {
        InetSocketAddress::View vInetAddr = cast<InetSocketAddress::View>(vAddr);
        return COH_TO_STRING(InetAddressHelper::toString(vInetAddr->getAddress())
                << ':' << vInetAddr->getPort());
        }

    return COH_TO_STRING(vAddr);
    }

String::View TcpUtil::toString(SocketAddress::View vAddr, int32_t nSubport)
    {
    if (nSubport != -1)
        {
        return COH_TO_STRING(TcpUtil::toString(vAddr) << '.' << nSubport);
        }
    return TcpUtil::toString(vAddr);
    }
    
void TcpUtil::validateBufferSize(bool fReceiveBuffer,
        int32_t cbActual, int32_t cbRequested)
    {
    String::View vsName = fReceiveBuffer ? "receive" : "send";
    if (cbActual < cbRequested)
        {
        COH_LOG("Failed to set a TCP Socket " << vsName << " buffer size to "
                << cbRequested << " bytes; actual size is " << cbActual << " bytes. "
                << "Consult your OS documentation regarding increasing the maximum "
                << "TCP Socket " << vsName << " buffer size. Proceeding with the "
                << "actual value may cause sub-optimal performance.", 2);
        }
    }

void TcpUtil::validateBufferSize(Socket::Handle hSocket,
        bool fReceiver, int32_t cbRequested)
    {
    int cbActual = fReceiver
            ? hSocket->getReceiveBufferSize()
            : hSocket->getSendBufferSize();
    validateBufferSize(fReceiver, cbActual, cbRequested);
    }

void TcpUtil::writeSubport(OutputStream::Handle hStream, int32_t nSubport)
    {
    const int32_t nProtocolId = 0x05AC1E000;
    
    Array<octet_t>::Handle hab = Array<octet_t>::create(8);

    hab[0] = (octet_t) (uint32_t(nProtocolId) >> 24);
    hab[1] = (octet_t) (uint32_t(nProtocolId) >> 16);
    hab[2] = (octet_t) (uint32_t(nProtocolId) >> 8);
    hab[3] = (octet_t) nProtocolId;
    hab[4] = (octet_t) (uint32_t(nSubport) >> 24);
    hab[5] = (octet_t) (uint32_t(nSubport) >> 16);
    hab[6] = (octet_t) (uint32_t(nSubport) >> 8);
    hab[7] = (octet_t) nSubport;
    
    hStream->write(hab);
    }
    
int32_t TcpUtil::calculateBaseport(int32_t nPort)
    {
    int32_t nBaseport = ~nPort >> 16;
    
    if (nBaseport == -1)
    {
        return nPort;
    }
    return nBaseport;
    }
    
int32_t TcpUtil::calculateSubport(int32_t nPort)
    {
    int32_t nBaseport = ~nPort >> 16;
    int32_t nSubport  = ~nPort & 0xFFFF;
    
    if (nBaseport == -1)
    {
        return -1;
    }
    return nSubport;
    }
    
COH_CLOSE_NAMESPACE5
