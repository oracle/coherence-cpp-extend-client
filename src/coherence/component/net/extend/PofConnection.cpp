/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/PofConnection.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/Random.hpp"

#include "private/coherence/component/net/extend/PofChannel.hpp"

#include "private/coherence/component/net/extend/protocol/AcceptChannelRequest.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyConnectionClosed.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannelRequest.hpp"
#include "private/coherence/component/net/extend/protocol/PeerProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/PingRequest.hpp"

#include "private/coherence/component/util/Peer.hpp"

#include "private/coherence/net/messaging/ConnectionAcceptor.hpp"

#include "private/coherence/util/HashArray.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <limits>

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::protocol::AcceptChannelRequest;
using coherence::component::net::extend::protocol::NotifyConnectionClosed;
using coherence::component::net::extend::protocol::OpenChannelRequest;
using coherence::component::net::extend::protocol::PeerProtocol;
using coherence::component::net::extend::protocol::PingRequest;
using coherence::component::util::Peer;
using coherence::net::messaging::ConnectionException;
using coherence::net::messaging::ConnectionAcceptor;
using coherence::util::ArrayList;
using coherence::util::HashArray;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::LongArrayIterator;
using coherence::util::Random;


// ----- constructors -------------------------------------------------------

PofConnection::PofConnection()
    : f_hlaChannel(self(), HashArray::create(), /*fMutable*/ true),
      f_hlaChannelPending(self(), HashArray::create(), /*fMutable*/ true),
      m_fCloseNotify(self(), false),
      m_fCloseOnExit(self(), false),
      m_oheCloseException(self()),
      m_whManager(self()),
      f_vUUID(self()),
      f_vMapMessageFactory(self()),
      m_fOpen(self(), false),
      f_vMember(self()),
      m_nPeerEdition(0),
      m_vUUIDPeer(self()),
      m_ldtPingLast(0),
      m_cStatsBytesReceived(0),
      m_cStatsBytesSent(0),
      m_cStatsReceived(0),
      m_ldtStatsReset(0),
      m_cStatsSent(0),
      f_hThreadGate(self(), ThreadGate::create())
    {
    }

void PofConnection::onInit()
    {
    super::onInit();

    PofChannel::Handle hChannel0 = PofChannel::create();
    hChannel0->setConnection(this);

    registerChannel(hChannel0);
    }


// ----- PofConnection interface ---------------------------------------

Request::Status::Handle PofConnection::acceptChannelInternal(URI::View vUri,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject,
        Array<octet_t>::View vab)
    {
    assertOpen();

    if (NULL == vSerializer)
        {
        COH_THROW (IllegalArgumentException::create("serializer cannot be null"));
        }

    int32_t nId;
    try
        {
        nId = Integer32::parse(vUri->getSchemeSpecificPart());
        }
    catch (Exception::View)
        {
        COH_THROW_STREAM(IllegalArgumentException, "illegal URI: " << vUri);
        }

    if (nId == 0)
        {
        COH_THROW (IllegalArgumentException::create("channel 0 is reserved"));
        }

    if (getChannel(nId) != NULL)
        {
        COH_THROW_STREAM(IllegalArgumentException, "duplicate channel: " << nId);
        }

    String::View vsProtocol = vUri->getFragment();
    if (NULL == vsProtocol)
        {
        COH_THROW_STREAM(IllegalArgumentException, "illegal URI: " << vUri);
        }

    Protocol::MessageFactory::View vFactory =
        cast<Protocol::MessageFactory::View>(getMessageFactoryMap()->get(vsProtocol));

    if (NULL == vFactory)
        {
        COH_THROW_STREAM(IllegalArgumentException, "unknown protocol: " << vsProtocol);
        }

    if (hReceiver != NULL)
        {
        if (hReceiver->getProtocol() != vFactory->getProtocol())
            {
            COH_THROW_STREAM(IllegalArgumentException, "protocol mismatch; expected "
                    << vFactory->getProtocol() << ", retrieved "
                    << hReceiver->getProtocol() << ")");
            }
        }

    // send a AcceptChannelRequest to the peer via "Channel0"
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel(0));
    Protocol::MessageFactory::View vFactory0 =
            cast<Protocol::MessageFactory::View>(hChannel0->getMessageFactory());

    AcceptChannelRequest::Handle hRequest  = cast<AcceptChannelRequest::Handle>(
            vFactory0->createMessage(AcceptChannelRequest::type_id));

    hRequest->setChannelId(nId);
    hRequest->setIdentityToken(vab);
    hRequest->setMessageFactory(vFactory);
    hRequest->setProtocolName(vsProtocol);
    hRequest->setReceiver(hReceiver);
    hRequest->setSerializer(vSerializer);
    hRequest->setSubject(vSubject);

    return hChannel0->send((Request::Handle) hRequest);
    }

