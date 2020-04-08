/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/TcpInitiatorOpenConnectionResponse.hpp"

#include "coherence/util/ArrayList.hpp"

#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/component/util/TcpInitiator.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofChannel;
using coherence::component::util::TcpInitiator;
using coherence::util::ArrayList;


// ----- constructors -------------------------------------------------------

TcpInitiatorOpenConnectionResponse::TcpInitiatorOpenConnectionResponse()
    : m_fRedirect(false),
      f_vListRedirect(self(), (List::View)NULL)
    {
    m_nImplVersion = 1;
    }


// ----- Runnable interface -------------------------------------------------

void TcpInitiatorOpenConnectionResponse::run()
    {
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel());

    // update the connection with redirection information
    TcpInitiator::TcpConnection::Handle hConnection =
            cast<TcpInitiator::TcpConnection::Handle>(hChannel0->getConnection());
    hConnection->setRedirect(isRedirect());
    hConnection->setRedirectList(getRedirectList());

    super::run();
    }


// ----- PortableObject implementation ----------------------------------

void TcpInitiatorOpenConnectionResponse::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);

    setRedirect(hIn->readBoolean(10));
    if (isRedirect())
        {
        setRedirectList(cast<List::View>(hIn->readCollection(11, ArrayList::create())));
        }
    }

void TcpInitiatorOpenConnectionResponse::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    hOut->writeBoolean(10, isRedirect());
    if (isRedirect())
        {
        hOut->writeCollection(11, getRedirectList());
        }
    }


// ----- accessor methods -----------------------------------------------

bool TcpInitiatorOpenConnectionResponse::isRedirect() const
    {
    return m_fRedirect;
    }

void TcpInitiatorOpenConnectionResponse::setRedirect(bool fRedirect)
    {
    m_fRedirect = fRedirect;
    }

List::View TcpInitiatorOpenConnectionResponse::getRedirectList() const
    {
    return f_vListRedirect;
    }

void TcpInitiatorOpenConnectionResponse::setRedirectList(List::View vList)
    {
    initialize(f_vListRedirect, vList);
    }

COH_CLOSE_NAMESPACE5
