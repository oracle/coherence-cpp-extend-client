/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/PriorityAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(39, PriorityAggregator);


// ----- constructors -------------------------------------------------------

PriorityAggregator::PriorityAggregator()
    : super(), f_hAggregator(self())
    {
    }

PriorityAggregator::PriorityAggregator(
            InvocableMap::ParallelAwareAggregator::Handle hAggregator)
    : super(), f_hAggregator(self(), hAggregator)
    {
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder PriorityAggregator::aggregate(Set::View vSetEntries)
    {
    return f_hAggregator->aggregate(vSetEntries);
    }

InvocableMap::EntryAggregator::Handle
            PriorityAggregator::getParallelAggregator()
    {
    return f_hAggregator->getParallelAggregator();
    }

Object::Holder PriorityAggregator::aggregateResults(
            Collection::View vCollResults)
    {
    return f_hAggregator->aggregateResults(vCollResults);
    }


// ----- PortableObject interface -------------------------------------------

void PriorityAggregator::readExternal(PofReader::Handle hIn)
    {
    AbstractPriorityTask::readExternal(hIn);
    initialize(f_hAggregator, cast<InvocableMap::ParallelAwareAggregator::Handle>(
                hIn->readObject(10)));
    }

void PriorityAggregator:: writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPriorityTask::writeExternal(hOut);
    hOut->writeObject(10, f_hAggregator);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> PriorityAggregator::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "("
        << getAggregator()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

InvocableMap::ParallelAwareAggregator::Handle
        PriorityAggregator::getAggregator()
    {
    return f_hAggregator;
    }

InvocableMap::ParallelAwareAggregator::View
        PriorityAggregator::getAggregator() const
    {
    return f_hAggregator;
    }

COH_CLOSE_NAMESPACE3
