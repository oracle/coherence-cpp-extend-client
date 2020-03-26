/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/PofChannel.hpp"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"

#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"

#include "coherence/net/PriorityTask.hpp"

#include "coherence/net/messaging/ConnectionException.hpp"

#include "coherence/util/HashMap.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/LongArrayIterator.hpp"
#include "coherence/util/SafeHashMap.hpp"

#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

#include "private/coherence/component/net/extend/protocol/NotifyChannelClosed.hpp"

#include "private/coherence/component/util/Peer.hpp"

#include "private/coherence/net/messaging/Response.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

#include "private/coherence/util/HashArray.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::net::extend::AbstractPofRequest;
using coherence::component::net::extend::PofConnection;
using coherence::component::net::extend::protocol::NotifyChannelClosed;
using coherence::component::util::Peer;
using coherence::io::OctetArrayWriteBuffer;
using coherence::io::ReadBuffer;
using coherence::io::Serializer;
using coherence::io::WriteBuffer;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::PofBufferReader;
using coherence::net::messaging::ConnectionException;
using coherence::net::messaging::Response;
using coherence::net::PriorityTask;
using coherence::security::SecurityHelper;
using coherence::util::HashArray;
using coherence::util::HashMap;
using coherence::util::Iterator;
using coherence::util::LongArrayIterator;
using coherence::util::SafeHashMap;


// ----- constructors -------------------------------------------------------

PofChannel::PofChannel()
    : f_hMapAttributes(self(), SafeHashMap::create()),
      m_fCloseNotify(self(), false),
      m_fCloseOnExit(self(), false),
      f_oheCloseException(self()),
      m_whConnection(self()),
      m_nId(0),
      f_vMessageFactory(self()),
      m_fOpen(self(), false),
      m_hReceiver(self()),
      f_hlaRequest(self(), HashArray::create()),
      f_hGateRequest(self(), ThreadGate::create()),
      f_vSerializer(self()),
      f_vSubject(self()),
      f_hThreadGate(self(), ThreadGate::create()),
      m_fSecureContext(self(), (SecurityHelper::getCurrentSubject() == NULL))
    {
    }


// ----- PofChannel interface -----------------------------------------------

void PofChannel::close(bool fNotify, Exception::Holder ohe)
    {
    if (isOpen())
        {
        if (getId() == 0)
            {
            COH_THROW (UnsupportedOperationException::create("cannot closed reserved channel: 0"));
            }

        Peer::Handle hManager = getConnectionManager();
        if (Thread::currentThread() == hManager->getThread())
            {
            closeInternal(fNotify, ohe, 0L);
            }
        else
            {
            COH_ENSURE(!isActiveThread());

            // COH-18404 - not necessary to wait for the close to complete
            hManager->closeChannel(this, fNotify, ohe, false);
            }
        }
    }

