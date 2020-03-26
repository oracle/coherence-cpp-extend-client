/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SOLARIS_ATOMIC32_HPP
#define COH_SOLARIS_ATOMIC32_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/native/NativeAtomic32.hpp"

#include <atomic.h>

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic32 interface -------------------------------------------

int32_t NativeAtomic32::get() const
    {
    membar_consumer();
    return m_nAtomic;
    }

void NativeAtomic32::set(int32_t nValue)
    {
    m_nAtomic = nValue;
    membar_producer();
    }

int32_t NativeAtomic32::update(int32_t nAssume, int32_t nValue)
    {
    COH_ALIGN(4, uint32_t, nAssumeAligned) = (uint32_t) nAssume;
    COH_ALIGN(4, uint32_t, nValueAligned ) = (uint32_t) nValue;

    return atomic_cas_32((uint32_t*) &m_nAtomic, nAssumeAligned, nValueAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_SOLARIS_ATOMIC32_HPP
