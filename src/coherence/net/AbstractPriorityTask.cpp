/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/net/AbstractPriorityTask.hpp"

COH_OPEN_NAMESPACE2(coherence,net)


// ----- constructors -------------------------------------------------------

AbstractPriorityTask::AbstractPriorityTask()
        : m_iSchedulingPriority(PriorityTask::schedule_standard),
          m_lExecutionTimeout(PriorityTask::timeout_default),
          m_lRequestTimeout(PriorityTask::timeout_default)
    {
    }


// ----- PriorityTask interface ---------------------------------------------

int32_t AbstractPriorityTask::getSchedulingPriority() const
    {
    return m_iSchedulingPriority;
    }

int64_t AbstractPriorityTask::getExecutionTimeoutMillis() const
    {
    return m_lExecutionTimeout;
    }

int64_t AbstractPriorityTask::getRequestTimeoutMillis() const
    {
    return m_lRequestTimeout;
    }

void AbstractPriorityTask::runCanceled(bool /* fAbandoned */)
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractPriorityTask::readExternal(PofReader::Handle hIn)
    {
    m_iSchedulingPriority = hIn->readInt32(0);
    m_lExecutionTimeout   = hIn->readInt64(1);
    m_lRequestTimeout     = hIn->readInt64(2);
    }

void AbstractPriorityTask::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, m_iSchedulingPriority);
    hOut->writeInt64(1, m_lExecutionTimeout);
    hOut->writeInt64(2, m_lRequestTimeout);
    }


// ----- Data member mutators -----------------------------------------------

void AbstractPriorityTask::setSchedulingPriority(int32_t iPriority)
    {
    if (iPriority < PriorityTask::schedule_standard
            || iPriority > PriorityTask::schedule_immediate)
        {
        COH_THROW_STREAM (IllegalArgumentException,
                     "Invalid priority" << iPriority);
        }
    m_iSchedulingPriority = iPriority;
    }

void AbstractPriorityTask::setExecutionTimeoutMillis(int64_t lTimeout)
    {
    if (lTimeout < PriorityTask::timeout_none)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                     "Invalid timeout" << lTimeout);
        }
    m_lExecutionTimeout = lTimeout;
    }

void AbstractPriorityTask::setRequestTimeoutMillis(int64_t lTimeout)
    {
    if (lTimeout < PriorityTask::timeout_none)
        {
        COH_THROW_STREAM(IllegalArgumentException,
                     "Invalid timeout" << lTimeout);
        }
    m_lRequestTimeout = lTimeout;
    }

COH_CLOSE_NAMESPACE2
