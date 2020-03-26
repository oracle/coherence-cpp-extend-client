/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/ConditionalIndex.hpp"

#include "coherence/util/MapIndex.hpp"
#include "coherence/util/SimpleMapIndex.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

ConditionalIndex::ConditionalIndex(Filter::View vFilter,
        ValueExtractor::View vExtractor,
        bool fOrdered,
        Comparator::View vComparator,
        bool fForwardIndex)
        : super(vExtractor, fOrdered, vComparator, false),
          f_vFilter(self(), vFilter),
          m_fForwardIndex(fForwardIndex),
          m_fPartial(false)
    {
    init(fForwardIndex);
    }


// ----- SimpleMapIndex methods -----------------------------------------

Object::Holder ConditionalIndex::get(Object::View vKey) const
    {
    if (isForwardIndexSupported())
        {
        Map::View      mapForward = f_hMapForward;
        Object::Holder oValue     = mapForward->get(vKey);

        return (oValue != NULL || mapForward->containsKey(vKey)) ?
               oValue : (Object::Holder) MapIndex::getNoValue();
        }
    return MapIndex::getNoValue();
    }

Object::Holder ConditionalIndex::get(Object::View vKey)
    {
    if (isForwardIndexSupported())
        {
        Map::Handle    hMapForward = f_hMapForward;
        Object::Holder oValue      = hMapForward->get(vKey);

        return (oValue != NULL || hMapForward->containsKey(vKey)) ?
               oValue : (Object::Holder) MapIndex::getNoValue();
        }
    return MapIndex::getNoValue();
    }

Map::Entry::View ConditionalIndex::getForwardEntry(Object::View vKey) const
    {
    return m_fForwardIndex ? super::getForwardEntry(vKey) : NULL;
    }

Map::Entry::Handle ConditionalIndex::getForwardEntry(Object::View vKey)
    {
    return m_fForwardIndex ? super::getForwardEntry(vKey) : NULL;
    }

void ConditionalIndex::removeForwardEntry(Object::View vKey)
    {
    if (m_fForwardIndex)
        {
        super::removeForwardEntry(vKey);
        }
    }

Map::Handle ConditionalIndex::instantiateForwardIndex() const
    {
    return m_fForwardIndex ? super::instantiateForwardIndex() : NULL;
    }

bool ConditionalIndex::isPartial() const
    {
    return m_fPartial || super::isPartial();
    }


// ----- helpers --------------------------------------------------------

Filter::View ConditionalIndex::getFilter() const
    {
    return f_vFilter;
    }

bool ConditionalIndex::isForwardIndexSupported() const
    {
    return m_fForwardIndex;
    }

bool ConditionalIndex::evaluateEntry(Map::Entry::View vEntry)
    {
    try
        {
        if (InvocableMapHelper::evaluateEntry(f_vFilter, vEntry))
            {
            return true;
            }
        }
    catch (RuntimeException::View)
        {
        // COH-6447: don't drop the index upon exception
        }

    m_fPartial = true;
    return false;
    }

void ConditionalIndex::insertInternal(Map::Entry::View vEntry)
    {
    if (evaluateEntry(vEntry))
        {
        super::insertInternal(vEntry);
        }
    }

void ConditionalIndex::updateInternal(Map::Entry::View vEntry)
    {
    if (evaluateEntry(vEntry))
        {
        super::updateInternal(vEntry);
        }
    else
        {
        removeInternal(vEntry);
        }
    }

void ConditionalIndex::removeInternal(Map::Entry::View vEntry)
    {
    try
        {
        if (instanceof<MapTrigger::Entry::View>(vEntry) &&
            !InvocableMapHelper::evaluateOriginalEntry(getFilter(), cast<MapTrigger::Entry::View>(vEntry)))
            {
            // the "original" entry would have been excluded; nothing to do
            return;
            }
        }
    catch (RuntimeException::View)
        {
        // COH-6447: attempt the delete anyway because the filter may have
        // allowed this value previously and it may be in the index
        }

    super::removeInternal(vEntry);
    }


// ----- Object interface -----------------------------------------------

TypedHandle<const String> ConditionalIndex::toString() const
    {
    return COH_TO_STRING(super::toString() << Class::getClassName(this)
        << "(Filter="        << f_vFilter
        << ", ForwardIndex=" << m_fForwardIndex << ")");
    }

bool ConditionalIndex::equals(Object::View v) const
    {
    if (!super::equals(v) || !instanceof<ConditionalIndex::View>(v))
        {
        return false;
        }

    ConditionalIndex::View vThat = cast<ConditionalIndex::View>(v);
    return Object::equals(getFilter(), vThat->getFilter())
        && isForwardIndexSupported() == vThat->isForwardIndexSupported();
    }

COH_CLOSE_NAMESPACE2
