/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/ProxyControlInvocable.hpp"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1600, ProxyControlInvocable);

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

// ----- constructors -------------------------------------------------------

ProxyControlInvocable::ProxyControlInvocable()
        : f_vsService(self(), NULL),
          m_nOperation(0)
    {
    }

ProxyControlInvocable::ProxyControlInvocable(String::View vsService, int32_t nOperation)
        : f_vsService(self(), vsService),
          m_nOperation(nOperation)
    {
    }

// ----- PortableObject interface -------------------------------------------

void ProxyControlInvocable::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vsService, hIn->readString(0));
    m_nOperation = hIn->readInt32(1);
    }

void ProxyControlInvocable::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeString(0, f_vsService);
    hOut->writeInt32(1, m_nOperation);
    }

COH_CLOSE_NAMESPACE2
