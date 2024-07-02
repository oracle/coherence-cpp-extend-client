/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/util/Peer.hpp"

#include "coherence/io/IOException.hpp"
#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/WrapperStreamFactory.hpp"

#include "coherence/net/RequestTimeoutException.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Listeners.hpp"
#include "coherence/util/Iterator.hpp"

#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/component/net/extend/PofCodec.hpp"

#include "private/coherence/component/net/extend/protocol/AcceptChannel.hpp"
#include "private/coherence/component/net/extend/protocol/AcceptChannelResponse.hpp"
#include "private/coherence/component/net/extend/protocol/CreateChannel.hpp"
#include "private/coherence/component/net/extend/protocol/CloseChannel.hpp"
#include "private/coherence/component/net/extend/protocol/CloseConnection.hpp"
#include "private/coherence/component/net/extend/protocol/EncodedMessage.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyStartup.hpp"
#include "private/coherence/component/net/extend/protocol/NotifyShutdown.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannel.hpp"
#include "private/coherence/component/net/extend/protocol/OpenChannelResponse.hpp"
#include "private/coherence/component/net/extend/protocol/OpenConnection.hpp"
#include "private/coherence/component/net/extend/protocol/PeerProtocol.hpp"
#include "private/coherence/component/net/extend/protocol/PeerMessageFactory.hpp"

#include "private/coherence/net/messaging/Request.hpp"

#include "private/coherence/run/xml/XmlHelper.hpp"

#include "private/coherence/util/StringHelper.hpp"

#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,component,util)

using coherence::component::net::extend::PofCodec;
using coherence::component::net::extend::protocol::AcceptChannel;
using coherence::component::net::extend::protocol::AcceptChannelResponse;
using coherence::component::net::extend::protocol::CreateChannel;
using coherence::component::net::extend::protocol::CloseChannel;
using coherence::component::net::extend::protocol::CloseConnection;
using coherence::component::net::extend::protocol::EncodedMessage;
using coherence::component::net::extend::protocol::NotifyStartup;
using coherence::component::net::extend::protocol::NotifyShutdown;
using coherence::component::net::extend::protocol::OpenChannel;
using coherence::component::net::extend::protocol::OpenChannelResponse;
using coherence::component::net::extend::protocol::OpenConnection;
using coherence::component::net::extend::protocol::PeerMessageFactory;
using coherence::component::net::extend::protocol::PeerProtocol;
using coherence::io::IOException;
using coherence::io::OctetArrayReadBuffer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::WrapperStreamFactory;
using coherence::net::RequestTimeoutException;
using coherence::net::messaging::ConnectionException;
using coherence::net::messaging::Protocol;
using coherence::net::messaging::Request;
using coherence::security::auth::Subject;
using coherence::run::xml::XmlHelper;
using coherence::util::ArrayList;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::Listeners;
using coherence::util::StringHelper;


// ----- static initialization ----------------------------------------------

const bool Peer::debug = Boolean::parse(System::getProperty(
        "coherence.messaging.debug", "false"));


// ----- constructor --------------------------------------------------------

Peer::Peer()
    : f_hChannel(self()),
      f_hConnection(self()),
      f_hCodec(self()),
      f_hConnectionListeners(self(), Listeners::create()),
      m_cPingInterval(0),
      m_ldtPingLastCheckMillis(0),
      m_ldtPingLastMillis(0),
      m_cPingTimeout(0),
      f_vProtocol(self()),
      f_hProtocolMap(self(), HashMap::create()),
      f_vProtocolVersionMap(self()),
      f_hReceiverMap(self(), HashMap::create()),
      m_cRequestTimeout(30000),
      m_cbMaxIncomingMessageSize(0),
      m_cbMaxOutgoingMessageSize(0),
      m_cStatsBytesReceived(0),
      m_cStatsBytesSent(0),
      m_cStatsSent(0),
      m_cStatsTimeoutCount(self(), 0),
      f_hWrapperStreamFactoryList(self()),
      m_wvParentService(self())
    {
    }


// ----- Peer interface -----------------------------------------------------

Channel::Handle Peer::acceptChannel(PofConnection::Handle hConnection,
                URI::View vUri, Channel::Receiver::Handle hReceiver,
                Subject::View vSubject)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle                hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    AcceptChannel::Handle hRequest = cast<AcceptChannel::Handle>(
            vFactory0->createMessage(AcceptChannel::type_id));

    hRequest->setChannelUri(vUri);
    hRequest->setConnection(hConnection);
    hRequest->setIdentityToken(serializeIdentityToken(generateIdentityToken(vSubject)));
    hRequest->setReceiver(hReceiver);
    hRequest->setSubject(vSubject);

    Request::Status::Handle hStatus = cast<Request::Status::Handle>(
            hChannel0->request(hRequest));

    AcceptChannelResponse::Handle hResponse = cast<AcceptChannelResponse::Handle>(
            hStatus->waitForResponse(getRequestTimeout()));

    return cast<Channel::Handle>(hResponse->getResult());
    }

void Peer::closeChannel(PofChannel::Handle hChannel, bool fNotify,
        Exception::Holder ohe, bool fWait)
    {
    COH_ENSURE(NULL != hChannel);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    CloseChannel::Handle hRequest = cast<CloseChannel::Handle>(
            vFactory0->createMessage(CloseChannel::type_id));

    hRequest->setCause(ohe);
    hRequest->setChannelClose(hChannel);
    hRequest->setNotify(fNotify);

    if (fWait)
        {
        hChannel0->request(hRequest);
        }
    else
        {
        hChannel0->send((Request::Handle) hRequest);
        }
    }

