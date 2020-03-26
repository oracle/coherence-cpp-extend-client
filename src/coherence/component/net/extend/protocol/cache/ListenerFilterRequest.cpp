/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/ListenerFilterRequest.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constructors -------------------------------------------------------

ListenerFilterRequest::ListenerFilterRequest()
    : m_lFilterId(0), m_fAdd(false), m_fLite(false),
      f_hTrigger(self(), (MapTrigger::Handle) NULL, /*fMutable*/ true)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t ListenerFilterRequest::getTypeId() const
    {
    return type_id;
    }


// ----- Describable interface ----------------------------------------------

String::View ListenerFilterRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", FilterId=" << getFilterId() << ", Add=" << isAdd() <<
            ", Lite=" << isLite() << ", Trigger=" << getTrigger());
    }


// ----- PortableObject interface -------------------------------------------

void ListenerFilterRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractFilterRequest::readExternal(hIn);
    setFilterId(hIn->readInt64(2));
    setAdd(hIn->readBoolean(3));
    setLite(hIn->readBoolean(4));
    setTrigger(cast<MapTrigger::Handle>(hIn->readObject(5)));
    if (getImplVersion() > 5)
        {
        setPriming(hIn->readBoolean(6));
        }
    }

void ListenerFilterRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractFilterRequest::writeExternal(hOut);
    hOut->writeInt64(2, getFilterId());
    hOut->writeBoolean(3, isAdd());
    hOut->writeBoolean(4, isLite());
    hOut->writeObject(5, getTrigger());
    if (getImplVersion() > 5)
        {
        hOut->writeBoolean(6, isPriming());
        }
    }


// ----- internal methods ---------------------------------------------------

void ListenerFilterRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

void ListenerFilterRequest::setFilterId(int64_t lFilterId)
    {
    m_lFilterId = lFilterId;
    }

int64_t ListenerFilterRequest::getFilterId() const
    {
    return m_lFilterId;
    }

void ListenerFilterRequest::setAdd(bool fAdd)
    {
    m_fAdd = fAdd;
    }

bool ListenerFilterRequest::isAdd() const
    {
    return m_fAdd;
    }

void ListenerFilterRequest::setLite(bool fLite)
    {
    m_fLite = fLite;
    }

bool ListenerFilterRequest::isLite() const
    {
    return m_fLite;
    }

void ListenerFilterRequest::setTrigger(MapTrigger::Handle hTrigger)
    {
    initialize(f_hTrigger, hTrigger);
    }

MapTrigger::Handle ListenerFilterRequest::getTrigger() const
    {
    return f_hTrigger;
    }

void ListenerFilterRequest::setPriming(bool fPriming)
    {
    m_fPriming = fPriming;
    }

bool ListenerFilterRequest::isPriming() const
    {
    return m_fPriming;
    }

COH_CLOSE_NAMESPACE6

