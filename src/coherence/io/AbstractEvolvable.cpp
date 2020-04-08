/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/AbstractEvolvable.hpp"

COH_OPEN_NAMESPACE2(coherence,io)
COH_REGISTER_CLASS(TypedBarrenClass<AbstractEvolvable>::create());

// ----- constructors -------------------------------------------------------

AbstractEvolvable::AbstractEvolvable()
    : f_vBinFuture(self())
    {
    }

AbstractEvolvable::AbstractEvolvable(const AbstractEvolvable& that)
    : m_nDataVersion(that.m_nDataVersion), f_vBinFuture(self(), that.f_vBinFuture)
    {
    }


// ----- Evolvable interface ------------------------------------------------

int32_t AbstractEvolvable::getDataVersion() const
    {
    return m_nDataVersion;
    }

void AbstractEvolvable::setDataVersion(int32_t nVersion)
    {
    m_nDataVersion = nVersion;
    }

Binary::View AbstractEvolvable::getFutureData() const
    {
    return f_vBinFuture;
    }

void AbstractEvolvable::setFutureData(Binary::View binFuture)
    {
    initialize(f_vBinFuture, binFuture);
    }

COH_CLOSE_NAMESPACE2
