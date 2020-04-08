/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/BetweenFilter.hpp"

#include "coherence/util/HashSet.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/GreaterEqualsFilter.hpp"
#include "coherence/util/filter/GreaterFilter.hpp"
#include "coherence/util/filter/LessEqualsFilter.hpp"
#include "coherence/util/filter/LessFilter.hpp"

using coherence::util::extractor::KeyExtractor;

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(64, BetweenFilter);


// ----- constructors -------------------------------------------------------

BetweenFilter::BetweenFilter()
    : super()
    {
    }

BetweenFilter::BetweenFilter(ValueExtractor::View vExtractor,
        Comparable::View vFrom, Comparable::View vTo)
    : super(GreaterEqualsFilter::create(vExtractor, vFrom),
            LessEqualsFilter::create(vExtractor, vTo))
    {
    }

BetweenFilter::BetweenFilter(ValueExtractor::View vExtractor, Comparable::View vLowerBound,
        Comparable::View vUpperBound, bool fIncludeLowerBound, bool fIncludeUpperBound)
    : super(fIncludeLowerBound
        ? (Filter::View) GreaterEqualsFilter::create(vExtractor, vLowerBound)
        : (Filter::View) GreaterFilter::create(vExtractor, vLowerBound),
      fIncludeUpperBound
        ? (Filter::View) LessEqualsFilter::create(vExtractor, vUpperBound)
        : (Filter::View) LessFilter::create(vExtractor, vUpperBound))
    {
    }

// ----- EntryFilter interface ------------------------------------------

bool BetweenFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    ValueExtractor::View  vExtractor = getValueExtractor();
    QueryMap::Entry::View vE         = cast<QueryMap::Entry::View>(vEntry, false);
    if (vE != NULL)
        {
        return evaluateExtracted(vE->extract(vExtractor));
        }
    else
        {
        KeyExtractor::View vKE = cast<KeyExtractor::View>(vExtractor, false);
        if (vKE != NULL)
            {
            return evaluateExtracted(vKE->extract(vEntry->getKey()));
            }
        else
            {
            return evaluateExtracted(vExtractor->extract(vEntry->getValue()));
            }
        }
    }

// ----- Filter interface -----------------------------------------------

bool BetweenFilter::evaluate(Object::View v) const
    {
	return evaluateExtracted(getValueExtractor()->extract(v));
    }

// ----- IndexAwareFilter interface -----------------------------------------

size32_t BetweenFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        return ExtractorFilter::eval_cost * vSetKeys->size();
        }

    Map::View vIndexContents = vIndex->getIndexContents();
    if (vIndex->isOrdered())
        {
        Comparable::View vLowerBound  = getLowerBound();
        Comparable::View vUpperBound  = getUpperBound();
        SortedMap::View  vMapContents = cast<SortedMap::View>(vIndex->getIndexContents());
        SortedMap::View  vMapHead     = vMapContents->headMap(vLowerBound);
        SortedMap::View  vMapTail     = vMapContents->tailMap(vUpperBound);
        return vMapContents->size() - vMapHead->size() - vMapTail->size();
        }

    return vIndexContents->size();
    }

Filter::View BetweenFilter::applyIndex(Map::View vMapIndexes, Set::Handle hSetKeys) const
    {
    Comparable::View vLowerBound = getLowerBound();
    Comparable::View vUpperBound = getUpperBound();

    if (NULL == vLowerBound || NULL == vUpperBound)
        {
        hSetKeys->clear();
        return NULL;
        }

    MapIndex::View vIndex = cast<MapIndex::View>(vMapIndexes->get(getValueExtractor()));
    if (NULL == vIndex)
        {
        return this;
        }

    Map::View vIndexContents = vIndex->getIndexContents();
    if (NULL == vIndexContents)
        {
        return this;
        }

    if (vIndex->isOrdered())
        {
        Comparator::View vIndexComparator = vIndex->getComparator();

        if (vIndexComparator == NULL)
            {
            SortedMap::View vMapContents = cast<SortedMap::View>(vIndexContents);
            return applySortedIndex(vMapContents, hSetKeys);
            }
        }

    Set::Handle hToRetain = HashSet::create();
    for (Iterator::Handle hIter = vIndexContents->entrySet()->iterator(); hIter->hasNext();)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        Comparable::View vTest  = cast<Comparable::View>(vEntry->getKey());
        Set::View        vKeys  = cast<Set::View>(vEntry->getValue());

        if (evaluateExtracted(vTest))
            {
            hToRetain->addAll(vKeys);
            }
        }

    hSetKeys->retainAll(hToRetain);

    return NULL;
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> BetweenFilter::toString() const
    {
    bool                 fLowerBoundInclusive = isLowerBoundInclusive();
    bool                 fUpperBoundInclusive = isUpperBoundInclusive();
    Comparable::View     vLowerBound          = getLowerBound();
    Comparable::View     vUpperBound          = getUpperBound();
    ValueExtractor::View vExtractor           = getValueExtractor();

    String::View vs = COH_TO_STRING("BetweenFilter: (" << vExtractor);
    if (fLowerBoundInclusive)
        {
        vs = COH_TO_STRING(vs << " >= ");
        }
    else
        {
        vs = COH_TO_STRING(vs << " > ");
        }
    vs = COH_TO_STRING(vs << vLowerBound << " AND " << vExtractor);
    if (fUpperBoundInclusive)
        {
        vs = COH_TO_STRING(vs << " <= ");
        }
    else
        {
        vs = COH_TO_STRING(vs << " < ");
        }
    return COH_TO_STRING(vs << vUpperBound << ")");
    }

