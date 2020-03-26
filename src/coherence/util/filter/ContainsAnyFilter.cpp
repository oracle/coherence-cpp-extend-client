/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ContainsAnyFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(66, ContainsAnyFilter);

namespace
    {
    Set::View ensureSet(Set::View vSetValues)
        {
        COH_ENSURE_PARAM(vSetValues);
        if (vSetValues->Object::isImmutable())
            {
            return vSetValues;
            }

        Set::Handle vSetNew = HashSet::create(vSetValues->size());
        vSetNew->addAll(vSetValues);
        return vSetNew;
        }
    }

// ----- constructors -------------------------------------------------------

ContainsAnyFilter::ContainsAnyFilter()
    : super()
    {
    }

ContainsAnyFilter::ContainsAnyFilter(ValueExtractor::View vExtractor,
            Set::View vSetValues)
    : super(vExtractor, ensureSet(vSetValues))
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool ContainsAnyFilter::evaluateExtracted(Object::View vExtracted) const
    {
    Collection::View vColValues = cast<Collection::View>(getValue());
    Collection::View vCol       = cast<Collection::View>(vExtracted, false);
    if (NULL != vCol)
        {
        for (Iterator::Handle iter = vColValues->iterator();
                    iter->hasNext();)
            {
            if (vCol->contains(iter->next()))
                {
                return true;
                }
            }
        return false;
        }
    ObjectArray::View vA = cast<ObjectArray::View>(vExtracted, false);
    if (NULL != vA)
        {
        size32_t cExtracted  = vA->length;
        for (Iterator::Handle iter = vColValues->iterator(); iter->hasNext();)
            {
            Object::Holder vValue = iter->next();
            for (size32_t i = 0; i < cExtracted; ++i)
                {
                if (Object::equals(vA[i], vValue))
                    {
                    return true;
                    }
                }
            }
        }
    return false;
    }

// ----- IndexAwareFilter interface -----------------------------------------

size32_t ContainsAnyFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    return vIndex == NULL ? calculateIteratorEffectiveness(vSetKeys->size())
                         : cast<Collection::View>(getValue())->size();
    }

Filter::View ContainsAnyFilter::applyIndex(Map::View vMapIndexes,
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
        Collection::View vColValues = cast<Collection::View>(getValue());
        Set::Handle      hSetIn     = HashSet::create();
        for (Iterator::Handle hIter = vColValues->iterator(); hIter->hasNext();)
            {
            Object::View oValue = hIter->next();
            Set::View    vSetEQ  = cast<Set::View>(vIndex->getIndexContents()->get(oValue));

            if (vSetEQ != NULL)
                {
                hSetIn->addAll(vSetEQ);
                }
            }

        if (hSetIn->isEmpty())
            {
            hSetKeys->clear();
            }
        else
            {
            hSetKeys->retainAll(hSetIn);
            }
        return NULL;
        }
    }

COH_CLOSE_NAMESPACE3
