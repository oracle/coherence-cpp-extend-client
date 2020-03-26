/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/AbstractKeyRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

AbstractKeyRequest::AbstractKeyRequest()
    : f_vKey(self())
    {
    }


// ----- PortableObject interface -------------------------------------------

void AbstractKeyRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setKey(hIn->readObject(1));
    }

void AbstractKeyRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeObject(1, getKey());

    //release state
    // m_vKey = NULL; // c++ optimization uses FinalView and thus can't be released
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractKeyRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Key=" << getKey());
    }


// ----- accessors ----------------------------------------------------------

Object::View AbstractKeyRequest::getKey() const
    {
    return f_vKey;
    }

void AbstractKeyRequest::setKey(Object::View vKey)
    {
    initialize(f_vKey, vKey);
    }

COH_CLOSE_NAMESPACE6
