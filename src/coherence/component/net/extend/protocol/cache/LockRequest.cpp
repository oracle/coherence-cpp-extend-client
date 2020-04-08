/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/LockRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

LockRequest::LockRequest()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t LockRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void LockRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeyRequest::readExternal(hIn);

    setTimeoutMillis(hIn->readInt64(2));
    }

void LockRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeyRequest::writeExternal(hOut);

    hOut->writeInt64(2, getTimeoutMillis());
    }


// ----- internal methods ---------------------------------------------------

void LockRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

int64_t LockRequest::getTimeoutMillis() const
    {
    return m_cTimeoutMillis;
    }

void LockRequest::setTimeoutMillis(int64_t cMillis)
    {
    m_cTimeoutMillis = cMillis;
    }

COH_CLOSE_NAMESPACE6
