/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ConcurrentMap.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_OPEN_NAMESPACE_ANON(ConcurrentMap)

// ----- constants ----------------------------------------------------------

class LockAll
    : public class_spec<LockAll>
    {
    friend class factory<LockAll>;

    protected:
        LockAll()
            {
            }

    public:
        virtual bool equals(Object::View /*v*/) const
            {
            COH_THROW (noimpl());
            }

        virtual size32_t hashCode() const
            {
            COH_THROW (noimpl());
            }

    protected:
        virtual UnsupportedOperationException::Handle noimpl() const
            {
            return UnsupportedOperationException::create(
                    "LOCK_ALL is not implemented for this cache");
            }
    };

COH_CLOSE_NAMESPACE_ANON

Object::View ConcurrentMap::getLockAll()
    {
    static FinalView<Object> v(System::common(), LockAll::create());
    return v;
    }
COH_STATIC_INIT(ConcurrentMap::getLockAll());


// ----- inner class: LockBlock ---------------------------------------------

ConcurrentMap::LockBlock::LockBlock(ConcurrentMap::Handle hMap, Object::View vKey,
        bool fTryLock, int64_t cWait)
    : m_hMap(hMap), m_vKey(vKey), m_fTryLock(fTryLock)
    {
    if (!hMap->lock(vKey, cWait))
        {
        m_hMap = NULL;
        }
    }

ConcurrentMap::LockBlock::LockBlock(const LockBlock& that)
    : m_hMap(that.m_hMap), m_vKey(that.m_vKey), m_fTryLock(that.m_fTryLock)
    {
    // Take ownership of the lock
    that.m_hMap = NULL;
    }

ConcurrentMap::LockBlock::~LockBlock()
    {
    if (m_hMap != NULL)
        {
        try
            {
            if(!m_hMap->unlock(m_vKey))
                {
                COH_LOG("Failed to unlock key: " << m_vKey << " in Map: " << m_hMap, 2);
                }
            }
        catch(Exception::View e)
            {
            COH_LOG("Error unlocking key: " << m_vKey << " in Map: " << m_hMap
                    << " due to: " << e, 1);
            // eat it
            }
        }
    }

ConcurrentMap::LockBlock::operator bool() const
    {
    if (m_hMap == NULL)
        {
        // not locked; return false for try, true for non-try
        return !m_fTryLock;
        }
    // locked; return true for try, false for non-try
    return m_fTryLock;
    }

COH_CLOSE_NAMESPACE2
