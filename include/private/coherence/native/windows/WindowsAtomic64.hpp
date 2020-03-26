/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_ATOMIC64_HPP
#define COH_WINDOWS_ATOMIC64_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/native/NativeAtomic64.hpp"

#include <intrin.h>
#include <winbase.h>

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic64 interface -------------------------------------------

int64_t NativeAtomic64::get() const
    {
    _ReadBarrier();
    return m_lAtomic;
    }

void NativeAtomic64::set(int64_t lValue)
    {
    m_lAtomic = lValue;
    _WriteBarrier();
    }

int64_t NativeAtomic64::update(int64_t lAssume, int64_t lValue)
    {
    COH_ALIGN(8, LONGLONG, lAssumeAligned) = lAssume;
    COH_ALIGN(8, LONGLONG, lValueAligned ) = lValue;

    return _InterlockedCompareExchange64((LONGLONG*) &m_lAtomic, lValueAligned, lAssumeAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_ATOMIC64_HPP
