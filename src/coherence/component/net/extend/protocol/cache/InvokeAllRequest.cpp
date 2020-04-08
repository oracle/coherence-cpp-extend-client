/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/InvokeAllRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

// ----- constructors -------------------------------------------------------

InvokeAllRequest::InvokeAllRequest()
    : f_hProcessor(self())
    {
    }

// ----- Message interface --------------------------------------------------

int32_t InvokeAllRequest::getTypeId() const
    {
    return type_id;
    }

// ----- PortableObject interface -------------------------------------------

void InvokeAllRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeySetRequest::readExternal(hIn);

    setProcessor(cast<InvocableMap::EntryProcessor::Handle>(hIn->readObject(2)));
    }

void InvokeAllRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeySetRequest::writeExternal(hOut);

    hOut->writeObject(2, getProcessor());
    }

// ----- PriorityTask interface ---------------------------------------------

int32_t InvokeAllRequest::getSchedulingPriority() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getSchedulingPriority()
           : PriorityTask::schedule_standard;
    }

int64_t InvokeAllRequest::getExecutionTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getExecutionTimeoutMillis()
           : PriorityTask::timeout_default;
    }

int64_t InvokeAllRequest::getRequestTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hProcessor)
           ? cast<PriorityTask::View>(f_hProcessor)->getRequestTimeoutMillis()
           : PriorityTask::timeout_default;
    }

void InvokeAllRequest::runCanceled(bool fAbandoned)
    {
    if (instanceof<PriorityTask::Handle>(f_hProcessor))
        {
        cast<PriorityTask::Handle>(f_hProcessor)->runCanceled(fAbandoned);
        }
    }

// ----- Describable interface ----------------------------------------------

String::View InvokeAllRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Processor=" << getProcessor());
    }

// ----- internal methods ---------------------------------------------------

void InvokeAllRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

// ----- accessors ----------------------------------------------------------

InvocableMap::EntryProcessor::View InvokeAllRequest::getProcessor() const
    {
    return f_hProcessor;
    }

InvocableMap::EntryProcessor::Handle InvokeAllRequest::getProcessor()
    {
    return f_hProcessor;
    }

void InvokeAllRequest::setProcessor(
            InvocableMap::EntryProcessor::Handle hProcessor)
    {
    initialize(f_hProcessor, hProcessor);
    }

COH_CLOSE_NAMESPACE6