void Peer::closeConnection(PofConnection::Handle hConnection, bool fNotify,
        Exception::Holder ohe, bool fWait)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    CloseConnection::Handle hRequest = cast<CloseConnection::Handle>(
            vFactory0->createMessage(CloseConnection::type_id));

    hRequest->setCause(ohe);
    hRequest->setConnectionClose(hConnection);
    hRequest->setNotify(fNotify);

    if (fWait)
        {
        hChannel0->request(hRequest);
        }
    else
        {
        hChannel0->send((Request::Handle) hRequest);
        }
    }

URI::Handle Peer::createChannel(PofConnection::Handle hConnection,
        Protocol::View vProtocol, Channel::Receiver::Handle hReceiver)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    CreateChannel::Handle hRequest = cast<CreateChannel::Handle>(
            vFactory0->createMessage(CreateChannel::type_id));

    hRequest->setConnection(hConnection);
    hRequest->setProtocol(vProtocol);
    hRequest->setReceiver(hReceiver);

    return cast<URI::Handle>(hChannel0->request(hRequest));
    }

Subject::View Peer::assertIdentityToken(Object::View vToken)
    {
    return getOperationalContext()->getIdentityAsserter()->
            assertIdentity(vToken, getParentService());
    }

Object::View Peer::generateIdentityToken(Subject::View vSubject)
    {
    return getOperationalContext()->getIdentityTransformer()->
            transformIdentity(vSubject, getParentService());
    }

Object::View Peer::deserializeIdentityToken(Array<octet_t>::View vabToken)
    {
    if (vabToken != NULL)
        {
        try
            {
            OctetArrayReadBuffer::Handle hBuf = OctetArrayReadBuffer::create(
                    vabToken, 0, vabToken->length);
            return ensureSerializer()->deserialize(hBuf->getBufferInput());
            }
        catch (Exception::View e)
            {
            COH_LOGEXMSG(e, "An exception occurred while deserializing an identity token", 1);
            COH_THROW(SecurityException::create("invalid identity token"));
            }
        }

    return NULL;
    }

Array<octet_t>::View Peer::serializeIdentityToken(Object::View vToken)
    {
    if (vToken != NULL)
        {
        try
            {
            OctetArrayWriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(1024);
            ensureSerializer()->serialize(hBuf->getBufferOutput(), vToken);
            return hBuf->toOctetArray();
            }
        catch (Exception::View e)
            {
            COH_LOGEXMSG(e, "An exception occurred while serializing an identity token", 1);
            COH_THROW(SecurityException::create("unable to produce identity token"));
            }
        }

    return NULL;
    }

void Peer::onChannelOpened(PofChannel::Handle hChannel)
    {
    if (hChannel->getId() != 0)
        {
        COH_LOG("Opened: " << hChannel, 6);
        }
    }

void Peer::onConnectionClosed(PofConnection::Handle hConnection)
    {
    if (get_Connection() == hConnection)
        {
        return;
        }

    dispatchConnectionEvent(hConnection, ConnectionEvent::connection_closed, NULL);

    COH_LOG("Closed: " << hConnection, 6);
    }

void Peer::onConnectionError(PofConnection::Handle hConnection,
        Exception::Holder ohe)
    {
    if (get_Connection() == hConnection)
        {
        return;
        }

    dispatchConnectionEvent(hConnection, ConnectionEvent::connection_error, ohe);

    COH_LOG("Closed: " << hConnection << " due to: " << ohe->getName() << ": " << ohe->getMessage(), 6);
    }

void Peer::onConnectionOpened(PofConnection::Handle hConnection)
    {
    if (get_Connection() == hConnection)
        {
        return;
        }

    dispatchConnectionEvent(hConnection, ConnectionEvent::connection_opened, NULL);

    COH_LOG("Opened: " << hConnection, 6);
    }

PofChannel::Handle Peer::openChannel(PofConnection::Handle hConnection,
        Protocol::View vProtocol, String::View vsName,
        Channel::Receiver::Handle hReceiver, Subject::View vSubject)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    OpenChannel::Handle hRequest = cast<OpenChannel::Handle>(
            vFactory0->createMessage(OpenChannel::type_id));

    hRequest->setConnection(hConnection);
    hRequest->setIdentityToken(serializeIdentityToken(generateIdentityToken(vSubject)));
    hRequest->setProtocol(vProtocol);
    hRequest->setReceiver(hReceiver);
    hRequest->setReceiverName(vsName);
    hRequest->setSubject(vSubject);

    Request::Status::Handle hStatus =
        cast<Request::Status::Handle>(hChannel0->request(hRequest));

    OpenChannelResponse::Handle hResponse =
        cast<OpenChannelResponse::Handle>(hStatus->waitForResponse(
                getRequestTimeout()));

    return cast<PofChannel::Handle>(hResponse->getResultChannel());
    }

void Peer::openConnection(PofConnection::Handle hConnection)
    {
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();

    OpenConnection::Handle hRequest = cast<OpenConnection::Handle>(
            vFactory0->createMessage(OpenConnection::type_id));

    hRequest->setConnectionOpen(hConnection);

    Request::Status::Handle hStatus = cast<Request::Status::Handle>(
            hChannel0->request(hRequest));
    if (NULL != hStatus)
        {
        try
            {
            hStatus->waitForResponse(getRequestTimeout());
            }
        catch (RequestTimeoutException::View e)
            {
            hConnection->close(false, e);
            COH_THROW (e);
            }
        }
    }