void PofConnection::acceptChannelRequest(int32_t nId, Subject::View vSubject)
    {
    if (nId == 0)
        {
        COH_THROW (IllegalArgumentException::create("channel 0 is reserved"));
        }

    if (NULL != getChannel(nId))
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "channel already exists: " << nId);
        }

    PofChannel::Handle hChannel = cast<PofChannel::Handle>(
            f_hlaChannelPending->remove(nId), false);
    if (NULL == hChannel)
        {
        COH_THROW_STREAM (IllegalArgumentException, "no such channel: " << nId);
        }

    hChannel->setSubject(vSubject);
    hChannel->openInternal();

    registerChannel(hChannel);
    }

Channel::Handle PofConnection::acceptChannelResponse(int32_t nId,
        Protocol::MessageFactory::View vFactory,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject)
    {
    assertOpen();

    if (NULL == vFactory)
        {
        COH_THROW (IllegalArgumentException::create("factory cannot be null"));
        }

    if (NULL == vSerializer)
        {
        COH_THROW (IllegalArgumentException::create("serializer cannot be null"));
        }

    PofChannel::Handle hChannel = PofChannel::create();
    hChannel->setId(nId);
    hChannel->setConnection(this);
    hChannel->setMessageFactory(vFactory);
    hChannel->setReceiver(hReceiver);
    hChannel->setSerializer(vSerializer);
    hChannel->setSubject(vSubject);
    hChannel->openInternal();

    registerChannel(hChannel);

    return hChannel;
    }

void PofConnection::close(bool fNotify, Exception::Holder ohe, bool fWait)
    {
    if (isOpen())
        {
        // ensure that the Connection hasn't already been closed
        Peer::Handle hManager = cast<Peer::Handle>(getConnectionManager());
        if (NULL == hManager)
            {
            return;
            }

        if (Thread::currentThread() == hManager->getThread())
            {
            closeInternal(fNotify, ohe, 0);
            }
        else
            {
            COH_ENSURE(!isActiveThread());

            hManager->closeConnection(this, fNotify, ohe, fWait);
            }
        }
    }

