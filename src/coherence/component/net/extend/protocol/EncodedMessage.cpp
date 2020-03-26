/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/protocol/EncodedMessage.hpp"

COH_OPEN_NAMESPACE5(coherence,component,net,extend,protocol)


// ----- constructors -------------------------------------------------------

EncodedMessage::EncodedMessage()
    : f_hConnection(self()), f_vReadBuffer(self())
    {
    }


// ----- Message interface --------------------------------------------------

int32_t EncodedMessage::getTypeId() const
    {
    return type_id;
    }

void EncodedMessage::run()
    {
    // no-op
    }

// ----- accessor methods ---------------------------------------------------

PofConnection::View EncodedMessage::getConnection() const
    {
    return f_hConnection;
    }

PofConnection::Handle EncodedMessage::getConnection()
    {
    return f_hConnection;
    }

ReadBuffer::View EncodedMessage::getReadBuffer() const
    {
    return f_vReadBuffer;
    }

ReadBuffer::View EncodedMessage::getReadBuffer()
    {
    return f_vReadBuffer;
    }

void EncodedMessage::setConnection(PofConnection::Handle hConnection)
    {
    initialize(f_hConnection, hConnection);
    }

void EncodedMessage::setReadBuffer(ReadBuffer::View vMessage)
    {
    initialize(f_vReadBuffer, vMessage);
    }

COH_CLOSE_NAMESPACE5
