/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/AggregateAllRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

// ----- constructors -------------------------------------------------------

AggregateAllRequest::AggregateAllRequest()
    : f_hAggregator(self())
    {
    }

// ----- Message interface --------------------------------------------------

int32_t AggregateAllRequest::getTypeId() const
    {
    return type_id;
    }

// ----- PortableObject interface -------------------------------------------

void AggregateAllRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeySetRequest::readExternal(hIn);

    setAggregator(cast<InvocableMap::EntryAggregator::Handle>(hIn->readObject(2)));
    }

void AggregateAllRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeySetRequest::writeExternal(hOut);

    hOut->writeObject(2, getAggregator());
    }

// ----- PriorityTask interface ---------------------------------------------

int32_t AggregateAllRequest::getSchedulingPriority() const
    {
    return instanceof<PriorityTask::View>(f_hAggregator)
           ? cast<PriorityTask::View>(f_hAggregator)->getSchedulingPriority()
           : PriorityTask::schedule_standard;
    }

int64_t AggregateAllRequest::getExecutionTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hAggregator)
           ? cast<PriorityTask::View>(f_hAggregator)->getExecutionTimeoutMillis()
           : PriorityTask::timeout_default;
    }

int64_t AggregateAllRequest::getRequestTimeoutMillis() const
    {
    return instanceof<PriorityTask::View>(f_hAggregator)
           ? cast<PriorityTask::View>(f_hAggregator)->getRequestTimeoutMillis()
           : PriorityTask::timeout_default;
    }

void AggregateAllRequest::runCanceled(bool fAbandoned)
    {
    if (instanceof<PriorityTask::Handle>(f_hAggregator))
        {
        cast<PriorityTask::Handle>(f_hAggregator)->runCanceled(fAbandoned);
        }
    }

// ----- Describable interface ----------------------------------------------

String::View AggregateAllRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Aggregator=" << getAggregator());
    }

// ----- internal methods ---------------------------------------------------

void AggregateAllRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

// ----- accessors ----------------------------------------------------------

InvocableMap::EntryAggregator::View AggregateAllRequest::getAggregator() const
    {
    return f_hAggregator;
    }

InvocableMap::EntryAggregator::Handle AggregateAllRequest::getAggregator()
    {
    return f_hAggregator;
    }

void AggregateAllRequest::setAggregator(
            InvocableMap::EntryAggregator::Handle hAggregator)
    {
    initialize(f_hAggregator, hAggregator);
    }

COH_CLOSE_NAMESPACE6
