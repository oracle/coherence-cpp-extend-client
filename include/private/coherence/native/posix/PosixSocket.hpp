/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POSIX_SOCKET_HPP
#define COH_POSIX_SOCKET_HPP

#include "coherence/lang.ns"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/IOException.hpp"

#include "private/coherence/util/logging/Logger.hpp"
#include "private/coherence/native/NativeSocket.hpp"
#include "private/coherence/native/posix/PosixInetHelper.hpp"
#include "private/coherence/native/posix/PosixInetSocketAddress.hpp"
#include "private/coherence/native/posix/PosixRawSocketAddress.hpp"
#include "private/coherence/native/posix/PosixSocketAddress.hpp"

#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

// Solaris doesn't automatically include filio.h from ioctl and this file
// doesn't exists on Linux. But it is there on OS X. FIONREAD is the constant
// that we need, check if it has been defined or not.
#ifndef  FIONREAD
#include <sys/filio.h>
#endif



COH_OPEN_NAMESPACE3(coherence,native,posix)

using coherence::io::EOFException;
using coherence::io::IOException;
using coherence::native::NativeSocket;


/**
* Throw an IOException based on a standard socket error.
*/
#define COH_THROW_ERRNO(ERR, MSG) \
    { \
    int  nErrorNo = ERR; \
    char achError[256]; \
    achError[0] = 0; \
    strerror_r(nErrorNo, achError, 255); \
    COH_LOG(MSG << "; errno[" << nErrorNo << "] " << achError, 9); \
    COH_THROW_STREAM (IOException, MSG << "; errno[" << nErrorNo \
            << "] " << achError); \
    }

