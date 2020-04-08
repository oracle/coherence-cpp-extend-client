/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/NotFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/MapIndex.hpp"
#include "coherence/util/SubSet.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"
#include "coherence/util/filter/KeyFilter.hpp"

#include "private/coherence/util/InvocableMapHelper.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(82, NotFilter);

using coherence::util::MapIndex;


// ----- constructors -------------------------------------------------------

NotFilter::NotFilter()
   : f_vFilter(self())
    {
    }

NotFilter::NotFilter(Filter::View vFilter)
    : f_vFilter(self(), vFilter)
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool NotFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    return !InvocableMapHelper::evaluateEntry(getFilter(), vEntry);
    }


// ----- Filter interface ---------------------------------------------------

bool NotFilter::evaluate(Object::View v) const
    {
    return !getFilter()->evaluate(v);
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t NotFilter::calculateEffectiveness(Map::View vMapIndexes,
        Set::View vSetKeys) const
    {
    Filter::View vFilter = getFilter();
    return  instanceof<IndexAwareFilter::View>(vFilter)
        ? cast<IndexAwareFilter::View>(vFilter)->calculateEffectiveness(vMapIndexes, vSetKeys)
        : vSetKeys->size()*ExtractorFilter::eval_cost;
    }

Filter::View NotFilter::applyIndex(Map::View vMapIndexes,
        Set::Handle hSetKeys) const
    {
    Filter::View vFilter = getFilter();
    if (instanceof<IndexAwareFilter::View>(vFilter))
        {
        // create delta set
        SubSet::Handle hSetDelta = SubSet::create(hSetKeys);

        // delegate to the not-ed filter, but only use the non-partial
        // indexes, since for a partial index the fact that it contains keys
        // for entries that fit the underlying filter does not mean that it
        // contains them all. As a result, the "negating" operation may
        // produce invalid result
        Filter::View vFilterNew  = cast<IndexAwareFilter::View>(vFilter)->applyIndex(
                getNonPartialIndexes(vMapIndexes), hSetDelta);

        // see if any keys were filtered out
        Set::View vSetRemoved = hSetDelta->getRemoved();

        if (vFilterNew == NULL || hSetDelta->isEmpty())
            {
            // invert the key selection by the delegated-to filter
            if (vSetRemoved->isEmpty())
                {
                // no keys were removed; therefore the result of the
                // "not" is to remove all keys (clear)
                hSetKeys->clear();
                }
            else if (hSetDelta->isEmpty())
                {
                // all keys were removed; therefore the result of the
                // "not" is to retain all keys (remove none)
                }
            else
                {
                // some keys were removed; therefore the result of the
                // "not" is to retain only those removed keys
                hSetKeys->retainAll(vSetRemoved);
                }

            // nothing left to do; the index fully resolved the filter
            return NULL;
            }
        else if (vSetRemoved->isEmpty())
            {
            // no obvious effect from the index application
            return vFilterNew == vFilter ? (Filter::View)this : (Filter::View)NotFilter::create(vFilterNew);
            }
        else
            {
            // some keys have been removed; those are definitely "in";
            // the remaining keys each need to be evaluated later
            KeyFilter::View vFilterKey = KeyFilter::create(vSetRemoved);
            NotFilter::View vFilterNot = vFilterNew == vFilter ? (NotFilter::View)this : (NotFilter::View)NotFilter::create(vFilterNew);
            return AndFilter::create(vFilterKey, vFilterNot);
            }
        }
    else
        {
        return this;
        }
    }


// ----- helpers ------------------------------------------------------------

Map::View NotFilter::getNonPartialIndexes(Map::View vMapIndexes) const
    {
    Map::Handle hMapNonPartialIndexes = HashMap::create();
    for (Iterator::Handle hIter = vMapIndexes->entrySet()->iterator(); hIter->hasNext();)
        {
        Map::Entry::View vEntry = cast<Map::Entry::View>(hIter->next());

        if (!cast<MapIndex::View>(vEntry->getValue())->isPartial())
            {
            hMapNonPartialIndexes->put(vEntry->getKey(), vEntry->getValue());
            }
        }
    return hMapNonPartialIndexes;
    }


// ----- PortableObject interface -------------------------------------------

void NotFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(0)));
    }

void NotFilter::writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vFilter);
    }


// ----- Object interface ---------------------------------------------------

bool NotFilter::equals(Object::View v) const
    {
    if (instanceof<NotFilter::View>(v))
        {
        NotFilter::View vThat = cast<NotFilter::View>(v);
        return Object::equals(f_vFilter, vThat->f_vFilter);
        }

    return false;
    }

size32_t NotFilter::hashCode() const
    {
    return f_vFilter->hashCode();
    }

TypedHandle<const String> NotFilter::toString() const
    {
    return COH_TO_STRING("NotFilter: !(" << getFilter() << ")");
    }


// ----- data members -------------------------------------------------------

Filter::View NotFilter::getFilter() const
    {
    return f_vFilter;
    }

COH_CLOSE_NAMESPACE3