void Peer::post(Message::Handle hMessage)
    {
    COH_ENSURE(NULL != hMessage);

    // monitor the event dispatcher queue and slow down if it gets too long
    EventDispatcher::Handle hDispatcher =
        cast<Peer::EventDispatcher::Handle>(getEventDispatcher());

    if (NULL != hDispatcher)
        {
        hDispatcher->drainOverflow();
        }

    Channel::Handle hChannel = hMessage->getChannel();
    COH_ENSURE(NULL != hChannel);

    if (this == hChannel->getReceiver() && hMessage->getTypeId() < 0)
        {
        // internal message
        getQueue()->add(hMessage);
        }
    else
        {
        // external message
        send(hMessage);
        }
    }

void Peer::receive(ReadBuffer::View vrb, PofConnection::Handle hConnection)
    {
    COH_ENSURE(NULL != vrb);
    COH_ENSURE(NULL != hConnection);

    Channel::Handle hChannel0 = getChannel();
    if (NULL == hChannel0)
        {
        return;
        }

    Protocol::MessageFactory::View vFactory0 = hChannel0->getMessageFactory();
    if (NULL == vFactory0)
        {
        return;
        }

    EncodedMessage::Handle hMessage = cast<EncodedMessage::Handle>(
            vFactory0->createMessage(EncodedMessage::type_id));

    hMessage->setChannel(hChannel0);
    hMessage->setConnection(hConnection);
    hMessage->setReadBuffer(vrb);

    post(hMessage);
    }

void Peer::registerProtocol(Protocol::View vProtocol)
    {
    if (getServiceState() > service_initial)
        {
        COH_THROW (IllegalStateException::create());
        }

    if (NULL == vProtocol)
        {
        COH_THROW (IllegalArgumentException::create("protocol cannot be null"));
        }

    String::View vsName = vProtocol->getName();
    if (NULL == vsName)
        {
        COH_THROW_STREAM(IllegalArgumentException, "missing protocol name: " << vProtocol);
        }

    getProtocolMap()->put(vsName, vProtocol);
    }

void Peer::registerReceiver(Channel::Receiver::Handle hReceiver)
    {
    if (getServiceState() > service_initial)
        {
        COH_THROW (IllegalStateException::create());
        }

    if (NULL == hReceiver)
        {
        COH_THROW (IllegalArgumentException::create("receiver cannot be null"));
        }

    String::View vsName = hReceiver->getName();
    if (NULL == vsName)
        {
        COH_THROW_STREAM(IllegalArgumentException,"missing receiver name: " << hReceiver);
        }

    getReceiverMap()->put(vsName, hReceiver);
    }

void Peer::removeConnectionListener(ConnectionListener::Handle hListener)
    {
    getConnectionListeners()->remove(hListener);
    }

void Peer::shutdown()
    {
    COH_SYNCHRONIZED (this)
        {
        if (isStarted())
            {
            if (getServiceState() < service_stopping)
                {
                // send the request to shut down
                PofChannel::Handle hChannel = getChannel();
                hChannel->send(hChannel->createMessage(NotifyShutdown::type_id));
                }
            }

        Thread::Handle hThread = getThread();
        if (hThread != Thread::currentThread())
            {
            // wait for the service to stop or the thread to die
            while (isStarted() && getServiceState() < service_stopped)
                {
                try
                    {
                    wait();
                    }
                catch (InterruptedException::View e)
                    {
                    Thread::currentThread()->interrupt();
                    COH_THROW (e);
                    }
                }

            if (getServiceState() != service_stopped)
                {
                stop();
                }
            }
        }
    }

WriteBuffer::Handle Peer::allocateWriteBuffer() const
    {
    return OctetArrayWriteBuffer::create(1024);
    }

void Peer::checkPingTimeout(PofConnection::Handle hConnection) const
    {
    int64_t cMillis = getPingTimeout();
    if (cMillis > 0L)
        {
        int64_t ldtPing = hConnection->getPingLastMillis();
        if (ldtPing > 0L)
            {
            if (System::safeTimeMillis() >= ldtPing + cMillis)
                {
                hConnection->close(false, ConnectionException::create(
                        COH_TO_STRING("did not receive a response to a ping within "
                        << cMillis << " millis"), (Exception::View) NULL,
                        hConnection));
                }
            }
        }
    }

Message::Handle Peer::decodeMessage(ReadBuffer::BufferInput::Handle hIn,
        PofConnection::Handle hConnection, bool fFilter)
    {
    COH_ENSURE(NULL != hIn);
    COH_ENSURE(NULL != hConnection);

    Codec::Handle hCodec = getCodec();
    COH_ENSURE(NULL != hCodec);

    // filter the input, if necessary
    if (fFilter)
        {
        hIn = filterBufferInput(hIn);
        }

    // resolve the Channel
    Channel::Handle hChannel = hConnection->getChannel(hIn->readInt32());
    if (NULL == hChannel || !hChannel->isOpen())
        {
        return NULL;
        }

    // attempt to decode the Message
    Message::Handle hMessage = hCodec->decode(hChannel, hIn);
    hMessage->setChannel(hChannel);
    return hMessage;
    }

void Peer::dispatchConnectionEvent(Connection::Handle hConnection,
        ConnectionEvent::Id nEvent, Exception::Holder ohe)
    {
    Listeners::Handle hListeners = getConnectionListeners();
    if (!hListeners->isEmpty())
        {
        dispatchEvent(ConnectionEvent::create(hConnection, nEvent, ohe), hListeners);
        }
    }

