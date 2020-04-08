/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/net/Socket.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"

#include "private/coherence/native/NativeSocket.hpp"
#include "private/coherence/net/SocketTimeoutException.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>
#include <sstream>

COH_OPEN_NAMESPACE2(coherence,net)

using coherence::io::IOException;
using coherence::io::OctetArrayReadBuffer;
using coherence::util::logging::Logger;
using coherence::native::NativeSocket;
using coherence::net::SocketTimeoutException;


// ----- constructors -------------------------------------------------------

Socket::Socket(NativeSocket* pNative)
        : m_pNative(pNative ? pNative : NativeSocket::create()),
          f_vAddrLocal(self(),  NULL, /*fMutable*/ true),
          f_vAddrRemote(self(), NULL, /*fMutable*/ true),
          m_cMillisTimeout(0), m_hInput(self()), m_hOutput(self()),
          m_fClosed(false)
    {
    if (pNative)
        {
        // born connected
        m_hInput  = SocketInput::create(this);
        m_hOutput = SocketOutput::create(this);
        }
    }

Socket::~Socket()
    {
    NativeSocket* pNative = m_pNative;
    if (pNative)
        {
        m_pNative = NULL;
        delete pNative;
        }
    }


// ----- Socket interface ---------------------------------------------------

void Socket::bind(SocketAddress::View vAddrLocal)
    {
    if (vAddrLocal->isUnresolved())
        {
        vAddrLocal = vAddrLocal->resolve();
        }
    COH_SYNCHRONIZED (this)
        {
        if (isBound())
            {
            COH_THROW (IOException::create("socket is already bound"));
            }
        else if (NULL != vAddrLocal)
            {
            ensureNativeSocket()->bind(vAddrLocal);
            initialize(f_vAddrLocal, vAddrLocal);
            }
        }
    }

bool Socket::isBound() const
    {
    return NULL != getLocalSocketAddress();
    }

SocketAddress::View Socket::getLocalSocketAddress() const
    {
    SocketAddress::View vAddrLocal = f_vAddrLocal;
    if (NULL == vAddrLocal)
        {
        COH_SYNCHRONIZED (this)
            {
            vAddrLocal = f_vAddrLocal;
            if (NULL == vAddrLocal)
                {
                vAddrLocal = ensureNativeSocket()->getLocalAddress();
                initialize(f_vAddrLocal, vAddrLocal);
                }
            }
        }
    return vAddrLocal;
    }

void Socket::connect(SocketAddress::View vAddrRemote, int64_t cMillis)
    {
    if (vAddrRemote->isUnresolved())
        {
        vAddrRemote = vAddrRemote->resolve();
        }

    COH_SYNCHRONIZED (this)
        {
        if (isConnected())
            {
            COH_THROW (IOException::create("socket is already connected"));
            }

        if (cMillis < 0)
            {
            COH_THROW (IllegalArgumentException::create("cMillisTimeout cannot be negative"));
            }

        int64_t cMillisInterrupt = System::getInterruptResolution();
        int64_t cMillisTimeout   = Thread::remainingTimeoutMillis();

        if (cMillisTimeout != Integer64::max_value && (cMillisTimeout < cMillis || cMillis == 0))
            {
            cMillis = cMillisTimeout;
            }

        int64_t cMillisRemain = cMillis;
        int64_t ldtEnd        = cMillis == 0 ? 0
                                 : System::currentTimeMillis() + cMillis;

        bool fConnected = false;
        while (!fConnected)
            {
            if (Thread::interrupted())
                {
                COH_THROW (SocketTimeoutException::create("socket connect interrupt"));
                }
            else if (cMillis == 0)
                {
                if (ensureNativeSocket()->connect(vAddrRemote, cMillisInterrupt))
                    {
                    fConnected = true;
                    }
                }
            else if (ensureNativeSocket()->connect(vAddrRemote,
                        std::min(cMillisRemain, cMillisInterrupt)))
                {
                fConnected = true;
                }
            else
                {
                int64_t ldtNow = System::currentTimeMillis();
                if (ldtNow >= ldtEnd || ldtNow < ldtEnd - cMillis)
                    {
                    COH_THROW_STREAM (SocketTimeoutException,
                            "socket connect timeout; " << cMillis << "ms");
                    }
                cMillisRemain = ldtEnd - ldtNow;
                }
            }

        m_hInput  = SocketInput::create(this);
        m_hOutput = SocketOutput::create(this);
        }
    }

