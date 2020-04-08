/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/PriorityFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(87, PriorityFilter);


// ----- constructors -------------------------------------------------------

PriorityFilter::PriorityFilter()
    : f_vFilter(self())
    {
    }

PriorityFilter::PriorityFilter(IndexAwareFilter::View vFilter)
    : f_vFilter(self(), vFilter)
    {
    }


// ----- EntryFilter interface ----------------------------------------------


bool PriorityFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    return f_vFilter->evaluateEntry(vEntry);
    }


// ----- Filter interface ---------------------------------------------------

bool PriorityFilter::evaluate(Object::View v) const
    {
    return f_vFilter->evaluate(v);
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t PriorityFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    return f_vFilter->calculateEffectiveness(vMapIndexes, vSetKeys);
    }

Filter::View PriorityFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    return f_vFilter->applyIndex(vMapIndexes, hSetKeys);
    }

// ----- PortableObject interface -------------------------------------------

void PriorityFilter::readExternal(PofReader::Handle hIn)
    {
    AbstractPriorityTask::readExternal(hIn);
    initialize(f_vFilter, cast<IndexAwareFilter::View>(hIn->readObject(10)));
    }

void PriorityFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    AbstractPriorityTask::writeExternal(hOut);
    hOut->writeObject(10, f_vFilter);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> PriorityFilter::toString() const
    {
    return COH_TO_STRING(Class::getClassName(this)
        << "("
        << getFilter()
        << ")");
    }


// ----- data members accessor-----------------------------------------------

IndexAwareFilter::View PriorityFilter::getFilter() const
    {
    return f_vFilter;
    }

COH_CLOSE_NAMESPACE3
