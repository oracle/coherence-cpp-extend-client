/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/DualQueue.hpp"

#include "coherence/util/CircularArrayList.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

DualQueue::DualQueue()
    : f_hHeadLock(self(), Object::create()),
      m_hElementList(self(), CircularArrayList::create()),
      m_hHeadElementList(*f_hHeadLock, CircularArrayList::create())
    {
    }


// ----- DualQueue interface ------------------------------------------------

bool DualQueue::swapNoWait()
    {
    List::Handle hListHead = getHeadElementList();

    if (hListHead->isEmpty())
        {
        List::Handle hListTail = getElementList();
        if (!hListTail->isEmpty())
            {
            setElementList(hListHead);
            setHeadElementList(hListTail);
            return true;
            }
        }
    // no swapping needed
    return false;
    }

List::Handle DualQueue::getHeadElementList()
    {
    return m_hHeadElementList;
    }

void DualQueue::setHeadElementList(List::Handle hList)
    {
    m_hHeadElementList = hList;
    }

List::Handle DualQueue::getElementList()
    {
    return m_hElementList;
    }

void DualQueue::setElementList(List::Handle hList)
    {
    m_hElementList = hList;
    }

Object::Handle DualQueue::getHeadLock()
    {
    return f_hHeadLock;
    }

// ----- Queue interface ----------------------------------------------------

bool DualQueue::add(Object::Holder oh)
    {
    if (NULL == oh)
        {
        COH_THROW (IllegalArgumentException::create(
                "The ConcurrentQueue does not support null values."));
        }

    List::Handle hList = getElementList();
    COH_SYNCHRONIZED(hList)
        {
        hList->add(oh);
        }

    onAddElement();
    return true;
    }

bool DualQueue::addHead(Object::Holder oh)
    {
    if (NULL == oh)
        {
        COH_THROW (IllegalArgumentException::create(
                "The ConcurrentQueue does not support null values."));
        }

    COH_SYNCHRONIZED(getHeadLock())
        {
        List::Handle hListHead = getHeadElementList();
        COH_SYNCHRONIZED(hListHead)
            {
            hListHead->add(0, oh);
            }
        }

    onAddElement();
    return true;
    }

bool DualQueue::isEmpty() const
    {
    return size() == 0;
    }

Object::Holder DualQueue::peekNoWait()
    {
    /* Optimization in Java, adds contention in C++
    if (isEmpty())
        {
        return NULL;
        }
    */

    List::Handle hListHead = getHeadElementList();
    if (hListHead->isEmpty())
        {
        if (!swapNoWait())
            {
            // tail was also empty
            return NULL;
            }
        hListHead = getHeadElementList();
        }

    COH_SYNCHRONIZED(hListHead)
        {
        return hListHead->get(0);
        }
    }

Object::Holder DualQueue::removeNoWait()
    {
    if (m_nElementCounter.get() == 0)
        {
        return NULL;
        }

    Object::Holder hEntry;
    COH_SYNCHRONIZED(getHeadLock())
        {
        List::Handle hListHead = getHeadElementList();
        if (hListHead->isEmpty())
            {
            if (!swapNoWait())
                {
                // tail was empty too
                return NULL;
                }
            // head and tail were swapped, get the new head
            hListHead = getHeadElementList();
            }

        COH_SYNCHRONIZED(hListHead)
            {
            hEntry = hListHead->remove(0);
            }
        }

    if (m_nElementCounter.adjust(-1) == 0)
        {
        onEmpty();
        }

    return hEntry;
    }

COH_CLOSE_NAMESPACE2