bool PofChannel::closeInternal(bool fNotify, Exception::Holder ohe,
        int64_t cMillis)
    {
    if (!isOpen())
        {
        return false;
        }

    // cancel all pending requests, keeping the gate closed to prevent new
    // requests from being registered
    HashArray::Handle hlaRequest     = f_hlaRequest;
    Receiver::Handle  hReceiver      = getReceiver();
    bool              fCloseReceiver = false;

    COH_GATE_CLOSE (f_hGateRequest, ThreadGate::infinite)
        {
        Exception::Holder oheStatus;
        if (ohe == NULL)
            {
            oheStatus = ConnectionException::create("channel closed",
                       (Exception::View) NULL, getConnection());
            }
        else
            {
            oheStatus = ohe;
            }

        for (LongArrayIterator::Handle hIter = hlaRequest->iterator();
                hIter->hasNext(); )
            {
            Request::Status::Handle hStatus = cast<Request::Status::Handle>
                    (hIter->next());

            hIter->remove();
            hStatus->cancel(oheStatus);
            }

        struct CloseFinally
            {
            CloseFinally(PofChannel::Handle _hPofChannel)
                : fClose(false), hPofChannel(_hPofChannel)
                {
                }

            ~CloseFinally()
                {
                if (fClose)
                    {
                    try
                        {
                        hPofChannel->gateOpen();
                        }
                    catch (Exception::View e)
                        {
                        COH_LOG("Error re-opening the gate: " << e, -1);
                        // eat it
                        }
                    }
                }

            bool               fClose;
            PofChannel::Handle hPofChannel;
            } finally(this);
        // close the Channel
        finally.fClose = gateClose(cMillis);

        if (!finally.fClose)
            {
            // can't close the gate; signal to the holding Thread(s) that it
            // must close the Channel immediately after exiting the gate
            setCloseOnExit(true);
            setCloseNotify(fNotify);
            setCloseException(ohe);

            // double check if we can close the gate, as we want to be sure
            // that the Thread(s) saw the close notification prior to exiting
            finally.fClose = gateClose(0);
            }

        if (finally.fClose && isOpen())
            {
            // notify the receiver that the Channel is closing
            if (hReceiver != NULL)
                {
                fCloseReceiver = true;
                try
                    {
                    hReceiver->unregisterChannel(this);
                    }
                catch (Exception::View ee)
                    {
                    COH_LOGEXMSG(ee, "Error unregistering channel from receiver: "
                            << hReceiver, 1);
                    }
                setReceiver(NULL);
                }

            setOpen(false);
            }
        else
            {
            return false;
            }
        }

    if (fCloseReceiver)
        {
        try
            {
            // needs to be done outside the request array synchronization block
            hReceiver->onChannelClosed(this);
            }
        catch (Exception::View ee)
            {
            COH_LOGEXMSG(ee, "Error notifying channel closed to receiver: "
                    << hReceiver, 1);
            }
        }

    // notify the peer that the Channel is now closed
    if (fNotify && !isOpen() && getId() != 0)
        {
        // send a NotifyChannelClosed to the peer via "Channel0"
        try
            {
            PofConnection::Handle          hConnection = cast<PofConnection::Handle>(getConnection());
            Channel::Handle                hChannel0   = hConnection->getChannel(0);
            Protocol::MessageFactory::View vFactory0   = hChannel0->getMessageFactory();

            NotifyChannelClosed::Handle hMessage =
                cast<NotifyChannelClosed::Handle>(
                        vFactory0->createMessage(NotifyChannelClosed::type_id));

            hMessage->setCause(ohe);
            hMessage->setChannelId(getId());

            hChannel0->send((Message::Handle) hMessage);
            }
        catch (Exception::View) {}
        }

    // notify the Connection that the Channel is closed
    (cast<PofConnection::Handle>(getConnection()))->unregisterChannel(this);

    // notify the ConnectionManager that the Channel is closed
    getConnectionManager()->onChannelClosed(this);

    return true;
    }

Message::Handle PofChannel::createMessage(int32_t nType) const
    {
    return getMessageFactory()->createMessage(nType);
    }

Object::Holder PofChannel::deserialize(PofReader::Handle hIn) const
    {
    Serializer::View vSerializer = getSerializer();

    if (instanceof<PofSerializer::View>(vSerializer))
        {
        return cast<PofSerializer::View>(vSerializer)->deserialize(hIn);
        }

    Binary::View vBin = hIn->readBinary(0);
    hIn->readRemainder();

    // use the serializer to read the object from a binary property
    return vSerializer->deserialize(vBin->getBufferInput());
    }

Object::Holder PofChannel::deserialize(ReadBuffer::BufferInput::Handle hIn) const
    {
    PofReader::Handle hReader = PofBufferReader::create(hIn, this);
    return hReader->readObject(-1);
    }

bool PofChannel::gateClose(int64_t cMillis)
    {
    return f_hThreadGate->close(cMillis);
    }

void PofChannel::gateOpen()
    {
    f_hThreadGate->open();
    }

void PofChannel::gateEnter()
    {
    PofConnection::Handle hConnection = ensureConnection();

    hConnection->gateEnter();
    try
        {
        // if the thread is entering for the first time, throw an exception if
        // the Channel has been marked for close; this prevents new threads from
        // entering the Channel and thus keeping it open longer than necessary
        if (isCloseOnExit() && !f_hThreadGate->isActiveThread())
            {
            // REVIEW
            COH_THROW (ConnectionException::create("channel is closing",
                    (Exception::View) NULL, hConnection));
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
                    (Exception::View) NULL, (Connection::View) hConnection));
            }
        }
    catch (...)
        {
        hConnection->gateExit();
        throw;
        }
    }

