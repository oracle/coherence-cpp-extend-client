/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/NotifyConnectionClosed.hpp"

#include "private/coherence/component/net/extend/PofChannel.hpp"
#include "private/coherence/component/net/extend/PofConnection.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)

using coherence::component::net::extend::PofChannel;
using coherence::component::net::extend::PofConnection;


// ----- constructors -------------------------------------------------------

NotifyConnectionClosed::NotifyConnectionClosed()
    : f_ohCause(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t NotifyConnectionClosed::getTypeId() const
    {
    return type_id;
    }

void NotifyConnectionClosed::run()
    {
    Channel::Handle hChannel0 = getChannel();
    COH_ENSURE_EQUALITY(hChannel0->getId(), 0);

    PofConnection::Handle hConnection = cast<PofConnection::Handle>(hChannel0->getConnection());
    if (hConnection != NULL)
        {
        hConnection->close(false, getCause());
        }
    }


// ----- PortableObject interface -------------------------------------------

void NotifyConnectionClosed::readExternal(PofReader::Handle hIn)
    {
    AbstractPofMessage::readExternal(hIn);

    setCause(cast<Exception::Holder>(hIn->readObject(0)));
    }

void NotifyConnectionClosed::writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPofMessage::writeExternal(hOut);

    hOut->writeObject(0, getCause());
    }


// ----- accessor methods ---------------------------------------------------

Exception::Holder NotifyConnectionClosed::getCause() const
    {
    return f_ohCause;
    }

void NotifyConnectionClosed::setCause(Exception::Holder ohCause)
    {
    initialize(f_ohCause, ohCause);
    }

COH_CLOSE_NAMESPACE5
