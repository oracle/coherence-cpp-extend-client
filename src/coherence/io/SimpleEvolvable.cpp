/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE2(coherence,io)

using coherence::util::Binary;


// ----- constructors -------------------------------------------------------

SimpleEvolvable::SimpleEvolvable(int32_t nImplVersion)
        : m_nImplVersion(nImplVersion),
          m_nDataVersion(),
          m_vBinFutureData(self())
        {
        }

// ---- Evolvable implementation --------------------------------------------

int32_t SimpleEvolvable::getImplVersion() const
    {
    return m_nImplVersion;
    }

int32_t SimpleEvolvable::getDataVersion() const
    {
    return m_nDataVersion;
    }

void SimpleEvolvable::setDataVersion(int32_t nDataVersion)
    {
    m_nDataVersion = nDataVersion;
    }

Binary::View SimpleEvolvable::getFutureData() const
    {
    return m_vBinFutureData;
    }

void SimpleEvolvable::setFutureData(Binary::View vBinFutureData)
    {
    m_vBinFutureData = vBinFutureData;
    }

COH_CLOSE_NAMESPACE2
