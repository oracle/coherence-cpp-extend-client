/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/PutRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

PutRequest::PutRequest()
    : m_cExpiryDelay(0), m_fReturnRequired(false),
      f_vValue(self(), NULL, /*fMutable*/ true)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t PutRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void PutRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeyRequest::readExternal(hIn);

    setValue(hIn->readObject(2));
    setExpiryDelay(hIn->readInt64(3));
    setReturnRequired(hIn->readBoolean(4));
    }

void PutRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeyRequest::writeExternal(hOut);

    hOut->writeObject(2, getValue());
    hOut->writeInt64(3, getExpiryDelay());
    hOut->writeBoolean(4, isReturnRequired());

    // release state
    // m_vValue = NULL;
    }


// ----- Describable interface ----------------------------------------------

String::View PutRequest::getDescription() const
    {
    Object::View oValue = getValue();
    
    return COH_TO_STRING(super::getDescription() << ", Value=" << (oValue == NULL ? "null" : COH_TO_STRING(Class::getSimpleClassName(oValue) << "(HashCode=" << oValue->hashCode() << ")")) << ", Expiry=" << getExpiryDelay());
    }


// ----- internal methods ---------------------------------------------------

void PutRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

int64_t PutRequest::getExpiryDelay() const
    {
    return m_cExpiryDelay;
    }

void PutRequest::setExpiryDelay(int64_t cMillis)
    {
    m_cExpiryDelay = cMillis;
    }

bool PutRequest::isReturnRequired() const
    {
    return m_fReturnRequired;
    }

void PutRequest::setReturnRequired(bool fRequired)
    {
    m_fReturnRequired = fRequired;
    }

Object::View PutRequest::getValue() const
    {
    return f_vValue;
    }

void PutRequest::setValue(Object::View vValue)
    {
    initialize(f_vValue, vValue);
    }

COH_CLOSE_NAMESPACE6
