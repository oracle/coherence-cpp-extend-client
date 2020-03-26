/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/processor/AbstractProcessor.hpp"

#include "coherence/util/HashMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

using coherence::util::HashMap;


// ----- InvocableMap::EntryProcessor interface -----------------------------

Object::Holder AbstractProcessor::process(
        InvocableMap::Entry::Handle /* hEntry */) const
    {
    COH_THROW (UnsupportedOperationException::create(
            "This entry processor cannot be invoked on the client."));
    }

Map::View AbstractProcessor::processAll(Set::View vSetEntries) const
    {
    Map::Handle hMapResults = HashMap::create();
    for (Iterator::Handle iter = vSetEntries->iterator(); iter->hasNext(); )
        {
        InvocableMap::Entry::Handle hEntry =
                cast<InvocableMap::Entry::Handle>(iter->next());
        hMapResults->put(hEntry->getKey(), process(hEntry));
        }
    return hMapResults;
    }

COH_CLOSE_NAMESPACE3