// ----- helper methods -------------------------------------------------

Filter::View BetweenFilter::applySortedIndex(SortedMap::View vIndexContent,
        Set::Handle hSetKeys) const
    {
    bool                 fLowerBoundInclusive = isLowerBoundInclusive();
    bool                 fUpperBoundInclusive = isUpperBoundInclusive();
    Comparable::View     vLowerBound          = getLowerBound();
    Comparable::View     vUpperBound          = getUpperBound();
    SortedMap::View      vSubMap              = vIndexContent->subMap(vLowerBound,
    		                                                          vUpperBound);
    Set::Handle          hToRetain            = HashSet::create();
    bool                 fInside              = fLowerBoundInclusive;

    for (Iterator::Handle hIter = vSubMap->entrySet()->iterator(); hIter->hasNext();)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());
        Comparable::View vTest  = cast<Comparable::View>(vEntry->getKey());
        Set::View        vKeys  = cast<Set::View>(vEntry->getValue());

        if (fInside || evaluateExtracted(vTest))
            {
            fInside = true;
            hToRetain->addAll(vKeys);
            }
        }

    if (fUpperBoundInclusive)
        {
        Set::View vKeys = cast<Set::View>(vIndexContent->get(vUpperBound));
        if (NULL != vKeys)
            {
            hToRetain->addAll(vKeys);
            }
        }

    if (hToRetain->size() == 0)
        {
        hSetKeys->clear();
        }
    else
        {
        hSetKeys->retainAll(hToRetain);
        }

    return NULL;
    }

bool BetweenFilter::evaluateExtracted(Object::View vExtracted) const
    {
    bool             fLowerBoundInclusive = isLowerBoundInclusive();
    bool             fUpperBoundInclusive = isUpperBoundInclusive();
    Comparable::View vLowerBound          = getLowerBound();
    Comparable::View vUpperBound          = getUpperBound();

    try
        {
        if (NULL == vExtracted || NULL == vLowerBound || NULL == vUpperBound)
            {
            return false;
            }

        int32_t cl = vLowerBound->compareTo(vExtracted);
        if (cl > 0 || (!fLowerBoundInclusive && cl == 0))
            {
            return false;
            }

        int32_t cu = vUpperBound->compareTo(vExtracted);
        if (cu < 0 || (!fUpperBoundInclusive && cu == 0))
            {
            return false;
            }

        return true;
        }
    catch (ClassCastException::View&)
        {
        return false;
        }
    }

// ----- accessor methods ----------------------------------------------------

ValueExtractor::View BetweenFilter::getValueExtractor() const
    {
    return cast<ComparisonFilter::View>(getFilters()[0])->getValueExtractor();
    }

Comparable::View BetweenFilter::getLowerBound() const
    {
    return cast<Comparable::View>(cast<ComparisonFilter::View>(getFilters()[0])->getValue());
    }

Comparable::View BetweenFilter::getUpperBound() const
    {
    return cast<Comparable::View>(cast<ComparisonFilter::View>(getFilters()[1])->getValue());
    }

bool BetweenFilter::isLowerBoundInclusive() const
    {
    return instanceof<GreaterEqualsFilter::View>(getFilters()[0]);
    }

bool BetweenFilter::isUpperBoundInclusive() const
    {
    return instanceof<LessEqualsFilter::View>(getFilters()[1]);
    }

COH_CLOSE_NAMESPACE3