void PofChannel::gateExit()
    {
    f_hThreadGate->exit();
    ensureConnection()->gateExit();

    // see if we've been asked to close the Channel
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

Peer::View PofChannel::getConnectionManager() const
    {
    Connection::View hConnection = getConnection();
    return hConnection == NULL ? (Peer::View) NULL
            : cast<Peer::View>(hConnection->getConnectionManager());
    }

Peer::Handle PofChannel::getConnectionManager()
    {
    Connection::Handle hConnection = getConnection();
    return hConnection == NULL ? (Peer::Handle) NULL
            : cast<Peer::Handle>(hConnection->getConnectionManager());
    }

void PofChannel::onRequestCompleted(Request::Status::View vStatus)
    {
    unregisterRequest(vStatus);
    }

void PofChannel::open()
    {
    openInternal();
    }

void PofChannel::openInternal()
    {
    if (isOpen())
        {
        return;
        }

    COH_ENSURE(getConnection() != NULL);
    COH_ENSURE(getMessageFactory() != NULL);
    COH_ENSURE(getSerializer() != NULL);

    setOpen(true);

    // notify the receiver that the Channel is open
    Receiver::Handle hReceiver = getReceiver();
    if (hReceiver != NULL)
        {
        try
            {
            hReceiver->registerChannel(this);
            }
        catch (Exception::View e)
            {
            COH_LOGEXMSG(e, "Error registering channel with receiver: " << hReceiver, 1);
            }
        }

    // notify the ConnectionManager that the Channel is opened
    getConnectionManager()->onChannelOpened(this);
    }

void PofChannel::receive(Message::Handle hMessage)
    {
    COH_ENSURE_PARAM(hMessage);

    try
        {
        gateEnter();
        }
    catch (ConnectionException::View)
        {
        // ignore: the Channel or Connection is closed or closing
        return;
        }

    struct receiveFinally
        {
        receiveFinally(PofChannel& thisChannel)
            : channel(thisChannel)
            {
            }

        ~receiveFinally()
            {
            try
                {
                channel.gateExit();
                }
            catch (Exception::View e)
                {
                COH_LOG("Error exiting the gate: " << e, -1);
                // eat it
                }
            }

        PofChannel& channel;
        } finally(*this);

    try
        {
        Response::Handle hResponse = cast<Response::Handle>(
                hMessage, /*fThrow*/ false);
        if (NULL != hResponse) // most common path for client; successful casts are cheaper then failed ones
            {
            int64_t                            lId = hResponse->getRequestId();
            AbstractPofRequest::Status::Handle hStatus = NULL;

            hStatus = cast<AbstractPofRequest::Status::Handle>(
                    f_hlaRequest->get(lId));

            if (NULL == hStatus)
                {
                // ignore unsolicited Responses
                }
            else
                {
                try
                    {
                    execute(hResponse);
                    if (hResponse->isFailure())
                        {
                        // cancel the Status
                        Object::Holder oResult = hResponse->getResult();
                        if (instanceof<Exception::Holder>(oResult))
                            {
                            hStatus->cancel(cast<Exception::Holder>(oResult));
                            }
                        else
                            {
                            hStatus->cancel(Exception::create(
                                    COH_TO_STRING(oResult)));
                            }
                        }
                    else
                        {
                        hStatus->setResponse(hResponse);
                        }
                    }
                catch (Exception::View e)
                    {
                    hStatus->cancel(e);
                    }
                }
            }
        else
            {
            Request::Handle hRequest = cast<Request::Handle>(
                hMessage, /*fThrow*/ false);
            if (NULL == hRequest)
                {
                execute(hMessage);
                }
            else
                {
                try
                    {
                    execute(hMessage);
                    }
                catch (Exception::View e)
                    {
                    hResponse = hRequest->ensureResponse();
                    COH_ENSURE(hResponse != NULL);
                    Exception::View ve = e;

                    // see Request#isIncoming and #run
                    if (NULL == hRequest->getStatus())
                    {
                    // report the exception and send it back to the peer
                    if (COH_LOGENABLED(5))
                        {
                        COH_LOG("An exception occurred while processing a "
                            << Class::getClassName(hMessage)
                            << " for Service="
                            << getConnectionManager()->getServiceName()
                            << ": " << ve, 5);
                        }
                    }

                    hResponse->setFailure(true);
                    hResponse->setResult(ve);
                    }

                hResponse = hRequest->ensureResponse();
                COH_ENSURE(hResponse != NULL);

                hResponse->setRequestId(hRequest->getId());

                send((Message::Handle) hResponse);
                }
            }
        }
    catch (Exception::View e)
        {
        PofConnection::Handle hConnection = cast<PofConnection::Handle>(getConnection());
        Exception::View       ve          = e;

        // see Acceptor#onServiceStopped and Initiator#onServiceStopped
        if (!hConnection->isCloseOnExit() || !Thread::currentThread()->isInterrupted())
            {
            COH_LOGEXMSG(ve, "Caught an unhandled exception while processing a "
                    << Class::getClassName(hMessage)
                    << " for Service=" << getConnectionManager()->getServiceName(), 1);
            }

        if (getId() == 0)
            {
            hConnection->setCloseOnExit(true);
            hConnection->setCloseNotify(true);
            hConnection->setCloseException(ve);
            }
        else
            {
            setCloseOnExit(true);
            setCloseNotify(true);
            setCloseException(ve);
            }
        }
    }

void PofChannel::serialize(PofWriter::Handle hOut, Object::View vo) const
    {
    Serializer::View vSerializer = getSerializer();

    if (instanceof<PofSerializer::View>(vSerializer))
        {
        cast<PofSerializer::View>(vSerializer)->serialize(hOut, vo);
        }
    else
        {
        WriteBuffer::Handle hBuf = OctetArrayWriteBuffer::create(32);

        // use the serializer to write the object out as a binary property
        vSerializer->serialize(hBuf->getBufferOutput(), vo);

        // prevent extra copy of the underlying octet array
        Array<octet_t>::View vab = hBuf->toOctetArray();

        // drop the handle to the WriteBuffer to avoid a clone
        // during construction of the Binary
        hBuf = NULL;

        hOut->writeBinary(0, Binary::create(vab, 0, vab->length));
        hOut->writeRemainder(NULL);
        }
    }

void PofChannel::serialize(WriteBuffer::BufferOutput::Handle hOut,
        Object::View v) const
    {
    PofBufferWriter::Handle hWriter = PofBufferWriter::create(hOut, this);
    hWriter->writeObject(-1, v);
    }


// ----- PofContext interface -----------------------------------------------

PofSerializer::View PofChannel::getPofSerializer(int32_t nTypeId) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->getPofSerializer(nTypeId);
        }

    if (nTypeId == 0)
        {
        return this;
        }

    String::View vsTarget;
    try
        {
        vsTarget = COH_TO_STRING(getConnectionManager());
        }
    catch (Exception::View)
        {
        vsTarget = COH_TO_STRING(this);
        }

    COH_THROW_STREAM(IllegalStateException, vsTarget <<
            " has not been configured with a PofContext; " <<
            " this channel cannot decode POF-encoded user types");
    }

