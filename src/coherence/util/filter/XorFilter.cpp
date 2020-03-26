/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/XorFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Filter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(86, XorFilter);


// ----- constructors -------------------------------------------------------

XorFilter::XorFilter()
    : super()
    {
    }

XorFilter::XorFilter(Filter::View vFilterLeft, Filter::View vFilterRight)
    : super()
    {
    ObjectArray::Handle haFilter = ObjectArray::create(2);
    haFilter[0] = vFilterLeft;
    haFilter[1] = vFilterRight;
    m_vaFilter = haFilter;
    }


// ----- EntryFilter interface ----------------------------------------------

bool XorFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    ObjectArray::View vaFilter = getFilters();
    Filter::View vFilterLeft = cast<Filter::View>(vaFilter[0]);
    Filter::View vFilterRight = cast<Filter::View>(vaFilter[1]);
    return InvocableMapHelper::evaluateEntry(vFilterLeft, vEntry)
        ^  InvocableMapHelper::evaluateEntry(vFilterRight, vEntry);
    }


// ----- Filter interface ---------------------------------------------------

bool XorFilter::evaluate(Object::View v) const
    {
    ObjectArray::View vaFilter = getFilters();
    Filter::View vFilterLeft = cast<Filter::View>(vaFilter[0]);
    Filter::View vFilterRight = cast<Filter::View>(vaFilter[1]);
    return vFilterLeft->evaluate(v) ^ vFilterRight->evaluate(v);
    }

// ----- IndexAwareFilter interface -----------------------------------------

size32_t XorFilter::calculateEffectiveness(Map::View /* vMapIndexes */,
        Set::View /* vSetKeys */) const
    {
    return Integer32::max_value;
    }

Filter::View XorFilter::applyIndex(Map::View /* vMapIndexes */,
        Set::Handle /* hSetKeys */) const
    {
    // no use for indexes
    return this;
    }

COH_CLOSE_NAMESPACE3
