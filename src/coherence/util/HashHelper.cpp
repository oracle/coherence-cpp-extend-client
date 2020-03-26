/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/HashHelper.hpp"

#include "coherence/lang.ns"

#include "private/coherence/native/NativeFloat.hpp"

COH_OPEN_NAMESPACE2(coherence, util)

using coherence::native::NativeFloat;

// ----- static functions ---------------------------------------------------

size32_t HashHelper::hash(bool fValue, size32_t nHash)
    {
    return swizzle(nHash) ^ (fValue ? 1231 : 1237);
    }

size32_t HashHelper::hash(octet_t bValue, size32_t nHash)
    {
    return swizzle(nHash) ^ bValue;
    }

size32_t HashHelper::hash(wchar16_t chValue, size32_t nHash)
    {
    return swizzle(nHash) ^ chValue;
    }

size32_t HashHelper::hash(float64_t dflValue, size32_t nHash)
    {
    int64_t lBits = NativeFloat::float64ToBits(dflValue);
    return swizzle(nHash) ^ (int) (lBits ^ (lBits >> 32));
    }

size32_t HashHelper::hash(float32_t flValue, size32_t nHash)
    {
    return swizzle(nHash) ^ NativeFloat::float32ToBits(flValue);
    }

size32_t HashHelper::hash(int32_t nValue, size32_t nHash)
    {
    return swizzle(nHash) ^ nValue;
    }

size32_t HashHelper::hash(int64_t lValue, size32_t nHash)
    {
    return swizzle(nHash) ^ (int32_t) (lValue ^ (lValue >> 32));
    }

size32_t HashHelper::hash(size32_t nValue, size32_t nHash)
    {
    return swizzle(nHash) ^ nValue;
    }

size32_t HashHelper::hash(size64_t lValue, size32_t nHash)
    {
    return swizzle(nHash) ^ (int32_t) (lValue ^ (lValue >> 32));
    }

size32_t HashHelper::hash(int16_t shValue, size32_t nHash)
    {
    return swizzle(nHash) ^ shValue;
    }

size32_t HashHelper::hash(Object::View vValue, size32_t nHash)
    {
    nHash = swizzle(nHash);
    if (NULL == vValue)
        {
        return nHash;
        }
    if (instanceof<ObjectArray::View>(vValue))
        {
        return nHash ^ hash(cast<ObjectArray::View>(vValue), nHash);
        }
    if (instanceof<Collection::View>(vValue))
        {
        return nHash ^ hash(cast<Collection::View>(vValue), nHash);
        }

    return nHash ^ vValue->hashCode();
    }

size32_t HashHelper::hash(ObjectArray::View avValue, size32_t nHash)
    {
    nHash = swizzle(nHash);
    if (NULL == avValue)
        {
        return nHash;
        }
    for (size32_t i = 0; i < avValue->length; ++i)
        {
        nHash = hash(cast<Object::View>(avValue[i]), nHash);
        }
    return nHash;
    }

size32_t HashHelper::hash(Collection::View vCol, size32_t nHash)
    {
    nHash = swizzle(nHash);
    for (Iterator::Handle hIter = vCol->iterator(); hIter->hasNext(); )
        {
        nHash ^= hIter->next()->hashCode();
        }
    return nHash;
    }

// ----- helpers ------------------------------------------------------------

size32_t HashHelper::swizzle(size32_t nHash)
    {
    return (nHash << 4) | ((nHash >> 28) & 0xF);
    }

COH_CLOSE_NAMESPACE2