int32_t PofChannel::getUserTypeIdentifier(Object::View v) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->getUserTypeIdentifier(v);
        }

    COH_ENSURE(v != NULL);
    return 0;
    }

int32_t PofChannel::getUserTypeIdentifier(Class::View vClass) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->getUserTypeIdentifier(vClass);
        }

    COH_ENSURE(vClass != NULL);
    return 0;
    }

int32_t PofChannel::getUserTypeIdentifier(String::View vsClass) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->getUserTypeIdentifier(vsClass);
        }

    COH_ENSURE(vsClass != NULL);
    return 0;
    }

String::View PofChannel::getClassName(int32_t nTypeId) const
    {
    return getClass(nTypeId)->getName();
    }

Class::View PofChannel::getClass(int32_t nTypeId) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->getClass(nTypeId);
        }

    COH_THROW_STREAM(IllegalStateException, "cannot determine class for user type ID: "
            << nTypeId);
    }

bool PofChannel::isUserType(Object::View v) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->isUserType(v);
        }

    COH_ENSURE(v != NULL);
    return false;
    }

bool PofChannel::isUserType(Class::View vClass) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->isUserType(vClass);
        }

    COH_ENSURE(vClass != NULL);
    return false;
    }

bool PofChannel::isUserType(String::View vsClass) const
    {
    PofContext::View vContext = cast<PofContext::View>(getSerializer(), false);
    if (NULL != vContext)
        {
        return vContext->isUserType(vsClass);
        }

    COH_ENSURE(vsClass != NULL);
    return false;
    }