bool PofConnection::closeInternal(bool fNotify, Exception::Holder ohe,
        int64_t cMillis)
    {
    if (!isOpen())
        {
        return false;
        }

    // close all open Channels, except for "Channel0"
    PofChannel::Handle hChannel0;
    LongArray::Handle  hla = f_hlaChannel;
    COH_SYNCHRONIZED (hla)
        {
        hChannel0 = cast<PofChannel::Handle>(hla->get(0));
        for (LongArrayIterator::Handle hIter = hla->iterator();
                hIter->hasNext(); )
            {
            PofChannel::Handle hChannel = cast<PofChannel::Handle>
                    (hIter->next());
            if (hChannel != hChannel0)
                {
                hIter->remove();
                hChannel->closeInternal(false, ohe, 0L);
                }
            }
        }

    //try
    if (true)
        {
        struct closeInternalFinally
            {
            closeInternalFinally(PofConnection::Handle hConnection)
                : fClose(false), hMyConnection(hConnection)
                {
                }

            ~closeInternalFinally()
                {
                if (fClose)
                    {
                    try
                        {
                        hMyConnection->gateOpen();
                        }
                    catch (Exception::View e)
                        {
                        COH_LOG("Error opening the gate: " << e, -1);
                        }
                    }
                }

            bool                  fClose;
            PofConnection::Handle hMyConnection;
            } finally(this);

        finally.fClose = gateClose(cMillis);

        if (!finally.fClose)
            {
            // can't close the gate; signal to the holding Thread(s) that it
            // must close the Connection immediately after exiting the gate
            setCloseOnExit(true);
            setCloseNotify(fNotify);
            setCloseException(ohe);

            // double check if we can close the gate, as we want to be sure
            // that the Thread(s) saw the close notification prior to exiting
            finally.fClose = gateClose(0L);
            }

        if (finally.fClose && isOpen())
            {
            // notify the peer that the Connection is now closed
            if (fNotify)
                {
                // send a NotifyConnectionClosed to the peer via "Channel0"
                try
                    {
                    Protocol::MessageFactory::View vFactory0 =
                        hChannel0->getMessageFactory();

                    NotifyConnectionClosed::Handle hMessage  =
                        cast<NotifyConnectionClosed::Handle>(
                                vFactory0->createMessage(NotifyConnectionClosed::type_id));

                    hMessage->setCause(ohe);
                    hChannel0->send((Message::Handle) hMessage);
                    }
                catch (Exception::View) {}
                }

            // clean up
            hChannel0->closeInternal(false, ohe, -1L);
            f_hlaChannelPending->clear();
            setPeerId(NULL);

            m_fOpen = false;
            }
        else
            {
            return false;
            }
        }

    // notify the ConnectionManager that the Connection is now closed
    Peer::Handle hManager = cast<Peer::Handle>(getConnectionManager());
    if (NULL == ohe)
        {
        hManager->onConnectionClosed(this);
        }
    else
        {
        hManager->onConnectionError(this, ohe);
        }

    return true;
    }

URI::View PofConnection::createChannelInternal(Protocol::View vProtocol,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver)
    {
    assertOpen();

    if (NULL == vProtocol)
        {
        COH_THROW (IllegalArgumentException::create("protocol cannot be null"));
        }

    String::View vsProtocol = vProtocol->getName();
    if (NULL == vsProtocol)
        {
        COH_THROW_STREAM (IllegalArgumentException, "missing protocol name: "
                << vProtocol);
        }

    Protocol::MessageFactory::View vFactory = cast<Protocol::MessageFactory::View>(
            getMessageFactoryMap()->get(vsProtocol));
    if (NULL == vFactory)
        {
        COH_THROW_STREAM (IllegalArgumentException, "unsupported protocol: "
                << vProtocol);
        }

    int32_t nChannelId = generateChannelId();

    // create a new Channel
    PofChannel::Handle hChannel = PofChannel::create();
    hChannel->setId(nChannelId);
    hChannel->setConnection(this);
    hChannel->setReceiver(hReceiver);
    hChannel->setMessageFactory(vFactory);
    hChannel->setSerializer(vSerializer);

    // add the new Channel to the pending map; a log a warning if the number
    // of pending channels is high
    size32_t iSize = f_hlaChannelPending->getSize();
    if (iSize > max_pending)
        {
        COH_LOG("There is a high number of pending open channel requests [" << iSize << "] for connection="
            << this, 2);
        }
    f_hlaChannelPending->set(nChannelId, hChannel);

    String::Handle hsId = COH_TO_STRING(nChannelId);

    return URI::create("channel", hsId, vsProtocol);
    }

bool PofConnection::gateClose(int64_t cMillis)
    {
    return f_hThreadGate->close(cMillis);
    }

void PofConnection::gateOpen()
    {
    f_hThreadGate->open();
    }

void PofConnection::gateEnter()
    {
    // if the thread is entering for the first time, throw an exception if the
    // Connection has been marked for close; this prevents new threads from
    // entering the Connection and thus keeping it open longer than necessary
    if (isCloseOnExit() && !f_hThreadGate->isActiveThread())
        {
        // REVIEW
        COH_THROW (ConnectionException::create("connection is closing",
                (Exception::View) NULL, this));
        }

    if (f_hThreadGate->enter(0)) // see #gateClose
        {
        try
            {
            assertOpen();
            }
        catch (...)
            {
            f_hThreadGate->exit();
            throw;
            }
        }
    else
        {
        // REVIEW
        COH_THROW (ConnectionException::create("connection is closing",
                (Exception::View) NULL, this));
        }
    }

