/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/invocation/InvocationServiceFactory.hpp"

COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,invocation)


// ----- constants ----------------------------------------------------------

InvocationServiceProtocol::Handle InvocationServiceProtocol::getInstance()
    {
    static FinalHandle<InvocationServiceProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(InvocationServiceProtocol::getInstance());


// ----- constructors -------------------------------------------------------

InvocationServiceProtocol::InvocationServiceProtocol()
    {
    }


// ----- Protocol interface -------------------------------------------------

String::View InvocationServiceProtocol::getName() const
    {
    return "InvocationServiceProtocol";
    }

int32_t InvocationServiceProtocol::getCurrentVersion() const
    {
    return 1;
    }

int32_t InvocationServiceProtocol::getSupportedVersion() const
    {
    return 1;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle InvocationServiceProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return InvocationServiceFactory::create();
    }

COH_CLOSE_NAMESPACE6
