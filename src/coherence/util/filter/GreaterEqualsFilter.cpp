/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/GreaterEqualsFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/SortedMap.hpp"


COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(69, GreaterEqualsFilter);


// ----- constructors -------------------------------------------------------

GreaterEqualsFilter::GreaterEqualsFilter()
    : super()
    {
    }

GreaterEqualsFilter::GreaterEqualsFilter(ValueExtractor::View vValueExtractor,
        Comparable::View vComparable)
    : super(vValueExtractor, vComparable)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool GreaterEqualsFilter::evaluateExtracted(Object::View vExtracted) const
    {
    try
        {
        Comparable::View cLeft  = cast<Comparable::View>(vExtracted);
        Comparable::View cRight = cast<Comparable::View>(getValue());
        return NULL != cLeft && NULL != cRight &&
               cLeft->compareTo(cRight) >= 0;
        }
    catch (ClassCastException::View)
        {
        return false;
        }
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t GreaterEqualsFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    return calculateRangeEffectiveness(vMapIndexes, vSetKeys);
    }

Filter::View GreaterEqualsFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    Object::View vValue = getValue();
    if (NULL == vValue)
        {
        // nothing could be compared to null
        hSetKeys->clear();
        return NULL;
        }

    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        // there is no relevant index
        return this;
        }

    if (vIndex->isOrdered())
        {
        SortedMap::View vMapContents = cast<SortedMap::View>(vIndex->getIndexContents());
        SortedMap::View vMapLT       = vMapContents->headMap(vValue);
        SortedMap::View vMapGE       = vMapContents->tailMap(vValue);
        bool            fHeadHeavy   = vMapLT->size() > vMapContents->size() / 2;

        if (fHeadHeavy || vIndex->isPartial())
            {
            Set::Handle hSetGE = HashSet::create();
            for (Iterator::Handle hIterGE = vMapGE->values()->iterator(); hIterGE->hasNext();)
                {
                Set::View vSet = cast<Set::View>(hIterGE->next());
                hSetGE->addAll(vSet);
                }
            hSetKeys->retainAll(hSetGE);
            }
        else
            {
            for (Iterator::Handle hIterLT = vMapLT->values()->iterator(); hIterLT->hasNext();)
                {
                Set::View vSet = cast<Set::View>(hIterLT->next());
                hSetKeys->removeAll(vSet);
                }
            }
        // Note: the NULL set doesn't get in
        }
    else
        {
        Map::View vMapContents = vIndex->getIndexContents();

        if (vIndex->isPartial())
            {
            Set::Handle hSetGE = HashSet::create();
            for (Iterator::Handle hIter = vMapContents->entrySet()->iterator();
                 hIter->hasNext();)
                {
                Map::Entry::View  vEntry = cast<Map::Entry::View>(hIter->next());
                Comparable::View  vTest  = cast<Comparable::View>(vEntry->getKey());
                if (NULL != vTest && vTest->compareTo(vValue) >= 0)
                    {
                    hSetGE->addAll(cast<Set::View>(vEntry->getValue()));
                    }
                }
            hSetKeys->retainAll(hSetGE);
            }
        else
            {
            for (Iterator::Handle hIter = vMapContents->entrySet()->iterator();
                 hIter->hasNext();)
                {
                Map::Entry::View  vEntry = cast<Map::Entry::View>(hIter->next());
                Comparable::View  vTest  = cast<Comparable::View>(vEntry->getKey());
                if (NULL == vTest || vTest->compareTo(vValue) < 0)
                    {
                    hSetKeys->removeAll(cast<Set::View>(vEntry->getValue()));
                    }
                }
            }
        }

    return NULL;
    }

COH_CLOSE_NAMESPACE3