void PofConnection::gateExit()
    {
    f_hThreadGate->exit();

    // see if we've been asked to close the Connection
    if (isCloseOnExit() && !f_hThreadGate->isActiveThread())
        {
        struct GateExitFinally
            {
            GateExitFinally(Handle h)
                    : hThis(h), fClose(h->gateClose(0))
                {
                }

            ~GateExitFinally()
                {
                if (fClose)
                    {
                    try
                        {
                        hThis->gateOpen();
                        }
                    catch (Exception::View e)
                        {
                        COH_LOG("Error opening the gate: " << e, -1);
                        // eat it
                        }
                    }
                }

            Handle hThis;
            bool   fClose;
            } finally(this);

        if (finally.fClose && isOpen())
            {
            gateOpen();
            finally.fClose = false;
            close(isCloseNotify(), getCloseException());
            }
        }
    }

void PofConnection::open()
    {
    if (!isOpen())
        {
        (cast<Peer::Handle>(getConnectionManager()))->openConnection(this);
        }
    }

Request::Status::Handle PofConnection::openChannelInternal(
        Protocol::View vProtocol,
        String::View vsName,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject,
        Array<octet_t>::View vabToken)
    {
    assertOpen();

    if (NULL == vProtocol)
        {
        COH_THROW (IllegalArgumentException::create("protocol cannot be null"));
        }

    if (NULL == vsName)
        {
        COH_THROW (IllegalArgumentException::create("name cannot be null"));
        }

    if (NULL == vSerializer)
        {
        COH_THROW (IllegalArgumentException::create("serializer cannot be null"));
        }

    String::View vsProtocol = vProtocol->getName();
    COH_ENSURE(vsProtocol != NULL);

    Protocol::MessageFactory::View vFactory =
        cast<Protocol::MessageFactory::View>(getMessageFactoryMap()->get(vsProtocol));

    if (NULL == vFactory)
        {
        COH_THROW_STREAM(IllegalArgumentException, "unknown protocol: " << vsProtocol);
        }

    if (hReceiver != NULL)
        {
        if (hReceiver->getProtocol() != vFactory->getProtocol())
            {
            COH_THROW_STREAM(IllegalArgumentException, "protocol mismatch; expected "
                    << vFactory->getProtocol() << ", retrieved "
                    << hReceiver->getProtocol() << ")");
            }
        }

    // send a ChannelOpenRequest to the peer via "Channel0"
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel(0));

    Protocol::MessageFactory::View vFactory0 =
        cast<Protocol::MessageFactory::View>(hChannel0->getMessageFactory());

    OpenChannelRequest::Handle hRequest  = cast<OpenChannelRequest::Handle>(
            vFactory0->createMessage(OpenChannelRequest::type_id));

    hRequest->setIdentityToken(vabToken);
    hRequest->setMessageFactory(vFactory);
    hRequest->setProtocolName(vsProtocol);
    hRequest->setReceiver(hReceiver);
    hRequest->setReceiverName(vsName);
    hRequest->setSerializer(vSerializer);
    hRequest->setSubject(vSubject);

    return hChannel0->send((Request::Handle) hRequest);
    }

int32_t PofConnection::openChannelRequest(String::View vsProtocol,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject)
    {
    assertOpen();

    if (NULL == vsProtocol)
        {
        COH_THROW (IllegalArgumentException::create(
                "protocol name cannot be NULL"));
        }

    if (NULL == vSerializer)
        {
        COH_THROW (IllegalArgumentException::create(
                "serializer cannot be NULL"));
        }

    Protocol::MessageFactory::View vFactory = cast<Protocol::MessageFactory::View>(
            getMessageFactoryMap()->get(vsProtocol));
    if (NULL == vFactory)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                "unknown protocol: " << vsProtocol);
        }

    if (NULL != hReceiver)
        {
        if (hReceiver->getProtocol() != vFactory->getProtocol())
            {
            COH_THROW_STREAM (IllegalArgumentException,
                    "protocol mismatch; expected "
                    << vFactory->getProtocol() << ", retrieved "
                    << hReceiver->getProtocol() << ')');
            }
        }

    int32_t nId = generateChannelId();

    PofChannel::Handle hChannel = PofChannel::create();
    hChannel->setConnection(this);
    hChannel->setId(nId);
    hChannel->setMessageFactory(vFactory);
    hChannel->setReceiver(hReceiver);
    hChannel->setSerializer(vSerializer);
    hChannel->setSubject(vSubject);
    hChannel->openInternal();

    registerChannel(hChannel);

    return nId;
    }

