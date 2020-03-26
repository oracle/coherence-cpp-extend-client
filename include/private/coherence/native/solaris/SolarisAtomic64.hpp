/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOLARIS_ATOMIC64_HPP
#define COH_SOLARIS_ATOMIC64_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/native/NativeAtomic64.hpp"

#include <atomic.h>

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic64 interface -------------------------------------------

int64_t NativeAtomic64::get() const
    {
    membar_consumer();
    return m_lAtomic;
    }

void NativeAtomic64::set(int64_t lValue)
    {
    m_lAtomic = lValue;
    membar_producer();
    }

int64_t NativeAtomic64::update(int64_t lAssume, int64_t lValue)
    {
    COH_ALIGN(8, uint64_t, lAssumeAligned) = (uint64_t) lAssume;
    COH_ALIGN(8, uint64_t, lValueAligned ) = (uint64_t) lValue;

    return atomic_cas_64((uint64_t*) &m_lAtomic, lAssumeAligned, lValueAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_SOLARIS_ATOMIC64_HPP