bool Socket::isConnected() const
    {
    return NULL != getRemoteSocketAddress();
    }

SocketAddress::View Socket::getRemoteSocketAddress() const
    {
    SocketAddress::View vAddrRemote = f_vAddrRemote;
    if (NULL == vAddrRemote)
        {
        COH_SYNCHRONIZED (this)
            {
            vAddrRemote = f_vAddrRemote;
            if (NULL == vAddrRemote)
                {
                vAddrRemote = ensureNativeSocket()->getRemoteAddress();
                initialize(f_vAddrRemote, vAddrRemote);
                }
            }
        }
    return vAddrRemote;
    }

void Socket::close()
    {
    // Note: we only perform a "logical close", we don't do an actual close
    // as that will release the file descriptor and lead to race conditions
    // if there were any threads actively reading/writting against that
    // descriptor. Though the OS may give a nice error about the FD being
    // invalid, the FD could also be immediately reused (by this process), in
    // which case the IO would occur against the wrong resource. It is for
    // these reasons that we only do a shutdown, and allow the eventual
    // deletion of the NativeSocket (in our destructor), to do the close
    COH_SYNCHRONIZED (this)
        {
        ensureNativeSocket()->shutdown();
        m_fClosed     = true;
        m_hInput      = NULL;
        m_hOutput     = NULL;
        }
    }

bool Socket::isClosed() const
    {
    return m_fClosed;
    }

void Socket::setKeepAlive(bool fKeepAlive)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::keep_alive, fKeepAlive);
    }

bool Socket::getKeepAlive() const
    {
    return ensureNativeSocket()->
        getSocketOption(NativeSocket::Options::keep_alive) != 0;
    }

void Socket::setSoLinger(bool fLinger, int32_t cLingerSec)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::linger, fLinger ? cLingerSec : -1);
    }

int32_t Socket::getSoLinger() const
    {
    return ensureNativeSocket()->getSocketOption(NativeSocket::Options::linger);
    }

void Socket::setReuseAddress(bool fReuse)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::reuse_addr, fReuse);
    }

bool Socket::getReuseAddress() const
    {
    return ensureNativeSocket()->
        getSocketOption(NativeSocket::Options::reuse_addr) != 0;
    }


void Socket::setTcpNoDelay(bool fNoDelay)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::tcp_no_delay, fNoDelay);
    }

bool Socket::getTcpNoDelay() const
    {
    return ensureNativeSocket()->
        getSocketOption(NativeSocket::Options::tcp_no_delay) != 0;
    }

void Socket::setReceiveBufferSize(int32_t cBytes)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::receive_buffer, cBytes);
    }

int32_t Socket::getReceiveBufferSize() const
    {
    return ensureNativeSocket()->
        getSocketOption(NativeSocket::Options::receive_buffer);
    }

void Socket::setSendBufferSize(int32_t cb)
    {
    ensureNativeSocket()->
        setSocketOption(NativeSocket::Options::send_buffer, cb);
    }

int32_t Socket::getSendBufferSize() const
    {
    return ensureNativeSocket()->
        getSocketOption(NativeSocket::Options::send_buffer);
    }

void Socket::setSoTimeout(int32_t cMillis)
    {
    m_cMillisTimeout = cMillis < 0 ? 0 : cMillis;
    }

int32_t Socket::getSoTimeout() const
    {
    return m_cMillisTimeout;
    }

