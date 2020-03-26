/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/AndFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(62, AndFilter);


// ----- constructors -------------------------------------------------------

AndFilter::AndFilter()
    : super()
    {
    }

AndFilter::AndFilter(Filter::View vFilterLeft,
            Filter::View vFilterRight)
    : super()
    {
    ObjectArray::Handle haFilter = ObjectArray::create(2);
    haFilter[0] = vFilterLeft;
    haFilter[1] = vFilterRight;
    m_vaFilter = haFilter;
    }

COH_CLOSE_NAMESPACE3
