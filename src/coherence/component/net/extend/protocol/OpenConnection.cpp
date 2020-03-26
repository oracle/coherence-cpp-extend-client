/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/OpenConnection.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

OpenConnection::OpenConnection()
    : f_hConnectionOpen(self()), f_vSubject(self()), f_vIdentityToken(self())
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle OpenConnection::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }


// ----- Message interface --------------------------------------------------

int32_t OpenConnection::getTypeId() const
    {
    return type_id;
    }

// ----- accessor methods ---------------------------------------------------

PofConnection::View OpenConnection::getConnectionOpen() const
    {
    return f_hConnectionOpen;
    }

PofConnection::Handle OpenConnection::getConnectionOpen()
    {
    return f_hConnectionOpen;
    }

void OpenConnection::setConnectionOpen(PofConnection::Handle hConnection)
    {
    initialize(f_hConnectionOpen, hConnection);
    }

Subject::View OpenConnection::getSubject() const
    {
    return f_vSubject;
    }

Array<octet_t>::View OpenConnection::getIdentityToken() const
    {
    return f_vIdentityToken;
    }

void OpenConnection::setSubject(Subject::View vSubject)
    {
    initialize(f_vSubject, vSubject);
    }

void OpenConnection::setIdentityToken(Array<octet_t>::View vToken)
    {
    initialize(f_vIdentityToken, vToken);
    }

COH_CLOSE_NAMESPACE5
