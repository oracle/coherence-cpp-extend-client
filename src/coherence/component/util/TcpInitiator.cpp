/*
 * Copyright (c) 2000, 2023, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/TcpInitiator.hpp"

#include "coherence/io/EOFException.hpp"
#include "coherence/io/IOException.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"

#include "coherence/net/AddressProvider.hpp"
#include "coherence/net/AddressProviderFactory.hpp"
#include "coherence/net/ConfigurableAddressProviderFactory.hpp"
#include "coherence/net/InetAddress.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"
#include "coherence/util/Random.hpp"

#include "private/coherence/component/net/extend/protocol/TcpInitiatorProtocol.hpp"

#include "private/coherence/component/net/extend/util/TcpUtil.hpp"

#include "private/coherence/io/BufferedInputStream.hpp"
#include "private/coherence/io/BufferedOutputStream.hpp"

#include "private/coherence/net/InetAddressHelper.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include "private/coherence/util/logging/Logger.hpp"


COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::protocol::TcpInitiatorProtocol;
using coherence::component::net::extend::util::TcpUtil;
using coherence::io::BufferedInputStream;
using coherence::io::BufferedOutputStream;
using coherence::io::IOException;
using coherence::io::EOFException;
using coherence::io::OctetArrayReadBuffer;
using coherence::net::AddressProvider;
using coherence::net::AddressProviderFactory;
using coherence::net::ConfigurableAddressProviderFactory;
using coherence::net::InetAddress;
using coherence::net::InetAddressHelper;
using coherence::net::messaging::ConnectionException;
using coherence::util::ArrayList;
using coherence::util::Iterator;
using coherence::util::List;
using coherence::util::Random;
using coherence::util::StringHelper;
using coherence::util::logging::Logger;


// ----- constructor --------------------------------------------------------

TcpInitiator::TcpInitiator()
    : m_fKeepAliveEnabled(false),
      m_lLingerTimeout(0),
      f_hLocalAddress(self()),
      m_fLocalAddressReusable(false),
      m_lReceiveBufferSize(0),
      m_hRemoteAddressProvider(self()),
      m_lSendBufferSize(0),
      m_fTcpDelayEnabled(false)
    {
    }


// ----- TcpInitiator interface ---------------------------------------------

void TcpInitiator::configureSocket(Socket::Handle hSocket) const
    {
    try
        {
        // configure SO_KEEPALIVE
        TcpUtil::setKeepAlive(hSocket, isKeepAliveEnabled());

        // configure SO_REUSEADDR
        TcpUtil::setReuseAddress(hSocket, isLocalAddressReusable());

        // configure TCP_NODELAY
        TcpUtil::setTcpNoDelay(hSocket, !isTcpDelayEnabled());

        // configure SO_RCVBUF
        TcpUtil::setReceiveBufferSize(hSocket, (int32_t) getReceiveBufferSize());

        // configure SO_SNDBUF
        TcpUtil::setSendBufferSize(hSocket, (int32_t) getSendBufferSize());

        // configure SO_LINGER
        int64_t cMillis = getLingerTimeout();
        int32_t cSecs   = cMillis >= 0 ? (int32_t) (cMillis / 1000L) : -1; // seconds
        TcpUtil::setSoLinger(hSocket, cSecs);

        // wait a maximum of request timeout for a message send to complete
        hSocket->setSendTimeout((int32_t) getRequestTimeout());
        }
    catch (Exception::View e)
        {
        COH_THROW (Exception::create("error configuring Socket", e));
        }

    // bind the socket to the local address
    InetSocketAddress::View vAddr = getLocalAddress();
    if (vAddr != NULL)
        {
        COH_LOG("Binding Socket to " << TcpUtil::toString(vAddr), 6);
        try
            {
            hSocket->bind(vAddr);
            }
        catch (Exception::View e)
            {
            COH_THROW (Exception::create(COH_TO_STRING("error binding Socket to "
                    << TcpUtil::toString(vAddr)), e));
            }

        if (!hSocket->isBound())
            {
            COH_THROW_STREAM(Exception, "could not bind Socket to "
                     << TcpUtil::toString(vAddr));
            }
        }
    }

Socket::Handle TcpInitiator::instantiateSocket() const
    {
    Socket::Handle hSocket = Socket::create();
    configureSocket(hSocket);

    return hSocket;
    }

InetSocketAddress::Handle TcpInitiator::parseLocalSocketAddress(XmlElement::View vXml)
    {
    XmlElement::View vXmlAddr = vXml->getElement("address");
    XmlElement::View vXmlPort = vXml->getElement("port");

    if (NULL == vXmlAddr && NULL == vXmlPort)
        {
        return NULL;
        }

    String::View vsAddr = (NULL == vXmlAddr) ? "localhost" : vXmlAddr->getString();
    int32_t      nPort  = (NULL == vXmlPort) ? 0           : vXmlPort->getInt32();

    InetAddress::View vAddr;
    try
        {
        if (vsAddr->equals("localhost"))
            {
            vAddr = InetAddressHelper::getLocalHost();
            }
        else
            {
            vAddr = InetAddress::getByName(vsAddr);
            }
        }
    catch (Exception::View e)
        {
        COH_THROW (Exception::create(COH_TO_STRING("the \"" << vXml->getName()
                << "\" configuration element contains an invalid \"address\" element"),
                e));
        }

    try
        {
        return InetSocketAddress::create(vAddr, (uint16_t)nPort);
        }
    catch (Exception::View e)
        {
        COH_THROW (Exception::create(COH_TO_STRING("the \"" << vXml->getName()
                << "\" configuration element contains an invalid \"port\" element"),
                e));
        }
    }

PofConnection::Handle TcpInitiator::openConnection()
    {
    AddressProvider::Handle hProvider = getRemoteAddressProvider();
    COH_ENSURE(NULL != hProvider);

    // determine the Socket connect timeout
    int32_t cMillis = std::max(0, (int32_t) getConnectTimeout());

    // open a new connection
    List::Handle     hListAddr     = ArrayList::create();
    Iterator::Handle hIterRedirect = NULL;
    Iterator::Handle hIterSubport  = NULL;
    Exception::View  vCause        = NULL;
    for ( ; ; )
        {
        TcpConnection::Handle hConnection = cast<TcpConnection::Handle>(
                instantiateConnection());

        InetSocketAddress::View vAddr;
        int32_t                 nSubport;
        if (NULL == hIterRedirect || !hIterRedirect->hasNext())
            {
            vAddr    = hProvider->getNextAddress();
            nSubport = getSubport();


            // reset redirection information
            hIterRedirect = NULL;
            hIterSubport  = NULL;
            }
        else
            {
            vAddr    = cast<InetSocketAddress::View>(hIterRedirect->next());
            nSubport = (cast<Integer32::View>(hIterSubport->next()))->getInt32Value();

            // update redirection information
            hConnection->setRedirect(true);
            }

        if (NULL == vAddr)
            {
            break;
            }

        String::View vsAddr = TcpUtil::toString(vAddr, nSubport);
        hListAddr->add(vsAddr);

        // create and configure a new Socket; otherwise, some JVMs
        // may throw a SocketException when the Socket is reused
        Socket::Handle hSocket = instantiateSocket();
        try
            {
            if (NULL == hIterRedirect)
                {
                COH_LOG("Connecting Socket to " << vsAddr, 5);
                }
            else
                {
                COH_LOG("Redirecting Socket to " << vsAddr, 5);
                }

            hSocket->connect(vAddr, cMillis);
            COH_LOG("Connected Socket to " << vsAddr, 3);
            if (nSubport != -1)
                {
                // write out subport info
                TcpUtil::writeSubport(hSocket->getOutputStream(), nSubport);
                }

            hConnection->setSocket(hSocket);
            }
        catch (Exception::View e)
            {
            COH_LOG("Error connecting Socket to " << vsAddr << ": " << e, 3);
            TcpUtil::close(hSocket);

            // if we aren't current redirecting, or we've tried the last
            // redirect address, reject the last address supplied by the
            // address provider
            if (NULL == hIterRedirect || !hIterRedirect->hasNext())
                {
                hProvider->reject(e);
                }
            continue;
            }

        try
            {
            hConnection->open();
            }
        catch (Exception::View e)
            {
            if (NULL == hIterRedirect && hConnection->isRedirect())
                {
                List::View vList = hConnection->getRedirectList();

                // create an InetSocketAddress list from from the redirect list
                List::Handle hListRedirect = ArrayList::create(vList->size());
                List::Handle hListSubport  = ArrayList::create(vList->size());
                for (Iterator::Handle hIter = vList->iterator(); hIter->hasNext(); )
                    {
                    ObjectArray::View vao = cast<ObjectArray::View>(hIter->next());
                    String::View      vs  = cast<String::View>(vao[0]);
                    int32_t           n   = cast<Integer32::View>(vao[1])->getInt32Value();

                    hListRedirect->add(InetSocketAddress::create(vs, TcpUtil::calculateBaseport(n),true));
                    hListSubport->add(Integer32::create(TcpUtil::calculateSubport(n)));
                    }
                hIterRedirect = hListRedirect->iterator();
                hIterSubport  = hListSubport->iterator();
                }
            else
                {
                vCause = e;
                COH_LOG("Error establishing a connection with " << vsAddr << ": " << e, 3);

                // if we aren't current redirecting, or we've tried the last redirect
                // address, reject the last address supplied by the address provider
                if (NULL == hIterRedirect || !hIterRedirect->hasNext())
                    {
                    hProvider->reject(e);
                    }
                }
            continue;
            }

        hProvider->accept();
        return hConnection;
        }

    COH_THROW (ConnectionException::create(
            COH_TO_STRING("could not establish a connection to one of the following addresses: " << hListAddr),
            vCause,
            (Connection::View) NULL));
    }

PofConnection::Handle TcpInitiator::instantiateConnection()
    {
    TcpConnection::Handle hConnection = TcpConnection::create();
    hConnection->setConnectionManager(this);
    return hConnection;
    }

void TcpInitiator::configure(XmlElement::View vXml)
    {
    COH_SYNCHRONIZED (this)
        {
        super::configure(vXml);
        if (NULL == vXml)
            {
            return;
            }

        XmlElement::View vXmlCat;
        XmlElement::View vXmlSub;
        XmlElement::View vXmlVal;

        // <tcp-initiator>
        vXmlCat = vXml->getSafeElement("tcp-initiator");

        // <local-address>
        vXmlSub = vXmlCat->getSafeElement("local-address");

        // <address>
        // <port>
        setLocalAddress(parseLocalSocketAddress(vXmlSub));

        // <reusable>
        setLocalAddressReusable(vXmlSub->getSafeElement("reusable")
                ->getBoolean(isLocalAddressReusable()));

        // <remote-addresses>
        bool fNameServiceAddressProvider = false;
        vXmlSub = vXmlCat->getElement("name-service-addresses");
        if (vXmlSub == NULL)
            {
            vXmlSub = vXmlCat->getElement("remote-addresses");
            if (vXmlSub == NULL)
                {
                fNameServiceAddressProvider = true; // implicit NS
                }
            }
        else
            {
            fNameServiceAddressProvider = true;
            }

        AddressProviderFactory::Handle hFactory;

        XmlElement::View vXmlProvider = vXmlSub == NULL ? NULL : vXmlSub->getElement("address-provider");
        if (vXmlSub != NULL && (vXmlProvider == NULL || vXmlProvider->isEmpty()))
            {
            ConfigurableAddressProviderFactory::Handle hFactoryImpl =
                    ConfigurableAddressProviderFactory::create();
            hFactoryImpl->setConfig(vXmlProvider == NULL ? vXmlSub : vXmlProvider);
            hFactory = hFactoryImpl;
            }
        else
            {
            String::View vsName = vXmlSub == NULL
                    ? (String::View) String::create("cluster-discovery")
                    : vXmlProvider->getString();

            // The address provider map should contain a corresponding address provider.
            hFactory = cast<AddressProviderFactory::Handle>
                    (getOperationalContext()->getAddressProviderMap()->get(vsName));
            if (hFactory == NULL)
                {
                COH_THROW_STREAM(IllegalArgumentException,
                        "Address-provider name \"" << vsName <<
                        "\" is undefined:\n" << vXml);
                }
            }

        setRemoteAddressProvider(hFactory->createAddressProvider());
        setNameServiceAddressProvider(fNameServiceAddressProvider);

        if (fNameServiceAddressProvider)
            {
            setSubport(TcpUtil::NameService);
            }
        else
            {
            setSubport(-1);
            }

        // <keep-alive-enabled/>
        setKeepAliveEnabled(vXmlCat->getSafeElement("keep-alive-enabled")
                ->getBoolean(isKeepAliveEnabled()));

        // <tcp-delay-enabled>
        setTcpDelayEnabled(vXmlCat->getSafeElement("tcp-delay-enabled")
                ->getBoolean(isTcpDelayEnabled()));

        // <receive-buffer-size>
        setReceiveBufferSize(parseMemorySize(vXmlCat, "receive-buffer-size",
                getReceiveBufferSize()));

        // <send-buffer-size>
        setSendBufferSize(parseMemorySize(vXmlCat, "send-buffer-size",
                getSendBufferSize()));

        // <linger-timeout>
        setLingerTimeout(parseTime(vXmlCat, "linger-timeout",
                getLingerTimeout()));
        }
    }

bool TcpInitiator::isKeepAliveEnabled() const
    {
    return m_fKeepAliveEnabled;
    }

int64_t TcpInitiator::getLingerTimeout() const
    {
    return m_lLingerTimeout;
    }

InetSocketAddress::Handle TcpInitiator::getLocalAddress()
    {
    return f_hLocalAddress;
    }

InetSocketAddress::View TcpInitiator::getLocalAddress() const
    {
    return f_hLocalAddress;
    }

bool TcpInitiator::isLocalAddressReusable() const
    {
    return m_fLocalAddressReusable;
    }

int64_t TcpInitiator::getReceiveBufferSize() const
    {
    return m_lReceiveBufferSize;
    }

AddressProvider::View TcpInitiator::getRemoteAddressProvider() const
    {
    return m_hRemoteAddressProvider;
    }

AddressProvider::Handle TcpInitiator::getRemoteAddressProvider()
    {
    return m_hRemoteAddressProvider;
    }

int64_t TcpInitiator::getSendBufferSize() const
    {
    return m_lSendBufferSize;
    }

bool TcpInitiator::isTcpDelayEnabled() const
    {
    return m_fTcpDelayEnabled;
    }

void TcpInitiator::setKeepAliveEnabled(bool fEnabled)
    {
    m_fKeepAliveEnabled = fEnabled;
    }

void TcpInitiator::setLingerTimeout(int64_t cMillis)
    {
    m_lLingerTimeout = cMillis;
    }

void TcpInitiator::setLocalAddress(InetSocketAddress::Handle hAddr)
    {
    initialize(f_hLocalAddress, hAddr);
    }

void TcpInitiator::setLocalAddressReusable(bool fReusable)
    {
    m_fLocalAddressReusable = fReusable;
    }

void TcpInitiator::setReceiveBufferSize(int64_t cb)
    {
    m_lReceiveBufferSize = cb;
    }

void TcpInitiator::setRemoteAddressProvider(AddressProvider::Handle hAddr)
    {
    m_hRemoteAddressProvider = hAddr;
    }

void TcpInitiator::setSendBufferSize(int64_t lSendBufferSize)
    {
    m_lSendBufferSize = lSendBufferSize;
    }

void TcpInitiator::setTcpDelayEnabled(bool fEnabled)
    {
    m_fTcpDelayEnabled = fEnabled;
    }


// ----- Describable interface ----------------------------------------------

String::View TcpInitiator::getDescription() const
    {
    String::View vs = super::getDescription();

    InetSocketAddress::View  vAddr = getLocalAddress();
    if (vAddr != NULL)
        {
        vs = COH_TO_STRING(vs << ", LocalAddress="
            << TcpUtil::toString(vAddr)
            << ", LocalAddressReusable="
            << Boolean::toString(isLocalAddressReusable()));
        }

    vs = COH_TO_STRING(vs << ", RemoteAddresses="
        << getRemoteAddressProvider());

    return COH_TO_STRING(vs << ", KeepAlive="
        << Boolean::toString(isKeepAliveEnabled())
        << ", TcpNoDelay="
        << Boolean::toString(!isTcpDelayEnabled())
        << ", ReceiveBufferSize="
        << StringHelper::toMemorySizeString(getReceiveBufferSize())
        << ", SendBufferSize="
        << StringHelper::toMemorySizeString(getSendBufferSize())
        << ", LingerTimeout="
        << getLingerTimeout());
    }

// ----- helper methods -----------------------------------------------------

int32_t TcpInitiator::readMessageLength(InputStream::Handle hIn)
    {
    uint32_t b     = hIn->read();
    uint32_t n     = b & 0x3F;         // only 6 bits of data in first octet
    uint8_t  cBits = 6;
    bool     fNeg  = (b & 0x40) != 0;  // seventh bit is a sign bit

    while ((b & 0x80) != 0)               // eighth bit is the continuation bit
        {
        b      = hIn->read();
        n     |= ((b & 0x7F) << cBits);
        cBits += 7;
        }

    if (fNeg)
        {
        n = ~n;
        }

    return n;
    }

void TcpInitiator::writeMessageLength(OutputStream::Handle hOut,
        size32_t cb)
    {
    // first octet contains sign bit (bit 7 set if neg)
    octet_t  b  = 0;

    // first octet contains only 6 data bits
    b |= (octet_t) (cb & 0x3F);
    cb >>= 6;

    while (cb != 0)
        {
        b |= 0x80;          // bit 8 is a continuation bit
        hOut->write(b);

        b = (octet_t) (cb & 0x7F);
        cb >>= 7;
        }

    hOut->write(b);
    }


// ----- nested class: TcpConnection ----------------------------------------

// ----- constructors ---------------------------------------------------

TcpInitiator::TcpConnection::TcpConnection()
    : f_hInput(self()),
      f_hOutput(self()),
      m_hReader(self()),
      m_fRedirect(false),
      f_vListRedirect(self(), (List::View)NULL)
    {
    }

// ----- nested class: TcpReader ----------------------------------------

// ----- constructors -----------------------------------------------

TcpInitiator::TcpConnection::TcpReader::TcpReader(TcpConnection::Handle hConnection)
    : m_whTcpConnection(self(), hConnection)
    {
    }

// ----- Daemon interface -------------------------------------------

String::View TcpInitiator::TcpConnection::TcpReader::getThreadName() const
    {
    TcpInitiator::View vInitiator = cast<TcpInitiator::View>(
            m_whTcpConnection->getConnectionManager());
    return COH_TO_STRING(vInitiator->getServiceName() << ':' << super::getThreadName());
    }

void TcpInitiator::TcpConnection::TcpReader::onException(Exception::Holder ohe)
    {
    // see TcpConnection::closeInternal()
    if (!isExiting())
        {
        try
            {
            m_whTcpConnection->close(
                    /*fNotify*/ false,
                    ohe,
                    /*fWait*/ false);
            }
        catch (Exception::View) {}
        }

    super::onException(ohe);
    }