// ----- Channel interface --------------------------------------------------

Connection::Handle PofChannel::getConnection()
    {
    return m_whConnection;
    }

Connection::View PofChannel::getConnection() const
    {
    return m_whConnection;
    }

int32_t PofChannel::getId() const
    {
    return m_nId;
    }

bool PofChannel::isOpen() const
    {
    return m_fOpen;
    }

void PofChannel::close()
    {
    close(true, Exception::View(NULL));
    }

Protocol::MessageFactory::View PofChannel::getMessageFactory() const
    {
    return f_vMessageFactory;
    }

Serializer::View PofChannel::getSerializer() const
    {
    return f_vSerializer;
    }

Subject::View PofChannel::getSubject() const
    {
    return f_vSubject;
    }

Object::Holder PofChannel::getAttribute(String::View vsName) const
    {
    return f_hMapAttributes->get(vsName);
    }

Map::View PofChannel::getAttributes() const
    {
    Map::View vMap = f_hMapAttributes;
    COH_SYNCHRONIZED (vMap)
        {
        return Immutable<Map>(vMap);
        }
    }

Object::Holder PofChannel::setAttribute(String::View vsName,
        Object::Holder ohValue)
    {
    return f_hMapAttributes->put(vsName, ohValue);
    }

Object::Holder PofChannel::removeAttribute(String::View vsName)
    {
    return f_hMapAttributes->remove(vsName);
    }

void PofChannel::send(Message::Handle hMessage)
    {
    if (NULL == hMessage)
        {
        COH_THROW (IllegalArgumentException::create("message cannot be NULL"));
        }

    post(hMessage);
    }

Request::Status::Handle PofChannel::send(Request::Handle hRequest)
    {
    if (NULL == hRequest)
        {
        COH_THROW (IllegalArgumentException::create("request cannot be NULL"));
        }

    Request::Status::Handle hStatus = registerRequest(hRequest);
    post(hRequest);

    return hStatus;
    }

Object::Holder PofChannel::request(Request::Handle hRequest)
    {
    return request(hRequest, -1);
    }

Object::Holder PofChannel::request(Request::Handle hRequest, int64_t cMillis)
    {
    Peer::Handle hManager = ensureConnectionManager();
    COH_ENSURE(hManager->getThread() != Thread::currentThread());

    // block until the service is ready
    hManager->waitAcceptingClients();

    if (NULL == hRequest)
        {
        COH_THROW (IllegalArgumentException::create("request cannot be null"));
        }

    Request::Status::Handle hStatus = registerRequest(hRequest);
    post(hRequest);

    Response::Handle hResponse = hStatus->waitForResponse(cMillis);
    if (hResponse->isFailure())
        {
        Object::Holder voResult = hResponse->getResult();
        if (instanceof<Exception::Handle>(voResult))
            {
            COH_THROW (cast<Exception::Handle>(voResult));
            }
        else if (instanceof<Exception::View>(voResult))
            {
            COH_THROW (cast<Exception::View>(voResult));
            }
        else
            {
            COH_THROW_STREAM(Exception, "received error: " << voResult);
            }
        }
    else
        {
        return hResponse->getResult();
        }
    }

Request::Handle PofChannel::getRequest(int64_t lId)
    {
    Request::Status::Handle hStatus = cast<Request::Status::Handle>(
            f_hlaRequest->get(lId));
    return NULL == hStatus ? Request::Handle(NULL) : hStatus->getRequest();
    }

Channel::Receiver::Handle PofChannel::getReceiver()
    {
    return m_hReceiver;
    }

Channel::Receiver::View PofChannel::getReceiver() const
    {
    return m_hReceiver;
    }


// ----- Describable interface ----------------------------------------------

