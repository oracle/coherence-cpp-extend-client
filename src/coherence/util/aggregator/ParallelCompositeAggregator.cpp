/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/ParallelCompositeAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/aggregator/CompositeAggregator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(27, ParallelCompositeAggregator);


// ----- constructors -------------------------------------------------------

ParallelCompositeAggregator::ParallelCompositeAggregator()
    : super()
    {
    }

ParallelCompositeAggregator::ParallelCompositeAggregator(
            ObjectArray::View vaAggregator)
    : super(vaAggregator)
    {
    }


// ----- InvocableMap::ParallelAwareAggregator interface --------------------

InvocableMap::EntryAggregator::Handle
            ParallelCompositeAggregator::getParallelAggregator()
    {
    ObjectArray::View   vaParallel    = f_vaAggregator;

    size32_t            cAggregators  = vaParallel->length;
    ObjectArray::Handle haAggregator  = ObjectArray::create(cAggregators);

    for (size32_t i = 0; i < cAggregators; i++)
        {
        InvocableMap::ParallelAwareAggregator::Handle hAggregator =
                cast<InvocableMap::ParallelAwareAggregator::Handle>(
                            vaParallel[i]);
        haAggregator[i] = hAggregator->getParallelAggregator();
        }
    return CompositeAggregator::create(haAggregator);
    }

Object::Holder ParallelCompositeAggregator::aggregateResults(
            Collection::View vCollResults)
    {
    ObjectArray::View vaParallel = f_vaAggregator;

    size32_t cAggregators = vaParallel->length;
    size32_t cResults     = vCollResults->size();

    // the collection of partial results must be a collection of lists
    // (each list of a size of the aggregator array);
    // first we need to transpose it into an array of collections
    // and then run the final aggregation pass
    ObjectArray::Handle haaResults = ObjectArray::create(cAggregators);
    for (size32_t iAggr = 0; iAggr < cAggregators; iAggr++)
        {
        haaResults[iAggr] = ObjectArray::create(cResults);
        }

    Iterator::Handle iterResult = vCollResults->iterator();
    for (size32_t iResult = 0; iterResult->hasNext(); iResult++)
        {
        Object::View vResultPart = iterResult->next();
        if (!instanceof<List::View>(vResultPart))
            {
            COH_THROW_STREAM(IllegalStateException,
                "Expected result type: coherence::util::List; actual type: "
                    << Class::getClassName(vResultPart));
            }

        List::View vListResultPart = cast<List::View>(vResultPart);
        if (vListResultPart->size() != cAggregators)
            {
            COH_THROW_STREAM(IllegalStateException,
                 "Expected result list size: " << cAggregators <<
                 "; actual size: " << vListResultPart->size());
            }

        for (size32_t iAggr = 0; iAggr < cAggregators; iAggr++)
            {
            ObjectArray::Handle hArray = cast<ObjectArray::Handle>(
                        haaResults[iAggr]);
            hArray[iResult] = vListResultPart->get(iAggr);
            }
        }

    ObjectArray::Handle haResult = ObjectArray::create(cAggregators);
    for (size32_t iAggr = 0; iAggr < cAggregators; iAggr++)
        {
        InvocableMap::ParallelAwareAggregator::Handle hAgg =
                cast<InvocableMap::ParallelAwareAggregator::Handle>(
                        vaParallel[iAggr]);
        ArrayList::Handle hList = ArrayList::create();
        ObjectArray::View vTemp = cast<ObjectArray::View>(
                    haaResults[iAggr]);
        for (size32_t i = 0, c = vTemp->length; i < c; i++)
            {
            hList->add(vTemp[i]);
            }

        haResult[iAggr] = hAgg->aggregateResults(hList);
        }

    //TODO replace with ArrayList when available.
    ArrayList::Handle hList = ArrayList::create();
    for (size32_t i = 0,  c = haResult->length; i < c; i++)
        {
        hList->add(haResult[i]);
        }

    return hList;
    }

COH_CLOSE_NAMESPACE3
