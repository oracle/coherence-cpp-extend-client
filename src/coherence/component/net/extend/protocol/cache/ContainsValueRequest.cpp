/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/ContainsValueRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

ContainsValueRequest::ContainsValueRequest()
    : f_vValue(self(), NULL, /*fMutable*/ true)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t ContainsValueRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void ContainsValueRequest::readExternal(PofReader::Handle hIn)
    {
    NamedCacheRequest::readExternal(hIn);

    setValue(hIn->readObject(1));
    }

void ContainsValueRequest::writeExternal(PofWriter::Handle hOut) const
    {
    NamedCacheRequest::writeExternal(hOut);

    hOut->writeObject(1, getValue());

    //release state
    // m_vValue = NULL;
    }


// ----- Describable interface ----------------------------------------------

String::View ContainsValueRequest::getDescription() const
    {
    Object::View vValue = getValue();

    return COH_TO_STRING(super::getDescription() << ", Value=" << (vValue == NULL ? "null" : COH_TO_STRING(Class::getSimpleClassName(vValue) << "(HashCode=" << vValue->hashCode() << ")")));
    }


// ----- internal methods ---------------------------------------------------

void ContainsValueRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

Object::View ContainsValueRequest::getValue() const
    {
    return f_vValue;
    }

void ContainsValueRequest::setValue(Object::View vValue)
    {
    initialize(f_vValue, vValue);
    }

COH_CLOSE_NAMESPACE6
