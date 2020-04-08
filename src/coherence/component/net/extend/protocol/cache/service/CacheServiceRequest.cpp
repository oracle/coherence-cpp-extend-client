/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceRequest.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)


// ----- constructors -------------------------------------------------------

CacheServiceRequest::CacheServiceRequest()
        : f_vsCacheName(self())
    {
    }


// ----- PortableObject interface -------------------------------------------

void CacheServiceRequest::readExternal(PofReader::Handle hIn)
    {
    AbstractPofRequest::readExternal(hIn);

    setCacheName(hIn->readString(1));
    }

void CacheServiceRequest::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofRequest::writeExternal(hOut);

    hOut->writeString(1, getCacheName());
    }


// ----- Describable interface ----------------------------------------------

String::View CacheServiceRequest::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", CacheName=" << getCacheName());
    }


// ----- accessors ----------------------------------------------------------

String::View CacheServiceRequest::getCacheName() const
    {
    return f_vsCacheName;
    }

void CacheServiceRequest::setCacheName(String::View vsName)
    {
    initialize(f_vsCacheName, vsName);
    }

COH_CLOSE_NAMESPACE7
