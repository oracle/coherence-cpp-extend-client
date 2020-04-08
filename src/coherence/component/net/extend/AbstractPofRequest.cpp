/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPofRequest.hpp"

#include "coherence/net/PriorityTask.hpp"
#include "coherence/net/RequestTimeoutException.hpp"

#include "private/coherence/component/util/Peer.hpp"
#include "private/coherence/util/logging/Logger.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::component::util::Peer;
using coherence::net::PriorityTask;
using coherence::net::RequestTimeoutException;


// ----- constructors -------------------------------------------------------

AbstractPofRequest::AbstractPofRequest()
    : m_lId(0), f_hResponse(self()), m_whStatus(self())
    {
    }

AbstractPofRequest::Status::Status()
    : f_hChannel(self()),
      m_fClosed(false),
      m_ldtDefaultTimeoutMillis(0),
      f_ohError(self()),
      m_ldtInitTimeMillis(System::currentTimeMillis()),
      f_hRequest(self()),
      f_hResponse(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

bool AbstractPofRequest::isIncoming() const
    {
    return NULL == getStatus();
    }


// ----- Request interface --------------------------------------------------

int64_t AbstractPofRequest::getId() const
    {
    return m_lId;
    }

void AbstractPofRequest::setId(int64_t lId)
    {
    m_lId = lId;
    }

Response::Handle AbstractPofRequest::ensureResponse()
    {
    Response::Handle hResponse = f_hResponse;
    if (NULL == hResponse)
        {
        Channel::View vChannel = getChannel();
        COH_ENSURE(NULL != vChannel);

        Protocol::MessageFactory::View vFactory = vChannel->getMessageFactory();
        COH_ENSURE(NULL != vFactory);

        hResponse = instantiateResponse(vFactory);
        initialize(f_hResponse, hResponse);
        }

    return hResponse;
    }

Request::Status::Handle AbstractPofRequest::getStatus()
    {
    return m_whStatus;
    }

Request::Status::View AbstractPofRequest::getStatus() const
    {
    return m_whStatus;
    }

void AbstractPofRequest::setStatus(Request::Status::Handle hStatus)
    {
    COH_ENSURE(NULL != hStatus);
    COH_ENSURE(NULL == m_whStatus);
    m_whStatus = hStatus;
    }

int64_t AbstractPofRequest::getRequestTimeoutMillis() const
    {
    return PriorityTask::timeout_default;
    }



// ----- Message interface --------------------------------------------------

void AbstractPofRequest::run()
    {
    AbstractPofResponse::Handle hResponse =
            cast<AbstractPofResponse::Handle>(ensureResponse());
    try
        {
        onRun(hResponse);
        }
    catch (Exception::View ve)
        {
        onException(ve);
        hResponse->setFailure(true);
        hResponse->setResult(ve);
        }
    }

bool AbstractPofRequest::isSolicited() const
    {
    return false;
    }


// ----- PortableObject interface -------------------------------------------

void AbstractPofRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofMessage::readExternal(hIn);

    setId(hIn->readInt64(0));
    }

void AbstractPofRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofMessage::writeExternal(hOut);

    hOut->writeInt64(0, getId());
    }


// ----- Status interface ---------------------------------------------------

bool AbstractPofRequest::Status::isClosed() const
    {
    return m_fClosed;
    }

void AbstractPofRequest::Status::cancel()
    {
    cancel(NULL);
    }

void AbstractPofRequest::Status::cancel(Exception::Holder ohe)
    {
    PofChannel::Handle hChannel;
    COH_SYNCHRONIZED (this)
        {
        if (isClosed())
            {
            hChannel = NULL;
            }
        else
            {
            hChannel  = getChannel();
            if (NULL == ohe)
                {
                // REVIEW
                initialize(f_ohError, Exception::create("request was canceled"));
                }
            else
                {
                initialize(f_ohError, ohe);
                }

            onCompletion();
            }
        }

    if (NULL != hChannel)
        {
        hChannel->onRequestCompleted(this);
        }
    }

Request::Handle AbstractPofRequest::Status::getRequest()
    {
    return f_hRequest;
    }

Request::View AbstractPofRequest::Status::getRequest() const
    {
    return f_hRequest;
    }

Response::Handle AbstractPofRequest::Status::getResponse()
    {
    Exception::Holder ohError = getError();
    if (NULL == ohError)
        {
        return f_hResponse;
        }
    if (instanceof<Exception::Handle>(ohError))
        {
        COH_THROW (cast<Exception::Handle>(ohError));
        }
    COH_THROW (ohError);
    }

Response::View AbstractPofRequest::Status::getResponse() const
    {
    return f_hResponse;
    }

