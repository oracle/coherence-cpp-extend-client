/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/IncrementorInvocable.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1500, IncrementorInvocable);


// ----- constructors -------------------------------------------------------

IncrementorInvocable::IncrementorInvocable()
    : m_n(0)
    {
    }


IncrementorInvocable::IncrementorInvocable(int32_t n)
        : m_n(n)
    {
    }

// ----- PortableObject interface -------------------------------------------

void IncrementorInvocable::readExternal(PofReader::Handle hIn)
    {
    m_n = hIn->readInt32(0);
    }

void IncrementorInvocable::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, m_n);
    }

COH_CLOSE_NAMESPACE2
