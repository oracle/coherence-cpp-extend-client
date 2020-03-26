/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/PresentFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(84, PresentFilter);


// ----- constructors -------------------------------------------------------

PresentFilter::PresentFilter()
    {
    }


// ----- EntryFilter interface ----------------------------------------------

bool PresentFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    InvocableMap::Entry::View vE = cast<InvocableMap::Entry::View>(vEntry, false);
    return (vE == NULL) || vE->isPresent();
    }


// ----- Filter interface ---------------------------------------------------

bool PresentFilter::evaluate(Object::View /* v */) const
    {
    return true;
    }


// ----- IndexAwareFilter interface -----------------------------------------

size32_t PresentFilter::calculateEffectiveness(Map::View /* vMapIndexes */,
        Set::View /* vSetKeys */) const
    {
    return 1;
    }

Filter::View PresentFilter::applyIndex(Map::View /* vMapIndexes */,
        Set::Handle /* hSetKeys */) const
    {
    return NULL;
    }

// ----- PortableObject interface -------------------------------------------

void PresentFilter::readExternal(PofReader::Handle /* hIn */ )
    {
    }

void PresentFilter:: writeExternal(PofWriter::Handle /* hOut */ ) const
    {
    }


// ----- Object interface ---------------------------------------------------

bool PresentFilter::equals(Object::View v) const
    {
    return instanceof<PresentFilter::View>(v);
    }

size32_t PresentFilter::hashCode() const
    {
    return 0xEF;
    }

TypedHandle<const String> PresentFilter::toString() const
    {
    return String::create("PresentFilter");
    }


// ---- constants -----------------------------------------------------------

PresentFilter::Handle PresentFilter::getInstance()
    {
    static FinalHandle<PresentFilter> hPF(System::common(),
            PresentFilter::create());
    return hPF;
    }
COH_STATIC_INIT(PresentFilter::getInstance());

COH_CLOSE_NAMESPACE3
