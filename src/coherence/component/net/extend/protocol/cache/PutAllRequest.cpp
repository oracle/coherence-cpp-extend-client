/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/PutAllRequest.hpp"

#include "coherence/util/HashMap.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::util::HashMap;


// ----- constructors -------------------------------------------------------

PutAllRequest::PutAllRequest()
    : f_vMap(self(), NULL, /*fMutable*/ true)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t PutAllRequest::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void PutAllRequest::readExternal(PofReader::Handle hIn)
    {
    NamedCacheRequest::readExternal(hIn);

    setMap(hIn->readMap(1, HashMap::create()));
    }

void PutAllRequest::writeExternal(PofWriter::Handle hOut) const
    {
    NamedCacheRequest::writeExternal(hOut);

    hOut->writeMap(1, getMap());

    //release state
    // m_vMap = NULL;
    }


// ----- Describable interface ----------------------------------------------

String::View PutAllRequest::getDescription() const
    {
    Map::View vMap = getMap();

    String::View vMapDesc = COH_TO_STRING((vMap == NULL ? "null" : COH_TO_STRING("Size=" << vMap->size() << ", HashCode=" << vMap->hashCode())));
    
    return COH_TO_STRING(super::getDescription() << ", Map=(" << vMapDesc << ")");
    }


// ----- internal methods ---------------------------------------------------

void PutAllRequest::onRun(AbstractPofResponse::Handle /*hResponse*/)
    {
    // no-op: this Request is only executed within the cluster
    }


// ----- accessors ----------------------------------------------------------

Map::View PutAllRequest::getMap() const
    {
    return f_vMap;
    }

void PutAllRequest::setMap(Map::View vMap)
    {
    initialize(f_vMap, vMap);
    }

COH_CLOSE_NAMESPACE6
