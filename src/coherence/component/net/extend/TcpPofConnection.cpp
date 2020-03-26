/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/TcpPofConnection.hpp"

#include "private/coherence/component/net/extend/util/TcpUtil.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::util::TcpUtil;


// ----- constructors -------------------------------------------------------

TcpPofConnection::TcpPofConnection()
    : f_hSocket(self())
    {
    }


// ----- Describable interface ----------------------------------------------

String::View TcpPofConnection::getDescription() const
    {
    String::View vs      = super::getDescription();
    Socket::View vSocket = getSocket();
    if (NULL != vSocket && !vSocket->isClosed())
        {
        vs = COH_TO_STRING(vs << ", LocalAddress="
            << TcpUtil::toString(vSocket->getLocalSocketAddress())
            << ", RemoteAddress="
            << TcpUtil::toString(vSocket->getRemoteSocketAddress()));
        }

    return vs;
    }


// ----- accessor methods ---------------------------------------------------

Socket::View TcpPofConnection::getSocket() const
    {
    return f_hSocket;
    }

Socket::Handle TcpPofConnection::getSocket()
    {
    return f_hSocket;
    }

 void TcpPofConnection::setSocket(Socket::Handle hSocket)
    {
     initialize(f_hSocket, hSocket);
    }

COH_CLOSE_NAMESPACE4