/**
* Run a socket command, and throw an IOException with error if it fails.
*/
#define COH_ENSURE_SOCKET(CMD) \
    if (CMD) \
        { \
        COH_THROW_ERRNO(errno, #CMD << " failed"); \
        }

/**
* PosixSocket interface.
*
* @author mf  2008.3.20
*/
class COH_EXPORT PosixSocket
        : public NativeSocket
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Create a new PosixSocket.
        *
        * @paran nSocket  an existing socket file descriptor, or -1 to create
        *                 a new one
        */
        PosixSocket(int nSocket = -1)
                : m_nSocket(nSocket), m_nTxFlags(0),
                  m_nRxFlags(0), m_cMillisTimeoutRx(0), m_cMillisTimeoutTx(0), m_fBlocking(false)
            {
            if (nSocket < 0)
                {
                // create a socket
                nSocket = socket(PosixInetHelper::getFamily(), SOCK_STREAM, IPPROTO_TCP);
                if (nSocket < 0)
                    {
                    COH_THROW_ERRNO(errno, "Posix socket creation failed");
                    }
                m_nSocket = nSocket;
                }

            // disable sig-pipe (platform specific). We must be ready to deal
            // with SIG_PIPE in the case that the remote side of the socket
            // disconnects, there are a few ways to do this:
#ifdef SO_NOSIGPIPE // BSD/OSX
            // disable the signal for this socket, and rely on the EPIPE error
            // code
            int nValue = 1;
            COH_ENSURE_SOCKET(setsockopt(nSocket, SOL_SOCKET, SO_NOSIGPIPE, (void*) &nValue, sizeof(nValue)));
#elif defined(MSG_NOSIGNAL) // Linux
            // disable the signal as part of each send/recv operation, and
            // rely on EPIPE
            // used in send/recv to disable signals, see transfer() method
            m_nRxFlags |= MSG_NOSIGNAL;
            m_nTxFlags |= MSG_NOSIGNAL;
#else // Solaris
            // Disabling signals on a per-socket basis is not supported on
            // this platform. Disable the signal at the process level.
            static bool fFirst = true;
            if (fFirst)
                {
                signal(SIGPIPE, SIG_IGN);
                fFirst = false;
                }
#endif

            // attempt to set SO_TIMEO in order to discover if we need to be non-blocking
            // we do this up front with a random value to avoid switching at runtime which
            // may not be safe especially if done concurrently on tx & rx
            if (setSoTimeout(nSocket, SO_RCVTIMEO, 1) < 0 ||
                setSoTimeout(nSocket, SO_SNDTIMEO, 1) < 0)
                {
                // reset (in case one succeeded)
                setSoTimeout(nSocket, SO_RCVTIMEO, 0);
                setSoTimeout(nSocket, SO_SNDTIMEO, 0);

                // switch to more expensive non-blocking mode
               ::fcntl(nSocket, F_SETFL, ::fcntl(nSocket, F_GETFL) | O_NONBLOCK);

                m_cMillisTimeoutRx = m_cMillisTimeoutTx = -1;
                m_fBlocking        = false;
                }
            else
                {
                m_cMillisTimeoutRx = m_cMillisTimeoutTx = 1;
                m_fBlocking        = true;
                }
            }

        /**
        * @internal
        */
        virtual ~PosixSocket()
            {
            try
                {
                PosixSocket::close();
                }
            catch (Exception::View e)
                {
                COH_LOGEXMSG (e,
                        "exception while closing Posix socket: " << this, 1);
                // eat it
                }
            }

    // ----- PosixSocket inteface -------------------------------------------

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
            int nSocket = m_nSocket;
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

            int nSelect = ::select(nSocket + 1, &fdRead, &fdWrite, &fdError,
                    &tvTimeout);

            switch (nSelect)
                {
                case -1: // error
                    {
                    int nError = errno;
                    if (EINTR == nError)
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
                                (void*) &nSockError, &cBytes))
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
         * @return the timeout or -1 if not supported
         */
        int64_t setSoTimeout(int nSocket, int nOpt, int64_t cMillisTimeout)
            {
            timeval tv;
            tv.tv_sec  = (int32_t) (cMillisTimeout / 1000);
            tv.tv_usec = (int32_t) (cMillisTimeout % 1000) * 1000;

            if (::setsockopt(nSocket, SOL_SOCKET, nOpt, (void*) &tv, (socklen_t) sizeof(timeval)) == 0)
                {
                return cMillisTimeout;
                }

            return -1;
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
            int     nSocket   = m_nSocket;
            bool    fBlocking = m_fBlocking;
            ssize_t nResult;
            if (chan == channel_in)
                {
                if (fBlocking && cMillisTimeout != m_cMillisTimeoutRx)
                    {
                    m_cMillisTimeoutRx = setSoTimeout(nSocket, SO_RCVTIMEO, cMillisTimeout);
                    }

                nResult = ::recv(nSocket, ab, cb, m_nRxFlags); // optimitic recv (try to avoid select cost)
                if (nResult < 0 && !fBlocking && select(chan, cMillisTimeout) == chan)
                    {
                    // prior recv was optimistic non-blocking, try again after select
                    nResult = ::recv(nSocket, ab, cb, m_nRxFlags);
                    if (nResult == 0) // disconnect
                        {
                        // select indicated that there was data, but recv
                        // returned nothing, this indicates that the socket
                        // has been closed by the remote end
                        return npos;
                        }
                    }
                }
            else
                {
                if (fBlocking && cMillisTimeout != m_cMillisTimeoutTx)
                    {
                    m_cMillisTimeoutTx = setSoTimeout(nSocket, SO_SNDTIMEO, cMillisTimeout);
                    }

                nResult = ::send(nSocket, ab, cb, m_nTxFlags); // optimistic send (try to avoid select cost)
                if (nResult < 0 && !fBlocking && select(chan, cMillisTimeout) == chan)
                    {
                    // prior send was optimistic non-blocking, try again after select
                    nResult = ::send(nSocket, ab, cb, m_nTxFlags); // select indicated send ready
                    }
                }

            if (nResult > 0) // work was done
                {
                return (size32_t) nResult;
                }
            else if (nResult < 0) // error
                {
                int nError = errno;
                switch (nError)
                    {
                    case EAGAIN:
    #if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                     // EAGAIN == EWOULDBLOCK on OS X
                     case EWOULDBLOCK:
    #endif
                    case EINTR:
                    case ETIMEDOUT:
                        break; // soft error
                               // return 0 in the case of recv

                    case EPIPE:
                    case ECONNRESET: // peer closed connection
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
                    oLinger.l_linger = nValue;

                    COH_ENSURE_SOCKET(setsockopt(m_nSocket, SOL_SOCKET, SO_LINGER,
                            (void*) &oLinger, (socklen_t) sizeof(oLinger)));
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
            int nSockValue = nValue;
            COH_ENSURE_SOCKET(setsockopt(m_nSocket, nLevel, nId,
                    (void*) &nSockValue, (socklen_t) sizeof(nSockValue)));
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
                            (void*) &oLinger, &cBytes));
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
            COH_ENSURE_SOCKET(getsockopt(m_nSocket, nLevel, nId, (void*) &nValue, &cBytes));
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

            COH_ENSURE_SOCKET(getsockname(m_nSocket, pAddr, &cbAddr));

            if (cbAddr == 0)
                {
                return NULL;
                }
            else if (cbAddr > cBytes)
                {
                // assumed size was insufficient, resize and retry
                habAddr = Array<octet_t>::create(cbAddr);
                pAddr   = (sockaddr*) &(habAddr[0]);
                COH_ENSURE_SOCKET(getsockname(m_nSocket, pAddr, &cbAddr));
                COH_ENSURE(cbAddr <= habAddr->length);
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
            for (socklen_t i = 0, c = cbAddr - (socklen_t) sizeof(sockaddr);
                    !fBound && i < c; ++i)
                {
                fBound |= pAddr->sa_data[i];
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
            socklen_t              cbAddr  = cBytes;

            if (getpeername(m_nSocket, pAddr, &cbAddr))
                {
                int nError = errno;

                if (ENOTCONN == nError || EINVAL == nError)
                    {
                    return NULL;
                    }
                COH_THROW_ERRNO (nError, "Socket getpeername failed");
                }

            if (cbAddr == 0)
                {
                return NULL;
                }
            else if (cbAddr > cBytes)
                {
                // assumed size was insufficient, resize and retry
                habAddr = Array<octet_t>::create(cbAddr);
                pAddr   = (sockaddr*) &(habAddr[0]);
                if (getpeername(m_nSocket, pAddr, &cbAddr))
                    {
                    int nError = errno;

                    if (EINVAL == nError)
                        {
                        nError = ENOTCONN;
                        }
                    COH_THROW_ERRNO (nError, "Socket getpeername failed");
                    }
                COH_ENSURE(cbAddr <= habAddr->length);
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
                if (::shutdown(m_nSocket, fIn && fOut ? SHUT_RDWR
                        : fIn ? SHUT_RD : SHUT_WR))
                    {
                    int nError = errno;
                    if (fConnectError || nError != ENOTCONN)
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
            int nSocket = m_nSocket;
            if (nSocket != -1)
                {
                // Note: If linger is enabled then Thread::interrupt will not
                // be effective here and would wait until the linger time
                // expired

                COH_ENSURE_SOCKET(::close(nSocket));
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
            int       nSocket  = m_nSocket;
            int       nConnect = ::connect(nSocket, vAddrPosix->getSockAddr(),
                                    cbAddr) ? errno : EISCONN;

            switch (nConnect)
                {
                case EISCONN:     // connected
                    return true;

                case EALREADY:    // connect already in progress
                case EINPROGRESS: // async connect
                case EINTR:       // OS interrupt occured, connect will be async
                     break;

                case EAFNOSUPPORT: // unsupported address type
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
                if (getpeername(nSocket, &addrPeer, &cbPeer))
                    {
                    int nError = errno;

                    if (EINVAL == nError)
                        {
                        nError = ENOTCONN;
                        }
                    COH_THROW_ERRNO (nError, "Socket getpeername failed");
                    }
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
                int nSocketAccept = ::accept(m_nSocket, NULL, 0);
                if (nSocketAccept < 0)
                     {
                     int nError = errno;
                     switch (nError)
                         {
                         case EAGAIN:
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                         // EAGAIN == EWOULDBLOCK on OS X
                         case EWOULDBLOCK:
#endif
                         case EINTR:
                         case ECONNABORTED:
                             break; // soft error

                         default:
                             COH_THROW_ERRNO (nError, "Socket accept failed");
                         }
                     }
                else
                    {
                    return new PosixSocket(nSocketAccept);
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
            int cb = 0;
            if (::ioctl(m_nSocket, FIONREAD, &cb) == -1)
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
        int m_nSocket;

        /**
        * Flags used in send.
        */
        int m_nTxFlags;

        /**
        * Flags used in recv.
        */
        int m_nRxFlags;

        /**
         * The configured receive timeout
         */
        int64_t m_cMillisTimeoutRx;

        /**
         * The configured send timeout
         */
        int64_t m_cMillisTimeoutTx;

        /**
         * True if the socket uses blocking IO.
         */
        bool m_fBlocking;
    };

COH_CLOSE_NAMESPACE3


// ----- NativeSocket statics -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,native)

NativeSocket* NativeSocket::create()
    {
    return new coherence::native::posix::PosixSocket();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_POSIX_SOCKET_HPP
