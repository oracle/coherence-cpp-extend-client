/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/NotEqualsFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(81, NotEqualsFilter);


// ----- constructors -------------------------------------------------------

NotEqualsFilter::NotEqualsFilter()
    : super()
    {
    }

NotEqualsFilter::NotEqualsFilter(ValueExtractor::View vValueExtractor,
        Object::View vValue)
    : super(vValueExtractor, vValue)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool NotEqualsFilter::evaluateExtracted(Object::View vExtracted) const
    {
    return !Object::equals(vExtracted, getValue());
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t NotEqualsFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    return calculateMatchEffectiveness(vMapIndexes, vSetKeys);
    }

Filter::View NotEqualsFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        // there is no relevant index
        return this;
        }
    else
        {
        Map::View    hMapContents = vIndex->getIndexContents();
        Object::View vValue       = getValue();

        if (vIndex->isPartial())
            {
            Set::Handle hSetNE = HashSet::create();
            for (Iterator::Handle hIter = hMapContents->entrySet()->iterator();
                 hIter->hasNext();)
                {
                Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
                if (!(vEntry->getKey()->equals(vValue)))
                    {
                    Set::View vSet = cast<Set::View>(vEntry->getValue());
                    hSetNE->addAll(vSet);
                    }
                }
            hSetKeys->retainAll(hSetNE);
            }
        else
            {
            Set::View vSetEquals = cast<Set::View>(hMapContents->get(vValue));
            if (NULL != vSetEquals && !vSetEquals->isEmpty())
                {
                hSetKeys->removeAll(vSetEquals);
                }
            }
        return NULL;
        }
    }

COH_CLOSE_NAMESPACE3