NativeSocket* Socket::ensureNativeSocket(bool fEnsureConnected)
    {
    if (isClosed())
        {
        COH_THROW (IOException::create("Socket is closed"));
        }

    if (!fEnsureConnected || isConnected())
        {
        return m_pNative;
        }

    COH_THROW (IOException::create("Socket is not connected"));
    }

const NativeSocket* Socket::ensureNativeSocket() const
    {
    if (isClosed())
        {
        COH_THROW (IOException::create("Socket is closed"));
        }

    return m_pNative;
    }

size32_t Socket::readInternal(octet_t* ab, size32_t cb)
    {
    int64_t  cMillis        = m_cMillisTimeout;
    int64_t  cMillisIntr    = System::getInterruptResolution();
    int64_t  cMillisTimeout = Thread::remainingTimeoutMillis();

    if (cMillisTimeout != Integer64::max_value && (cMillisTimeout < cMillis || cMillis == 0))
        {
        cMillis = cMillisTimeout;
        }

    int64_t ldtEnd = cMillis ? System::currentTimeMillis() + cMillis : 0;

    while (true)
        {
        if (Thread::interrupted())
           {
           COH_THROW (InterruptedIOException::create("socket read interrupt"));
           }

        size32_t cRead = ensureNativeSocket(true)->read(ab, cb,
                cMillis ? std::min(cMillisIntr, cMillis) : cMillisIntr);

        if ((cRead == 0 || cRead == NativeSocket::npos) && Thread::interrupted())
            {
            // in the case of interrupt and close, we must throw interrupt
            COH_THROW (InterruptedIOException::create("socket read interrupt"));
            }
        else if (cRead == NativeSocket::npos)
            {
            COH_THROW (IOException::create("socket disconnect"));
            }

        // if there is no octets remaining to read or at least one byte was read
        // no bytes were returned during the last read.
        if (cRead > 0)
            {
            return cRead;
            }

        if (cMillis)
             {
             // check for timeout
             cMillis = ldtEnd - System::currentTimeMillis();
             if (cMillis <= 0)
                 {
                 COH_THROW (
                         SocketTimeoutException::create("socket read timeout"));
                 }
             }
        }
    }

void Socket::writeInternal(const octet_t* ab, size32_t cb)
    {
    int64_t cMillis        = m_cMillisTimeout;
    int64_t cMillisIntr    = System::getInterruptResolution();
    int64_t cMillisTimeout = Thread::remainingTimeoutMillis();

    if (cMillisTimeout != Integer64::max_value && (cMillisTimeout < cMillis || cMillis == 0))
        {
        cMillis = cMillisTimeout;
        }

    int64_t  ldtEnd   = cMillis ? System::currentTimeMillis() + cMillis : 0;
    size32_t cbRemain = cb;

    while (true)
        {
        if (Thread::interrupted())
            {
            COH_THROW (InterruptedIOException::create("socket write interrupt",
                   cb - cbRemain));
            }

        size32_t cWrite = ensureNativeSocket(true)->write(ab, cbRemain,
                cMillis ? std::min(cMillisIntr, cMillis) : cMillisIntr);

        if ((cWrite == 0 || cWrite == NativeSocket::npos) && Thread::interrupted())
            {
            // in the case of interrupt and close, we must throw interrupt
            COH_THROW (InterruptedIOException::create("socket read interrupt"));
            }
        else if (cWrite == NativeSocket::npos)
            {
            COH_THROW (IOException::create("socket disconnect"));
            }

        if ((cbRemain -= cWrite) == 0)
            {
            return;
            }
        ab += cWrite;

        if (cMillis)
             {
             // check for timeout
             cMillis = ldtEnd - System::currentTimeMillis();
             if (cMillis <= 0)
                 {
                 COH_THROW (SocketTimeoutException::create(
                         "socket write timeout", cb - cbRemain));
                 }
             }
        }
    }

