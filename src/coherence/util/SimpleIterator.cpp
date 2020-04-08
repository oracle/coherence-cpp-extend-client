/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/SimpleIterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

SimpleIterator::SimpleIterator(ObjectArray::Holder ohaItem)
    : f_ohaItem(self(), ohaItem),
      m_iItem(0),
      m_ofLimit(ohaItem->length),
      m_fForward(true)
    {
    }

SimpleIterator::SimpleIterator(ObjectArray::Holder ohaItem,
        size32_t ofStart, size32_t cItems, bool fForward, bool fCopy)
    : f_ohaItem(self(), fCopy && cItems > 1 ?
            cast<ObjectArray::Holder>(ohaItem->clone()) : ohaItem),
      m_iItem(ofStart),
      m_ofLimit(cItems),
      m_fForward(fForward)
    {
    }


// ----- Iterator interface -------------------------------------------------

bool SimpleIterator::hasNext() const
    {
    return m_fForward ? m_iItem < m_ofLimit : m_iItem > m_ofLimit;
    }

Object::Holder SimpleIterator::next()
    {
    if (!hasNext())
        {
        COH_THROW (NoSuchElementException::create());
        }

    try
        {
        ObjectArray::Holder ohaItem = cast<ObjectArray::Holder>(f_ohaItem);
        Object::Holder      oh       = ohaItem[m_iItem];

        if (m_fForward)
            {
            m_iItem++;
            }
        else
            {
            m_iItem--;
            }
        return oh;
        }
    catch (IndexOutOfBoundsException::View vex)
        {
        COH_THROW (NoSuchElementException::create());
        }
    }

COH_CLOSE_NAMESPACE2