String::View PofChannel::getDescription() const
    {
    String::View vs = COH_TO_STRING(super::getDescription() << "Id=" << getId());

    bool fOpen = isOpen();
    vs = COH_TO_STRING(vs << ", Open=" << Boolean::toString(fOpen));

    if (fOpen)
        {
        Connection::View vConnection = getConnection();
        vs = COH_TO_STRING(vs << ", Connection=");
        if (NULL == vConnection)
            {
            vs = COH_TO_STRING(vs << "NULL");
            }
        else
            {
            vs = COH_TO_STRING(vs << vConnection->getId());
            }
        }

    Protocol::MessageFactory::View vMessageFactory = getMessageFactory();
    if (NULL != vMessageFactory)
        {
        Protocol::View vProtocol = vMessageFactory->getProtocol();
        if (NULL != vProtocol)
            {
            vs = COH_TO_STRING(vs << ", Protocol=" << vProtocol <<
                 ", NegotiatedProtocolVersion=" <<
                 vMessageFactory->getVersion());
            }
        }

    Channel::Receiver::View vReceiver = getReceiver();
    if (NULL != vReceiver)
        {
        vs = COH_TO_STRING(vs << ", Receiver=" << vReceiver);
        }

    return vs;
    }


// ----- internal methods ---------------------------------------------------

void PofChannel::assertOpen() const
    {
    if (!isOpen())
        {
        // REVIEW
        COH_THROW (ConnectionException::create("channel is closed",
                (Exception::View) NULL, getConnection()));
        }
    }

int64_t PofChannel::calculateRequestTimeout(Request::Handle hRequest)
    {
    Peer::Handle hManager = getConnectionManager();

    int64_t cMillis = 0;
    if (hManager != NULL)
        {
        cMillis = hManager->getRequestTimeout();
        int64_t cAdjustMillis = hRequest->getRequestTimeoutMillis();
        if (cAdjustMillis != 0)
            {
            cMillis = hManager->adjustTimeout(cMillis, cAdjustMillis);
            }

        // when the RequestContext is in place (COH-1026) we will also have:
        /*
        RequestContext ctx = RequestContext.getContext();
        if (ctx != null)
            {
            cMillis = manager.adjustTimeout(cMillis, ctx.getRequestTimeout());
            }
        */
        }

    return cMillis;
    }

void PofChannel::execute(Message::Handle hMessage)
    {
    Receiver::Handle hReceiver;

    if (hMessage->isSolicited())
        {
        // solicited Message
        hReceiver = NULL;
        }
    else
        {
        // unsolicited Message
        hReceiver = getReceiver();
        }

    // execute the Message in the context of the Channel's Subject;
    // since the doAs() cost is relatively high, we avoid that call
    // in the most common case when neither the Channel nor the proxy service
    // has associated Subjects
    if (isSecureContext())
        {
        COH_RUN_AS (getSubject())
            {
            if (NULL == hReceiver)
                {
                hMessage->run();
                }
            else
                {
                hReceiver->onMessage(hMessage);
                }
            }
        }
    else
        {
        if (NULL == hReceiver)
            {
            hMessage->run();
            }
        else
            {
            hReceiver->onMessage(hMessage);
            }
        }
    }

void PofChannel::post(Message::Handle hMessage)
    {
    struct PostFinally
        {
        PostFinally(PofChannel::Handle hChannel)
            : hMyChannel(hChannel), fExit(false)
            {
            }

        ~PostFinally()
            {
            if (fExit)
                {
                try
                    {
                    hMyChannel->gateExit();
                    }
                catch (Exception::View e)
                    {
                    COH_LOG("PostFinally Unexpected exception in destructor: " << e, -1);
                    // eat it, just eat it
                    }
                }
            }
        PofChannel::Handle hMyChannel;
        bool               fExit;
        } finally (this);

    if (hMessage->isSolicited())
        {
        COH_ENSURE(isActiveThread());
        }
    else
    {
        gateEnter();
        finally.fExit = true;
        }

    try
        {
        hMessage->setChannel(this);
        ensureConnectionManager()->post(hMessage);
        }
    catch (Exception::View ve)
        {
        if (instanceof<Request::Handle>(hMessage))
            {
            Request::Status::Handle hStatus = cast<Request::Handle>(hMessage)->getStatus();
            if (hStatus != NULL)
                {
                hStatus->cancel(ve);
                }
            }
        COH_THROW (ve);
        }
    }

