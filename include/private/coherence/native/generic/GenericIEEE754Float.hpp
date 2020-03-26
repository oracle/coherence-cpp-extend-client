/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GENERIC_IEEE754_FLOAT_HPP
#define COH_GENERIC_IEEE754_FLOAT_HPP

#include "private/coherence/native/NativeFloat.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


// ----- NativeFloat interface ----------------------------------------------

/*
* This generic version works on processors who store floats in IEEE754
* format, which is quite common. For non-conforming processors a custom
* NativeFloat implementation is required.
*
* author djl
*/

uint32_t NativeFloat::float32ToBits(float32_t fl)
    {
    union
        {
        uint32_t n;
        float32_t fl;
        } u;
    u.fl = fl;
    const uint32_t lExponent = 0x7F800000;
    const uint32_t lFraction = 0x007FFFFF;
    if ((u.n & lExponent) == lExponent && (u.n & lFraction) != 0)
        {
        // We return the canonical QNan representation
        return 0x7FC00000;
        }
    return u.n;
    }

float32_t NativeFloat::bitsToFloat32(uint32_t n)
    {
    union
        {
        uint32_t n;
        float32_t fl;
        } u;
    u.n = n;
    return u.fl;
    }

uint64_t NativeFloat::float64ToBits(float64_t dfl)
    {
    union
        {
        uint64_t l;
        float64_t dfl;
        } u;
    u.dfl = dfl;
    const uint64_t lExponent = (uint64_t) 0x7ff00000 << 32;
    const uint64_t lFraction = (uint64_t) 0x000FFFFF << 32
                            | (uint64_t) 0xFFFFFFFF;
    if ((u.l & lExponent) == lExponent && (u.l & lFraction) != 0)
        {
        // We return the canonical QNan representation
        return ((uint64_t) 0x7FF80000) << 32;
       }

    return u.l;
    }

float64_t NativeFloat::bitsToFloat64(uint64_t l)
    {
    union
        {
        uint64_t l;
        float64_t dfl;
        } u;
    u.l = l;
    return u.dfl;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GENERIC_IEEE754_FLOAT_HPP