void Peer::encodeMessage(Message::Handle hMessage,
        WriteBuffer::BufferOutput::Handle hOut, bool fFilter)
    {
    COH_ENSURE(NULL != hMessage);
    COH_ENSURE(NULL != hOut);

    Channel::Handle hChannel = hMessage->getChannel();
    COH_ENSURE(NULL != hChannel);

    Codec::Handle hCodec = getCodec();
    COH_ENSURE(NULL != hCodec);

    // filter the output, if necessary
    if (fFilter)
        {
        hOut = filterBufferOutput(hOut);
        }

    // write the Channel ID
    hOut->writeInt32(hChannel->getId());

    // encode the Message
    hCodec->encode(hChannel, hMessage, hOut);
    }

void Peer::enforceMaxIncomingMessageSize(int64_t cbSize)
    {
    int64_t cbMax = getMaxIncomingMessageSize();
    if (cbMax > 0 && cbSize > cbMax)
        {
        COH_THROW_STREAM(IOException, "message length: " << cbSize << " exceeds the maximum incoming message size");
        }
    }

void Peer::enforceMaxOutgoingMessageSize(int64_t cbSize)
    {
    int64_t cbMax = getMaxOutgoingMessageSize();
    if (cbMax > 0 && cbSize > cbMax)
        {
        COH_THROW_STREAM(IOException, "message length: " << cbSize << " exceeds the maximum outgoing message size");
        }
    }

ReadBuffer::BufferInput::Handle Peer::filterBufferInput(
        ReadBuffer::BufferInput::Handle hIn) const
    {
    List::View vList = getWrapperStreamFactoryList();
    if (NULL == vList || vList->isEmpty())
        {
        return hIn;
        }

    ReadBuffer::BufferInput::Handle hResult = hIn;
    for (size32_t i = 0, c = vList->size(); i < c; ++i)
        {
        hResult = cast<WrapperStreamFactory::View>(
                vList->get(i))->getInputStream(hResult);
        }

    return hResult;
    }

WriteBuffer::BufferOutput::Handle Peer::filterBufferOutput(
                WriteBuffer::BufferOutput::Handle hOut) const
    {
    List::View vList = getWrapperStreamFactoryList();
    if (NULL == vList || vList->isEmpty())
        {
        return hOut;
        }

    // wrap the BufferOutput
    WriteBuffer::BufferOutput::Handle hResult = hOut;
    for (size32_t i = 0, c = vList->size(); i < c; ++i)
        {
        hResult = cast<WrapperStreamFactory::View>(
                vList->get(i))->getOutputStream(hResult);
        }

    return hResult;
    }

void Peer::onMessageDecodeException(Exception::Holder ohe,
        ReadBuffer::BufferInput::Handle hIn, PofConnection::Handle hConnection,
        bool fFilter)
    {
    COH_LOGEXMSG(ohe, "An exception occurred while decoding a Message for Service="
            << getServiceName() << " received from: " << hConnection, 1);

    // resolve the Channel
    PofChannel::Handle hChannel;
    try
        {
        // filter the input, if necessary
        if (fFilter)
            {
            hIn = filterBufferInput(hIn);
            }
        hChannel = cast<PofChannel::Handle>(
                hConnection->getChannel(hIn->readInt32()));
        }
    catch (IOException::View)
        {
        hChannel = NULL;
        }

    // close the Channel or Connection
    if (NULL == hChannel || !hChannel->isOpen() || hChannel->getId() == 0)
        {
        hConnection->close(true, ohe, false);
        }
    else
        {
        hChannel->close(true, ohe);
        }
    }

void Peer::onMessageEncodeException(Exception::Holder ohe,
        Message::Handle hMessage)
    {
    COH_LOGEXMSG(ohe, "An exception occurred while encoding a "
            << Class::getClassName(hMessage)
            << " for Service=" << getServiceName(), 1);

    // close the Channel or Connection
    PofChannel::Handle hChannel = cast<PofChannel::Handle>(hMessage->getChannel());
    if (!hChannel->isOpen() || hChannel->getId() == 0)
        {
        PofConnection::Handle hConnection =
            cast<PofConnection::Handle>(hChannel->getConnection());

        // see #send and Channel::receive
        hConnection->setCloseOnExit(true);
        hConnection->setCloseNotify(true);
        hConnection->setCloseException(ohe);
        }
    else
        {
        // see #send and Channel::receive
        hChannel->setCloseOnExit(true);
        hChannel->setCloseNotify(true);
        hChannel->setCloseException(ohe);
        }
    }

