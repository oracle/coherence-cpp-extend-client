/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/CloseChannel.hpp"

#include "private/coherence/component/net/extend/protocol/PeerResponse.hpp"
#include "private/coherence/component/util/Peer.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::util::Peer;


// ----- constructors -------------------------------------------------------

CloseChannel::CloseChannel()
    : f_oheCause(self()), f_hChannelClose(self()), m_fNotify(false)
    {
    }


// ----- AbstractPofRequest interface ---------------------------------------

AbstractPofResponse::Handle CloseChannel::instantiateResponse(
        Protocol::MessageFactory::View vFactory) const
    {
    return cast<PeerResponse::Handle>(
            vFactory->createMessage(PeerResponse::type_id));
    }

void CloseChannel::onRun(AbstractPofResponse::Handle)
    {
    getChannelClose()->closeInternal(isNotify(), getCause(), 0L);
    }


// ----- Message interface --------------------------------------------------

int32_t CloseChannel::getTypeId() const
    {
    return type_id;
    }


// ----- accessor methods ---------------------------------------------------

Exception::Holder CloseChannel::getCause() const
    {
    return f_oheCause;
    }

PofChannel::View CloseChannel::getChannelClose() const
    {
    return f_hChannelClose;
    }

PofChannel::Handle CloseChannel::getChannelClose()
    {
    return f_hChannelClose;
    }

bool CloseChannel::isNotify() const
    {
    return m_fNotify;
    }

void CloseChannel::setCause(Exception::Holder oheCause)
    {
    initialize(f_oheCause, oheCause);
    }

void CloseChannel::setChannelClose(PofChannel::Handle hChannel)
    {
    initialize(f_hChannelClose, hChannel);
    }

void CloseChannel::setNotify(bool fNotify)
    {
    m_fNotify = fNotify;
    }

COH_CLOSE_NAMESPACE5
