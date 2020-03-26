/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"

#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/ReadOnlyArrayList.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::io::pof::PofHelper;
using coherence::util::ArrayList;
using coherence::util::Map;
using coherence::util::ReadOnlyArrayList;


// ----- constructors -------------------------------------------------------

AbstractPofResponse::AbstractPofResponse()
    : m_fFailure(false),
      m_lRequestId(0),
      m_ohResult(self()),
      m_nResultFormat(fmt_generic)
    {
    }


// ----- Response interface -------------------------------------------------

int64_t AbstractPofResponse::getRequestId() const
    {
    return m_lRequestId;
    }

void AbstractPofResponse::setRequestId(int64_t lId)
    {
    m_lRequestId = lId;
    }

bool AbstractPofResponse::isFailure() const
    {
    return m_fFailure;
    }

void AbstractPofResponse::setFailure(bool fFailure)
    {
    m_fFailure = fFailure;
    }

Object::Holder AbstractPofResponse::getResult() const
    {
    return m_ohResult;
    }

void AbstractPofResponse::setResult(Object::Holder ohResult)
    {
    m_ohResult      = ohResult;
    m_nResultFormat = fmt_generic;
    }

// ----- Message interface --------------------------------------------------

bool AbstractPofResponse::isSolicited() const
    {
    return true;
    }

// ----- PortableObject interface -------------------------------------------

void AbstractPofResponse::readExternal(PofReader::Handle hIn)
    {
    AbstractPofMessage::readExternal(hIn);

    setRequestId(hIn->readInt64(0));
    setFailure(hIn->readBoolean(1));

    // determine which result format is being used
    switch (m_nResultFormat = hIn->readInt32(2))
        {
        default:
        case fmt_generic:
            setResult(hIn->readObject(3));
            break;

        case fmt_collection:
            {
            Collection::Handle hCol = ArrayList::create();
            hIn->readCollection(4, hCol);
            setResult(ReadOnlyArrayList::create(hCol->toArray()));
            }
            break;

        case fmt_map:
            {
            Map::Handle hMap = PofHelper::ReadableEntrySetMap::create();
            hIn->readMap(5, hMap);
            setResult(hMap->entrySet());
            }
            break;
        }
    }

void AbstractPofResponse::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofMessage::writeExternal(hOut);

    hOut->writeInt64(0, getRequestId());
    hOut->writeBoolean(1, isFailure());

    int32_t nFormat = m_nResultFormat;
    hOut->writeInt32(2, nFormat);
    switch (nFormat)
        {
        default:
        case fmt_generic:
            hOut->writeObject(3, getResult());
            break;

        case fmt_collection:
            hOut->writeCollection(4, cast<Collection::View>(getResult()));
            break;

        case fmt_map:
            hOut->writeMap(5, cast<Map::View>(getResult()));
            break;
        }
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPofResponse::getDescription() const
    {
    String::View vs = COH_TO_STRING(super::getDescription() << ", RequestId=" << getRequestId());
    if (isFailure())
        {
        vs = COH_TO_STRING(vs << ", Failure=");
        vs = COH_TO_STRING(vs << getResult());
        }
    else
        {
        vs = COH_TO_STRING(vs << ", Result=");
        Object::View vResult = getResult();
        vs = COH_TO_STRING(vs << (vResult == NULL ? "null" : COH_TO_STRING(Class::getSimpleClassName(vResult) << "(HashCode=" << vResult->hashCode() << ")")));
        }
    return vs;
    }

// ----- accessors ----------------------------------------------------------

void AbstractPofResponse::setResultAsCollection(Collection::Handle hCol)
    {
    setResult(hCol);
    m_nResultFormat = fmt_collection;
    }

void AbstractPofResponse::setResultAsEntrySet(Set::Handle hSet)
    {
    setResult(PofHelper::WriteableEntrySetMap::create(hSet));
    m_nResultFormat = fmt_map;
    }

COH_CLOSE_NAMESPACE4
