/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/cache/service/CacheServiceFactory.hpp"

COH_OPEN_NAMESPACE7(coherence,component,net,extend,protocol,cache,service)


// ----- constants ----------------------------------------------------------

CacheServiceProtocol::Handle CacheServiceProtocol::getInstance()
    {
    static FinalHandle<CacheServiceProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(CacheServiceProtocol::getInstance());


// ----- constructors -------------------------------------------------------

CacheServiceProtocol::CacheServiceProtocol()
    {
    }


// ----- Protocol interface -------------------------------------------------

String::View CacheServiceProtocol::getName() const
    {
    return "CacheServiceProtocol";
    }

int32_t CacheServiceProtocol::getCurrentVersion() const
    {
    return 1;
    }

int32_t CacheServiceProtocol::getSupportedVersion() const
    {
    return 1;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle CacheServiceProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return CacheServiceFactory::create();
    }

COH_CLOSE_NAMESPACE7
