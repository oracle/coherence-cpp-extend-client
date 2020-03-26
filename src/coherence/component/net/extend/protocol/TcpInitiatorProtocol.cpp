/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/TcpInitiatorProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/TcpInitiatorMessageFactory.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constants ----------------------------------------------------------

TcpInitiatorProtocol::Handle TcpInitiatorProtocol::getInstance()
    {
    static FinalHandle<TcpInitiatorProtocol> hProtocol
            (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(TcpInitiatorProtocol::getInstance());


// ----- constructors -------------------------------------------------------

TcpInitiatorProtocol::TcpInitiatorProtocol()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle TcpInitiatorProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return TcpInitiatorMessageFactory::create();
    }

COH_CLOSE_NAMESPACE5