Request::Status::Handle PofChannel::registerRequest(Request::Handle hRequest)
    {
    COH_ENSURE(NULL != hRequest)

    AbstractPofRequest::Status::Handle hStatus = AbstractPofRequest::Status::create();
    hStatus->setChannel(this);
    hStatus->setDefaultTimeoutMillis(calculateRequestTimeout(hRequest));
    hStatus->setRequest(hRequest);

    hRequest->setStatus(hStatus);

    COH_GATE_ENTER (f_hGateRequest, ThreadGate::infinite) // see #closeInternal
        {
        assertOpen();
        hRequest->setId(f_hlaRequest->add(hStatus));
        }

    return hStatus;
    }

void PofChannel::unregisterRequest(Request::Status::View vStatus)
    {
    COH_ENSURE(NULL != vStatus);
    f_hlaRequest->remove(vStatus->getRequest()->getId());
    }


// ----- accessors ----------------------------------------------------------

PofConnection::Handle PofChannel::ensureConnection()
    {
    PofConnection::Handle hConnection =
            cast<PofConnection::Handle>(getConnection());
    if (NULL == hConnection)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("channel is closed",
                (Exception::View) NULL, hConnection));
        }

    return hConnection;
    }

PofConnection::View PofChannel::ensureConnection() const
    {
    PofConnection::View vConnection =
                cast<PofConnection::View>(getConnection());
    if (NULL == vConnection)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("channel is closed",
                (Exception::View) NULL, vConnection));
        }

    return vConnection;
    }

Peer::Handle PofChannel::ensureConnectionManager()
    {
    Peer::Handle hManager = cast<Peer::Handle>(getConnectionManager());
    if (NULL == hManager)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("channel is closed",
                (Exception::View) NULL, getConnection()));
        }

    return hManager;
    }

Peer::View PofChannel::ensureConnectionManager() const
    {
    Peer::View vManager = cast<Peer::View>(getConnectionManager());
    if (NULL == vManager)
        {
        // REVIEW
        COH_THROW (ConnectionException::create("channel is closed",
                (Exception::View) NULL, getConnection()));
        }

    return vManager;
    }

bool PofChannel::isActiveThread() const
    {
    return f_hThreadGate->isActiveThread();
    }

bool PofChannel::isCloseNotify() const
    {
    return m_fCloseNotify;
    }

void PofChannel::setCloseNotify(bool f)
    {
    m_fCloseNotify = f;
    }

bool PofChannel::isCloseOnExit() const
    {
    return m_fCloseOnExit;
    }

void PofChannel::setCloseOnExit(bool f)
    {
    m_fCloseOnExit = f;
    }

Exception::Holder PofChannel::getCloseException() const
    {
    return f_oheCloseException;
    }

void PofChannel::setCloseException(Exception::Holder ohe)
    {
    initialize(f_oheCloseException, ohe);
    }

void PofChannel::setConnection(Connection::Handle hConnection)
    {
    COH_ENSURE(!isOpen());
    m_whConnection = hConnection;
    }

void PofChannel::setId(int32_t nId)
    {
    COH_ENSURE(!isOpen());
    m_nId = nId;
    }

void PofChannel::setMessageFactory(Protocol::MessageFactory::View vFactory)
    {
    COH_ENSURE(!isOpen());
    initialize(f_vMessageFactory, vFactory);
    }

void PofChannel::setOpen(bool fOpen)
    {
    m_fOpen = fOpen;
    }

void PofChannel::setReceiver(Receiver::Handle hReceiver)
    {
    COH_ENSURE(!isOpen() || hReceiver == NULL);

    m_hReceiver = hReceiver;
    }

void PofChannel::setSerializer(Serializer::View vSerializer)
    {
    COH_ENSURE(!isOpen());
    initialize(f_vSerializer, vSerializer);
    }

void PofChannel::setSubject(Subject::View vSubject)
    {
    setSecureContext(isSecureContext() || vSubject != NULL);
    initialize(f_vSubject, vSubject);
    }

bool PofChannel::isSecureContext() const
    {
    return m_fSecureContext;
    }

void PofChannel::setSecureContext(bool fSecureContext)
    {
    m_fSecureContext = fSecureContext;
    }

COH_CLOSE_NAMESPACE4
