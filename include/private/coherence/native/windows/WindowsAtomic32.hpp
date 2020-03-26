/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_ATOMIC32_HPP
#define COH_WINDOWS_ATOMIC32_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/native/NativeAtomic32.hpp"

#include <intrin.h>
#include <winbase.h>

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic32 interface -------------------------------------------

int32_t NativeAtomic32::get() const
    {
    _ReadBarrier();
    return m_nAtomic;
    }

void NativeAtomic32::set(int32_t nValue)
    {
    m_nAtomic = nValue;
    _WriteBarrier();
    }

int32_t NativeAtomic32::update(int32_t nAssume, int32_t nValue)
    {
    COH_ALIGN(4, LONG, nAssumeAligned) = nAssume;
    COH_ALIGN(4, LONG, nValueAligned ) = nValue;

    return _InterlockedCompareExchange((LONG*) &m_nAtomic, nValueAligned, nAssumeAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_ATOMIC32_HPP
