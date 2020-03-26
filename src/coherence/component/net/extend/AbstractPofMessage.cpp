/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)


// ----- constructors -------------------------------------------------------

AbstractPofMessage::AbstractPofMessage()
    : f_hChannel(self()),
      m_nDataVersion(0),
      m_nImplVersion(0),
      f_vBinFuture(self())
    {
    }


// ----- Message interface --------------------------------------------------

Channel::Handle AbstractPofMessage::getChannel()
    {
    return f_hChannel;
    }

Channel::View AbstractPofMessage::getChannel() const
    {
    return f_hChannel;
    }

void AbstractPofMessage::setChannel(Channel::Handle hChannel)
    {
    COH_ENSURE(NULL != hChannel);
    initialize(f_hChannel, hChannel);
    }

bool AbstractPofMessage::isExecuteInOrder() const
    {
    return false;
    }

bool AbstractPofMessage::isSolicited() const
    {
    return false;
    }


// ----- EvolvablePortableObject interface ----------------------------------

int32_t AbstractPofMessage::getImplVersion() const
    {
    return m_nImplVersion;
    }

int32_t AbstractPofMessage::getDataVersion() const
    {
    return m_nDataVersion;
    }

void AbstractPofMessage::setDataVersion(int32_t nVersion)
    {
    m_nDataVersion = nVersion;
    }

Binary::View AbstractPofMessage::getFutureData() const
    {
    return f_vBinFuture;
    }

void AbstractPofMessage::setFutureData(Binary::View vBinFuture)
    {
    initialize(f_vBinFuture, vBinFuture);
    }

void AbstractPofMessage::readExternal(PofReader::Handle /*hIn*/)
    {
    }

void AbstractPofMessage::writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPofMessage::getDescription() const
    {
    String::View vs = COH_TO_STRING(super::getDescription() << "Type=" << getTypeId() << ", Channel=");

    Channel::View vChannel = getChannel();
    if (NULL == vChannel)
        {
        vs = COH_TO_STRING(vs << "NULL");
        }
    else
        {
        vs = COH_TO_STRING(vs << vChannel->getId());
        }

    return vs;
    }


// ----- accessors ----------------------------------------------------------

void AbstractPofMessage::setImplVersion(int32_t nVersion)
    {
    m_nImplVersion = nVersion;
    }

COH_CLOSE_NAMESPACE4
