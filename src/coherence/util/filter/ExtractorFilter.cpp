/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ExtractorFilter.hpp"

#include "coherence/util/QueryMap.hpp"
#include "coherence/util/extractor/KeyExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::util::extractor::KeyExtractor;

// ----- constructors -------------------------------------------------------

ExtractorFilter::ExtractorFilter()
    : f_vExtractor(self())
    {
    }

ExtractorFilter::ExtractorFilter(ValueExtractor::View vExtractor)
    : f_vExtractor(self(), vExtractor)
    {
    COH_ENSURE_PARAM(vExtractor);
    }


// ----- EntryFilter interface ----------------------------------------------

Object::View ExtractorFilter::extract(Object::View v) const
    {
    return getValueExtractor()->extract(v);
    }

bool ExtractorFilter::evaluateEntry(Map::Entry::View vEntry) const
    {
    ValueExtractor::View  vExtractor = getValueExtractor();
    QueryMap::Entry::View vE         = cast<QueryMap::Entry::View>(vEntry, false);
    if (vE != NULL)
        {
        return evaluateExtracted(vE->extract(vExtractor));
        }
    else
        {
        KeyExtractor::View vKE = cast<KeyExtractor::View>(vExtractor, false);
        if (vKE != NULL)
            {
            return evaluateExtracted(vKE->extract(vEntry->getKey()));
            }
        else
            {
            return evaluateExtracted(vExtractor->extract(vEntry->getValue()));
            }
        }
   }


// ----- Filter interface ---------------------------------------------------

bool ExtractorFilter::evaluate(Object::View v) const
    {
    return evaluateExtracted(extract(v));
    }


// ----- PortableObject interface -------------------------------------------

void ExtractorFilter::readExternal(PofReader::Handle hIn)
    {
    initialize(f_vExtractor, cast<ValueExtractor::View>(hIn->readObject(0)));
    }

void ExtractorFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeObject(0, f_vExtractor);
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> ExtractorFilter::toString() const
    {
    return COH_TO_STRING("ExtractorFilter: (" << getValueExtractor() << ")");
    }


// ----- data members accessor-----------------------------------------------

ValueExtractor::View ExtractorFilter::getValueExtractor() const
    {
    return f_vExtractor;
    }

COH_CLOSE_NAMESPACE3