Response::Handle AbstractPofRequest::Status::waitForResponse(int64_t cMillis)
    {
    if (cMillis == -1L)
        {
        cMillis = getDefaultTimeoutMillis();
        }

    if (cMillis <= 0)
        {
        COH_SYNCHRONIZED (this)
            {
            while (!isClosed())
                {
                try
                    {
                    wait();
                    }
                catch (InterruptedException::View e)
                    {
                    cancel(e);
                    Thread::currentThread()->interrupt();
                    COH_THROW (e);
                    }
                }
            }
        }
    else
        {
        COH_SYNCHRONIZED (this)
            {
            int64_t ldtStart = -1;
            int64_t cRemain  = cMillis;

            while (!isClosed())
                {
                if (ldtStart < 0)
                    {
                    ldtStart = System::currentTimeMillis();
                    }

                try
                    {
                    wait(cRemain);
                    }
                catch (InterruptedException::View e)
                    {
                    cancel(e);
                    Thread::currentThread()->interrupt();
                    COH_THROW (e);
                    }

                if (isClosed())
                    {
                    break;
                    }
                else if ((cRemain -= std::max(System::currentTimeMillis() - ldtStart,
                        (int64_t) 0)) <= 0)
                    {
                    cancel(RequestTimeoutException::create(COH_TO_STRING(
                            "request timed out after " << cMillis << " millis")));
                    break;
                    }
                }
            }
        }

    return getResponse();
    }

Response::Handle AbstractPofRequest::Status::waitForResponse()
    {
    return waitForResponse(-1L);
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPofRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Id=" << getId() << ", Status=" << getStatus());
    }

String::View AbstractPofRequest::Status::getDescription() const
    {
    bool fClosed = isClosed();

    String::View vs = COH_TO_STRING(super::getDescription() << "InitTimeMillis=" << getInitTimeMillis() << ", Closed=" << fClosed);

    if (fClosed)
        {
        Exception::View ve = getError();
        if (NULL == ve)
            {
            vs = COH_TO_STRING(vs << ", Response=" << getResponse());
            }
        else
            {
            vs = COH_TO_STRING(vs << ", Error=" << ve);
            }
        }

    return vs;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofResponse::Handle AbstractPofRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPofResponse::Handle>(vFactory->createMessage(0));
    }

void AbstractPofRequest::onException(Exception::Holder ohe)
    {
    if (isIncoming())
        {
        if (COH_LOGENABLED (5))
            {
            Peer::Handle hManager = cast<Peer::Handle>(
                    getChannel()->getConnection()->getConnectionManager());

            COH_LOG("An exception occurred while processing a "
                    << Class::getClassName(this) << " for Service="
                    << hManager->getServiceName()
                    << ": " << ohe, 5);
            }
        }
    }

void AbstractPofRequest::Status::onCompletion()
    {
    m_fClosed = true;
    notifyAll();
    }


// ----- accessors ----------------------------------------------------------

int64_t AbstractPofRequest::Status::getDefaultTimeoutMillis() const
    {
    return m_ldtDefaultTimeoutMillis;
    }

Exception::Holder AbstractPofRequest::Status::getError() const
    {
    return f_ohError;
    }

int64_t AbstractPofRequest::Status::getInitTimeMillis() const
    {
    return m_ldtInitTimeMillis;
    }

PofChannel::Handle AbstractPofRequest::Status::getChannel()
    {
    return f_hChannel;
    }

PofChannel::View AbstractPofRequest::Status::getChannel() const
    {
    return f_hChannel;
    }

void AbstractPofRequest::Status::setChannel(PofChannel::Handle hChannel)
    {
    COH_ENSURE(!isClosed() && NULL != hChannel);

    initialize(f_hChannel, hChannel);
    }

void AbstractPofRequest::Status::setRequest(Request::Handle hRequest)
    {
    COH_ENSURE(!isClosed() && NULL != hRequest);

    initialize(f_hRequest, hRequest);
    }

void AbstractPofRequest::Status::setResponse(Response::Handle hResponse)
    {
    COH_ENSURE(!isClosed() && NULL != hResponse && NULL == getResponse());

    PofChannel::Handle hChannel;
    COH_SYNCHRONIZED (this)
        {
        if (isClosed())
            {
            hChannel = NULL;
            }
        else
            {
            hChannel    = cast<PofChannel::Handle>(getChannel());
            initialize(f_hResponse, hResponse);

            onCompletion();
            }
        }

    if (NULL != hChannel)
        {
        hChannel->onRequestCompleted(this);
        }
    }

void AbstractPofRequest::Status::setDefaultTimeoutMillis(int64_t cMillis)
    {
    m_ldtDefaultTimeoutMillis = cMillis;
    }

COH_CLOSE_NAMESPACE4