void Peer::onNotify()
    {
    int64_t ldtStart  = System::safeTimeMillis();
    int64_t cMessage  = getStatsReceived();
    int64_t cbReceive = getStatsBytesReceived();

    Queue::Handle hQueue = getQueue();
    while (!isExiting())
        {
        Message::Handle hMessage = cast<Message::Handle>(hQueue->removeNoWait());

        if (NULL == hMessage)
            {
            break;
            }
        else
            {
            cMessage++;
            }

        // decode the Message if necessary
        EncodedMessage::Handle hMessageImpl =
            cast<EncodedMessage::Handle>(hMessage, /*fThrow*/ false);
        if (NULL != hMessageImpl)
            {
            ReadBuffer::View vrb = hMessageImpl->getReadBuffer();
            if (NULL == vrb || vrb->length() == 0)
                {
                continue;
                }
            size32_t cb = vrb->length();

            // update stats
            PofConnection::Handle hConnection = hMessageImpl->getConnection();
            hConnection->setStatsBytesReceived(hConnection->getStatsBytesReceived() + cb);
            cbReceive += cb;

            struct NotifyFinally
                {
                NotifyFinally(Peer& thisPeer, ReadBuffer::View vPeerRb)
                    : peer(thisPeer), vrb(vPeerRb)
                    {
                    }

                ~NotifyFinally()
                    {
                    try
                        {
                        peer.releaseReadBuffer(vrb);
                        }
                    catch (Exception::View e)
                        {
                        COH_LOG("Error releasing read buffer: " << e, -1);
                        // eat it
                        }
                    }

                Peer&            peer;
                ReadBuffer::View vrb;
                } finally(*this, vrb);

            // decode the Message
            try
                {
                hMessage = decodeMessage(vrb->getBufferInput(), hConnection, true);
                }
            catch (Exception::View e)
                {
                onMessageDecodeException(e,
                        vrb->getBufferInput(), hConnection, true);
                continue;
                }

            if (NULL == hMessage)
                {
                continue;
                }

            if (debug)
                {
                COH_LOG("Received: " << hMessage, 6);
                }
            }

        // make sure the target Channel is still open
        PofChannel::Handle hChannel = cast<PofChannel::Handle>(
                hMessage->getChannel());
        if (NULL == hChannel || !hChannel->isOpen())
            {
            continue;
            }

        // make sure the target Connection is still open
        PofConnection::Handle hConnection = cast<PofConnection::Handle>(
                hChannel->getConnection());
        if (NULL == hConnection || !hConnection->isOpen())
            {
            continue;
            }

        // update stats
        hConnection->setStatsReceived(hConnection->getStatsReceived() + 1);

        hChannel->receive(hMessage);
        }

    // heartbeat
    int64_t ldtNow = System::safeTimeMillis();
    if (ldtNow >= getPingNextCheckMillis())
        {
        checkPingTimeouts();
        setPingLastCheckMillis(ldtNow);
        }
    if (ldtNow >= getPingNextMillis())
        {
        ping();
        setPingLastCheckMillis(0L);
        setPingLastMillis(ldtNow);
        }

    setStatsReceived(cMessage);
    setStatsBytesReceived(cbReceive);
    setStatsCpu(getStatsCpu() + (ldtNow - ldtStart));

    super::onNotify();
    }

void Peer::releaseReadBuffer(ReadBuffer::View)
    {
    }

void Peer::releaseWriteBuffer(WriteBuffer::Handle, Exception::Holder)
    {
    }

int64_t Peer::parseMemorySize(XmlElement::View vXml, String::View vsName,
        int64_t cbDefault)
    {
    if (NULL == vXml)
        {
        return cbDefault;
        }

    String::View vsBytes = vXml->getSafeElement(vsName)->getString();
    if (vsBytes->length() == 0)
        {
        return cbDefault;
        }

    return StringHelper::parseMemorySize(vsBytes);
    }

void Peer::send(Message::Handle hMessage)
    {
    PofChannel::Handle hChannel = cast<PofChannel::Handle>(
            hMessage->getChannel());
    COH_ENSURE(NULL != hChannel);
    COH_ENSURE(hChannel->isActiveThread());

    // allocate a WriteBuffer
    WriteBuffer::Handle hWb = allocateWriteBuffer();

    // encode the Message
    try
        {
        encodeMessage(hMessage, hWb->getBufferOutput(), true);
        enforceMaxOutgoingMessageSize(hWb->length());
        }
    catch (Exception::View ve)
        {
        releaseWriteBuffer(hWb, ve);
        onMessageEncodeException(ve, hMessage);
        COH_THROW (ve);
        }

    // send the Message
    PofConnection::Handle hConnection = cast<PofConnection::Handle>(
            hChannel->getConnection());
    try
        {
        hConnection->send(hWb);
        releaseWriteBuffer(hWb, (Exception::View) NULL);
        }
    catch (Exception::View ve)
        {
        releaseWriteBuffer(hWb, ve);

        // see Channel#post
        hConnection->setCloseOnExit(true);
        hConnection->setCloseNotify(false);
        hConnection->setCloseException(ve);

        COH_THROW (ve);
        }

    // update stats
    setStatsSent(getStatsSent() + 1);
    setStatsBytesSent(getStatsBytesSent() + hWb->length());

    if (debug)
        {
        COH_LOG("Sent: " << hMessage, 6);
        }
    }


// ----- Channel::Receiver interface ----------------------------------------

String::View Peer::getName() const
    {
    return getServiceName();
    }

Protocol::View Peer::getProtocol() const
    {
    return f_vProtocol;
    }

void Peer::registerChannel(Channel::Handle)
    {
    }

void Peer::onMessage(Message::Handle hMessage)
    {
    hMessage->run();
    }

void Peer::onChannelClosed(Channel::Handle hChannel)
    {
    if (hChannel->getId() != 0)
        {
        COH_LOG("Closed: " << hChannel, 6);
        }
    }

void Peer::unregisterChannel(Channel::Handle)
    {
    }

// ----- ConnectionManager interface ----------------------------------------

void Peer::addConnectionListener(ConnectionListener::Handle hListener)
    {
    ensureEventDispatcher();
    getConnectionListeners()->add(hListener);
    }

