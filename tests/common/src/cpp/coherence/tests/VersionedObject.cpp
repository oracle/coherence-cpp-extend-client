/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/tests/VersionedObject.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

COH_REGISTER_PORTABLE_CLASS(1501, VersionedObject);

// ----- constructors -------------------------------------------------------

VersionedObject::VersionedObject(int64_t lVersion)
        : m_lVersion( lVersion )
    {
    }

    // ----- Versionable interface ------------------------------------------

Comparable::View VersionedObject::getVersionIndicator() const
    {
    return Integer64::create(m_lVersion);
    }

void VersionedObject::incrementVersion()
    {
    m_lVersion += m_lVersion;
    }


// ----- PortableObject interface -------------------------------------------

void VersionedObject::readExternal(PofReader::Handle hIn)
    {
    m_lVersion = hIn->readInt64(0);
    }

void VersionedObject::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt64(0, m_lVersion);
    }

// ----- accessors ----------------------------------------------------------

int64_t VersionedObject::getVersion() const
    {
    return m_lVersion;
    }

COH_CLOSE_NAMESPACE2