void TcpInitiator::TcpConnection::TcpReader::onNotify()
    {
    TcpConnection::Handle hConnection = m_whTcpConnection;
    COH_ENSURE(hConnection != NULL);

    InputStream::Handle hIn = hConnection->getInputStream();
    COH_ENSURE(hIn != NULL);

    TcpInitiator::Handle hManager = cast<TcpInitiator::Handle>(
            hConnection->getConnectionManager());
    COH_ENSURE(hManager != NULL);

    while (!isExiting())
        {
        try
            {
            size32_t cb = TcpInitiator::readMessageLength(hIn); // Message length
            hManager->enforceMaxIncomingMessageSize(cb);

            if (cb == TcpInitiator::npos)
                {
                COH_THROW (IOException
                        ::create("Received a message with a negative length"));
                }
            else if (cb == 0)
                {
                COH_THROW (IOException
                        ::create("Received a message with a length of zero"));
                }
            else
                {
                Array<octet_t>::Handle hab = Array<octet_t>::create(cb);
                hIn->readFully(hab);

                OctetArrayReadBuffer::View vBuffer = OctetArrayReadBuffer
                        ::create(hab, 0, cb, false);

                // update stats
                hConnection->setStatsBytesReceived(
                        hConnection->getStatsBytesReceived() + cb);
                hConnection->setStatsReceived(
                        hConnection->getStatsReceived() + 1);

                // dispatch Message
                hManager->receive(vBuffer, hConnection);
                }
            }
        catch (IOException::View e)
            {
            // see TcpConnection::closeInternal()
            if (!isExiting())
                {
                try
                    {
                    hConnection->close(
                            /*fNotify*/ false,
                            ConnectionException::create(e->getMessage(), e, hConnection),
                            /*fWait*/ false);
                    }
                catch (Exception::View) {}
                stop();
                }
            }
        }
    }