void Peer::configure(XmlElement::View vXml)
    {
    COH_SYNCHRONIZED (this)
        {
        if (NULL == vXml)
            {
            return;
            }
        super::configure(vXml);

        XmlElement::View vXmlCat;
        XmlElement::View vXmlSub;

        // <outgoing-message-handler>
        vXmlCat = vXml->getSafeElement("outgoing-message-handler");

        // <request-timeout>
        setRequestTimeout(parseTime(vXmlCat, "request-timeout", getRequestTimeout()));

        // <heartbeat-timeout>
        setPingTimeout(parseTime(vXmlCat, "heartbeat-timeout", getRequestTimeout()));

        // <heartbeat-interval>
        setPingInterval(parseTime(vXmlCat, "heartbeat-interval", getPingInterval()));

        // make sure the heartbeat timeout <= interval
        if (getPingInterval() > 0L)
            {
            if (getPingTimeout() == 0L)
                {
                setPingTimeout(getPingInterval());
                }
            else
                {
                setPingTimeout(std::min(getPingInterval(), getPingTimeout()));
                }
            }

        // <max-outgoing-message-size>
        setMaxOutgoingMessageSize(parseMemorySize(vXmlCat, "max-message-size", 0));

        // <incoming-message-handler>
        vXmlCat = vXml->getSafeElement("incoming-message-handler");

        super::configure(vXmlCat);

        // <max-incoming-message-size>
        if (NULL != vXmlCat)
            {
            setMaxIncomingMessageSize((int32_t) parseMemorySize(vXmlCat,
                    "max-message-size", 0));
            }

        // <use-filters>
        vXmlCat = vXml->getSafeElement("use-filters");

        if (vXmlCat != NULL)
            {
            List::Handle hList = ArrayList::create();

            // <filter-name>'s
            Map::View vFilterMap = getOperationalContext()->getFilterMap();
            for (Iterator::Handle hIter = vXmlCat->getElements("filter-name");
                    hIter->hasNext(); )
                {
                vXmlSub = cast<XmlElement::View>(hIter->next());

                Object::Holder oh = vFilterMap->get(vXmlSub->getString());
                if (NULL == oh)
                    {
                    COH_THROW_STREAM (IllegalArgumentException,
                        "Filter "
                        << vXmlSub->getString()
                        << " not found");
                    }
                else
                    {
                    hList->add(oh);
                    }
                }

            if (!hList->isEmpty())
                {
                setWrapperStreamFactoryList(hList);
                }
            }

        // <message-codec>
        vXmlCat = vXml->getElement("message-codec");

        if (NULL != vXmlCat)
            {
            ClassLoader::View vLoader = SystemClassLoader::getInstance();
            setCodec(cast<Codec::Handle>(XmlHelper::createInstance(vXmlCat, vLoader,
                    vLoader->loadByType(typeid(Codec)))));
            }

        setServiceConfig(vXml);
        }
    }

Channel::Receiver::View Peer::getReceiver(String::View vsReceiverName) const
    {
    return cast<Channel::Receiver::View>(getReceiverMap()->get(vsReceiverName));
    }

Channel::Receiver::Handle Peer::getReceiver(String::View vsReceiverName)
    {
    return cast<Channel::Receiver::Handle>(getReceiverMap()->get(vsReceiverName));
    }

Map::View Peer::getReceivers() const
    {
    Map::View vMap = getReceiverMap();
    COH_SYNCHRONIZED (vMap)
        {
        HashMap::Handle hMap = HashMap::create();
        hMap->putAll(vMap);
        return hMap;
        }
    }

Protocol::View Peer::getProtocol(String::View vsName) const
    {
    return cast<Protocol::View>(getProtocolMap()->get(vsName));
    }

Map::View Peer::getProtocols() const
    {
    Map::View hMap = getProtocolMap();
    COH_SYNCHRONIZED (hMap)
        {
        Map::Handle hResult = HashMap::create();
        hResult->putAll(hMap);
        return hResult;
        }
    }


// ----- Service interface --------------------------------------------------

String::View Peer::formatStats() const
    {
    int64_t cTotal  = std::max((System::currentTimeMillis() - getStatsReset()), (int64_t)0);
    int64_t cbRcvd  = getStatsBytesReceived();
    int64_t cbSent  = getStatsBytesSent();
    int64_t cbpsIn  = cTotal == 0L ? 0L : (cbRcvd / cTotal)*1000L;
    int64_t cbpsOut = cTotal == 0L ? 0L : (cbSent / cTotal)*1000L;

    return COH_TO_STRING(super::formatStats()
           << ", BytesReceived="
           << StringHelper::toMemorySizeString(cbRcvd, false)
           << ", BytesReceived="
           << StringHelper::toMemorySizeString(cbSent, false)
           << ", ThroughputInbound="
           << StringHelper::toMemorySizeString(cbpsIn, false)
           << ", ThroughputOutbound="
           << StringHelper::toMemorySizeString(cbpsOut, false));
    }

void Peer::onEnter()
    {
    // open the internal Connection and Channel
    get_Connection()->openInternal();

    setStartTimeStamp(System::safeTimeMillis());

    resetStats();

    setServiceState(service_starting);

    PofChannel::Handle hChannel = getChannel();
    hChannel->send(hChannel->createMessage(NotifyStartup::type_id));
    }

void Peer::onExit()
    {
    super::onExit();

    get_Connection()->closeInternal(false, NULL, -1);
    }

