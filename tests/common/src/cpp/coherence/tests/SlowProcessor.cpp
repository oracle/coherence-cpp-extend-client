/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/SlowProcessor.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

COH_REGISTER_PORTABLE_CLASS(3012, SlowProcessor);


// ----- constructors -------------------------------------------------------

SlowProcessor::SlowProcessor()
        : f_vsReturnValue(self())
    {
    }

SlowProcessor::SlowProcessor(int32_t nProcessTimeMillis, String::View vsReturnValue)
        : f_vsReturnValue(self(), vsReturnValue)
    {
    m_nProcessTimeMillis = nProcessTimeMillis;
    }


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder SlowProcessor::process(InvocableMap::Entry::Handle /* hEntry */) const
    {
    return f_vsReturnValue;
    }

// ----- PortableObject interface -------------------------------------------

void SlowProcessor::readExternal(PofReader::Handle hIn)
    {
    m_nProcessTimeMillis = hIn->readInt32(0);
    initialize(f_vsReturnValue, hIn->readString(1));
    }

void SlowProcessor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, m_nProcessTimeMillis);
    hOut->writeString(1, f_vsReturnValue);
    }

COH_CLOSE_NAMESPACE2
