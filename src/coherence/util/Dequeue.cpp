/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/Dequeue.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_EXPORT_SPEC_MEMBER(const size32_t Dequeue::default_max_elements)

// ----- constructors -------------------------------------------------------

Dequeue::Dequeue()
        : m_aoItem(self(), ObjectArray::create(Dequeue::default_max_elements)),
          m_iFront(0),
          m_iBack(0)
    {
    }

Dequeue::Dequeue(Dequeue::View vDequeue)
        : m_aoItem(self(), ObjectArray::copy(vDequeue->m_aoItem, 0,
                ObjectArray::create(vDequeue->m_aoItem->length))),
          m_iFront(vDequeue->m_iFront),
          m_iBack(vDequeue->m_iBack)
    {
    }


// ----- Iterator interface -------------------------------------------------

bool Dequeue::hasNext() const
    {
    return !isEmpty();
    }

Object::Holder Dequeue::next()
    {
    if (isEmpty())
        {
        COH_THROW (NoSuchElementException::create("No remaining elements."));
        }
    // get the next element
    ObjectArray::Handle aoItem = m_aoItem;
    size32_t            iFront = m_iFront = next(m_iFront);
    Object::Holder o           = aoItem[iFront];

    // release the reference to the next element (allows it to be gc'd)
    aoItem[iFront] = NULL;

    return o;
    }


// ----- Dequeue interface --------------------------------------------------

void Dequeue::addElement(Object::Holder ohObject)
    {
    if (isFull())
        {
        grow();
        }

    m_iBack                                  = next(m_iBack);
    ((ObjectArray::Handle) m_aoItem)[m_iBack] = ohObject;
    }

void Dequeue::putBackElement(Object::Holder ohObject)
    {
    if (isFull())
        {
        grow();
        }
    ((ObjectArray::Handle) m_aoItem)[m_iFront] = ohObject;
    m_iFront                                   = prev(m_iFront);
    }

void Dequeue::removeAllElements()
    {
    m_aoItem = ObjectArray::create(Dequeue::default_max_elements);
    m_iFront = 0;
    m_iBack  = 0;
    }

size32_t Dequeue::size() const
    {
    if (m_iFront <= m_iBack)
        {
        return m_iBack - m_iFront;
        }
    else
        {
        return m_aoItem->length - (m_iFront - m_iBack);
        }
    }

bool Dequeue::isEmpty() const
    {
    return (m_iFront == m_iBack);
    }

bool Dequeue::isFull() const
    {
    return (m_iFront == next(m_iBack));
    }

size32_t Dequeue::capacity() const
    {
    if (m_aoItem->length == 0) return 0;
    return m_aoItem->length - 1;
    }


// ----- internal methods ---------------------------------------------------

size32_t Dequeue::next(size32_t i) const
    {
    return (++i == m_aoItem->length ? 0 : i);
    }

size32_t Dequeue::prev(size32_t i) const
    {
    return (i == 0 ? m_aoItem->length - 1 : --i);
    }

void Dequeue::grow()
    {
    size32_t cOld = m_aoItem->length;
    // grow by 50%, but by 256 maximum ...
    size32_t cGrow = (cOld >> 1) < 256 ? (cOld >> 1) : 256;
    // ... or 32 minimum
    cGrow = cGrow > 32 ? cGrow : 32;
    size32_t cNew = cOld + cGrow;

    // create new array and copy existing elements to it
    ObjectArray::Handle aoNew = ObjectArray::create(cNew);
    if (m_iFront < m_iBack)
        {
        arrayCopy(m_iFront, aoNew, m_iFront, m_iBack - m_iFront + 1);
        }
    else if (m_iFront > m_iBack)
        {
        arrayCopy(0, aoNew, 0, m_iBack + 1);
        arrayCopy(m_iFront, aoNew, cNew - (cOld - m_iFront), cOld - m_iFront);
        m_iFront += cNew - cOld;
        }

    m_aoItem = aoNew;
    }

void Dequeue::arrayCopy(size32_t cSrcStart, ObjectArray::Handle haTgt,
        size32_t cTgtStart, size32_t cLen )
    {
    ObjectArray::Handle aoItem = m_aoItem;
    for ( size32_t i=0; i < cLen; ++i  )
        {
        haTgt[cTgtStart+i] = aoItem[cSrcStart + i];
        }
    }

COH_CLOSE_NAMESPACE2