void Peer::onServiceStarting()
    {
    super::onServiceStarting();

    // make sure a Codec is set up
    if (NULL == getCodec())
        {
        setCodec(PofCodec::create());
        }

    // set up the Protocol version map
    Map::Handle hMap = HashMap::create();
    for (Iterator::Handle hIter = getProtocolMap()->values()->iterator(); hIter->hasNext(); )
        {
        Protocol::View vProtocol       = cast<Protocol::View>(hIter->next());
        String::View   vsName          = vProtocol->getName();
        int32_t        nVersionCurrent = vProtocol->getCurrentVersion();
        int32_t        nVersionSupport = vProtocol->getSupportedVersion();

        if (NULL == vsName)
            {
            COH_THROW_STREAM(IllegalArgumentException, "protocol has no name: "
                    << vProtocol);
            }

        ObjectArray::Handle haVersion = ObjectArray::create(2);
        haVersion[0] = Integer32::create(nVersionCurrent);
        haVersion[1] = Integer32::create(nVersionSupport);
        hMap->put(vsName, haVersion);
        }

    setProtocolVersionMap(hMap);
    }

void Peer::onServiceStopped()
    {
    super::onServiceStopped();

    COH_LOG("Stopped: " << self(), 5);
    }


// ----- Object interface ---------------------------------------------------

void Peer::onInit()
    {
    Protocol::View vProtocol = ensureInitializedProtocol();

    // add the MessagingProtocol
    registerProtocol(vProtocol);

    // initialize the internal Connection and Channel
    PofConnection::Handle hConnection = PofConnection::create();
    hConnection->setConnectionManager(this);
    hConnection->setId(getProcessId());
    // TODO NSA - Need to Replace this code in when we have Collections.singletonMap
    HashMap::Handle hMap = HashMap::create();
    hMap->put(vProtocol->getName(),
            vProtocol->getMessageFactory(vProtocol->getCurrentVersion()));
    hConnection->setMessageFactoryMap(cast<Map::View>(hMap));
    // transition the map to be immutable
    hMap = NULL;

    PofChannel::Handle hChannel = cast<PofChannel::Handle>(hConnection->getChannel(0));
    hChannel->setReceiver(this);

    setChannel(hChannel);
    set_Connection(hConnection);

    super::onInit();
    }


// ----- accessor methods ---------------------------------------------------

PofChannel::Handle Peer::getChannel()
    {
    return f_hChannel;
    }

PofChannel::View Peer::getChannel() const
    {
    return f_hChannel;
    }

PofConnection::Handle Peer::get_Connection()
    {
    return f_hConnection;
    }

PofConnection::View Peer::get_Connection() const
    {
    return f_hConnection;
    }

Codec::Handle Peer::getCodec()
    {
    return f_hCodec;
    }

Codec::View Peer::getCodec() const
    {
    return f_hCodec;
    }

void Peer::setCodec(Codec::Handle hCodec)
    {
    initialize(f_hCodec, hCodec);
    }

Listeners::Handle Peer::getConnectionListeners()
    {
    return f_hConnectionListeners;
    }

void Peer::onServiceStarted()
    {
    super::onServiceStarted();

    COH_LOG("Started: " << self(), 5);
    }

void Peer::resetStats()
    {
    setStatsBytesReceived(0);
    setStatsBytesSent(0);
    setStatsSent(0);
    setStatsTimeoutCount(0);

    super::resetStats();
    }

int64_t Peer::getPingInterval() const
    {
    return m_cPingInterval;
    }

int64_t Peer::getPingLastCheckMillis() const
    {
    return m_ldtPingLastCheckMillis;
    }

int64_t Peer::getPingLastMillis() const
    {
    return m_ldtPingLastMillis;
    }

int64_t Peer::getPingNextCheckMillis() const
    {
    int64_t ldtLast = getPingLastMillis();
    int64_t cMillis = getPingTimeout();

    return cMillis == 0L || ldtLast == 0L || getPingLastCheckMillis() > 0L ?
           Integer64::max_value : ldtLast + cMillis;
    }

int64_t Peer::getPingNextMillis() const
    {
    int64_t ldtLast = getPingLastMillis();
    int64_t cMillis = getPingInterval();

    return cMillis == 0L ? Integer64::max_value :
           ldtLast == 0L ? System::safeTimeMillis() : ldtLast + cMillis;
    }

int64_t Peer::getPingTimeout() const
    {
    return m_cPingTimeout;
    }

int64_t Peer::getMaxIncomingMessageSize() const
    {
    return m_cbMaxIncomingMessageSize;
    }

int64_t Peer::getMaxOutgoingMessageSize() const
    {
    return m_cbMaxOutgoingMessageSize;
    }

UUID::View Peer::getProcessId()
    {
    static FinalView<UUID> vProcessId(System::common(), UUID::create());
    return vProcessId;
    }
COH_STATIC_INIT(Peer::getProcessId());

Map::View Peer::getProtocolMap() const
    {
    return f_hProtocolMap;
    }

Map::Handle Peer::getProtocolMap()
    {
    return f_hProtocolMap;
    }

Map::View Peer::getProtocolVersionMap() const
    {
    return f_vProtocolVersionMap;
    }

Map::View Peer::getReceiverMap() const
    {
    return f_hReceiverMap;
    }

Map::Handle Peer::getReceiverMap()
    {
    return f_hReceiverMap;
    }

int64_t Peer::getRequestTimeout() const
    {
    return m_cRequestTimeout;
    }

int64_t Peer::getStatsBytesReceived() const
    {
    return m_cStatsBytesReceived;
    }

int64_t Peer::getStatsBytesSent() const
    {
    return m_cStatsBytesSent;
    }

int64_t Peer::getStatsSent() const
    {
    return m_cStatsSent;
    }

int64_t Peer::getStatsTimeoutCount() const
    {
    return m_cStatsTimeoutCount;
    }

