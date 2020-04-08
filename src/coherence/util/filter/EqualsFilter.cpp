/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/EqualsFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/MapIndex.hpp"


COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(68, EqualsFilter);


// ----- constructors -------------------------------------------------------

EqualsFilter::EqualsFilter()
    : super()
    {
    }

EqualsFilter::EqualsFilter(ValueExtractor::View vValueExtractor,
        Object::View vValue)
    : super(vValueExtractor, vValue)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool EqualsFilter::evaluateExtracted(Object::View vExtracted) const
    {
    return Object::equals(vExtracted, getValue());
    }

// ----- IndexAwareFilter interface -----------------------------------------

size32_t EqualsFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    return calculateMatchEffectiveness(vMapIndexes, vSetKeys);
    }

Filter::View EqualsFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (vIndex == NULL)
        {
        // there is no relevant index
        return this;
        }
    else
        {
        Set::View vSetEquals = cast<Set::View>(vIndex->getIndexContents()->get(getValue()));
        if (vSetEquals == NULL || vSetEquals->isEmpty())
            {
            hSetKeys->clear();
            }
        else
            {
            hSetKeys->retainAll(vSetEquals);
            }
        return NULL;
        }
    }

COH_CLOSE_NAMESPACE3
