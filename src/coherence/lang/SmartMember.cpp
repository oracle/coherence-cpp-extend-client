/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/SmartMember.hpp"

#include "coherence/lang.ns"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


// ----- constructor --------------------------------------------------------

SmartMember::SmartMember()
    : m_pGuardian(NULL), m_pNext(NULL)
    {
    }

void SmartMember::unlinkGuardian()
    {
    Object const* pGuardian = m_pGuardian;
    bool          fError    = false;

    if (pGuardian != NULL)
        {
        SynchronizedMemberWriteBlock::Guard syncWrite(*pGuardian);
        SmartMember* pCurr = pGuardian->m_pSmartMemberStack;

        if (pCurr == this)
            {
            // we are at the top of the stack
            pGuardian->m_pSmartMemberStack = m_pNext;
            }
        else
            {
            // find link which references this SmartMember
            while (pCurr != NULL && pCurr->m_pNext != this)
                {
                pCurr = pCurr->m_pNext;
                }

            if (NULL == pCurr)
                {
                fError = true;
                }
            else
                {
                pCurr->m_pNext = m_pNext;
                }
            }
        }

    if (fError)
        {
        COH_LOG("Error unlinking SmartMember not on the guardian's stack", -1);
        }
    }


// ----- operators ----------------------------------------------------------

SmartMember& SmartMember::operator=(const SmartMember& /*that*/)
    {
    // no-op, ensure that local data members are not overwritten by the
    // default assignment operator
    return *this;
    }


// ----- SmartMember interface ----------------------------------------------

void SmartMember::setGuardian(const Object& oGuardian)
    {
    Object const* pGuardian = m_pGuardian;
    if (NULL != pGuardian)
        {
        unlinkGuardian();
        }

    // setup new guardian
    m_pGuardian = &oGuardian;
        {
        SynchronizedMemberWriteBlock::Guard syncWrite(*m_pGuardian);
        m_pNext                       = oGuardian.m_pSmartMemberStack;
        oGuardian.m_pSmartMemberStack = this;
        }
    }

COH_CLOSE_NAMESPACE2
