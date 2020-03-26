/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/NotifyChannelClosed.hpp"

#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/net/messaging/Connection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofChannel;
using coherence::net::messaging::Connection;


// ----- constructors -------------------------------------------------------

NotifyChannelClosed::NotifyChannelClosed()
    : f_ohCause(self()), m_iChannelId(0)
    {
    }


// ----- Message interface --------------------------------------------------

int32_t NotifyChannelClosed::getTypeId() const
    {
    return type_id;
    }

void NotifyChannelClosed::run()
    {
    PofChannel::Handle hChannel0 = cast<PofChannel::Handle>(getChannel());
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    Connection::Handle hConnection = hChannel0->getConnection();

    COH_ENSURE(NULL != hConnection);

    PofChannel::Handle hChannel = cast<PofChannel::Handle>(
            hConnection->getChannel(getChannelId()));

    if (NULL != hChannel)
        {
        hChannel->close(false, getCause());
        }
    }


// ----- PortableObject interface -------------------------------------------

void NotifyChannelClosed::readExternal(PofReader::Handle hIn)
    {
    AbstractPofMessage::readExternal(hIn);

    setChannelId(hIn->readInt32(0));
    setCause(cast<Exception::Holder>(hIn->readObject(1)));
    }

void NotifyChannelClosed::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofMessage::writeExternal(hOut);

    hOut->writeInt32(0, getChannelId());
    hOut->writeObject(1, getCause());
    }


// ----- Describable interface ----------------------------------------------

String::View NotifyChannelClosed::getDescription() const
    {
    return COH_TO_STRING(super::getDescription() << ", ChannelId=" << getChannelId());
    }


// ----- accessor methods ---------------------------------------------------

Exception::Holder NotifyChannelClosed::getCause() const
    {
    return f_ohCause;
    }

int32_t NotifyChannelClosed::getChannelId() const
    {
    return m_iChannelId;
    }

void NotifyChannelClosed::setCause(Exception::Holder ohCause)
    {
    initialize(f_ohCause, ohCause);
    }

void NotifyChannelClosed::setChannelId(int32_t iId)
    {
    m_iChannelId = iId;
    }

COH_CLOSE_NAMESPACE5
