/*
 * Copyright (c) 2000, 2021, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/MapEventMessage.hpp"

#include "private/coherence/component/net/extend/RemoteNamedCache.hpp"

#include "private/coherence/component/util/RunnableCacheEvent.hpp"

#include "private/coherence/net/messaging/Channel.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::RemoteNamedCache;
using coherence::component::util::RunnableCacheEvent;
using coherence::net::messaging::Channel;


// ----- constructors -------------------------------------------------------

MapEventMessage::MapEventMessage()
    : m_nId(0), m_lFilterId(0), f_vKey(self()), f_vNewValue(self()),
      f_vOldValue(self()), m_fSynthetic(false),
      f_alFilterIds(self()), m_nTransformState(0)
    {
    }


// ----- Runnable interface -------------------------------------------------

void MapEventMessage::run()
    {
    Channel::Handle hChannel = getChannel();
    COH_ENSURE(hChannel != NULL);

    RemoteNamedCache::Handle hCache = cast<RemoteNamedCache::Handle>
            (hChannel->getReceiver());
    COH_ENSURE(hCache != NULL);

    if (isTruncate())
        {
        Listeners::View vListeners = hCache->getDeactivationListeners();
        if (!vListeners->isEmpty())
            {
            MapEvent::Handle hEvtTruncated = MapEvent::create(hCache, 
                                                              MapEvent::entry_updated, 
                                                              (Object::View) NULL, 
                                                              (Object::View) NULL, 
                                                              (Object::View) NULL);
            // dispatch the event to the listeners, which are all synchronous (hence the null Queue)
            RunnableCacheEvent::dispatchSafe(hEvtTruncated, vListeners, NULL);
            }
        }
    else
        {
        hCache->getBinaryCache()->dispatch(getId(), getFilterIds(), getKey(),
                getValueOld(), getValueNew(), isSynthetic(), getTransformState(), isPriming());
        }
    }


// ----- Message interface --------------------------------------------------

int32_t MapEventMessage::getTypeId() const
    {
    return type_id;
    }

bool MapEventMessage::isExecuteInOrder() const
    {
    return true;
    }


// ----- PortableObject interface -------------------------------------------

void MapEventMessage::readExternal(PofReader::Handle hIn)
    {
    AbstractPofMessage::readExternal(hIn);

    setId(hIn->readInt32(0));
    // COH-8238
    int32_t implVersion = getImplVersion();
    if (implVersion > 3)
        {
        setFilterIds(hIn->readInt64Array(1));
        }
    else
        {
        setFilterId(hIn->readInt64(1));
        }
    setKey(hIn->readObject(2));
    setValueNew(hIn->readObject(3));
    setValueOld(hIn->readObject(4));
    setSynthetic(hIn->readBoolean(5));

    // COH-9355
    if (implVersion > 4)
        {
        setTransformState(hIn->readInt32(6));
        }

    // COH-10216
    if (implVersion > 5)
        {
        setTruncate(hIn->readBoolean(7));
        }

    // COH-18376
    if (implVersion > 6)
        {
        setPriming(hIn->readBoolean(8));
        }
    }

void MapEventMessage::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofMessage::writeExternal(hOut);

    hOut->writeInt32(0, getId());
    // COH-8238
    int32_t implVersion = getImplVersion();
    if (implVersion > 3)
        {
        hOut->writeInt64Array(1, getFilterIds());
        }
    else
        {
        hOut->writeInt64(1, getFilterId());
        }
    hOut->writeObject(2, getKey());
    hOut->writeObject(3, getValueNew());
    hOut->writeObject(4, getValueOld());
    hOut->writeBoolean(5, isSynthetic());

    // COH-9355
    if (implVersion > 4)
        {
        hOut->writeInt32(6, getTransformState());
        }

    // COH-10216
    if (implVersion > 5)
        {
        hOut->writeBoolean(7, isTruncate());
        }

    // ﻿COH-18376
    if (implVersion > 6)
        {
        hOut->writeBoolean(8, isPriming());
        }
    }


// ----- Describable interface ----------------------------------------------

String::View MapEventMessage::getDescription() const
    {
    Object::View vOldValue = getValueOld();
    Object::View vNewValue = getValueNew();

    return COH_TO_STRING(super::getDescription() << ", Action="    << MapEvent::getDescription(getId()) <<
           ", FilterId="  << getFilterId()       <<
           ", FilterIds=" << getFilterIds()      <<
           ", Key="       << getKey()            <<
           ", OldValue="  << (vOldValue == NULL ? "null" : COH_TO_STRING(Class::getSimpleClassName(vOldValue) << "(HashCode=" << vOldValue->hashCode() << ")")) <<
           ", NewValue="  << (vNewValue == NULL ? "null" : COH_TO_STRING(Class::getSimpleClassName(vNewValue) << "(HashCode=" << vNewValue->hashCode() << ")")) <<
           ", Synthetic=" << isSynthetic()       <<
           ", Transform=" << getTransformState() <<
           ", Truncate="  << isTruncate()        <<
           ", Priming="   << isPriming());
    }


// ----- accessors ----------------------------------------------------------

void MapEventMessage::setId(int32_t nId)
    {
    m_nId = nId;
    }

int32_t MapEventMessage::getId() const
    {
    return m_nId;
    }

void MapEventMessage::setFilterId(int64_t lFilterId)
    {
    m_lFilterId = lFilterId;
    }

int64_t MapEventMessage::getFilterId() const
    {
    return m_lFilterId;
    }

void MapEventMessage::setFilterIds(Array<int64_t>::View alFilterIds)
    {
    initialize(f_alFilterIds, alFilterIds);
    }

Array<int64_t>::View MapEventMessage::getFilterIds() const
    {
    return f_alFilterIds;
    }

void MapEventMessage::setKey(Object::View vKey)
    {
    initialize(f_vKey, vKey);
    }

Object::View MapEventMessage::getKey() const
    {
    return f_vKey;
    }

void MapEventMessage::setValueNew(Object::View vNewVal)
    {
    initialize(f_vNewValue, vNewVal);
    }

Object::View MapEventMessage::getValueNew() const
    {
    return f_vNewValue;
    }

void MapEventMessage::setValueOld(Object::View vOldVal)
    {
    initialize(f_vOldValue, vOldVal);
    }

Object::View MapEventMessage::getValueOld() const
    {
    return f_vOldValue;
    }

void MapEventMessage::setSynthetic(bool fSynthetic)
    {
    m_fSynthetic = fSynthetic;
    }

bool MapEventMessage::isSynthetic() const
    {
    return m_fSynthetic;
    }

void MapEventMessage::setTransformState(int32_t nTransformState)
    {
    m_nTransformState = nTransformState;
    }

int32_t MapEventMessage::getTransformState() const
    {
    return m_nTransformState;
    }

void MapEventMessage::setTruncate(bool fTruncate)
    {
    m_fTruncate = fTruncate;
    }

bool MapEventMessage::isTruncate() const
    {
    return m_fTruncate;
    }

void MapEventMessage::setPriming(bool fPriming)
    {
    m_fPriming = fPriming;
    }

bool MapEventMessage::isPriming() const
    {
    return m_fPriming;
    }
COH_CLOSE_NAMESPACE6

