/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GCC_ATOMIC64
#define COH_GCC_ATOMIC64

#include "coherence/lang/compatibility.hpp"
#include "coherence/native/NativeAtomic64.hpp"

COH_OPEN_NAMESPACE2(coherence,native)

// ----- NativeAtomic64 interface -------------------------------------------

int64_t NativeAtomic64::get() const
    {
    // We need a fence here to ensure the value we load is up to date
    // This guarantees that all store's prior to this load are
    // completed.
    __sync_synchronize();
    return m_lAtomic;
    }

void NativeAtomic64::set(int64_t lValue)
    {
    m_lAtomic = lValue;
    // This fence will guarantee that the above store is committed
    // before any other store.
    __sync_synchronize();
    }

int64_t NativeAtomic64::update(int64_t lAssume, int64_t lValue)
    {
    COH_ALIGN(8, int64_t, lAssumeAligned) = lAssume;
    COH_ALIGN(8, int64_t, lValueAligned ) = lValue;

    return __sync_val_compare_and_swap(&m_lAtomic, lAssumeAligned, lValueAligned);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GCC_ATOMIC64
