/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Float64Sum.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(33, Float64Sum);


// ----- constructors -------------------------------------------------------

Float64Sum::Float64Sum()
    : super()
    {
    }

Float64Sum::Float64Sum(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Float64Sum::Float64Sum(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Float64Sum::init(bool fFinal)
    {
    AbstractFloat64Aggregator::init(fFinal);
    m_dflResult = 0.0;
    }

void Float64Sum::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_dflResult += cast<Number::View>(v)->getFloat64Value();
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
