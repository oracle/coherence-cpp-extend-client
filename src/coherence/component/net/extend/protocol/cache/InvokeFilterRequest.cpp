/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/InvokeFilterRequest.hpp"

#include "private/coherence/component/net/extend/protocol/cache/PartialResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

// ----- constructors -------------------------------------------------------

InvokeFilterRequest::InvokeFilterRequest()
    : f_hProcessor(self()), f_vCookie(self())
    {
    }

// ----- Message interface --------------------------------------------------

int32_t InvokeFilterRequest::getTypeId() const
    {
    return type_id;
    }

// ----- PortableObject interface -------------------------------------------

void InvokeFilterRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractFilterRequest::readExternal(hIn);

    setProcessor(cast<InvocableMap::EntryProcessor::Handle>(hIn->readObject(2)));
    setCookie(hIn->readBinary(3));
    }

void InvokeFilterRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractFilterRequest::writeExternal(hOut);

    hOut->writeObject(2, getProcessor());
    hOut->writeBinary(3, getCookie());
    }

// ----- PriorityTask interface ---------------------------------------------

int32_t InvokeFilterRequest::getSchedulingPriority() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getSchedulingPriority()
           : PriorityTask::schedule_standard;
    }

int64_t InvokeFilterRequest::getExecutionTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getExecutionTimeoutMillis()
           : PriorityTask::timeout_default;
    }

int64_t InvokeFilterRequest::getRequestTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getRequestTimeoutMillis()
           : PriorityTask::timeout_default;
    }

void InvokeFilterRequest::runCanceled(bool fAbandoned)
    {
    if (instanceof<PriorityTask::Handle>(f_hProcessor))
        {
        cast<PriorityTask::Handle>(f_hProcessor)->runCanceled(fAbandoned);
        }
    }

// ----- Describable interface ----------------------------------------------

String::View InvokeFilterRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Processor=" << getProcessor() << ", Cookie=" << getCookie());
    }

// ----- internal methods ---------------------------------------------------

AbstractPofResponse::Handle InvokeFilterRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPartialResponse::Handle>(
            vFactory->createMessage(PartialResponse::type_id));
    }

void InvokeFilterRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

// ----- accessors ----------------------------------------------------------

InvocableMap::EntryProcessor::View InvokeFilterRequest::getProcessor() const
    {
    return f_hProcessor;
    }

InvocableMap::EntryProcessor::Handle InvokeFilterRequest::getProcessor()
    {
    return f_hProcessor;
    }

void InvokeFilterRequest::setProcessor(
            InvocableMap::EntryProcessor::Handle hProcessor)
    {
    initialize(f_hProcessor, hProcessor);
    }

Binary::View InvokeFilterRequest::getCookie() const
    {
    return f_vCookie;
    }

void InvokeFilterRequest::setCookie(Binary::View vBin)
    {
    initialize(f_vCookie, vBin);
    }

COH_CLOSE_NAMESPACE6
