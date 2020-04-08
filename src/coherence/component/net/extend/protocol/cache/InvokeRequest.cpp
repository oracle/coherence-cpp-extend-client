/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/InvokeRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

// ----- constructors -------------------------------------------------------

InvokeRequest::InvokeRequest()
    : f_hProcessor(self())
    {
    }

// ----- Message interface --------------------------------------------------

int32_t InvokeRequest::getTypeId() const
    {
    return type_id;
    }

// ----- PortableObject interface -------------------------------------------

void InvokeRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeyRequest::readExternal(hIn);

    setProcessor(cast<InvocableMap::EntryProcessor::Handle>(hIn->readObject(2)));
    }

void InvokeRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeyRequest::writeExternal(hOut);

    hOut->writeObject(2, getProcessor());
    }

// ----- PriorityTask interface ---------------------------------------------

int32_t InvokeRequest::getSchedulingPriority() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getSchedulingPriority()
           : PriorityTask::schedule_standard;
    }

int64_t InvokeRequest::getExecutionTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getExecutionTimeoutMillis()
           : PriorityTask::timeout_default;
    }

int64_t InvokeRequest::getRequestTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getRequestTimeoutMillis()
           : PriorityTask::timeout_default;
    }

void InvokeRequest::runCanceled(bool fAbandoned)
    {
    if (instanceof<PriorityTask::Handle>(f_hProcessor))
        {
        cast<PriorityTask::Handle>(f_hProcessor)->runCanceled(fAbandoned);
        }
    }

// ----- Describable interface ----------------------------------------------

String::View InvokeRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Processor=" << getProcessor());
    }

// ----- internal methods ---------------------------------------------------

void InvokeRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

// ----- accessors ----------------------------------------------------------

InvocableMap::EntryProcessor::View InvokeRequest::getProcessor() const
    {
    return f_hProcessor;
    }

InvocableMap::EntryProcessor::Handle InvokeRequest::getProcessor()
    {
    return f_hProcessor;
    }

void InvokeRequest::setProcessor(
            InvocableMap::EntryProcessor::Handle hProcessor)
    {
    initialize(f_hProcessor, hProcessor);
    }

COH_CLOSE_NAMESPACE6
