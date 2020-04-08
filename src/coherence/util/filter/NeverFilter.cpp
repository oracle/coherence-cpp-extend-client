/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/NeverFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(80, NeverFilter);


// ----- constructors -------------------------------------------------------

NeverFilter::NeverFilter()
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool NeverFilter::evaluateEntry(Map::Entry::View /* vEntry */ ) const
    {
    return false;
    }


// ----- Filter interface ---------------------------------------------------

bool NeverFilter::evaluate(Object::View /* v */ ) const
    {
    return false;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t NeverFilter::calculateEffectiveness(Map::View /* vMapIndexes */,
        Set::View /* vSetKeys */) const
    {
    return 1;
    }

Filter::View NeverFilter::applyIndex(Map::View /* vMapIndexes */,
        Set::Handle hSetKeys) const
    {
    hSetKeys->clear();
    return NULL;
    }

// ----- PortableObject interface -------------------------------------------

void NeverFilter::readExternal(PofReader::Handle /* hIn */ )
    {
    }

void NeverFilter::writeExternal(PofWriter::Handle /* hOut */ ) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool NeverFilter::equals(Object::View v) const
    {
    return instanceof<NeverFilter::View>(v);
    }

size32_t NeverFilter::hashCode() const
    {
    return 0xEF;
    }

TypedHandle<const String> NeverFilter::toString() const
    {
    return String::create("NeverFilter");
    }


// ---- constants -----------------------------------------------------------

NeverFilter::Handle NeverFilter::getInstance()
    {
    static FinalHandle<NeverFilter> hNF(System::common(),
            NeverFilter::create());
    return hNF;
    }
COH_STATIC_INIT(NeverFilter::getInstance());

COH_CLOSE_NAMESPACE3
