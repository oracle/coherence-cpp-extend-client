/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ContainsAllFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(65, ContainsAllFilter);


// ----- constructors -------------------------------------------------------

ContainsAllFilter::ContainsAllFilter()
    : super()
    {
    }

ContainsAllFilter::ContainsAllFilter(ValueExtractor::View vExtractor,
            Set::View vSetValues)
    : super(vExtractor, vSetValues)
    {
    }


// ----- ExtractorFilter interface ----------------------------------------------

bool ContainsAllFilter::evaluateExtracted(Object::View vExtracted) const
    {
    Collection::View vCollValues = cast<Collection::View>(getValue());
    Collection::View vColl       = cast<Collection::View>(vExtracted, false);
    if (NULL != vColl)
        {
        return vColl->containsAll(vCollValues);
        }
    ObjectArray::View vA = cast<ObjectArray::View>(vExtracted, false);
    if (NULL != vA)
        {
        size32_t cExtracted  = vA->length;
        for (Iterator::Handle iter = vCollValues->iterator(); iter->hasNext();)
            {
            Object::Holder vValue = iter->next();
            for (size32_t i = 0; i < cExtracted; ++i)
                {
                if (Object::equals(vA[i], vValue))
                    {
                    goto NextValue;
                    }
                }
            return false;
            NextValue: continue;
            }
        }
    return true;
   }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t ContainsAllFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    return vIndex == NULL ? calculateIteratorEffectiveness(vSetKeys->size())
                         : (cast<Collection::View>(getValue()))->size();
    }

Filter::View ContainsAllFilter::applyIndex(Map::View vMapIndexes,
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
        for (Iterator::Handle hIter = (cast<Collection::View>(getValue()))->iterator(); hIter->hasNext();)
            {
            Object::View oValue = hIter->next();
            Set::View    vSetEQ = cast<Set::View>(vIndex->getIndexContents()->get(oValue));

            if (vSetEQ == NULL)
                {
                hSetKeys->clear();
                break;
                }
            else
                {
                hSetKeys->retainAll(vSetEQ);
                if (hSetKeys->isEmpty())
                    {
                    break;
                    }
                }
            }
        return NULL;
        }
    }

COH_CLOSE_NAMESPACE3
