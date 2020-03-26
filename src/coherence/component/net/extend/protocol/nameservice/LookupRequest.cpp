/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/nameservice/LookupRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

// ----- constructors -------------------------------------------------------

LookupRequest::LookupRequest()
    : m_vsLookupName(self())
    {
    }

// ----- Message interface --------------------------------------------------

int32_t LookupRequest::getTypeId() const
    {
    return type_id;
    }

// ----- PortableObject interface -------------------------------------------

void LookupRequest::readExternal(PofReader::Handle hIn)
    {
    NameServiceRequest::readExternal(hIn);

    setLookupName(cast<String::View>(hIn->readObject(1)));
    }

void LookupRequest::writeExternal(PofWriter::Handle hOut) const
    {
    NameServiceRequest::writeExternal(hOut);

    hOut->writeObject(1, getLookupName());
    }

// ----- Describable interface ----------------------------------------------

String::View LookupRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", LookupName=" << getLookupName());
    }

// ----- internal methods ---------------------------------------------------

void LookupRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }

// ----- accessors ----------------------------------------------------------

String::View LookupRequest::getLookupName() const
    {
    return m_vsLookupName;
    }

void LookupRequest::setLookupName(String::View vsLookupName)
    {
    m_vsLookupName = vsLookupName;
    }

COH_CLOSE_NAMESPACE6
