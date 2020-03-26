/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/extractor/AbstractUpdater.hpp"

COH_OPEN_NAMESPACE3(coherence,util,extractor)


// ----- ValueUpdater interface ---------------------------------------------

/**
* {@inheritDoc}
*/
void AbstractUpdater::update(Object::Handle /*hTarget*/,
        Object::Holder /*ohValue*/) const
    {
    COH_THROW(UnsupportedOperationException::create());
    }


// ----- subclassing support ------------------------------------------------

void AbstractUpdater::updateEntry(Map::Entry::Handle hEntry,
        Object::Holder ohValue) const
    {
    // the code below is identical to the bottom part of the
    // InvocableMapHelper#updateEntry(), but I couldn't
    // figure out a neat way to coalesce these small blocks of code... (GG)

    Object::Handle hTarget = cast<Object::Handle>(hEntry->getValue());

    update(hTarget, ohValue);

    if (instanceof<InvocableMap::Entry::Handle>(hEntry))
        {
        cast<InvocableMap::Entry::Handle>(hEntry)->setValue(hTarget, false);
        }
    else
        {
        cast<InvocableMap::Entry::Handle>(hEntry)->setValue(hTarget);
        }
    }

COH_CLOSE_NAMESPACE3
