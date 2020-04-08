/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructor --------------------------------------------------------

AbstractList::AbstractList()
        : m_nHashCode(0)
    {
    }

AbstractList::AbstractList(const AbstractList& /*that*/)
        : m_nHashCode(0)
    {
    }


// ----- List interface -----------------------------------------------------

Object::Holder AbstractList::get(size32_t i)
    {
    return ((const AbstractList*) this)->get(i);
    }


// ----- Object interface ---------------------------------------------------

bool AbstractList::equals(Object::View v) const
    {
    List::View vThat = cast<List::View>(v, false);

    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }

    if (size() != vThat->size())
        {
        return false;
        }

    for (Iterator::Handle hIterThis = iterator(), hIterThat = vThat->iterator();
            hIterThis->hasNext(); )
        {
        if (!Object::equals(hIterThis->next(), hIterThat->next()))
            {
            return false;
            }
        }

    return true;
    }

bool AbstractList::isImmutable() const
    {
    if (m_nHashCode == 0)
        {
        bool fImmutable = Object::isImmutable();
        for (Iterator::Handle hIter = iterator(); fImmutable && hIter->hasNext(); )
            {
            fImmutable = fImmutable && hIter->next()->isImmutable();
            }

        if (fImmutable)
            {
            // take the hashCode so that the results are cached in the future
            hashCode();
            }

        return fImmutable;
        }

    return true;
    }

size32_t AbstractList::hashCode() const
    {
    size32_t nHash = m_nHashCode;
    if (nHash == 0)
        {
        bool fImmutable = Object::isImmutable();

        nHash = 1;
        for (Iterator::Handle hIter = iterator(); hIter->hasNext(); )
            {
            Object::Holder oh = hIter->next();
            if (NULL != oh)
                {
                nHash = 31* nHash + oh->hashCode();
                fImmutable &= oh->isImmutable();
                }
            }

        if (fImmutable)
            {
            // cache the hash
            COH_SYNCHRONIZED_MEMBER_WRITE
                {
                m_nHashCode = nHash;
                }
            }
        }
    else
        {
        // dirty read gave us non-zero value, though it may not be necessary
        // perform a clean read to ensure we didn't see a partial write
        COH_SYNCHRONIZED_MEMBER_READ
            {
            nHash = m_nHashCode;
            }
        }
    return nHash;
    }

COH_CLOSE_NAMESPACE2