Channel::Handle PofConnection::openChannelResponse(int32_t nId,
        Protocol::MessageFactory::View vFactory,
        Serializer::View vSerializer,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject)
    {
    assertOpen();

    if (NULL == vFactory)
        {
        COH_THROW (IllegalArgumentException::create(
                "factory cannot be NULL"));
        }

    if (NULL == vSerializer)
        {
        COH_THROW (IllegalArgumentException::create(
                "serializer cannot be NULL"));
        }

    PofChannel::Handle hChannel = PofChannel::create();
    hChannel->setId(nId);
    hChannel->setConnection(this);
    hChannel->setMessageFactory(vFactory);
    hChannel->setReceiver(hReceiver);
    hChannel->setSerializer(vSerializer);
    hChannel->setSubject(vSubject);
    hChannel->openInternal();

    registerChannel(hChannel);

    return hChannel;
    }

void PofConnection::openInternal()
    {
    if (isOpen())
        {
        return;
        }

    Peer::Handle hManager = cast<Peer::Handle>(getConnectionManager());
    COH_ENSURE(hManager != NULL);

    // make sure the ConnectionManager has the MessagingProtocol
    PeerProtocol::View vProtocol = cast<PeerProtocol::View>(
            hManager->getProtocol(PeerProtocol::getProtocolName()));
    COH_ENSURE(vProtocol != NULL);

    // get the appropriate MessagingProtocol MessageFactory
    Protocol::MessageFactory::View vFactory = vProtocol->getMessageFactory(
            vProtocol->getCurrentVersion());

    // open "Channel0"
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel(0));
    hChannel0->setMessageFactory(vFactory);
    hChannel0->setReceiver(hManager);
    hChannel0->setSerializer(hManager->ensureSerializer());
    hChannel0->openInternal();

    m_fOpen = true;

    // note that we do not notify the ConnectionManager that the Connection has
    // opened just yet; the Connection still needs to be connected or accepted
    // (See ConnectionOpenRequest and ConnectionOpenResponse)
    }

bool PofConnection::ping()
    {
    if (getPingLastMillis() == 0)
        {
        PofChannel::Handle             hChannel0 = cast<PofChannel::Handle>(
                    getChannel(0));
        Protocol::MessageFactory::View vFactory  = hChannel0->getMessageFactory();
        PingRequest::Handle            hRequest  = cast<PingRequest::Handle>(
                vFactory->createMessage(PingRequest::type_id));

        try
            {
            hChannel0->send((Request::Handle) hRequest);
            }
        catch (Exception::View)
            {
            return false;
            }

        setPingLastMillis(System::safeTimeMillis());
        return true;
        }
    else
        {
        return false;
        }
    }

void PofConnection::resetStats()
    {
    setStatsBytesReceived(0);
    setStatsBytesSent(0);
    setStatsReceived(0);
    setStatsSent(0);
    setStatsReset(System::safeTimeMillis());
    m_ldtStatsReset = System::currentTimeMillis();
    }

void PofConnection::send(WriteBuffer::View vwb)
    {
    assertOpen();

    // update stats
    setStatsBytesSent(getStatsBytesSent() + vwb->length());
    setStatsSent(getStatsSent() + 1);
    }

void PofConnection::unregisterChannel(Channel::View vChannel)
    {
    if (0 == vChannel->getId())
        {
        // never unregister "Channel0"
        // see PofConnection()
        return;
        }

    LongArray::Handle hlaChannel = f_hlaChannel;
    COH_SYNCHRONIZED (hlaChannel)
        {
        hlaChannel->remove(vChannel->getId());
        }
    }


// ----- Connection interface -----------------------------------------------

ConnectionManager::Handle PofConnection::getConnectionManager()
    {
    return m_whManager;
    }

