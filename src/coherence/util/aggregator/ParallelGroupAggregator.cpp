/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/ParallelGroupAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(35, ParallelGroupAggregator);


// ----- constructors -------------------------------------------------------

ParallelGroupAggregator::ParallelGroupAggregator()
    : super()
    {
    }

ParallelGroupAggregator::ParallelGroupAggregator(
            ValueExtractor::View vExtractor,
            InvocableMap::ParallelAwareAggregator::Handle hAggregator,
            Filter::View vFilter)
    : super(vExtractor, hAggregator, vFilter)
    {
    }


// ----- InvocableMap::ParallelAwareAggregator interface --------------------

InvocableMap::EntryAggregator::Handle
            ParallelGroupAggregator::getParallelAggregator()
    {
    InvocableMap::ParallelAwareAggregator::Handle hAggregator =
        cast<InvocableMap::ParallelAwareAggregator::Handle>(
                    f_hAggregator);

    return GroupAggregator::create(
        f_vExtractor, hAggregator->getParallelAggregator(), f_vFilter);
    }

Object::Holder ParallelGroupAggregator::aggregateResults(
            Collection::View vCollResults)
    {
    InvocableMap::ParallelAwareAggregator::Handle hAggregator =
        cast<InvocableMap::ParallelAwareAggregator::Handle>(
                    f_hAggregator);

    Map::Handle hMapResult = HashMap::create();
    for (Iterator::Handle hIter = vCollResults->iterator();
            hIter->hasNext();)
        {
        // partial aggregation results are maps with distinct values
        // as keys and partial aggregation results as values
        Map::View vMapPart = cast<Map::View>(hIter->next());

        for (Iterator::Handle hIterPart = vMapPart->entrySet()->iterator();
                hIterPart->hasNext();)
            {
            Map::Entry::View vEntry = cast<Map::Entry::View>(
                        hIterPart->next());

            Object::View vDistinct = vEntry->getKey();
            Object::Holder vResult   = vEntry->getValue();

            // collect all the aggregation results per group
            Set::Handle hSetGroup = cast<Set::Handle>(
                        hMapResult->get(vDistinct));
            if (hSetGroup == NULL)
                {
                hMapResult->put(vDistinct, hSetGroup = HashSet::create());
                }
            hSetGroup->add(vResult);
            }
        }

    if (hMapResult->isEmpty())
        {
        // we need to call "aggregateResults" on the underlying
        // aggregator to fulfill our contract, even though any result
        // will be discarded
        hAggregator->aggregateResults(HashSet::create());
        }
    else
        {
        Filter::View vFilter = f_vFilter;
        for (Muterator::Handle hIter = hMapResult->entrySet()->iterator();
                hIter->hasNext();)
            {
            Map::Entry::Handle hEntry = cast<Map::Entry::Handle>(
                            hIter->next());

            Set::View    vSetGroup = cast<Set::View>(hEntry->getValue());
            Object::Holder vResult   = hAggregator->aggregateResults(
                        vSetGroup);
            if (vFilter == NULL || vFilter->evaluate(vResult))
                {
                hEntry->setValue(vResult);
                }
            else
                {
                hIter->remove();
                }
            }
        }

    return hMapResult;
    }

COH_CLOSE_NAMESPACE3
