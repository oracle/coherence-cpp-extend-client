/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/QueryRecorder.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/SimpleQueryRecord.hpp"


COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(251, QueryRecorder);


// ----- constructors -------------------------------------------------------

QueryRecorder::QueryRecorder()
    {
    }

QueryRecorder::QueryRecorder(QueryRecorder::RecordType type)
    : m_type(type)
    {
    }


// ----- accessors ----------------------------------------------------------

QueryRecorder::RecordType QueryRecorder::getType() const
    {
    return m_type;
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder QueryRecorder::aggregate(Set::View /*vSetEntries*/)
    {
    COH_THROW (UnsupportedOperationException::create("QueryRecorder::aggregate()"));
    }


// ----- ParallelAwareAggregator interface ----------------------------------

InvocableMap::EntryAggregator::Handle QueryRecorder::getParallelAggregator()
    {
    return this;
    }

Object::Holder QueryRecorder::aggregateResults(Collection::View vCollResults)
    {
    return SimpleQueryRecord::create(m_type, vCollResults);
    }


// ----- PortableObject interface -------------------------------------------

void QueryRecorder::readExternal(PofReader::Handle hIn)
    {
    m_type = (QueryRecorder::RecordType) hIn->readInt32(0);
    }

void QueryRecorder:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, (int32_t) m_type);
    }

COH_CLOSE_NAMESPACE3
