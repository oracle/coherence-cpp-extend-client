/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/ValueChangeEventFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(85, ValueChangeEventFilter);


// ----- constructors -------------------------------------------------------

ValueChangeEventFilter::ValueChangeEventFilter()
   : super()
    {
    }

ValueChangeEventFilter::ValueChangeEventFilter(
        ValueExtractor::View vValueExtractor)
    : super(vValueExtractor)
    {
    }


// ----- ExtractorFilter interface ------------------------------------------

bool ValueChangeEventFilter::evaluateExtracted(
            Object::View /* vExtracted */) const
    {
    COH_THROW (UnsupportedOperationException::create(
            "ValueChangeEventFilter::evaluateExtracted"));
    }


// ----- Filter interface ---------------------------------------------------

bool ValueChangeEventFilter::evaluate(Object::View /* v */ ) const
    {
    //TODO implement when MapEvent exists
    COH_THROW (UnsupportedOperationException::create(
              "ValueChangeEventFilter::evaluate"));
      }


// ----- Object interface ---------------------------------------------------

bool ValueChangeEventFilter::equals(Object::View v) const
    {
    ValueChangeEventFilter::View that = cast<ValueChangeEventFilter::View>(v, false);
    if (NULL != that)
        {
        return f_vExtractor->equals(that->f_vExtractor);
        }
    return false;
     }

size32_t ValueChangeEventFilter::hashCode() const
    {
    return f_vExtractor->hashCode();
    }

TypedHandle<const String> ValueChangeEventFilter::toString() const
    {
    return COH_TO_STRING("ValueChangeEventFilter(extractor="
        << getValueExtractor()
        << ")");
    }

COH_CLOSE_NAMESPACE3
