/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/AbstractExtractor.hpp"

#include "coherence/util/comparator/SafeComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)

using coherence::util::comparator::SafeComparator;

// ----- constructors -------------------------------------------------------

AbstractExtractor::AbstractExtractor()
    : m_nTarget(value)
    {
    }


// ----- QueryMapComparator interface ---------------------------------------

int32_t AbstractExtractor::compareEntries(QueryMap::Entry::View vEntry1,
            QueryMap::Entry::View vEntry2) const
    {
    return SafeComparator::compareSafe(NULL,vEntry1->extract(this), vEntry2->extract(this));
    }


// ----- Comparator interface -----------------------------------------------

int32_t AbstractExtractor::compare(Object::View v1, Object::View v2) const
    {
    return SafeComparator::compareSafe(NULL, extract(v1), extract(v2));
    }


// ----- ValueExtractor interface -------------------------------------------

Object::Holder AbstractExtractor::extract(Object::Holder ohTarget) const
    {
    if (NULL == ohTarget)
        {
        return NULL;
        }
    else
        {
        COH_THROW (UnsupportedOperationException::create());
        }
    }


// ----- subclassing support ------------------------------------------------

Object::Holder AbstractExtractor::extractFromEntry(
        Map::Entry::Holder ohEntry) const
    {
    return extract((m_nTarget == value)
            ? (Object::Holder) ohEntry->getValue()
            : (Object::Holder) ohEntry->getKey());
    }

Object::Holder AbstractExtractor::extractOriginalFromEntry(
        MapTrigger::Entry::Holder ohEntry) const
    {
    return m_nTarget == key
        ? extract((Object::Holder) ohEntry->getKey())
        : ohEntry->isOriginalPresent()
            ? extract(ohEntry->getOriginalValue()) : NULL;
    }

int32_t AbstractExtractor::getTarget() const
    {
    return m_nTarget;
    }

COH_CLOSE_NAMESPACE3
