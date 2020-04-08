/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/CloseConnection.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

CloseConnection::CloseConnection()
    : f_oheCause(self()), f_hConnectionClose(self()), m_fNotify(false)
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle CloseConnection::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }

void CloseConnection::onRun(AbstractPofResponse::Handle)
    {
    getConnectionClose()->closeInternal(isNotify(), getCause(), 0L);
    }

// ----- Message interface --------------------------------------------------

int32_t CloseConnection::getTypeId() const
    {
    return type_id;
    }


// ----- accessor methods ---------------------------------------------------

Exception::Holder CloseConnection::getCause() const
    {
    return f_oheCause;
    }

PofConnection::View CloseConnection::getConnectionClose() const
    {
    return f_hConnectionClose;
    }

PofConnection::Handle CloseConnection::getConnectionClose()
    {
    return f_hConnectionClose;
    }

bool CloseConnection::isNotify() const
    {
    return m_fNotify;
    }

void CloseConnection::setCause(Exception::Holder oheCause)
    {
    initialize(f_oheCause, oheCause);
    }

void CloseConnection::setConnectionClose(PofConnection::Handle hConnection)
    {
    initialize(f_hConnectionClose, hConnection);
    }

void CloseConnection::setNotify(bool fNotify)
    {
    m_fNotify = fNotify;
    }

COH_CLOSE_NAMESPACE5
