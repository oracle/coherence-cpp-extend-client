/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/AnyFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/SubSet.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"
#include "coherence/util/filter/KeyFilter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(63, AnyFilter);


// ----- constructors -------------------------------------------------------

AnyFilter::AnyFilter()
    : super()
    {
    }

AnyFilter::AnyFilter(ObjectArray::View vaFilter)
    : super(vaFilter)
    {
    }

// ----- EntryFilter interface ----------------------------------------------

bool AnyFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    ObjectArray::View vaFilter = getFilters();
    for (size32_t i = 0, c = vaFilter->length; i < c; i++)
        {
        Filter::View vFilter = cast<Filter::View>(vaFilter[i]);
        if (InvocableMapHelper::evaluateEntry(vFilter, vEntry))
            {
            return true;
            }
        }
    return false;
    }


// ----- Filter interface ---------------------------------------------------

bool AnyFilter::evaluate(Object::View v) const
    {
    ObjectArray::View vaFilter = getFilters();
    for (size32_t i = 0, c = vaFilter->length; i < c; i++)
        {
        Filter::View vFilter = cast<Filter::View>(vaFilter[i]);
        if (vFilter->evaluate(v))
            {
            return true;
            }
        }
    return false;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t AnyFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    optimizeFilterOrder(vMapIndexes, vSetKeys);

    ObjectArray::View vaFilter = m_vaFilter;
    size32_t          cFilters = vaFilter->length;

    if (cFilters > 0)
        {
        Filter::View vFilterN = cast<Filter::View>(vaFilter[vaFilter->length - 1]);

        return instanceof<IndexAwareFilter::View>(vFilterN)
            ? cast<IndexAwareFilter::View>(vFilterN)
                ->calculateEffectiveness(vMapIndexes, vSetKeys)
            : vSetKeys->size() * ExtractorFilter::eval_cost;
        }

    return 1;
    }

Filter::View AnyFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    optimizeFilterOrder(vMapIndexes, hSetKeys);

    ObjectArray::View vaFilter    = m_vaFilter;
    int32_t           cFilters    = vaFilter->length;
    List::Handle      hlistFilter = ArrayList::create(cFilters);
    Set::Handle       hSetMatch   = HashSet::create(hSetKeys->size());

    // listFilter is an array of filters that will have to be re-applied
    // setMatch   is an accumulating set of already matching keys

    for (int32_t i = 0; i < cFilters; i++)
        {
        Filter::View vFilter = cast<Filter::View>(vaFilter[i]);
        if (instanceof<IndexAwareFilter::View>(vFilter))
            {
            Set::Handle hSetRemain = SubSet::create(hSetKeys);
            if (!hSetMatch->isEmpty())
                {
                hSetRemain->removeAll(hSetMatch);
                }
            Filter::View vFilterDefer = applyFilter(
                    cast<IndexAwareFilter::View>(vFilter), vMapIndexes, hSetRemain);

            if (NULL == vFilterDefer)
                {
                // these are definitely "in"
                hSetMatch->addAll(hSetRemain);
                }
            else
                {
                int32_t cKeys   = hSetKeys->size();
                int32_t cRemain = hSetRemain->size();
                if (cRemain < cKeys)
                    {
                    // some keys are definitely "out" for this filter;
                    // we need to incorporate this knowledge into a deferred
                    // filter
                    if (cRemain > 0)
                        {
                        KeyFilter::View vFilterKey = KeyFilter::create(hSetRemain);
                        hlistFilter->add(AndFilter::create(vFilterDefer, vFilterKey));
                        }
                    else
                        {
                        // though a filter was returned, the key set was
                        // fully reduced; this should have the same effect
                        // as a fully resolved filter without any matches
                        }
                    }
                else
                    {
                    hlistFilter->add(vFilterDefer);
                    }
                }
            }
        else
            {
            hlistFilter->add(vFilter);
            }
        }

    int32_t cMatches = hSetMatch->size();

    cFilters = hlistFilter->size();
    if (cFilters == 0)
        {
        if (cMatches > 0)
            {
            hSetKeys->retainAll(hSetMatch);
            }
        else
            {
            hSetKeys->clear();
            }
        }
    else if (cFilters == 1 && cMatches == 0)
        {
        return cast<Filter::View>(hlistFilter->get(0));
        }
    else
        {
        if (cMatches > 0)
            {
            // the keys that have been matched are definitely "in";
            // the remaining keys each need to be evaluated later
            KeyFilter::View vFilterKey = KeyFilter::create(hSetMatch);
            hlistFilter->add(0, vFilterKey);
            cFilters++;
            }

        return AnyFilter::create(hlistFilter->toArray());
        }
    return NULL;
    }

COH_CLOSE_NAMESPACE3
