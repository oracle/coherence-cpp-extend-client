/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GCC_ATOMIC32
#define COH_GCC_ATOMIC32

#include "coherence/lang/compatibility.hpp"
#include "coherence/native/NativeAtomic32.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic32 interface -------------------------------------------

int32_t NativeAtomic32::get() const
    {
    // We need a fence here to ensure the value we load is up to date
    // This guarantees that all store's prior to this load are
    // completed.
    __sync_synchronize();
    return m_nAtomic;
    }

void NativeAtomic32::set(int32_t nValue)
    {
    m_nAtomic = nValue;
    // This fence will guarantee that the above store is committed
    // before any other store.
    __sync_synchronize();
    }

int32_t NativeAtomic32::update(int32_t nAssume, int32_t nValue)
    {
    COH_ALIGN(4, int32_t, nAssumeAligned) = nAssume;
    COH_ALIGN(4, int32_t, nValueAligned ) = nValue;

    return __sync_val_compare_and_swap(&m_nAtomic, nAssumeAligned, nValueAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GCC_ATOMIC32
