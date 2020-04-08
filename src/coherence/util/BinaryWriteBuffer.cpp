/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/BinaryWriteBuffer.hpp"

#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

BinaryWriteBuffer::BinaryWriteBuffer(size32_t cbCap)
    : super(cbCap)
    {
    }

BinaryWriteBuffer::BinaryWriteBuffer(size32_t cbCap, size32_t cbMax)
    : super(cbCap, cbMax)
    {
    }


// ----- WriteBuffer interface ----------------------------------------------

Binary::View BinaryWriteBuffer::toBinary() const
    {
    return Binary::create(this);
    }


// ----- internal -----------------------------------------------------------

Array<octet_t>::View BinaryWriteBuffer::getInternalOctetArray() const
    {
    m_fReadOnly = true;
    return m_hab;
    }

void BinaryWriteBuffer::checkBounds(size32_t of, size32_t cb)
    {
    if (m_fReadOnly)
        {
        COH_THROW(IllegalStateException::create("WriteBuffer is immutable"));
        }
    super::checkBounds(of, cb);
    }

COH_CLOSE_NAMESPACE2
