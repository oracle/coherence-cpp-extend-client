/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GENERIC_ATOMIC_WORD_HPP
#define COH_GENERIC_ATOMIC_WORD_HPP

#include "coherence/lang.ns"

#include "coherence/native/NativeAtomic64.hpp"

#include "private/coherence/native/NativeMutex.hpp"

COH_OPEN_NAMESPACE2(coherence,native)


/**
* Mutex based implementation of NativeAtomic64.
*
* @author mf 2007.12.01
*/
namespace
    {
    /**
    * Return the pre-allocated "common mutex" associated with the specified
    * pointer. There is a small set of pre-allocated mutex which are used for
    * all generic NativeAtomic64 synchronization. See the Java documentation
    * for Base::getCommonMonitor for a full description of this shared
    * mutex/monitor pattern.
    *
    * @param the pointer to use an an index in finding the common mutex
    *
    * @return NativeMutex*
    */
    NativeMutex* getMutex(const NativeAtomic64* pAtomic)
        {
        static const size32_t  cMutex = 61;
        static NativeMutex** aMutex = NULL;
        if (NULL == aMutex)
            {
            aMutex = new NativeMutex*[cMutex];
            for (size32_t i = 0; i < cMutex; ++i)
                {
                aMutex[i] = NativeMutex::create();
                }
            }
        return aMutex[size32_t(pAtomic) % cMutex];
        }
    COH_STATIC_INIT(getMutex(0));
    }


// ----- NativeAtomic64 interface -------------------------------------------

int64_t NativeAtomic64::get() const
    {
    int64_t lValue;
    NativeMutex* pMutex = getMutex(this);
    pMutex->lock();
    lValue = m_lAtomic;
    pMutex->unlock();
    return lValue;
    }

void NativeAtomic64::set(int64_t lValue)
    {
    NativeMutex* pMutex = getMutex(this);
    pMutex->lock();
    m_lAtomic = lValue;
    pMutex->unlock();
    }

int64_t NativeAtomic64::update(int64_t lAssume, int64_t lValue)
    {
    int64_t lRetval;
    NativeMutex* pMutex = getMutex(this);
    pMutex->lock();
    lRetval = m_lAtomic;
    if (lRetval == lAssume)
        {
        m_lAtomic = lValue;
        }
    pMutex->unlock();
    return lRetval;
    }

COH_CLOSE_NAMESPACE2

#endif // COH_GENERIC_ATOMIC_WORD_HPP