void TcpInitiator::TcpConnection::TcpReader::onWait()
    {
    // all work is done in onNotify()
    return;
    }

// ---- Connection interface --------------------------------------------

bool TcpInitiator::TcpConnection::closeInternal(bool fNotify,
        Exception::Holder ohe, int64_t cMillis)
    {
    if (super::closeInternal(fNotify, ohe, cMillis))
        {
        TcpReader::Handle hReader = getReader();
        if (NULL != hReader)
            {
            hReader->stop();
            setReader(NULL);
            }

        InputStream::Handle hIn = getInputStream();
        if (NULL != hIn)
            {
            try
                {
                hIn->close();
                }
            catch (Exception::View) { /*ignore*/ }
            // setInputStream(NULL); // final in c++
            }

        OutputStream::Handle hOut = getOutputStream();
        if (NULL != hOut)
            {
            try
                {
                hOut->close();
                }
            catch (Exception::View) { /*ignore*/ }
            // setOutputStream(NULL); // final in c++
            }

        TcpUtil::close(getSocket());

        return true;
        }

    return false;
    }

void TcpInitiator::TcpConnection::openInternal()
    {
    super::openInternal();

    Socket::Handle hSocket = getSocket();
    COH_ENSURE(hSocket != NULL);

    setInputStream(BufferedInputStream::create(hSocket->getInputStream()));
    setOutputStream(BufferedOutputStream::create(hSocket->getOutputStream()));

    TcpReader::Handle hReader = instantiateTcpReader();
    hReader->start();
    setReader(hReader);
    }

