/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/GroupAggregator.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/HashMap.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(34, GroupAggregator);


// ----- factory methods ----------------------------------------------------

GroupAggregator::Handle GroupAggregator::create()
    {
    return this_spec::create();
    }

GroupAggregator::Handle GroupAggregator::create(
            ValueExtractor::View vExtractor,
            InvocableMap::EntryAggregator::Handle hAggregator,
            Filter::View vFilter)
    {
    return this_spec::create(vExtractor, hAggregator, vFilter);
    }


// ----- constructors -------------------------------------------------------

GroupAggregator::GroupAggregator()
        : f_vExtractor(self()),
          f_hAggregator(self()),
          f_vFilter(self())
    {
    }

GroupAggregator::GroupAggregator(ValueExtractor::View vExtractor,
            InvocableMap::EntryAggregator::Handle hAggregator,
            Filter::View vFilter)
        : f_vExtractor(self(), vExtractor),
          f_hAggregator(self(),hAggregator),
          f_vFilter(self(),vFilter)
    {
    COH_ENSURE_PARAM(vExtractor);
    COH_ENSURE_PARAM(hAggregator);
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder GroupAggregator::aggregate(Set::View vSetEntries)
    {
    ValueExtractor::View                   vExtractor  = f_vExtractor;
    InvocableMap::EntryAggregator::Handle  hAggregator = f_hAggregator;
    Filter::View                           vFilter     = f_vFilter;

    // create non-intersecting groups of entry sets
    Map::Handle hMapResult =  HashMap::create();
    for (Iterator::Handle hIter = vSetEntries->iterator(); hIter->hasNext();)
        {
        InvocableMap::Entry::View vEntry =
                cast<InvocableMap::Entry::View>(hIter->next());
        if (vEntry->isPresent())
            {
            // extract a distinct value (or a tuple)
            Object::View vDistinct = vEntry->extract(vExtractor);

            // add the entry to the corresponding group
            Set::Handle hSetGroup = cast<Set::Handle>(
                        hMapResult->get(vDistinct));
            if (hSetGroup == NULL)
                {
                hMapResult->put(vDistinct, hSetGroup = HashSet::create());
                }
            hSetGroup->add(vEntry);
            }
        }

    // run the aggregation
    for (Muterator::Handle hIter = hMapResult->entrySet()->iterator();
                hIter->hasNext();)
        {
        Map::Entry::Handle hEntry     = cast<Map::Entry::Handle>(
                    hIter->next());
        Set::View          vSetGroup  = cast<Set::View>(
                    hEntry->getValue());
        Object::Holder     vResult    =  hAggregator->aggregate(vSetGroup);
        if (vFilter == NULL || vFilter->evaluate(vResult))
            {
            hEntry->setValue(vResult);
            }
        else
            {
            hIter->remove();
            }
        }

    return hMapResult;
    }


// ----- PortableObject interface -------------------------------------------

void GroupAggregator::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObjectArray(0)));
    initialize(f_hAggregator, cast<InvocableMap::EntryAggregator::Handle>(
                hIn->readObjectArray(1)));
    initialize(f_vFilter, cast<Filter::View>(hIn->readObjectArray(2)));
    }

void GroupAggregator:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vExtractor);
    hOut->writeObject(1, f_hAggregator);
    hOut->writeObject(2, f_vFilter);
    }


// ----- Object interface ---------------------------------------------------

bool GroupAggregator::equals(Object::View v) const
    {
    GroupAggregator::View that = cast<GroupAggregator::View>(v, false);
    if (that != NULL)
         {
         return this == that ||
             (f_vExtractor->equals(that->f_vExtractor)
             && f_hAggregator->equals(that->f_hAggregator));
         }
     return false;
    }

size32_t GroupAggregator::hashCode() const
    {
    return getExtractor()->hashCode() + getAggregator()->hashCode();
    }

TypedHandle<const String> GroupAggregator::toString() const
    {
    String::View vs = COH_TO_STRING(Class::getClassName(this)
        << "("
        << getExtractor() << ", " << getAggregator());
    if (f_vFilter != NULL)
        {
        vs = COH_TO_STRING(vs <<", " << f_vFilter);
        }
    return COH_TO_STRING(vs << ")");
    }


// ----- data members accessor-----------------------------------------------

ValueExtractor::View GroupAggregator::getExtractor() const
    {
    return f_vExtractor;
    }

InvocableMap::EntryAggregator::Handle GroupAggregator::getAggregator()
    {
    return f_hAggregator;
    }

InvocableMap::EntryAggregator::View GroupAggregator::getAggregator() const
    {
    return f_hAggregator;
    }

COH_CLOSE_NAMESPACE3
