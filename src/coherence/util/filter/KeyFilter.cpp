/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/KeyFilter.hpp"

#include "coherence/util/HashSet.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


// ----- constructors -------------------------------------------------------

KeyFilter::KeyFilter(Set::View vSetKeys)
    : f_vSetKeys(self())
    {
    HashSet::Handle hHash = HashSet::create(vSetKeys);
    initialize(f_vSetKeys, cast<Set::View>(hHash));
    }


// ----- EntryFilter interface ----------------------------------------------

bool KeyFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    return getKeys()->contains(vEntry->getKey());
    }


// ----- Filter interface ---------------------------------------------------

bool KeyFilter::evaluate(Object::View /* v */) const
    {
    COH_THROW (UnsupportedOperationException::create("KeyFilter::evaluate"));
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t KeyFilter::calculateEffectiveness(Map::View /* vMapIndexes */,
        Set::View /* vSetKeys */) const
    {
    return 1;
    }

Filter::View KeyFilter::applyIndex(Map::View /* vMapIndexes */,
        Set::Handle hSetKeys) const
    {
    hSetKeys->retainAll(f_vSetKeys);
    return NULL;
    }

// ----- Object interface ---------------------------------------------------

TypedHandle<const String> KeyFilter::toString() const
    {
    return COH_TO_STRING("Key in " << getKeys());
    }


// ----- data members accessor-----------------------------------------------

Set::View KeyFilter::getKeys() const
    {
    return f_vSetKeys;
    }

COH_CLOSE_NAMESPACE3
