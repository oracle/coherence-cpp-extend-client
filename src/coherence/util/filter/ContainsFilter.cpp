/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ContainsFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(67, ContainsFilter);


// ----- constructors -------------------------------------------------------

ContainsFilter::ContainsFilter()
    : super()
    {
    }

ContainsFilter::ContainsFilter(ValueExtractor::View vExtractor,
        Object::View vValue)
    : super(vExtractor, vValue)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool ContainsFilter::evaluateExtracted(Object::View vExtracted) const
    {
    Object::View     vValue = getValue();
    Collection::View vColl  = cast<Collection::View>(vExtracted, false);
    if (NULL != vColl)
        {
        return vColl->contains(vValue);
        }
    ObjectArray::View vA = cast<ObjectArray::View>(vExtracted, false);
    if (NULL != vA)
        {
        size32_t cExtracted = vA->length;
        for (size32_t i = 0; i < cExtracted; ++i)
            {
            if (Object::equals(vA[i], vValue))
                {
                return true;
                }
            }
        }
    return false;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t ContainsFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    return calculateMatchEffectiveness(vMapIndexes, vSetKeys);
    }

Filter::View ContainsFilter::applyIndex(Map::View vMapIndexes,
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
