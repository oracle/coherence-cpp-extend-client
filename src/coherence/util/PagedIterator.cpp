/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/PagedIterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructor --------------------------------------------------------

PagedIterator::PagedIterator(Advancer::Handle hAdvancer)
        : f_hAdvancer(self(), hAdvancer, true),
          m_hIterPage(self(), (Iterator::Handle)NULL, true),
          m_ohCurr(self(), (Object::Handle)NULL)
    {
    }


// ----- Iterator interface -------------------------------------------------

bool PagedIterator::hasNext() const
    {
    Iterator::Handle hIter = m_hIterPage;
    while (NULL == hIter || !hIter->hasNext())
        {
        Collection::Handle hColPage = f_hAdvancer->nextPage();
        if (NULL == hColPage)
            {
            return false;
            }
        hIter = m_hIterPage = hColPage->iterator();
        }
    return true;

    }

Object::Holder PagedIterator::next()
    {
    Iterator::Handle hIter = m_hIterPage;
    while (NULL == hIter || !hIter->hasNext())
        {
        Collection::Handle hColPage = f_hAdvancer->nextPage();
        if (NULL == hColPage)
            {
            COH_THROW (NoSuchElementException::create());
            }
        hIter = m_hIterPage = hColPage->iterator();
        }
    return m_ohCurr = hIter->next();
    }

COH_CLOSE_NAMESPACE2
