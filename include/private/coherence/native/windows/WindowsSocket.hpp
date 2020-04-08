/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_SOCKET_HPP
#define COH_WINDOWS_SOCKET_HPP

#include "coherence/lang.ns"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/IOException.hpp"

#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/native/NativeSocket.hpp"
#include "private/coherence/native/posix/PosixInetHelper.hpp"
#include "private/coherence/native/posix/PosixInetSocketAddress.hpp"
#include "private/coherence/native/posix/PosixRawSocketAddress.hpp"
#include "private/coherence/native/posix/PosixSocketAddress.hpp"
#include "private/coherence/native/windows/WinSockInitializer.hpp"

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <sstream>
#include <string.h>

COH_OPEN_NAMESPACE3(coherence,native,windows)

using coherence::io::EOFException;
using coherence::io::IOException;
using coherence::native::posix::PosixInetHelper;
using coherence::native::posix::PosixInetSocketAddress;
using coherence::native::posix::PosixRawSocketAddress;
using coherence::native::posix::PosixSocketAddress;
using coherence::native::NativeSocket;


/**
* Build up a case statement for COH_THROW_ERRNO
*/
#define COH_CASE_ERROR(ERRNO) \
    case ERRNO: \
        achError = #ERRNO; \
        break

/**
* Throw an IOException based on WSA error.
*/
#define COH_THROW_ERRNO(ERR, MSG) \
    { \
    int         nErrorNo = ERR; \
    const char* achError = NULL; \
    switch (nErrorNo) \
        { \
        COH_CASE_ERROR(WSAEAFNOSUPPORT); \
        COH_CASE_ERROR(WSAEALREADY); \
        COH_CASE_ERROR(WSAECONNABORTED); \
        COH_CASE_ERROR(WSAECONNRESET); \
        COH_CASE_ERROR(WSAEINPROGRESS); \
        COH_CASE_ERROR(WSAEINVAL); \
        COH_CASE_ERROR(WSAEINTR); \
        COH_CASE_ERROR(WSAEISCONN); \
        COH_CASE_ERROR(WSAENOTCONN); \
        COH_CASE_ERROR(WSAEWOULDBLOCK); \
        default: \
            achError = "refer to winsock2 error code for details"; \
            break; \
        } \
    COH_LOG(MSG << "; WSA error[" << nErrorNo << "] " << achError, 9); \
    COH_THROW_STREAM (IOException, MSG << "; WSA error[" << nErrorNo \
            << "] " << achError); \
    }

