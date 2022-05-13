/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/cache/NamedCacheFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)


// ----- constants ----------------------------------------------------------

NamedCacheProtocol::Handle NamedCacheProtocol::getInstance()
    {
    static FinalHandle<NamedCacheProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(NamedCacheProtocol::getInstance());


// ----- constructors -------------------------------------------------------

NamedCacheProtocol::NamedCacheProtocol()
    {
    }


// ----- Protocol interface -------------------------------------------------

String::View NamedCacheProtocol::getName() const
    {
    return "NamedCacheProtocol";
    }

int32_t NamedCacheProtocol::getCurrentVersion() const
    {
    return 10;
    }

int32_t NamedCacheProtocol::getSupportedVersion() const
    {
    return 2;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle NamedCacheProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return NamedCacheFactory::create();
    }

COH_CLOSE_NAMESPACE6