void TcpInitiator::TcpConnection::send(WriteBuffer::View vwb)
    {
    super::send(vwb);

    Array<octet_t>::View vab = vwb->toOctetArray();
    size32_t             cb  = vwb->length();

    // write the length-encoded Message to the Socket OutputStream. According
    // to the following post, there is no guarantee that the write operation
    // is thread safe, so we must synchronize on the OutputStream:
    //
    // http://forum.java.sun.com/thread.jspa?threadID=792640&tstart=165
    //
    OutputStream::Handle hOut = getOutputStream();

    m_cConcurrentWriters.postAdjust(1, /*fSafe*/ false);
    COH_SYNCHRONIZED (hOut)
        {
        try
            {
            // Message length
            TcpInitiator::writeMessageLength(hOut, cb);
            // Message contents
            hOut->write(vab);
            }
        catch (IOException::View ve)
            {
            if (m_cConcurrentWriters.adjust(-1, /*fSafe*/ false) == 0)
                {
                // only the last of the concurrent writers needs to flush
                hOut->flush();
                }
            COH_THROW (ConnectionException::create(ve->getMessage(), ve, this));
            }

        if (m_cConcurrentWriters.adjust(-1, /*fSafe*/ false) == 0)
            {
            // only the last of the concurrent writers needs to flush
            hOut->flush();
            }
        }
    }


