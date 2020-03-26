/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenConnectionResponse.hpp"

#include "private/coherence/component/net/extend/protocol/OpenConnectionRequest.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

OpenConnectionResponse::OpenConnectionResponse()
    : f_vProtocolVersionMap(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t OpenConnectionResponse::getTypeId() const
    {
    return type_id;
    }


// ----- PortableObject interface -------------------------------------------

void OpenConnectionResponse::readExternal(PofReader::Handle hIn)
    {
    AbstractPofResponse::readExternal(hIn);

    setProtocolVersionMap(hIn->readMap(6, NULL));
    }

void OpenConnectionResponse::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofResponse::writeExternal(hOut);

    ClassLoader::View vLoader = SystemClassLoader::getInstance();
    hOut->writeMap(6, getProtocolVersionMap(), 
            vLoader->loadByType(typeid(String)),
            vLoader->loadByType(typeid(Integer32)));
    }


// ----- Describable interface ----------------------------------------------

String::View OpenConnectionResponse::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", ProtocolVersionMap=" << getProtocolVersionMap());
    }


// ----- accessors ------------------------------------------------------

Map::View OpenConnectionResponse::getProtocolVersionMap() const
    {
    return f_vProtocolVersionMap;
    }

void OpenConnectionResponse::setProtocolVersionMap(Map::View vMap)
    {
    initialize(f_vProtocolVersionMap, vMap);
    }

COH_CLOSE_NAMESPACE5
