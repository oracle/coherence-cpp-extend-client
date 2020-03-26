/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/InitiatorProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/InitiatorMessageFactory.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constants ----------------------------------------------------------

InitiatorProtocol::Handle InitiatorProtocol::getInstance()
    {
    static FinalHandle<InitiatorProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(InitiatorProtocol::getInstance());


// ----- constructors -------------------------------------------------------

InitiatorProtocol::InitiatorProtocol()
    {
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle InitiatorProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return InitiatorMessageFactory::create();
    }

COH_CLOSE_NAMESPACE5
