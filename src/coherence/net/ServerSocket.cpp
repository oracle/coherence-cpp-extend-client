/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/ServerSocket.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/net/SocketAddress.hpp"

#include "private/coherence/native/NativeSocket.hpp"
#include "private/coherence/net/SocketTimeoutException.hpp"

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::IOException;
using coherence::native::NativeSocket;


// ----- constructors -------------------------------------------------------

ServerSocket::ServerSocket()
    {
    }


// ----- ServerSocket interface ---------------------------------------------

void ServerSocket::bind(SocketAddress::View vAddr)
    {
    bind(vAddr, 0);
    }

void ServerSocket::bind(SocketAddress::View vAddr, int32_t cMaxBacklog)
    {
    super::bind(vAddr);
    ensureNativeSocket()->listen(cMaxBacklog);
    }

Socket::Handle ServerSocket::accept()
    {
    NativeSocket* pNativeAccept    = NULL;
    int64_t       cMillisInterrupt = System::getInterruptResolution();
    int64_t       cMillisTimeout   = Thread::remainingTimeoutMillis();

    if (!isBound())
        {
        COH_THROW (IOException::create("Socket is not bound."));
        }

    do
        {
        if (Thread::interrupted())
            {
            COH_THROW (SocketTimeoutException::create("socket accept interrupt"));
            }

        pNativeAccept = ensureNativeSocket()->accept(cMillisTimeout < cMillisInterrupt ? cMillisTimeout : cMillisInterrupt);
        }
    while (NULL == pNativeAccept);

    return Socket::create(pNativeAccept);
    }

COH_CLOSE_NAMESPACE2