ConnectionManager::View PofConnection::getConnectionManager() const
    {
    return m_whManager;
    }

UUID::View PofConnection::getId() const
    {
    return f_vUUID;
    }

UUID::View PofConnection::getPeerId() const
    {
    return m_vUUIDPeer;
    }

bool PofConnection::isOpen() const
    {
    return m_fOpen;
    }

void PofConnection::close()
    {
    close(true, NULL);
    }

Channel::Handle PofConnection::openChannel(Protocol::View vProtocol,
        String::View vsName,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject)
    {
    return ensureConnectionManager()->openChannel(this, vProtocol, vsName,
            hReceiver, vSubject);
    }

URI::View PofConnection::createChannel(Protocol::View vProtocol,
        Channel::Receiver::Handle hReceiver)
    {
    Peer::Handle hManager = ensureConnectionManager();
    if (Thread::currentThread() == hManager->getThread())
        {
        return createChannelInternal(vProtocol,
                hManager->ensureSerializer(),
                hReceiver);
        }
    else
        {
        return hManager->createChannel(this, vProtocol, hReceiver);
        }
    }

Channel::Handle PofConnection::acceptChannel(URI::View vUri,
        Channel::Receiver::Handle hReceiver,
        Subject::View vSubject)
    {
    return ensureConnectionManager()->acceptChannel(this, vUri, hReceiver,
            vSubject);
    }

Channel::Handle PofConnection::getChannel(int32_t nId)
    {
    LongArray::Handle hla = f_hlaChannel;

    // avoid synchronization if possible; see Peer#decodeMessage
    if (ensureConnectionManager()->isServiceThread())
        {
        return cast<Channel::Handle>(hla->get(nId));
        }

    COH_SYNCHRONIZED (hla)
        {
        return cast<Channel::Handle>(hla->get(nId));
        }
    }

Channel::View PofConnection::getChannel(int32_t nId) const
    {
    LongArray::Handle hla = f_hlaChannel;

    // avoid synchronization if possible; see Peer#decodeMessage
    if (ensureConnectionManager()->isServiceThread())
        {
        return cast<Channel::View>(hla->get(nId));
        }

    COH_SYNCHRONIZED (hla)
        {
        return cast<Channel::View>(hla->get(nId));
        }
    }

Collection::View PofConnection::getChannels() const
    {
    LongArray::View    vla   = f_hlaChannel;
    ArrayList::Handle hList = ArrayList::create();
    COH_SYNCHRONIZED (vla)
        {
        for (Iterator::Handle hIter = vla->iterator(); hIter->hasNext(); )
            {
            hList->add(hIter->next());
            }
        }
    return hList;
    }


// ----- Describable interface ----------------------------------------------

String::View PofConnection::getDescription() const
    {
    String::View vs = COH_TO_STRING(super::getDescription() << "Id=" << getId() << ", Open=" << Boolean::toString(isOpen()));

    Member::View vMember = getMember();
    if (vMember != NULL)
        {
        vs = COH_TO_STRING(vs << ", " << vMember);
        }

    return vs;
    }


// ----- internal methods ---------------------------------------------------

void PofConnection::assertOpen() const
    {
    if (!isOpen())
        {
        // REVIEW
        COH_THROW (ConnectionException::create("connection is closed",
                (Exception::View) NULL, this));
        }
    }

int32_t PofConnection::generateChannelId() const
    {
    int32_t nScale = instanceof<ConnectionAcceptor::View>(
            ensureConnectionManager()) ? 1 : -1;
    int32_t nId;
    do
        {
        nId = Random::getInstance()->nextInt32(
                Integer32::max_value) * nScale;
        }
    while (nId == 0 || getChannel(nId) != NULL ||
            f_hlaChannelPending->get(nId) != NULL);

    return nId;
    }

void PofConnection::registerChannel(Channel::Handle hChannel)
    {
    LongArray::Handle hla = f_hlaChannel;
    COH_SYNCHRONIZED (hla)
        {
        Object::Holder ohChannel = hla->set(hChannel->getId(), hChannel);
        if (NULL != ohChannel)
            {
            // REVIEW
            hla->set(hChannel->getId(),
                    cast<Channel::Handle>(ohChannel, false));
            COH_THROW_STREAM (IllegalArgumentException,
                    "duplicate channel: " << hChannel);
            }
        }
    }


