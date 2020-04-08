/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/QueryRequest.hpp"

#include "private/coherence/component/net/extend/protocol/cache/PartialResponse.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

QueryRequest::QueryRequest()
    : m_fKeysOnly(false), f_vCookie(self()), f_vFilterCookie(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t QueryRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void QueryRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractFilterRequest::readExternal(hIn);

    setKeysOnly(hIn->readBoolean(2));
    setCookie(hIn->readBinary(3));
    setFilterCookie(hIn->readObject(4));
    }

void QueryRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractFilterRequest::writeExternal(hOut);

    hOut->writeBoolean(2, isKeysOnly());
    hOut->writeBinary(3, getCookie());
    hOut->writeObject(4, getFilterCookie());
    }


// ----- Describable interface ----------------------------------------------

String::View QueryRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", KeysOnly=" << isKeysOnly());
    }


// ----- internal methods ---------------------------------------------------

AbstractPofResponse::Handle QueryRequest::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<AbstractPartialResponse::Handle>(
            vFactory->createMessage(PartialResponse::type_id));
    }

void QueryRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

bool QueryRequest::isKeysOnly() const
    {
    return m_fKeysOnly;
    }

void QueryRequest::setKeysOnly(bool fKeysOnly)
    {
    m_fKeysOnly = fKeysOnly;
    }

Binary::View QueryRequest::getCookie() const
    {
    return f_vCookie;
    }

void QueryRequest::setCookie(Binary::View vBin)
    {
    initialize(f_vCookie, vBin);
    }

Object::View QueryRequest::getFilterCookie() const
    {
    return f_vFilterCookie;
    }

void QueryRequest::setFilterCookie(Object::View vCookie)
    {
    initialize(f_vFilterCookie, vCookie);
    }

COH_CLOSE_NAMESPACE6
