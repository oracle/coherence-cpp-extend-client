/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/IsNullFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(74, IsNullFilter);


// ----- constructors -------------------------------------------------------

IsNullFilter::IsNullFilter()
    : super()
    {
    }

IsNullFilter::IsNullFilter(ValueExtractor::View vValueExtractor)
    : super(vValueExtractor, (Object::View) NULL)
    {
    }

COH_CLOSE_NAMESPACE3
