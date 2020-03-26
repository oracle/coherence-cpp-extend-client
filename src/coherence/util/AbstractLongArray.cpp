/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/AbstractLongArray.hpp"
#include "coherence/util/Collections.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- LongArray interface ------------------------------------------------

Object::Holder AbstractLongArray::get(int64_t lIndex) const
    {
    LongArrayIterator::Handle hIter = iterator(lIndex);
    if (hIter->hasNext())
        {
        Object::Holder oh = hIter->next();
        return hIter->getIndex() == lIndex ? oh : NULL;
        }
    else
        {
        return NULL;
        }
    }

int64_t AbstractLongArray::add(Object::Holder ohValue)
    {
    int64_t lIndex = getLastIndex() + 1;
    set(lIndex, ohValue);
    return lIndex;
    }

bool AbstractLongArray::exists(int64_t lIndex) const
    {
    LongArrayIterator::Handle hIter = iterator(lIndex);
    if (hIter->hasNext())
        {
        hIter->next();
        return hIter->getIndex() == lIndex;
        }
    else
        {
        return false;
        }
    }

Object::Holder AbstractLongArray::remove(int64_t lIndex)
    {
    LongArrayIterator::Handle hIter = iterator(lIndex);
    if (hIter->hasNext())
        {
        Object::Holder oh = hIter->next();
        if (hIter->getIndex() == lIndex)
            {
            hIter->remove();
            return oh;
            }
        }
        return NULL;
    }

bool AbstractLongArray::contains(Object::View vElement) const
    {
    LongArrayIterator::Handle hIter = iterator();
    while (hIter->hasNext())
        {
        if (Object::equals(vElement, hIter->next()))
            {
            return true;
            }
        }
    return false;
    }

void AbstractLongArray::clear()
    {
    LongArrayIterator::Handle hIter = iterator();
    while (hIter->hasNext())
        {
        hIter->next();
        hIter->remove();
        }
    }

bool AbstractLongArray::isEmpty() const
    {
    return getSize() == 0;
    }

size32_t AbstractLongArray::getSize() const
    {
    int32_t                   cItems = 0;
    LongArrayIterator::Handle hIter  = iterator();

    while (hIter->hasNext())
        {
        hIter->next();
        ++cItems;
        }
    return cItems;
    }

int64_t AbstractLongArray::getFirstIndex() const
    {
    LongArrayIterator::Handle hIter = iterator();
    if (hIter->hasNext())
        {
        hIter->next();
        return hIter->getIndex();
        }
    else
        {
        return -1;
        }
    }

int64_t AbstractLongArray::getLastIndex() const
    {
    LongArrayIterator::Handle hIter = iterator();
    if (!hIter->hasNext())
        {
        return -1;
        }

    while (hIter->hasNext())
        {
        hIter->next();
        }
    return hIter->getIndex();
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> AbstractLongArray::toString() const
    {
    return Collections::toString((Iterator::Handle) iterator());
    }

bool AbstractLongArray::equals(Object::View v) const
    {
    AbstractLongArray::View vThat = cast<AbstractLongArray::View>(v, false);
    if (NULL == vThat)
        {
        return false;
        }
    else if (this == vThat)
        {
        return true;
        }

    if (getSize() == vThat->getSize())
        {
        // short-cut:  both are empty?
        if (isEmpty())
            {
            return true;
            }

        // perform an in-order traversal, comparing each element
        LongArrayIterator::Handle hIterThis = iterator();
        LongArrayIterator::Handle hIterThat = vThat->iterator();

        while (hIterThis->hasNext() && hIterThat->hasNext())
            {
            if (!Object::equals(hIterThis->next(), hIterThat->next())
                || hIterThis->getIndex() != hIterThat->getIndex())
                {
                return false;
                }
            }

        return hIterThis->hasNext() == hIterThat->hasNext();
        }
    return false;
    }

size32_t AbstractLongArray::hashCode() const
    {
    size32_t nHash = 0;
    for (LongArrayIterator::Handle i = iterator(); i->hasNext(); )
        {
        nHash += Object::hashCode(i->next());
        }
    return nHash;
    }
	
COH_CLOSE_NAMESPACE2

