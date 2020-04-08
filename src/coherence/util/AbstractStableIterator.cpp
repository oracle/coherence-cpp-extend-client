/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractStableIterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructors -------------------------------------------------------

AbstractStableIterator::AbstractStableIterator()
        : m_fNextReady(false), m_ohNext(self()), m_ohPrev(self()),
          m_fCanDelete(false)
    {
    }

// ----- Muterator interface ------------------------------------------------

void AbstractStableIterator::remove()
    {
    if (m_fCanDelete)
        {
        m_fCanDelete = false;
        remove(m_ohPrev);
        }
    else
        {
        COH_THROW (IllegalStateException::create());
        }
    }

// ----- Iterator interface -------------------------------------------------

bool AbstractStableIterator::hasNext() const
    {
    return m_fNextReady;
    }

Object::Holder AbstractStableIterator::next()
    {
    if (m_fNextReady)
        {
        Object::Holder ohNext = m_ohNext;

        m_fNextReady = false;
        m_fCanDelete = true;
        m_ohPrev     = ohNext;

        advance(); // pre-load
        return ohNext;
        }

    COH_THROW (NoSuchElementException::create("end of iteration"));
    }


// ----- internal -----------------------------------------------------------

Object::Holder AbstractStableIterator::getPrevious() const
    {
    return m_ohPrev;
    }

void AbstractStableIterator::setNext(Object::Holder ohNext)
    {
    m_ohNext     = ohNext;
    m_fNextReady = true;
    }

void AbstractStableIterator::remove(Object::Holder /*oh*/)
    {
    COH_THROW (UnsupportedOperationException::create());
    }


// ----- Object interface ---------------------------------------------------

void AbstractStableIterator::onInit()
    {
    advance(); // pre-load
    super::onInit();
    }

COH_CLOSE_NAMESPACE2
