/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPofMessageFactory.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)


// ----- constructors -------------------------------------------------------

AbstractPofMessageFactory::AbstractPofMessageFactory()
    : f_vProtocol(self())
    {
    }


// ----- MessageFactory interface -------------------------------------------

Protocol::View AbstractPofMessageFactory::getProtocol() const
    {
    return f_vProtocol;
    }

int32_t AbstractPofMessageFactory::getVersion() const
    {
    return m_nVersion;
    }

Message::Handle AbstractPofMessageFactory::createMessage(int32_t nType) const
    {
    AbstractPofMessage::Handle hMessage = instantiateMessage(nType);

    // set the Message version
    hMessage->setImplVersion(getVersion());

    return hMessage;
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPofMessageFactory::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << "Protocol=" << getProtocol() << ", Version=" << getVersion());
    }


// ----- accessors ----------------------------------------------------------

void AbstractPofMessageFactory::setProtocol(Protocol::View vProtocol)
    {
    initialize(f_vProtocol, vProtocol);
    }

void AbstractPofMessageFactory::setVersion(int32_t nVersion)
    {
    m_nVersion = nVersion;
    }

COH_CLOSE_NAMESPACE4
