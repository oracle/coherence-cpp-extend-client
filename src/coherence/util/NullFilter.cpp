/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/NullFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

COH_REGISTER_PORTABLE_CLASS(10, NullFilter);


// ----- constructors -------------------------------------------------------

NullFilter::NullFilter()
    {
    }

NullFilter::NullFilter(const NullFilter& that)
    : super(that)
    {
    }


// ----- Filter interface ---------------------------------------------------

bool NullFilter::evaluate(Object::View v) const
    {
    return (v != NULL);
    }


// ----- PortableObject interface -------------------------------------------

void NullFilter::readExternal(PofReader::Handle /*hIn*/ )
    {
    }

void NullFilter:: writeExternal(PofWriter::Handle /*hOut*/) const
    {
    }

COH_CLOSE_NAMESPACE2
