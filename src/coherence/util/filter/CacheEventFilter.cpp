/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/CacheEventFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/net/cache/CacheEvent.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::net::cache::CacheEvent;

COH_REGISTER_PORTABLE_CLASS(203, CacheEventFilter);

COH_EXPORT_SPEC_MEMBER(const int32_t CacheEventFilter::e_synthetic)
COH_EXPORT_SPEC_MEMBER(const int32_t CacheEventFilter::e_natural)


// ----- constructors -------------------------------------------------------

CacheEventFilter::CacheEventFilter()
    : super(0), m_nMaskSynthetic(0)
    {
    }

CacheEventFilter::CacheEventFilter(int32_t nMask, int32_t nMaskSynthetic)
    : super(nMask), m_nMaskSynthetic(nMaskSynthetic)
    {
    }

CacheEventFilter::CacheEventFilter(Filter::View vFilter, int32_t nMaskSynthetic)
    : super(vFilter), m_nMaskSynthetic(nMaskSynthetic)
    {
    }

CacheEventFilter::CacheEventFilter(int32_t nMask, Filter::View vFilter, int32_t nMaskSynthetic)
    : super(nMask, vFilter), m_nMaskSynthetic(nMaskSynthetic)
    {
    }


// ----- Filter interface ---------------------------------------------------

bool CacheEventFilter::evaluate(Object::View v) const
    {
    CacheEvent::View vEvent = cast<CacheEvent::View>(v, false);
    if (NULL == vEvent)
        {
        return super::evaluate(v);
        }
    else
        {
        int32_t nMaskSynthetic = m_nMaskSynthetic;
        bool    fSynthetic     = vEvent->isSynthetic();

        return (((nMaskSynthetic & e_synthetic) != 0 && fSynthetic) ||
                ((nMaskSynthetic & e_natural) != 0 && !fSynthetic)) &&
            super::evaluate(v);
        }
    }

// ----- Object interface ---------------------------------------------------

bool CacheEventFilter::equals(Object::View v) const
    {
    CacheEventFilter::View that = cast<CacheEventFilter::View>(v, false);
    if (NULL != that)
        {
        return m_nMaskSynthetic == that->m_nMaskSynthetic && super::equals(v);
        }

    return false;
    }

size32_t CacheEventFilter::hashCode() const
    {
    return super::hashCode() ^ m_nMaskSynthetic;
    }

String::View CacheEventFilter::getDescription() const
    {
    int32_t nMaskSynthetic = m_nMaskSynthetic;

    String::View vs = ", synthetic-mask=";
    
    if (nMaskSynthetic == 0)
        {
        vs = COH_TO_STRING(vs << "<none>");
        }
    else
        {
        bool fSeparator = false;
        if ((nMaskSynthetic & e_natural) != 0)
            {
            vs = COH_TO_STRING(vs << "NATURAL");
            fSeparator = true;
            }
        if ((nMaskSynthetic & e_synthetic) != 0)
            {
            if (fSeparator)
                {
                vs = COH_TO_STRING(vs << "|");
                }

            vs = COH_TO_STRING(vs << "SYNTHETIC");
            }
        }

    return vs;
    }


// ----- PortableObject interface -------------------------------------------

void CacheEventFilter::readExternal(PofReader::Handle hIn)
    {
    super::readExternal(hIn);

    m_nMask = hIn->readInt32(10);
   }

void CacheEventFilter::writeExternal(PofWriter::Handle hOut) const
    {
    super::writeExternal(hOut);

    hOut->writeInt32(10,m_nMaskSynthetic);
    }


COH_CLOSE_NAMESPACE3
