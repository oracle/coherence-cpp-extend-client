/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/FilterMuterator.hpp"


COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructor ----------------------------------------------------

FilterMuterator::FilterMuterator(Iterator::Handle hIter, Filter::View vTest)
        : f_hIter(self(), hIter, /*fMutable*/ true), f_vTest(self(), vTest),
          m_fNext(false), m_fPrev(false),
          m_ohNext(self(), NULL, /*fMutable*/ true)
    {
    }


// ----- Iterator interface ---------------------------------------------


bool FilterMuterator::hasNext() const
    {
    bool fNext = m_fNext;
    if (fNext)
        {
        return true;
        }

    // find if there's a next one
    Iterator::Handle hIter = f_hIter;
    Filter::View     vTest = f_vTest;
    while (hIter->hasNext())
        {
        Object::Holder ohNext = hIter->next();
        if (vTest->evaluate(ohNext))
            {
            m_ohNext = ohNext;
            fNext = true;
            break;
            }
        }

    // can't call remove now (because we'd end up potentially removing the
    // wrong one
    m_fPrev = false;
    m_fNext = fNext;

    return fNext;
    }

Object::Holder FilterMuterator::next()
    {
    if (hasNext())
        {
        m_fNext = false;
        m_fPrev = true;
        return m_ohNext;
        }
    else
        {
        COH_THROW (NoSuchElementException::create());
        }
    }


// ----- Muterator interface ------------------------------------------------

void FilterMuterator::remove()
    {
    Iterator::Handle hIter = f_hIter;
    if (instanceof<Muterator::Handle>(hIter))
        {
        if (m_fPrev)
            {
            m_fPrev = false;
            cast<Muterator::Handle>(hIter)->remove();
            }
        else
            {
            COH_THROW (IllegalStateException::create());
            }
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    }

COH_CLOSE_NAMESPACE2
