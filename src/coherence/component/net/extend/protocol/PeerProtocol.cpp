/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/PeerProtocol.hpp"

#include "private/coherence/component/net/extend/protocol/PeerMessageFactory.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constants ----------------------------------------------------------

PeerProtocol::Handle PeerProtocol::getInstance()
    {
    static FinalHandle<PeerProtocol> hProtocol
        (System::common(), create());
    return hProtocol;
    }
COH_STATIC_INIT(PeerProtocol::getInstance());


// ----- constructors -------------------------------------------------------

PeerProtocol::PeerProtocol()
    {
    }


// ----- Protocol interface -------------------------------------------------

String::View PeerProtocol::getName() const
    {
    return getProtocolName();
    }

int32_t PeerProtocol::getCurrentVersion() const
    {
    return 3;
    }

int32_t PeerProtocol::getSupportedVersion() const
    {
    return 2;
    }


// ----- internal methods ---------------------------------------------------

AbstractPofMessageFactory::Handle PeerProtocol::instantiateMessageFactory(
        int32_t /*nVersion*/) const
    {
    return PeerMessageFactory::create();
    }

String::View PeerProtocol::getProtocolName()
    {
    static FinalView<String>
        vProtocolName(System::common(), String::create("MessagingProtocol"));
    return vProtocolName;
    }
COH_STATIC_INIT(PeerProtocol::getProtocolName());

COH_CLOSE_NAMESPACE5
