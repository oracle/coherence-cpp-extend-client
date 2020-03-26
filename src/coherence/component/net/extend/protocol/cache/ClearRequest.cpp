/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/ClearRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

ClearRequest::ClearRequest()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t ClearRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void ClearRequest::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);

    if (getImplVersion() > 5)
        {
        setTruncate(hIn->readBoolean(1));
        }
    }

void ClearRequest::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    if (getImplVersion() > 5)
        {
        hOut->writeBoolean(1, isTruncate());
        }
    }


// ----- internal methods ---------------------------------------------------

void ClearRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

void ClearRequest::setTruncate(bool fTruncate)
    {
    m_fTruncate = fTruncate;
    }

bool ClearRequest::isTruncate() const
    {
    return m_fTruncate;
    }

COH_CLOSE_NAMESPACE6
