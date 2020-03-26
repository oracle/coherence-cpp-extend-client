/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Count.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(28, Count);


// ----- constructors -------------------------------------------------------

Count::Count()
    {
    }


// ----- InvocableMap.EntryAggregator interface -----------------------------

Object::Holder Count::aggregate(Set::View vSetEntries)
    {
    int32_t c = 0;
    for (Iterator::Handle iter = vSetEntries->iterator(); iter->hasNext(); )
        {
        InvocableMap::Entry::View vEntry =
                cast<InvocableMap::Entry::View>(iter->next());
        if (vEntry->isPresent())
            {
            c++;
            }
        }
    return Integer32::valueOf(c);
    }


// ----- ParallelAwareAggregator interface ------------------------------

InvocableMap::EntryAggregator::Handle Count::getParallelAggregator()
    {
    return this;
    }

Object::Holder Count::aggregateResults(Collection::View vCollResults)
    {
    int32_t c = 0;
    for (Iterator::Handle iter = vCollResults->iterator(); iter->hasNext(); )
        {
        c += cast<Integer32::View>(iter->next())->getInt32Value();
        }
    return Integer32::valueOf(c);
    }


// ----- PortableObject interface -------------------------------------------

void Count::readExternal(PofReader::Handle /* hIn */)
    {
    }

void Count:: writeExternal(PofWriter::Handle /* hOut */) const
    {
    }

COH_CLOSE_NAMESPACE3