InputStream::Handle Socket::getInputStream()
    {
    SocketInput::Handle hInput = m_hInput;
    if (hInput == NULL)
        {
        COH_THROW (IOException::create("SocketInput closed or not connected"));
        }
    return hInput;
    }

OutputStream::Handle Socket::getOutputStream()
    {
    SocketOutput::Handle hOutput = m_hOutput;
    if (hOutput == NULL)
        {
        COH_THROW (IOException::create("SocketOutput closed or not connected"));
        }
    return hOutput;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Socket::toString() const
    {
    String::View vs = "Socket[";
    if (isClosed())
        {
        vs = COH_TO_STRING(vs << "closed");
        }
    else
        {
        if (isConnected())
            {
            vs = COH_TO_STRING(vs << "remote=" << getRemoteSocketAddress());
            }
        else
            {
            vs = COH_TO_STRING(vs << "unconnected");
            }

        vs = COH_TO_STRING(vs << ", ");

        if (isBound())
            {
            vs = COH_TO_STRING(vs << "local=" << getLocalSocketAddress());
            }
        else
            {
            vs = COH_TO_STRING(vs << "unbound");
            }
        }
    return COH_TO_STRING(vs << ']');
    }

// ------ inner class: SocketInput ------------------------------------------

Socket::SocketInput::SocketInput(Socket::Handle hSocket)
        : f_hSocket(self(), hSocket)
    {
    }

octet_t Socket::SocketInput::read()
    {
    octet_t b;
    f_hSocket->readInternal(&b, 1);
    return b;
    }

size32_t Socket::SocketInput::read(Array<octet_t>::Handle hab, size32_t i,
        size32_t cb)
    {
    size32_t cbBuffer = hab->length;

    if (cb == npos)
        {
        cb = cbBuffer - i;
        }

    if (i + cb > cbBuffer)
        {
        COH_THROW (IndexOutOfBoundsException::create());
        }

    return f_hSocket->readInternal(hab->raw + i, cb);
    }

size32_t Socket::SocketInput::available()
    {
    return f_hSocket->ensureNativeSocket(true)->available();
    }

void Socket::SocketInput::close()
    {
    Socket::Handle hSocket = f_hSocket;
    COH_SYNCHRONIZED (hSocket)
        {
        hSocket->ensureNativeSocket()->shutdown(/*fIn*/ true, /*fOut*/ false);
        hSocket->m_hInput = NULL;
        }
    }

TypedHandle<const String> Socket::SocketInput::toString() const
    {
    return COH_TO_STRING("SocketInput[" << f_hSocket << ']');
    }

// ----- inner class: SocketOutput ------------------------------------------

Socket::SocketOutput::SocketOutput(Socket::Handle hSocket)
        : f_hSocket(self(), hSocket)
    {
    }

void Socket::SocketOutput::write(octet_t b)
    {
    f_hSocket->writeInternal(&b, 1);
    }

void Socket::SocketOutput::write(Array<octet_t>::View vab, size32_t i,
        size32_t cb)
    {
    size32_t cbBuffer = vab->length;

    if (cb == npos)
        {
        cb = cbBuffer - i;
        }

    if (i + cb > cbBuffer)
        {
        COH_THROW (IndexOutOfBoundsException::create());
        }

    f_hSocket->writeInternal(vab->raw + i, cb);
    }

void Socket::SocketOutput::close()
    {
    Socket::Handle hSocket = f_hSocket;
    COH_SYNCHRONIZED (hSocket)
        {
        hSocket->ensureNativeSocket()->shutdown(/*fIn*/ false, /*fOut*/ true);
        hSocket->m_hOutput = NULL;
        }
    }

TypedHandle<const String> Socket::SocketOutput::toString() const
    {
    return COH_TO_STRING("SocketOutput[" << f_hSocket << ']');
    }

COH_CLOSE_NAMESPACE2
