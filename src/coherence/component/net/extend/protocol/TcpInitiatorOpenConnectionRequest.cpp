/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/TcpInitiatorOpenConnectionRequest.hpp"

#include "private/coherence/component/util/TcpInitiator.hpp"
#include "private/coherence/component/net/extend/protocol/TcpInitiatorOpenConnectionResponse.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::TcpInitiator;


// ----- constructors -------------------------------------------------------

TcpInitiatorOpenConnectionRequest::TcpInitiatorOpenConnectionRequest()
    {
    }


// ----- PortableObject implementation --------------------------------------

void TcpInitiatorOpenConnectionRequest::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);

    if (hIn->readBoolean(10)) /* redirect supported? */
        {
        setRedirect(hIn->readBoolean(11));
        }        
    }

void TcpInitiatorOpenConnectionRequest::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    hOut->writeBoolean(10, isRedirectSupported());
    hOut->writeBoolean(11, isRedirect());
    }


// ----- accessor methods ---------------------------------------------------

void TcpInitiatorOpenConnectionRequest::setConnectionOpen(PofConnection::Handle hConnection)
    {
    setRedirect(cast<TcpInitiator::TcpConnection::Handle>(hConnection)->isRedirect());
    super::setConnectionOpen(hConnection);
    }

bool TcpInitiatorOpenConnectionRequest::isRedirectSupported() const
    {
    return true;
    }

bool TcpInitiatorOpenConnectionRequest::isRedirect() const
    {
    return m_fRedirect;
    }

void TcpInitiatorOpenConnectionRequest::setRedirect(bool fRedirect)
    {
    m_fRedirect = fRedirect;
    }

COH_CLOSE_NAMESPACE5
