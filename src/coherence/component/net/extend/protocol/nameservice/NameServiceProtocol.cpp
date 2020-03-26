/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/nameservice/NameServiceFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,nameservice)

// ----- constants ----------------------------------------------------------

NameServiceProtocol::Handle NameServiceProtocol::getInstance()
    {
    static FinalHandle<NameServiceProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(NameServiceProtocol::getInstance());

// ----- constructors -------------------------------------------------------

NameServiceProtocol::NameServiceProtocol()
    {
    }

// ----- Protocol interface -------------------------------------------------

String::View NameServiceProtocol::getName() const
    {
    return "NameServiceProtocol";
    }

int32_t NameServiceProtocol::getCurrentVersion() const
    {
    return 1;
    }

int32_t NameServiceProtocol::getSupportedVersion() const
    {
    return 1;
    }

// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle NameServiceProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return NameServiceFactory::create();
    }

COH_CLOSE_NAMESPACE6