// ---- accessors -----------------------------------------------------------

Peer::Handle PofConnection::ensureConnectionManager()
    {
    // ensure that our ConnectionManager still holds a reference to us;
    // if it does not, our weak reference will be NULL which we will treat
    // as if the Connection was explicitly closed
    Peer::Handle hManager = cast<Peer::Handle>(getConnectionManager());
    if (NULL == hManager)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("connection is closed",
                (Exception::View) NULL, this));
        }

    return hManager;
    }

Peer::View PofConnection::ensureConnectionManager() const
    {
    // ensure that our ConnectionManager still holds a reference to us;
    // if it does not, our weak reference will be NULL which we will treat
    // as if the Connection was explicitly closed
    Peer::View vManager = cast<Peer::View>(getConnectionManager());
    if (NULL == vManager)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("connection is closed",
                (Exception::View) NULL, this));
        }

    return vManager;
    }

bool PofConnection::isActiveThread() const
    {
    return f_hThreadGate->isActiveThread();
    }

bool PofConnection::isCloseNotify() const
    {
    return m_fCloseNotify;
    }

void PofConnection::setCloseNotify(bool f)
    {
    m_fCloseNotify = f;
    }

bool PofConnection::isCloseOnExit() const
    {
    return m_fCloseOnExit;
    }

void PofConnection::setCloseOnExit(bool f)
    {
    m_fCloseOnExit = f;
    }

Exception::Holder PofConnection::getCloseException() const
    {
    return m_oheCloseException;
    }

void PofConnection::setCloseException(Exception::Holder ohe)
    {
    m_oheCloseException = ohe;
    }

void PofConnection::setConnectionManager(ConnectionManager::Handle hManager)
    {
    m_whManager = hManager;
    }

void PofConnection::setId(UUID::View vUUID)
    {
    initialize(f_vUUID, vUUID);
    }

Member::View PofConnection::getMember() const
    {
    return f_vMember;
    }

void PofConnection::setMember(Member::View vMember)
    {
    initialize(f_vMember, vMember);
    }

Map::View PofConnection::getMessageFactoryMap() const
    {
    return f_vMapMessageFactory;
    }

void PofConnection::setMessageFactoryMap(Map::View vMap)
    {
    initialize(f_vMapMessageFactory, vMap);
    }

int32_t PofConnection::getPeerEdition() const
    {
    return m_nPeerEdition;
    }

void PofConnection::setPeerEdition(int32_t nEdition)
    {
    m_nPeerEdition = nEdition;
    }

void PofConnection::setPeerId(UUID::View vUUID)
    {
    m_vUUIDPeer = vUUID;
    }

int64_t PofConnection::getPingLastMillis() const
    {
    return m_ldtPingLast;
    }

void PofConnection::setPingLastMillis(int64_t ldt)
    {
    m_ldtPingLast = ldt;
    }

int64_t PofConnection::getStatsBytesReceived() const
    {
    return m_cStatsBytesReceived;
    }

void PofConnection::setStatsBytesReceived(int64_t cb)
    {
    m_cStatsBytesReceived = cb;
    }

int64_t PofConnection::getStatsBytesSent() const
    {
    return m_cStatsBytesSent;
    }

void PofConnection::setStatsBytesSent(int64_t cb)
    {
    m_cStatsBytesSent = cb;
    }

int64_t PofConnection::getStatsReceived() const
    {
    return m_cStatsReceived;
    }

void PofConnection::setStatsReceived(int64_t cMessage)
    {
    m_cStatsReceived = cMessage;
    }

void PofConnection::setStatsReset(int64_t ldtReset)
    {
    m_ldtStatsReset = ldtReset;
    }

int64_t PofConnection::getStatsReset() const
    {
    return m_ldtStatsReset;
    }

int64_t PofConnection::getStatsSent() const
    {
    return m_cStatsSent;
    }

void PofConnection::setStatsSent(int64_t cMessage)
    {
    m_cStatsSent = cMessage;
    }

COH_CLOSE_NAMESPACE4
