/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_UNSAFE_ATOMIC_WORD_HPP
#define COH_UNSAFE_ATOMIC_WORD_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic64.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* An unsafe implemenation of NativeAtomic64. This implementation is largely
* for testing purposes, though it would also be suitable for a
* single-threaded build of the Coherence library.
*
* @author mf 2008.02.22
*/


// ----- NativeAtomic64 interface -------------------------------------------

int64_t NativeAtomic64::get() const
    {
    return m_lAtomic;
    }

void NativeAtomic64::set(int64_t lValue)
    {
    m_lAtomic = lValue;
    }

int64_t NativeAtomic64::update(int64_t lAssume, int64_t lValue)
    {
    int64_t lRetval = m_lAtomic;
    if (lRetval == lAssume)
        {
        m_lAtomic = lValue;
        }
    return lRetval;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_UNSAFE_ATOMIC_WORD_HPP
