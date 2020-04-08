/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/ListenerKeyRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

ListenerKeyRequest::ListenerKeyRequest()
    : m_fAdd(false), m_fLite(false),
      f_hTrigger(self(), (MapTrigger::Handle) NULL, true)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t ListenerKeyRequest::getTypeId() const
    {
    return type_id;
    }


// ----- Describable interface ----------------------------------------------

String::View ListenerKeyRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", Add=" << isAdd() << ", Lite=" << isLite() <<
            ", Trigger=" << getTrigger() << ", Priming=" << isPriming());
    }


// ----- PortableObject interface -------------------------------------------

void ListenerKeyRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractKeyRequest::readExternal(hIn);
    setAdd(hIn->readBoolean(2));
    setLite(hIn->readBoolean(3));
    setTrigger(cast<MapTrigger::Handle>(hIn->readObject(4)));

    if (getImplVersion() > 5)
        {
        setPriming(hIn->readBoolean(5));
        }
    }

void ListenerKeyRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractKeyRequest::writeExternal(hOut);
    hOut->writeBoolean(2, isAdd());
    hOut->writeBoolean(3, isLite());
    hOut->writeObject(4, getTrigger());

    if (getImplVersion() > 5)
        {
        hOut->writeBoolean(5, isPriming());
        }
    }


// ----- internal methods ---------------------------------------------------

void ListenerKeyRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

void ListenerKeyRequest::setAdd(bool fAdd)
    {
    m_fAdd = fAdd;
    }

bool ListenerKeyRequest::isAdd() const
    {
    return m_fAdd;
    }

void ListenerKeyRequest::setLite(bool fLite)
    {
    m_fLite = fLite;
    }

bool ListenerKeyRequest::isLite() const
    {
    return m_fLite;
    }

void ListenerKeyRequest::setTrigger(MapTrigger::Handle hTrigger)
    {
    initialize(f_hTrigger, hTrigger);
    }

MapTrigger::Handle ListenerKeyRequest::getTrigger() const
    {
    return f_hTrigger;
    }

void ListenerKeyRequest::setPriming(bool fPriming)
    {
    m_fPriming = fPriming;
    }

bool ListenerKeyRequest::isPriming() const
    {
    return m_fPriming;
    }

COH_CLOSE_NAMESPACE6

