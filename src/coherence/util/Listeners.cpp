/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/Listeners.hpp"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/SynchronousListener.hpp"

#include <algorithm>

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constants ----------------------------------------------------------

namespace
    {
    ObjectArray::Handle getBlankList()
        {
        static FinalHandle<ObjectArray> ah(System::common(),
                ObjectArray::create(0));
        return ah;
        }
    COH_STATIC_INIT(getBlankList());
    }


// ----- constructors -------------------------------------------------------

Listeners::Listeners()
    : m_haAsyncListeners(self(), getBlankList()), m_haSyncListeners(self(), getBlankList()),
      m_vaFilters(self(), 0)
    {
    }


// ----- Listeners interface ------------------------------------------------

void Listeners::add(EventListener::Handle hListener)
    {
    COH_SYNCHRONIZED (this)
        {
        if (NULL == hListener)
            {
            return;
            }

        if (!contains(hListener))
            {
            ObjectArray::Handle haOld      = getListenerListFor(hListener);
            size32_t            cListeners = haOld->length;
            ObjectArray::Handle haNew      = ObjectArray::copy(haOld, 0,
                    ObjectArray::create(cListeners + 1));

            haNew[cListeners] = hListener;
            setListenerListFor(hListener, haNew);
            }
        }
    }

void Listeners::remove(EventListener::View vListener)
    {
    COH_SYNCHRONIZED (this)
        {
        if (NULL == vListener)
            {
            return;
            }

        ObjectArray::Handle haOld = getListenerListFor(vListener);
        size32_t            c     = haOld->length;

        // most common case - exactly one listener
        if (c == 1)
            {
            if (vListener->equals(haOld[0]))
                {
                setListenerListFor(vListener, getBlankList());
                }
            return;
            }

        // locate the listener in the list
        for (size32_t i = 0; i < c; ++i)
            {
            if (vListener->equals(haOld[i]))
                {
                // create a new array copying up to the removed element
                ObjectArray::Handle haNew = ObjectArray::copy(haOld, 0,
                        ObjectArray::create(c - 1), 0, i);

                if (i != c - 1)
                    {
                    // copy elements after the removed element
                    ObjectArray::copy(haOld, i + 1, haNew, i, c - i - 1);
                    }

                setListenerListFor(vListener, haNew);
                break;
                }
            }
        }
    }

void Listeners::addAll(Listeners::View vListeners)
    {
    COH_SYNCHRONIZED (this)
        {
        if (NULL == vListeners)
            {
            return;
            }

        ObjectArray::View vaAsyncAdd = vListeners->m_haAsyncListeners;
        if (vaAsyncAdd->length > 0)
          {
          m_haAsyncListeners = unionArrays(m_haAsyncListeners, 
                                           ObjectArray::copy(vaAsyncAdd, 0, ObjectArray::create(vaAsyncAdd->length)));
          }

        ObjectArray::View vaSyncAdd = vListeners->m_haSyncListeners;
        if (vaSyncAdd->length > 0)
          {
          m_haSyncListeners = unionArrays(m_haSyncListeners, 
                                          ObjectArray::copy(vaSyncAdd, 0, ObjectArray::create(vaSyncAdd->length)));
          }
        }
    }

void Listeners::removeAll()
    {
    COH_SYNCHRONIZED (this)
        {
        m_haAsyncListeners = getBlankList();
        m_haSyncListeners  = getBlankList();
        }
    }

bool Listeners::isEmpty() const
    {
    return m_haAsyncListeners->length == 0 && m_haSyncListeners->length == 0;
    }

bool Listeners::contains(EventListener::Handle hListener) const
    {
    ObjectArray::View va = instanceof<SynchronousListener::Handle>(hListener) ? 
            m_haSyncListeners : m_haAsyncListeners;
    for (size32_t i = 0, c = va->length; i < c; ++i)
        {
        if (hListener->equals(va[i]))
            {
            return true;
            }
        }

    return false;
    }

ObjectArray::View Listeners::listeners() const
    {
    ObjectArray::View vaSync  = m_haSyncListeners;
    ObjectArray::View vaAsync = m_haAsyncListeners;
    size32_t          cSync   = vaSync->length;
    size32_t          cAsync  = vaAsync->length;

    // check common cases
    if (cSync == 0)
        {
        return vaAsync;
        }
    if (cAsync == 0)
        {
        return vaSync;
        }

    // concatinate lists
    ObjectArray::Handle haNew = ObjectArray::create(cSync + cAsync);
    ObjectArray::copy(vaSync, 0, haNew, 0, cSync);
    ObjectArray::copy(vaAsync, 0, haNew, cSync, cAsync);
    return haNew;
    }


// ----- helper methods -----------------------------------------------------

ObjectArray::Handle Listeners::unionArrays(ObjectArray::Handle haListener1, ObjectArray::Handle haListener2)
    {
    size32_t cListener1 = haListener1->length;
    size32_t cListener2 = haListener2->length;

    // check the degenerate cases
    if (cListener1 == 0)
        {
        return haListener2;
        }
    if (cListener2 == 0)
        {
        return haListener1;
        }

    // Remove duplicates
    Set::Handle hSetUnion = HashSet::create();
    for (size32_t i = 0; i < cListener1; i++)
        {
        hSetUnion->add(haListener1[i]);
        }
    for (size32_t i = 0; i < cListener2; i++)
        {
        hSetUnion->add(haListener2[i]);
        }

    // check the cheap cases where one array is a subset of the other
    size32_t cSize = hSetUnion->size();
    if (cSize == cListener1)
        {
        return haListener1;
        }
    if (cSize == cListener2)
        {
        return haListener2;
        }

    return hSetUnion->toArray();
    }

ObjectArray::Handle Listeners::getListenerListFor(EventListener::View vListener)
    {
    return instanceof<SynchronousListener::View>(vListener) ? m_haSyncListeners : m_haAsyncListeners;
    }

void Listeners::setListenerListFor(EventListener::View vListener, ObjectArray::Handle hListenerArray)
    {
    if (instanceof<SynchronousListener::View>(vListener))
        {
        m_haSyncListeners = hListenerArray;
        }
    else 
        {
        m_haAsyncListeners = hListenerArray;
        }
    }

void Listeners::setFilters(ObjectArray::View vaFilters)
    {
    m_vaFilters = vaFilters;
    }

ObjectArray::View Listeners::getFilters() const
    {
    return m_vaFilters;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> Listeners::toString() const
    {
    return COH_TO_STRING("Listeners" << listeners());
    }

COH_CLOSE_NAMESPACE2
