/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/aggregator/Float64Min.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"

#include <algorithm>
#include <limits>

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

COH_REGISTER_PORTABLE_CLASS(32, Float64Min);


// ----- constructors -------------------------------------------------------

Float64Min::Float64Min()
    : super()
    {
    }

Float64Min::Float64Min(ValueExtractor::View vExtractor)
    : super(vExtractor)
    {
    }

Float64Min::Float64Min(String::View vsMethod)
    : super(vsMethod)
    {
    }

// ----- AbstractAggregator Interface  --------------------------------------

void Float64Min::init(bool fFinal)
    {
    AbstractFloat64Aggregator::init(fFinal);
    m_dflResult = std::numeric_limits<float64_t>::max();
    }

void Float64Min::process(Object::View v, bool /* fFinal */)
    {
    if (v != NULL)
          {
          m_dflResult = std::min(m_dflResult,
                      cast<Number::View>(v)->getFloat64Value());
          m_count++;
          }
    }

COH_CLOSE_NAMESPACE3
