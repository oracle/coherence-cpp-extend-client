/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/AllFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(60,AllFilter);


// ----- constructors -------------------------------------------------------

AllFilter::AllFilter()
    : super()
    {
    }

AllFilter::AllFilter(ObjectArray::View vaFilter)
    : super(vaFilter)
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool AllFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    ObjectArray::View vaFilter = getFilters();
    for (size32_t i = 0, c = vaFilter->length; i < c; i++)
        {
        if (!InvocableMapHelper::evaluateEntry(cast<Filter::View>(vaFilter[i]),
                vEntry))
            {
            return false;
            }
        }
    return true;
    }


// ----- Filter interface ---------------------------------------------------

bool AllFilter::evaluate(Object::View v) const
    {
    ObjectArray::View vaFilter = getFilters();
    for (size32_t i = 0, c = vaFilter->length; i < c; i++)
        {
        if (!cast<Filter::View>(vaFilter[i])->evaluate(v))
            {
            return false;
            }
        }
    return true;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t AllFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    optimizeFilterOrder(vMapIndexes, vSetKeys);

    ObjectArray::View vaFilter = m_vaFilter;
    size32_t          cFilters = vaFilter->length;

    if (cFilters > 0)
        {
        Filter::View vFilter0 = cast<Filter::View>(vaFilter[0]);

        return instanceof<IndexAwareFilter::View>(vFilter0)
            ? cast<IndexAwareFilter::View>(vFilter0)
                ->calculateEffectiveness(vMapIndexes, vSetKeys)
            : vSetKeys->size() * ExtractorFilter::eval_cost;
        }

    return 1;
    }

Filter::View AllFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    optimizeFilterOrder(vMapIndexes, hSetKeys);

    ObjectArray::View vaFilter    = m_vaFilter;
    int32_t           cFilters    = vaFilter->length;
    List::Handle      hListFilter = ArrayList::create(cFilters);

    // listFilter is an array of filters that will have to be re-applied

    for (int32_t i = 0; i < cFilters; i++)
        {
        Filter::View vFilter = cast<Filter::View>(vaFilter[i]);
        if (instanceof<IndexAwareFilter::View>(vFilter))
            {
            Filter::View vFilterNew = applyFilter(
                cast<IndexAwareFilter::View>(vFilter), vMapIndexes, hSetKeys);

            if (hSetKeys->isEmpty())
                {
                return NULL;
                }

            if (NULL != vFilterNew)
                {
                hListFilter->add(vFilterNew);
                }
            }
        else
            {
            hListFilter->add(vFilter);
            }
        }

    cFilters = hListFilter->size();
    if (cFilters == 0)
        {
        return NULL;
        }
    else if (cFilters == 1)
        {
        return cast<Filter::View>(hListFilter->get(0));
        }
    else
        {
        return AllFilter::create(hListFilter->toArray());
        }
    }

COH_CLOSE_NAMESPACE3
