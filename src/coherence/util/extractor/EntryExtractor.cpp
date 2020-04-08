/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/EntryExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


// ----- constructors -------------------------------------------------------

EntryExtractor::EntryExtractor()
    {
    m_nTarget = value;
    }

EntryExtractor::EntryExtractor(int32_t nTarget)
    {
    m_nTarget = nTarget;
    }


// ----- PortableObject interface -------------------------------------------

void EntryExtractor::readExternal(PofReader::Handle hIn)
    {
    m_nTarget = hIn->readInt32(0);
    }

void EntryExtractor::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0, m_nTarget);
    }

COH_CLOSE_NAMESPACE3
