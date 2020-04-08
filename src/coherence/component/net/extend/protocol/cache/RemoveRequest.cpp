/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/RemoveRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

RemoveRequest::RemoveRequest()
    {
    }


// ----- Message interface --------------------------------------------------

int32_t RemoveRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void RemoveRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeyRequest::readExternal(hIn);

    setReturnRequired(hIn->readBoolean(2));
    }

void RemoveRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeyRequest::writeExternal(hOut);

    hOut->writeBoolean(2, isReturnRequired());
    }


// ----- internal methods ---------------------------------------------------

void RemoveRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

bool RemoveRequest::isReturnRequired() const
    {
    return m_fReturnRequired;
    }

void RemoveRequest::setReturnRequired(bool fRequired)
    {
    m_fReturnRequired = fRequired;
    }

COH_CLOSE_NAMESPACE6