/**
* Run a socket command, and throw an IOException with error if it fails.
*/
#define COH_ENSURE_SOCKET(CMD) \
    if (CMD) \
        { \
        COH_THROW_ERRNO(WSAGetLastError(), #CMD << " failed"); \
        }

/**
* WindowsSocket interface.
*
* @author mf  2008.3.28
*/
class COH_EXPORT WindowsSocket
        : public NativeSocket
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create a new WindowsSocket.
        *
        * @paran nSocket  an existing socket file descriptor, or -1 to create
        *                 a new one
        */
        WindowsSocket(SOCKET nSocket = INVALID_SOCKET)
                : m_nSocket(nSocket), m_cMillisTimeoutRx(0), m_cMillisTimeoutTx(0)
            {
            // ensure winsock has been loaded
            static const bool init_once = (WinSockInitializer::ensure(), true);

            if (nSocket == INVALID_SOCKET)
                {
                // create a socket
                nSocket = socket(PosixInetHelper::getFamily(), SOCK_STREAM, IPPROTO_TCP);
                if (nSocket == INVALID_SOCKET)
                    {
                    COH_THROW_ERRNO(WSAGetLastError(), "Windows socket creation failed");
                    }
                m_nSocket = nSocket;
                }
            }

        /**
        * @internal
        */
        virtual ~WindowsSocket()
            {
            try
                {
                WindowsSocket::close();
                }
            catch (Exception::View e)
                {
                COH_LOGEXMSG (e,
                        "exception while closing Windows socket: " << this, 1);
                // eat it
                }
            }

    // ----- WindowsSocket inteface -------------------------------------------

    private:

        /**
        * Socket channel definitions
        */
        typedef enum
            {
            channel_none = 0,
            channel_in   = 1,
            channel_out  = 2,
            channel_err  = 4
            } channel_t;

        /**
        * Wait for data on socket.
        *
        * @param chan            the channels to check
        * @param cMillisTimeout  the maximum duration to wait for data, or 0
        *                        for indefinite
        *
        * @return the available channels or channel_none indicating a timeout
        *
        * @throws IOException on IO error
        */
        channel_t select(int chan, int64_t cMillisTimeout)
            {
            // wait for socket to connect, error, or timeout
            SOCKET nSocket = m_nSocket;
            if (nSocket == -1)
                {
                COH_THROW(IOException::create("Socket closed"));
                }

            fd_set fdRead, fdWrite, fdError;
            FD_ZERO(&fdRead);
            FD_ZERO(&fdWrite);
            FD_ZERO(&fdError);

            if (chan & channel_in)
                {
                FD_SET(nSocket, &fdRead);
                }
            if (chan & channel_out)
                {
                FD_SET(nSocket, &fdWrite);
                }
            if (chan & channel_err)
                {
                FD_SET(nSocket, &fdError);
                }

            timeval tvTimeout;
            tvTimeout.tv_sec  = (int)  (cMillisTimeout / 1000);
            tvTimeout.tv_usec = (int) ((cMillisTimeout % 1000) * 1000);

            int nSelect = ::select((int) (nSocket + 1), &fdRead, &fdWrite, &fdError,
                    &tvTimeout);

            switch (nSelect)
                {
                case -1: // error
                    {
                    int nError = WSAGetLastError();
                    if (WSAEINTR == nError)
                        {
                        // interrupt had occured
                        return channel_none;
                        }
                    COH_THROW_ERRNO (nError, "Socket select error");
                    }

                case 0: // timeout
                return channel_none;

                default:
                    {
                    if (FD_ISSET(nSocket, &fdError))
                        {
                        int       nSockError;
                        socklen_t cBytes = sizeof(nSockError);
                        if (::getsockopt(nSocket, SOL_SOCKET, SO_ERROR,
                                        (char*) &nSockError, &cBytes))
                            {
                            // error code not available
                            COH_THROW (IOException::create("Socket SO_ERROR"));
                            }
                        else
                            {
                            COH_THROW_ERRNO (nSockError, "Socket SO_ERROR");
                            }
                        }

                    // either read, write, or error is set
                    int chanRet = channel_none;
                    if (chan & channel_in && FD_ISSET(nSocket, &fdRead))
                        {
                        chanRet |= channel_in;
                        }
                    if (chan & channel_out && FD_ISSET(nSocket, &fdWrite))
                        {
                        chanRet |= channel_out;
                        }
                    if (chan & channel_err && FD_ISSET(nSocket, &fdError))
                        {
                        chanRet |= channel_err;
                        }
                    return (channel_t) chanRet;
                    }
                }
            }

        /**
         * Set the socket timeout.
         *
         * @param nSocket         the socket
         * @param nOpt            the timeout option to set
         * @param cMillisTimeout  the timeout
         *
         * @return the timeout
         */
        int64_t setSoTimeout(SOCKET nSocket, int nOpt, int64_t cMillisTimeout)
            {
            DWORD tv = (DWORD) cMillisTimeout;

            COH_ENSURE_SOCKET(::setsockopt(nSocket, SOL_SOCKET, nOpt, (const char*) &tv, (socklen_t) sizeof(tv)));

            return cMillisTimeout;
            }

        /**
        * Read or write data from the socket.
        *
        * @param chan            a single channel on which to transfer data
        * @param ab              source or destination array
        * @param cb              size of array
        * @param cMillisTimeout  the operation timeout
        *
        * @return the amount of data read or written, or npos if the socket
        *         has been closed
        */
        size32_t transfer(channel_t chan, octet_t ab[], size32_t cb,
                int64_t cMillisTimeout)
            {
            SOCKET nSocket = m_nSocket;
            int nResult;
            if (chan == channel_in)
                {
                if (cMillisTimeout != m_cMillisTimeoutRx)
                    {
                    m_cMillisTimeoutRx = setSoTimeout(nSocket, SO_RCVTIMEO, cMillisTimeout);
                    }

                nResult = ::recv(nSocket, (char*) ab, (int) cb, 0);
                }
            else
                {
                if (cMillisTimeout != m_cMillisTimeoutTx)
                    {
                    m_cMillisTimeoutTx = setSoTimeout(nSocket, SO_SNDTIMEO, cMillisTimeout);
                    }

                nResult = ::send(nSocket, (const char*) ab, (int) cb, 0);
                }

            if (nResult > 0) // ready
                {
                return (size32_t) nResult;
                }
            else if (nResult < 0) // error
                {
                int nError = WSAGetLastError();
                switch (nError)
                    {
                    case WSA_IO_PENDING: // Bug 25237462 - we should not get WSA_IO_PENDING. Handle as a soft error
                        COH_LOG("Handling unexpected socket " << (chan == channel_in ? "input" : "output") << " return of WSA_IO_PENDING as WSAEWOULDBLOCK", 9);
                        // fall through
                    case WSAEWOULDBLOCK:
                    case WSAEINTR:
                    case WSAEINPROGRESS:
                    case WSAETIMEDOUT:
                    break; // soft error
                    // return with 0 octets read
                    case WSAECONNRESET: // peer closed connection
                        COH_THROW (EOFException::create("connection reset by peer"));

                    default:
                        COH_THROW_ERRNO (nError, "Socket read/write returned " << nResult);
                    }
                }

            return 0; // timeout
            }


    // ----- NativeSocket interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void setSocketOption(Options::so_option_t nOption, int32_t nValue)
            {
            int nId;
            int nLevel = SOL_SOCKET;

            switch (nOption)
                {
                case Options::linger:
                    {
                    linger oLinger;

                    oLinger.l_onoff  = nValue >= 0;
                    oLinger.l_linger = (u_short) nValue;

                    COH_ENSURE_SOCKET(setsockopt(m_nSocket, SOL_SOCKET, SO_LINGER,
                            (char*) &oLinger, sizeof(oLinger)));
                    return;
                    }

                case Options::tcp_no_delay:
                    nLevel = IPPROTO_TCP;
                    nId    = TCP_NODELAY;
                    break;

                case Options::reuse_addr:
                    nId = SO_REUSEADDR;
                    break;

                case Options::keep_alive:
                    nId = SO_KEEPALIVE;
                    break;

                case Options::receive_buffer:
                    nId = SO_RCVBUF;
                    break;

                case Options::send_buffer:
                    nId = SO_SNDBUF;
                    break;

                default:
                    COH_THROW_STREAM (IllegalArgumentException,
                            "unrecognized socket option " << nOption);
                }

            // int based values
            COH_ENSURE_SOCKET(setsockopt(m_nSocket, nLevel, nId,
                    (char*) &nValue, sizeof(nValue)));
            }

        /**
        * {@inheritDoc}
        */
        virtual int32_t getSocketOption(Options::so_option_t nOption) const
            {
            int nLevel = SOL_SOCKET;
            int nId;

            switch (nOption)
                {
                case Options::linger:
                    {
                    linger    oLinger;
                    socklen_t cBytes = sizeof(oLinger);

                    COH_ENSURE_SOCKET(getsockopt(m_nSocket, SOL_SOCKET, SO_LINGER,
                            (char*) &oLinger, &cBytes));
                    COH_ENSURE_EQUALITY(cBytes, sizeof(oLinger));
                    return oLinger.l_onoff ? oLinger.l_linger : -1;
                    }

                case Options::tcp_no_delay:
                    nLevel = IPPROTO_TCP;
                    nId    = TCP_NODELAY;
                    break;

                case Options::reuse_addr:
                    nId = SO_REUSEADDR;
                    break;

                case Options::keep_alive:
                    nId = SO_KEEPALIVE;
                    break;

                case Options::receive_buffer:
                    nId = SO_RCVBUF;
                    break;

                case Options::send_buffer:
                    nId = SO_SNDBUF;
                    break;

                 default:
                    COH_THROW_STREAM (IllegalArgumentException,
                            "unrecognized socket option " << nOption);
                }

            // int based values
            int       nValue = 0;
            socklen_t cBytes = sizeof(nValue);
            COH_ENSURE_SOCKET(getsockopt(m_nSocket, nLevel, nId, (char*) &nValue, &cBytes));
            COH_ENSURE_RELATION(socklen_t, cBytes, <=, sizeof(nValue));
            return nValue;
            }

        /**
        * {@inheritDoc}
        */
        virtual SocketAddress::View getLocalAddress(size32_t cBytes = 128) const
            {
            Array<octet_t>::Handle habAddr = Array<octet_t>::create(cBytes);
            sockaddr*              pAddr   = (sockaddr*) &(habAddr[0]);
            socklen_t              cbAddr  = (socklen_t) cBytes;

            if (getsockname(m_nSocket, pAddr, &cbAddr))
                {
                int nError = WSAGetLastError();

                if (WSAEINVAL == nError)
                    {
                    // not bound yet
                    return NULL;
                    }
                COH_THROW_ERRNO (nError, "Socket getsockname failed");
                }

            if (cbAddr == 0)
                {
                return NULL;
                }
            else if (size32_t(cbAddr) > cBytes)
                {
                // assumed size was insufficient, resize and retry
                habAddr = Array<octet_t>::create(cbAddr);
                pAddr   = (sockaddr*) &(habAddr[0]);
                COH_ENSURE_SOCKET(getsockname(m_nSocket, pAddr, &cbAddr));
                COH_ENSURE(size32_t(cbAddr) <= habAddr->length);
                }
            else
                {
                // resize to exact
                habAddr = Array<octet_t>::copy(habAddr, 0, Array<octet_t>::create(cbAddr));
                pAddr   = (sockaddr*) &(habAddr[0]);
                }

            // test for unbound socket, by testing if family is the only
            // non-zero field, ugh
            bool fBound = false;
            for (socklen_t i = 0, c = cbAddr - sizeof(sockaddr);
                    !fBound && i < c; ++i)
                {
                fBound |= (pAddr->sa_data[i] != 0);
                }
            if (!fBound)
                {
                return NULL;
                }

            switch (pAddr->sa_family)
                {
                case AF_INET:
                case AF_INET6:
                    return PosixInetSocketAddress::create(
                            (InetAddress::View) NULL, habAddr);

                default:
                    return PosixRawSocketAddress::create(habAddr);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual SocketAddress::View getRemoteAddress(size32_t cBytes = 128) const
            {
            Array<octet_t>::Handle habAddr = Array<octet_t>::create(cBytes);
            sockaddr*              pAddr   = (sockaddr*) &(habAddr[0]);
            socklen_t              cbAddr  = (socklen_t) cBytes;

            if (getpeername(m_nSocket, pAddr, &cbAddr))
                {
                int nError = WSAGetLastError();

                if (WSAENOTCONN == nError)
                    {
                    return NULL;
                    }
                COH_THROW_ERRNO (nError, "Socket getpeername failed");
                }

            if (cbAddr == 0)
                {
                return NULL;
                }
            else if (size32_t(cbAddr) > cBytes)
                {
                // assumed size was insufficient, resize and retry
                habAddr = Array<octet_t>::create(cbAddr);
                pAddr   = (sockaddr*) &(habAddr[0]);
                COH_ENSURE_SOCKET(getpeername(m_nSocket, pAddr, &cbAddr));
                COH_ENSURE(size32_t(cbAddr) <= habAddr->length);
                }
            else
                {
                // resize to exact
                habAddr = Array<octet_t>::copy(habAddr, 0, Array<octet_t>::create(cbAddr));
                pAddr   = (sockaddr*) &(habAddr[0]);
                }

            switch (pAddr->sa_family)
                {
                case AF_INET:
                case AF_INET6:
                    return PosixInetSocketAddress::create(
                            (InetAddress::View) NULL, habAddr);

                default:
                    return PosixRawSocketAddress::create(habAddr);
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual void bind(SocketAddress::View vAddrLocal)
            {
            PosixSocketAddress::View vAddrPosix =
                    cast<PosixSocketAddress::View>(vAddrLocal);
            COH_ENSURE_SOCKET(::bind(m_nSocket, vAddrPosix->getSockAddr(),
                    vAddrPosix->getSize()));
            }

        /**
        * {@inheritDoc}
        */
        virtual void listen(int32_t cMaxBacklog)
            {
            COH_ENSURE_SOCKET(::listen(m_nSocket, cMaxBacklog));
            }

        /**
        * {@inheritDoc}
        */
        virtual void shutdown(bool fIn, bool fOut, bool fConnectError)
            {
            if (fIn || fOut)
                {
                if (::shutdown(m_nSocket, fIn && fOut ? SD_BOTH
                        : fIn ? SD_RECEIVE : SD_SEND))
                    {
                    int nError = WSAGetLastError();
                    if (fConnectError || nError != WSAENOTCONN)
                        {
                        COH_THROW_ERRNO(nError, "Socket shutdown failed");
                        }
                    }
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual void close()
            {
            SOCKET nSocket = m_nSocket;
            if (nSocket != -1)
                {
                if (getSocketOption(Options::linger) != -1)
                    {
                    // On Windows linger only works on blocking sockets,
                    // otherwise closesocket will fail

                    // Note: Thread::interrupt will not be effective here,
                    // and would wait until the linger time expired
                    u_long lBlocking = 0;
                    COH_ENSURE_SOCKET(ioctlsocket(nSocket, FIONBIO, &lBlocking));
                    }

                COH_ENSURE_SOCKET(::closesocket(nSocket));
                m_nSocket = -1; // mark as closed
                }
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isClosed() const
            {
            return m_nSocket == -1;
            }

        /**
        * {@inheritDoc}
        */
        virtual bool connect(SocketAddress::View vAddrRemote, int64_t cMillisTimeout)
            {
            PosixSocketAddress::View vAddrPosix =
                    cast<PosixSocketAddress::View>(vAddrRemote, false);
            COH_ENSURE_PARAM(vAddrPosix);

            socklen_t cbAddr   = vAddrPosix->getSize();
            SOCKET    nSocket  = m_nSocket;
            int       nConnect = ::connect(nSocket, vAddrPosix->getSockAddr(),
                                    cbAddr) ? WSAGetLastError() : WSAEISCONN;

            switch (nConnect)
                {
                case WSAEISCONN:     // connected
                    return true;

                case WSAEWOULDBLOCK: // not connected yet, proceede async
                    return false;

                case WSAEALREADY:    // connect already in progress
                case WSAEINVAL:      // connect already in progress (Windows sucks)
                case WSAEINPROGRESS: // async connect
                case WSAEINTR:       // OS interrupt occured, connect will be async
                    break;

                case WSAEAFNOSUPPORT: // unsupported address type
                    COH_THROW_STREAM (IllegalArgumentException,
                            "unsupported address type: " << vAddrPosix);

                default: // error
                    COH_THROW_ERRNO (nConnect, "Socket connect failed");
                }

            if (select((channel_t)channel_in | channel_out, cMillisTimeout) != channel_none)
                {
                // channel available
                // ensure connection succeeded
                sockaddr  addrPeer;
                socklen_t cbPeer = sizeof(addrPeer);
                COH_ENSURE_SOCKET(getpeername(nSocket, &addrPeer, &cbPeer));
                if (cbPeer)
                    {
                    return true; // connected
                    }
                }

            // not connected
            return false;
            }

        /**
        * {@inheritDoc}
        */
        virtual NativeSocket* accept(int64_t cMillisTimeout)
            {
            // wait for connection
            if (select(channel_in, cMillisTimeout) == channel_in)
                {
                // channel available
                SOCKET nSocketAccept = ::accept(m_nSocket, NULL, 0);
                if (nSocketAccept < 0)
                     {
                     int nError = WSAGetLastError();
                     switch (nError)
                         {
                         case WSAEWOULDBLOCK:
                         case WSAEINTR:
                         case WSAECONNABORTED:
                         case WSAEINPROGRESS:
                             break; // soft error

                         default:
                             COH_THROW_ERRNO (nError, "Socket accept failed");
                         }
                     }
                else
                    {
                    return new WindowsSocket(nSocketAccept);
                    }
                }

            // no new connection
            return NULL;
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t read(octet_t ab[], size32_t cb,
                int64_t cMillisTimeout)
            {
            return transfer(channel_in, ab, cb, cMillisTimeout);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t write(const octet_t ab[], size32_t cb,
                int64_t cMillisTimeout)
            {
            return transfer(channel_out, const_cast<octet_t*>(ab), cb,
                    cMillisTimeout);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t available()
            {
            unsigned long cb = 0;
            if (::ioctlsocket(m_nSocket, FIONREAD, &cb) == -1)
                {
                COH_THROW_ERRNO(errno, "ioctl(FIONREAD)");
                }

            return (size32_t) cb;
            }


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The socket file descriptor.
        */
        SOCKET m_nSocket;

        /**
         * The configured receive timeout
         */
        int64_t m_cMillisTimeoutRx;

        /**
         * The configured send timeout
         */
        int64_t m_cMillisTimeoutTx;
    };

COH_CLOSE_NAMESPACE3


// ----- NativeSocket statics -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeSocket* NativeSocket::create()
    {
    return new coherence::native::windows::WindowsSocket();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_SOCKET_HPP