int64_t Peer::getWaitMillis() const
    {
    int64_t cMillis = super::getWaitMillis();
    if (getPingInterval() > 0L)
        {
        int64_t ldtNow  = System::safeTimeMillis();
        int64_t ldtNext = std::min(getPingNextMillis(), getPingNextCheckMillis());
        int64_t cNext   = ldtNext > ldtNow ? ldtNext - ldtNow : -1L;

        return cMillis == 0L ? cNext : std::min(cNext, cMillis);
        }
    else
        {
        return cMillis;
        }
    }

List::View Peer::getWrapperStreamFactoryList() const
    {
    return f_hWrapperStreamFactoryList;
    }

OperationalContext::View Peer::getOperationalContext() const
    {
    return Service::getOperationalContext();
    }

void Peer::setOperationalContext(OperationalContext::View vContext)
    {
    Service::setOperationalContext(vContext);
    }

coherence::net::Service::View Peer::getParentService() const
    {
    return m_wvParentService;
    }

void Peer::setParentService(coherence::net::Service::View vService)
    {
    m_wvParentService = vService;
    }

Protocol::MessageFactory::View Peer::getMessageFactory()
    {
    Protocol::View vProtocol = getProtocol();
    return vProtocol->getMessageFactory(vProtocol->getCurrentVersion());
    }

void Peer::set_Connection(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

void Peer::setChannel(PofChannel::Handle hChannel)
    {
    initialize(f_hChannel, hChannel);
    }

void Peer::setPingInterval(int64_t cMillis)
    {
    m_cPingInterval = cMillis;
    }

void Peer::setPingLastCheckMillis(int64_t ldt)
    {
    m_ldtPingLastCheckMillis = ldt;
    }

void Peer::setPingLastMillis(int64_t ldt)
    {
    m_ldtPingLastMillis = ldt;
    }

void Peer::setPingTimeout(int64_t cMillis)
    {
    m_cPingTimeout = cMillis;
    }

void Peer::setProtocol(Protocol::View vProtocol)
    {
    initialize(f_vProtocol, vProtocol);
    }

void Peer::setRequestTimeout(int64_t cMillis)
    {
    m_cRequestTimeout = cMillis;
    }

void Peer::setMaxIncomingMessageSize(int64_t cbMax)
    {
    m_cbMaxIncomingMessageSize = cbMax;
    }

void Peer::setMaxOutgoingMessageSize(int64_t cbMax)
    {
    m_cbMaxOutgoingMessageSize = cbMax;
    }

void Peer::setStatsBytesReceived(int64_t cb)
    {
    m_cStatsBytesReceived = cb;
    }

void Peer::setStatsBytesSent(int64_t cb)
    {
    m_cStatsBytesSent = cb;
    }

void Peer::setStatsSent(int64_t cStatsSent)
    {
    m_cStatsSent = cStatsSent;
    }

void Peer::setStatsTimeoutCount(int64_t cRequests)
    {
    m_cStatsTimeoutCount = cRequests;
    }

void Peer::setWrapperStreamFactoryList(List::Handle hList)
    {
    initialize(f_hWrapperStreamFactoryList, hList);
    }

void Peer::setProtocolVersionMap(Map::View vMap)
    {
    initialize(f_vProtocolVersionMap, vMap);
    }


// ----- Describable interface ----------------------------------------------

String::View Peer::getDescription() const
    {
    String::View vs = COH_TO_STRING(super::getDescription() << ", ThreadCount=0");

    List::View vList = getWrapperStreamFactoryList();
    if (NULL != vList && !vList->isEmpty())
        {
        vs = COH_TO_STRING(vs <<", Filters=[");

        for (Iterator::Handle hIter = vList->iterator(); hIter->hasNext(); )
            {
            vs = COH_TO_STRING(vs << Class::getClassName(hIter->next()));
            if (hIter->hasNext())
                {
                vs = COH_TO_STRING(vs << ',');
                }
            }

        vs = COH_TO_STRING(vs << ']');
        }

    Codec::View vCodec = getCodec();
    if (NULL != vCodec)
        {
        vs = COH_TO_STRING(vs << ", Codec=" << vCodec);
        }

    return COH_TO_STRING(vs << ", PingInterval="
       << getPingInterval()
       << ", PingTimeout="
       << getPingTimeout()
       << ", RequestTimeout="
       << getRequestTimeout()
       << ", MaxIncomingMessageSize="
       << getMaxIncomingMessageSize()
       << ", MaxOutgoingMessageSize="
       << getMaxOutgoingMessageSize());
    }


// ----- nested class: DispatchEvent ----------------------------------------

// ----- constructor --------------------------------------------------------

Peer::DispatchEvent::DispatchEvent()
    {
    }


// ----- DispatchEvent interface --------------------------------------------

void Peer::DispatchEvent::run()
    {
    ConnectionEvent::View vEvt = cast<ConnectionEvent::View>(getEvent(), false);

    if (NULL == vEvt)
        {
        super::run();
        }
    else
        {
        vEvt->dispatch(getListeners());
        }
    }


// ---- Peer child component factory ----------------------------------------

coherence::component::util::Service::DispatchEvent::Handle
        Peer::instantiateDispatchEvent() const
    {
    return Peer::DispatchEvent::create();
    }

Protocol::View Peer::instantiateProtocol() const
    {
    return PeerProtocol::getInstance();
    }

Protocol::View Peer::ensureInitializedProtocol()
    {
    Protocol::View vResult = getProtocol();

    if (NULL == vResult)
        {
        vResult = instantiateProtocol();
        setProtocol(vResult);
        }

    return vResult;
    }

COH_CLOSE_NAMESPACE3
