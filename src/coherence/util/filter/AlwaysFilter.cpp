/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/AlwaysFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(61, AlwaysFilter);


// ----- constructors -------------------------------------------------------

AlwaysFilter::AlwaysFilter()
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool AlwaysFilter::evaluateEntry(Map::Entry::View /* vEntry */) const
    {
    return true;
    }


// ----- Filter interface ---------------------------------------------------

bool AlwaysFilter::evaluate(Object::View /* v */) const
    {
    return true;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t AlwaysFilter::calculateEffectiveness(Map::View /* vMapIndexes */,
        Set::View /* vSetKeys */) const
    {
    return 1;
    }

Filter::View AlwaysFilter::applyIndex(Map::View /* vMapIndexes */,
        Set::Handle /* hSetKeys */) const
    {
    return NULL;
    }


// ----- PortableObject interface -------------------------------------------

void AlwaysFilter::readExternal(PofReader::Handle /* hIn */ )
    {
    }

void AlwaysFilter:: writeExternal(PofWriter::Handle /* hOut */) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool AlwaysFilter::equals(Object::View v) const
    {
    return instanceof<AlwaysFilter::View>(v);
    }

size32_t AlwaysFilter::hashCode() const
    {
    return 0xAF;
    }

TypedHandle<const String> AlwaysFilter::toString() const
    {
    return String::create("AlwaysFilter");
    }


// ---- constants -----------------------------------------------------------

AlwaysFilter::Handle AlwaysFilter::getInstance()
    {
    static FinalHandle<AlwaysFilter> hAF(System::common(),
            AlwaysFilter::create());
    return hAF;
    }
COH_STATIC_INIT(AlwaysFilter::getInstance());

COH_CLOSE_NAMESPACE3