// ----- Peer child component factory -----------------------------------

Protocol::View TcpInitiator::instantiateProtocol() const
    {
    return TcpInitiatorProtocol::getInstance();
    }


// ----- accessor methods -----------------------------------------------

InputStream::Handle TcpInitiator::TcpConnection::getInputStream()
    {
    return f_hInput;
    }

void TcpInitiator::TcpConnection::setInputStream(InputStream::Handle hIn)
    {
    initialize(f_hInput, hIn);
    }

OutputStream::Handle TcpInitiator::TcpConnection::getOutputStream()
    {
    return f_hOutput;
    }

void TcpInitiator::TcpConnection::setOutputStream(OutputStream::Handle hOut)
    {
    initialize(f_hOutput, hOut);
    }

TcpInitiator::TcpConnection::TcpReader::Handle
    TcpInitiator::TcpConnection::getReader()
    {
    return m_hReader;
    }

void TcpInitiator::TcpConnection::setReader(TcpReader::Handle hReader)
    {
    m_hReader = hReader;
    }

bool TcpInitiator::TcpConnection::isRedirect() const
    {
    return m_fRedirect;
    }

void TcpInitiator::TcpConnection::setRedirect(bool fRedirect)
    {
    m_fRedirect = fRedirect;
    }

List::View TcpInitiator::TcpConnection::getRedirectList() const
    {
    return f_vListRedirect;
    }

void TcpInitiator::TcpConnection::setRedirectList(List::View vList)
    {
    initialize(f_vListRedirect, vList);
    }

bool TcpInitiator::isNameServiceAddressProvider() const
    {
    return m_fNameServiceAddressProvider;
    }

void TcpInitiator::setNameServiceAddressProvider(bool fNameServiceAddressProvider)
    {
    m_fNameServiceAddressProvider = fNameServiceAddressProvider;
    }

int32_t TcpInitiator::getSubport() const
    {
    return m_nSubport;
    }

void TcpInitiator::setSubport(int32_t nSubport)
    {
    m_nSubport = nSubport;
    }


// ----- TcpConnection child component --------------------------

TcpInitiator::TcpConnection::TcpReader::Handle
    TcpInitiator::TcpConnection::instantiateTcpReader()
    {
    return TcpReader::create(this);
    }

COH_CLOSE_NAMESPACE3
